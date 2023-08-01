#include "pch.h"
#include "Player.h"
#include <string>
Player::Player()
{
	//<< 기본 수치
	radius = 10;
	movement = Vector2D(0, 0);
	//>>

	//<<그리기 BOOL 변수
	IsDrawing = false;
	SuccessedDrawing = false;
	NowDrawing = false;
	CanMoveX = CanMoveY = false;
	IsReturning = false;
	//>>

	//<<이동 상태 및 이동 속도
	mov = STOP;
	dt = 0;
	OnLineCnt = 1;
	health = 3;

	hBrush = CreateSolidBrush(RGB(255, 128, 0));
	PolygonPen = CreatePen(PS_SOLID, 2,RGB(128, 64, 64));
	PlayerPen = CreatePen(PS_SOLID, 2, RGB(128, 64, 64));
	percentage = 0.0f;
	Cleared = false;

}
Player::~Player()
{
	delete[] PointsArr;
	if (nullptr != pTex)
		delete pTex;
	if (nullptr != pBackTex)
		delete pBackTex;

	DeleteObject(hBrush);
	DeleteObject(PolygonPen);
	DeleteObject(PlayerPen);
}
void Player::SetRect(const RECT& rv)
{
	PercentageDisplayPos = { 10, 10 };
	HealthDisplayPos = { 200,10 };
	//<<기본 외곽 사각형 정의

	Points.push_back({ rv.left + 50, rv.top + 100 });
	Points.push_back({ rv.left + 50,rv.bottom - 50 });
	Points.push_back({ rv.right - 50,rv.bottom - 50 });
	Points.push_back({ rv.right - 50, rv.top + 100 });
	CCore::GetInst()->SetEnemyPoints(Points);
	/*for (int i = 0; i < Points.size(); i++)
		ImageRect[i] = Points[i];*/
		//>>

		//기본 위치 초기화
	center = Vector2D(Points[0].x, Points[0].y);
	PointsArr = new POINT[Points.size()];
	for (int i = 0; i < Points.size(); i++)
	{
		PointsArr[i] = Points[i];
	}
	CurLine = { 3,0 };
	CastedLinePoint = { (LONG)center.getX(), (LONG)center.getY() };
	PointSize = ImageSize = GetArea(Points);
}

void Player::SetDT(float _dt)
{
	dt = _dt;
}

void Player::SetMovement(int m)
{
	//<<스페이스바 누르고 있을 때 반대 방향으로 이동 못하게
	if (IsDrawing && mov == UP && m == DOWN)
		return;
	else if (IsDrawing && mov == DOWN && m == UP)
		return;
	else if (IsDrawing && mov == LEFT && m == RIGHT)
		return;
	else if (IsDrawing && mov == RIGHT && m == LEFT)
		return;
	//<<

	//deltaTime 정의
	//dt = _dt;

	//>>이동 방향을 꺾었을 때
	if (mov != STOP && m != STOP && m != mov)
	{
		Turn();
	}
	//<<

	//>>새로 입력된 이동방향에 따른 로직
	switch (m)
	{
	case 0:
		movement.setX(0);
		movement.setY(-1);
		mov = UP;
		break;
	case 1:
		movement.setX(0);
		movement.setY(1);
		mov = DOWN;
		break;
	case 2:
		movement.setX(-1);
		movement.setY(0);
		mov = LEFT;
		break;
	case 3:
		movement.setX(1);
		movement.setY(0);
		mov = RIGHT;
		break;
	default:
		movement.setX(0);
		movement.setY(0);
		mov = STOP;
		break;
	}
	//<<
}

void Player::Draw(HDC hdc)
{
	if (Cleared)return;
	DrawPoints(hdc);

	//printf("Cur Line : %d, %d\n", CurLine.first, CurLine.second);
	DrawLine(hdc);

	int Width = (int)pTex->Width();
	int Height = (int)pTex->Height();
	/*BitBlt(hdc, int(center.getX()-Width/2), int(center.getY()-Height/2),Width,Height,
		pTex->GetDC(),0,0,SRCCOPY);*/
	TransparentBlt(hdc, int(center.getX() - Width / 2), int(center.getY() - Height / 2),
		Width, Height,
		pTex->GetDC(),
		0,0,Width,Height,
		RGB(0,255,255));
	DisplayPercentage(hdc);
	DisplayHealth(hdc);
}

void Player::Update()
{
	if (IsReturning)
	{
		GetBackToFirstPoint();
		//sleep
		return;
	}
	CheckCanMove();
	if (CanMoveX)
		center.setX(center.getX() + movement.getX() * Speed * dt);
	if (CanMoveY)
		center.setY(center.getY() + movement.getY() * Speed * dt);

	//printf("now on line between %d, %d\n", CurLine.first, CurLine.second);
	//printf("canMoveX, canMoveY : %d, %d\n", CanMoveX, CanMoveY);
	//printf("IsDrawing : %d, NowDrawing : %d, SuccessedDrawing %d\n", IsDrawing, NowDrawing, SuccessedDrawing);

	if (IsCollidedWithDrawedLine())
	{
		printf("Collided\n");
		EndDrawing();
	}
	if (IsCollidedWithBorderLine())
	{
		printf("Collided with borderline\n");
		EndDrawing();
	}
}

void Player::CheckCanMove()
{
	if (IsDrawing)
	{
		if (CastLine() % 2 == 1)
			CanMoveX = CanMoveY = true;
		else
			CanMoveX = CanMoveY = false;
	}
	else
	{
		CanMoveX = CanMoveY = false;
		CheckCurLine();
		//printf("OnLineCnt : %d\n", OnLineCnt);
		//points 폴리곤 위에서 움직이기
		if (OnLineCnt == 2)// 꼭짓점에 있을 때
		{
			CanMoveX = CanMoveY = true;
			return;
		}
		if (OnLineCnt == 1)
		{
			CanMoveX = CanMoveY = true;
			//x선분일 때
			if (Points[CurLine.first].y == Points[CurLine.second].y)
			{
				LONG max = max(Points[CurLine.first].x, Points[CurLine.second].x);
				LONG min = min(Points[CurLine.first].x, Points[CurLine.second].x);

				if (center.getX() >= max)
					center.setX(max);
				if (center.getX() <= min)
					center.setX(min);
				CanMoveY = false;
			}
			if (Points[CurLine.first].x == Points[CurLine.second].x)
			{
				LONG max = max(Points[CurLine.first].y, Points[CurLine.second].y);
				LONG min = min(Points[CurLine.first].y, Points[CurLine.second].y);

				if (center.getY() >= max)
					center.setY(max);
				if (center.getY() <= min)
					center.setY(min);
				CanMoveX = false;
			}
		}

	}
}

void Player::StartDrawing()
{
	if (IsReturning) return;
	//<<그리기 시작, IsDrawing을 True 로
	if (!IsDrawing)
	{
		tempPoints.push_back({ (int)center.getX(),(int)center.getY() });
		printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
		printf("start Drawing\n");
		IsDrawing = true;
	}
}

void Player::Turn()
{
	if (IsDrawing)
	{
		tempPoints.push_back({ (int)center.getX(),(int)center.getY() });
		printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
		NowDrawing = true;
	}
}

void Player::DrawPoints(HDC hdc)
{
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Polygon(hdc, PointsArr, Points.size());
	SelectObject(hdc, hOldBrush);


	HPEN hOldPen = (HPEN)SelectObject(hdc, PolygonPen);
	//선 그리는 로직
	for (int i = 0; i < Points.size(); i++)
	{
		MoveToEx(hdc, Points[i].x, Points[i].y, nullptr);
		LineTo(hdc, Points[(i + 1)%Points.size()].x, Points[(i + 1) % Points.size()].y);
	}
	SelectObject(hdc, hOldPen);
}

void Player::SetTexture()
{
	pTex = new CTexture;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += L"Images\\kirby.bmp";
	pTex->Load(strFilePath);

}

int Player::CastLine()
{
	int x, y;
	int cnt = 0;
	//이동방향에 대한 선분 정의
	switch (mov)
	{
	case UP:
		x = (int)center.getX();
		y = (int)center.getY() - LineLength;
		break;
	case DOWN:
		x = (int)center.getX();
		y = (int)center.getY() + LineLength;
		break;
	case LEFT:
		x = (int)center.getX() - LineLength;
		y = (int)center.getY();
		break;
	case RIGHT:
		x = (int)center.getX() + LineLength;
		y = (int)center.getY();
		break;
	default:
		x = (int)center.getX();
		y = (int)center.getY();
		break;
	}
	CastedLinePoint = { x,y };
	POINT tempCenter = { center.getX(), center.getY() };

	for (int i = 0; i < Points.size(); i++)
	{
		//Points의 x선분
		if (Points[i].y == Points[(i + 1) % Points.size()].y)
		{
			int minX = min((int)Points[i].x, Points[(i + 1) % Points.size()].x);
			int maxX = max((int)Points[i].x, Points[(i + 1) % Points.size()].x);
			int minY = min((int)center.getY(), CastedLinePoint.y);
			int maxY = max((int)center.getY(), CastedLinePoint.y);
			if (Points[i].y < maxY && Points[i].y>minY && tempCenter.x > minX && tempCenter.x < maxX)
				cnt++;
			//return;
		}
		if (Points[i].x == Points[(i + 1) % Points.size()].x)
		{
			int minX = min((int)center.getX(), CastedLinePoint.x);
			int maxX = max((int)center.getX(), CastedLinePoint.x);
			int minY = min((int)Points[i].y, Points[(i + 1) % Points.size()].y);
			int maxY = max((int)Points[i].y, Points[(i + 1) % Points.size()].y);
			if (Points[i].x < maxX && Points[i].x>minX && tempCenter.y > minY && tempCenter.y < maxY)
				cnt++;
			//return;
		}
	}
	//printf("cnt : %d\n", cnt);
	return cnt;
}

void Player::DrawLine(HDC hdc)
{
	HPEN hOldPen = (HPEN)SelectObject(hdc, PlayerPen);
	if ((IsDrawing || IsReturning) && tempPoints.size() > 0)
	{
		for (int i = 0; i < tempPoints.size() - 1; i++)
		{
			MoveToEx(hdc, tempPoints[i].x, tempPoints[i].y, nullptr);
			LineTo(hdc, tempPoints[i + 1].x, tempPoints[i + 1].y);
		}
	}
	if ((IsDrawing || IsReturning) && tempPoints.size() > 0)
	{
		MoveToEx(hdc, tempPoints.back().x, tempPoints.back().y, nullptr);
		LineTo(hdc, center.getX(), center.getY());
	}
	SelectObject(hdc, hOldPen);
}

BOOL Player::IsCollidedWithDrawedLine()
{
	//현재 그리고 있는 도형에 닿으면 true 반환
	if (IsDrawing && tempPoints.size() > 2)
	{
		for (int i = 0; i < tempPoints.size() - 2; i++)
		{
			//y축 선분
			if (tempPoints[i].x == tempPoints[(i + 1) % tempPoints.size()].x && (int)center.getX() == tempPoints[i].x)
			{
				int Max = max(tempPoints[i].y, tempPoints[(i + 1) % tempPoints.size()].y);
				int Min = min(tempPoints[i].y, tempPoints[(i + 1) % tempPoints.size()].y);
				if (center.getY() > Min && center.getY() < Max)
				{
					return true;
				}
			}

			//x축 선분
			if (tempPoints[i].y == tempPoints[(i + 1) % tempPoints.size()].y && (int)center.getY() == tempPoints[i].y)
			{
				int Max = max(tempPoints[i].x, tempPoints[(i + 1) % tempPoints.size()].x);
				int Min = min(tempPoints[i].x, tempPoints[(i + 1) % tempPoints.size()].x);
				if (center.getX() > Min && center.getX() < Max)
				{
					return true;
				}
			}
		}
	}
	return false;
}

BOOL Player::IsCollidedWithBorderLine()
{

	if (!NowDrawing)
		return false;
	//<<Points에 닿으면
	for (int i = 0; i < Points.size(); i++)
	{
		//x선분에 겹침
		if (Points[i].y == Points[(i + 1) % Points.size()].y && (int)center.getY() == Points[i].y)
		{
			LONG max = max(Points[i].x, Points[(i + 1) % Points.size()].x);
			LONG min = min(Points[i].x, Points[(i + 1) % Points.size()].x);
			if (center.getX() >= min && center.getX() <= max)
			{
				tempPoints.push_back({ (int)center.getX(), Points[i].y });
				printf("Pushed %d, %d\n", (int)center.getX(), Points[i].y);
				SuccessedDrawing = true;
				break;
			}
		}
		if (Points[i].x == Points[(i + 1) % Points.size()].x && (int)center.getX() == Points[i].x)
		{
			LONG max = max(Points[i].y, Points[(i + 1) % Points.size()].y);
			LONG min = min(Points[i].y, Points[(i + 1) % Points.size()].y);

			if (center.getY() >= min && center.getY() <= max)
			{
				tempPoints.push_back({ Points[i].x, (int)center.getY() });
				printf("Pushed %d, %d\n", Points[i].x, (int)center.getY());
				SuccessedDrawing = true;
				break;
			}
		}
	}
	//<<
	//>>성공적으로 그리기를 마쳤다면
	if (SuccessedDrawing)
	{


		return true;
	}
	return false;
}

BOOL Player::IsCollidedWithMyPolygon()
{
	return 0;
}



int Player::GetArea(const std::vector<POINT>& _polygon)
{
	//매듭법을 사용해서 넓이 구하기
	int temp1, temp2, sum;
	sum = temp1 = temp2 = 0;

	for (int i = 0; i < _polygon.size() - 1; i++)
		temp1 += _polygon[i].x * _polygon[i + 1].y;
	for (int i = 1; i < _polygon.size(); i++)
		temp2 += _polygon[i].x * _polygon[i - 1].y;

	sum = abs(temp1 - temp2) / 2;
	return sum;
}

void Player::UpdatePoints()
{
	if (tempPoints.size() < 2 || !SuccessedDrawing) return;
	//CurLine을 기반으로 어떤 점을 기준으로 나눠야 할지 결정
	std::pair<int, int> prevLine = CurLine;
	std::pair<int, int> nextLine = CurLine;

	std::vector<POINT> tempPolygon1;
	std::vector<POINT> tempPolygon2;

	for (int i = 0; i < Points.size(); i++)
	{
		//Points의 두 점이 x축으로 평행 && 그 y값이 center의 y값과 같고, 
		if (Points[i].y == Points[(i + 1) % Points.size()].y && (int)center.getY() == Points[i].y)
		{
			LONG max = max(Points[i].x, Points[(i + 1) % Points.size()].x);
			LONG min = min(Points[i].x, Points[(i + 1) % Points.size()].x);

			if (center.getX() <= max && center.getX() >= min)
				nextLine = { i,(i + 1) % Points.size() };

		}
		if (Points[i].x == Points[(i + 1) % Points.size()].x && (int)center.getX() == Points[i].x)
		{
			LONG max = max(Points[i].y, Points[(i + 1) % Points.size()].y);
			LONG min = min(Points[i].y, Points[(i + 1) % Points.size()].y);

			if (center.getY() <= max && center.getY() >= min)
				nextLine = { i,(i + 1) % Points.size() };
		}
	}
	printf("prevLine : %d, %d , nextLine : %d, %d\n", prevLine.first, prevLine.second, nextLine.first, nextLine.second);
	if (prevLine == nextLine)
	{
		//반시계일때 그대로 넣고 시계일땐 거꾸로 넣어야 됨
		tempPolygon1 = tempPoints;
		printf("tempPoly 1 : \n");
		for (auto e : tempPolygon1)
			printf("\t %d, %d\n", e.x, e.y);

		if (CheckClockWise())
			Points.insert(Points.begin() + prevLine.first + 1, tempPoints.begin(), tempPoints.end());
		else
			Points.insert(Points.begin() + prevLine.first + 1, tempPoints.rbegin(), tempPoints.rend());
		tempPolygon2 = Points;
		printf("tempPoly 2 : \n");
		for (auto e : tempPolygon2)
			printf("\t %d, %d\n", e.x, e.y);
		SetPoints(tempPolygon1, tempPolygon2);
		//DeleteSamePoints();
		return;
	}
	//tempPolygon 1 생성
	for (int i = prevLine.second; i != (nextLine.first + 1) % Points.size(); i = (i + 1) % Points.size())
	{
		tempPolygon1.push_back(Points[i]);
		printf("tempPoly1 : pushed %d\n", i);
	}

	for (auto it = tempPoints.rbegin(); it != tempPoints.rend(); it++)
		tempPolygon1.push_back(*it);

	printf("tempPoly 1 : \n");
	for (auto e : tempPolygon1)
		printf("\t %d, %d\n", e.x, e.y);

	for (auto it = tempPoints.begin(); it != tempPoints.end(); it++)
		tempPolygon2.push_back(*it);

	for (int i = nextLine.second; i != (prevLine.first + 1) % Points.size(); i = (i + 1) % Points.size())
	{
		tempPolygon2.push_back(Points[i]);
		printf("tempPoly2 : pushed %d\n", i);
	}

	printf("tempPoly 2 : \n");
	for (auto e : tempPolygon2)
		printf("\t %d, %d\n", e.x, e.y);
	SetPoints(tempPolygon1, tempPolygon2);
	//DeleteSamePoints();
}

void Player::EndDrawing()
{

	if (IsDrawing)
	{
		if (!SuccessedDrawing)
		{
			CurTempLine = { POINT{(LONG)center.getX(), (LONG)center.getY()}, tempPoints.back() };
			IsDrawing = false;
			NowDrawing = false;
			IsReturning = true;
			return;
			//되돌아가는 로직
			//center = Vector2D(tempPoints[0].x, tempPoints[0].y);
			//tempPoints.clear();
			//GetBackToFirstPoint();
		}

		//CheckCanMove();
		UpdatePoints();
		tempPoints.clear();
		printf("tempPoints Cleared\n");
		IsDrawing = false;
		NowDrawing = false;
		SuccessedDrawing = false;
		//CurTempLine = {}
		//IsReturning = true;
	}
}

void Player::SetPoints(std::vector<POINT>& p1, std::vector<POINT>& p2)
{
	printf("Polygon1 area : %d, Polygon2 Area : %d\n ", (GetArea(p1)), (GetArea(p2)));
	if (GetArea(p1) > GetArea(p2))
		Points = p1;
	else
		Points = p2;

	//DeleteUnnecessaryPoints();

	delete[] PointsArr;
	PointsArr = new POINT[Points.size()];
	for (int i = 0; i < Points.size(); i++)
	{
		PointsArr[i] = Points[i];
		
	}
	PointSize = GetArea(Points);
	CCore::GetInst()->SetEnemyPoints(Points);
}

BOOL Player::CheckClockWise()
{
	//y선분일 경우
	if (Points[CurLine.first].x == Points[CurLine.second].x)
	{
		if ((Points[CurLine.second].y - Points[CurLine.first].y) * (tempPoints.back().y - tempPoints.front().y) > 0)
			return true;
		else
			return false;
	}
	//x선분일 경우
	if (Points[CurLine.first].y == Points[CurLine.second].y)
	{
		if ((Points[CurLine.second].x - Points[CurLine.first].x) * (tempPoints.back().x - tempPoints.front().x) > 0)
			return true;
		else
			return false;

	}
	return 0;
}

void Player::CheckCurLine()
{
	OnLineCnt = 0;
	for (int i = 0; i < Points.size(); i++)
	{
		if (Points[i].y == Points[(i + 1) % Points.size()].y && (int)center.getY() == Points[i].y)
		{
			LONG max = max(Points[i].x, Points[(i + 1) % Points.size()].x);
			LONG min = min(Points[i].x, Points[(i + 1) % Points.size()].x);

			if ((int)center.getX() <= max + 1 && (int)center.getX() >= min - 1)
			{
				OnLineCnt++;
				CurLine = { i,(i + 1) % Points.size() };
			}
			//CanMoveX = true;
		}
		if (Points[i].x == Points[(i + 1) % Points.size()].x && (int)center.getX() == Points[i].x)
		{
			LONG max = max(Points[i].y, Points[(i + 1) % Points.size()].y);
			LONG min = min(Points[i].y, Points[(i + 1) % Points.size()].y);

			if ((int)center.getY() <= max + 1 && (int)center.getY() >= min - 1)
			{
				OnLineCnt++;
				CurLine = { i,(i + 1) % Points.size() };
			}
			//CanMoveY = true;
		}
	}
}

void Player::DeleteUnnecessaryPoints()
{
	int idx = 0;
	for (int i = 1; i < Points.size(); i++)
	{
		if (Points[i - 1].y == Points[(i + 1) % Points.size()].y)
		{
			LONG max = max(Points[i - 1].x, Points[(i + 1) % Points.size()].x);
			LONG min = min(Points[i - 1].x, Points[(i + 1) % Points.size()].x);
			if (Points[i].x<max && Points[i].x>min)
			{
				idx = i;
				break;
			}

		}
		if (Points[i - 1].x == Points[(i + 1) % Points.size()].x)
		{
			LONG max = max(Points[i - 1].y, Points[(i + 1) % Points.size()].y);
			LONG min = min(Points[i - 1].y, Points[(i + 1) % Points.size()].y);
			if (Points[i].y <= max && Points[i].y >= min)
			{
				idx = i;
				break;
			}

		}

	}
	if (idx)
	{
		printf("Points[%d] erased\n", idx);
		Points.erase(Points.begin() + idx);
	}
}

void Player::DisplayPercentage(HDC hdc)
{
	//printf("ImageSize : %d, PointSize : %d\n", ImageSize, PointSize);
	percentage = 100.0f * (float)(ImageSize - PointSize) / (float)ImageSize;
	if (percentage > 80)
	{
		CCore::GetInst()->GameClear();
	}
	std::wstring str = std::to_wstring(percentage);
	str += '%';
	TextOut(hdc, PercentageDisplayPos.x, PercentageDisplayPos.y, str.c_str(), str.size());
}

void Player::DisplayHealth(HDC hdc)
{
	std::wstring str = L"남은 체력 : ";
	str += std::to_wstring(health);
	TextOut(hdc, HealthDisplayPos.x, HealthDisplayPos.y, str.c_str(), str.size());
}

void Player::GetBackToFirstPoint()
{
	POINT start = CurTempLine.first;
	POINT end = CurTempLine.second;

	if (start.x == end.x)
	{
		LONG max = max(start.y, end.y);
		LONG min = min(start.y, end.y);
		//y방향 선분이고 아래로 이동해야 할 경우
		if (start.y == min)
		{
			if (center.getY() < end.y)
			{
				center.setY(center.getY() + 2*Speed * dt);
				return;
			}
		}
		//위로 올라가야 할 경우
		if (start.y == max)
		{
			if (center.getY() > end.y + 1)
			{
				center.setY(center.getY() - 2*Speed * dt);
				return;
			}
		}
	}
	if (start.y == end.y)
	{
		LONG max = max(start.x, end.x);
		LONG min = min(start.x, end.x);
		//x방향 선분이고 오른쪽으로 이동해야 할 경우
		if (start.x == min)
		{
			if (center.getX() < end.x)
			{
				center.setX(center.getX() + 2*Speed * dt);
				return;
			}
		}
		//왼쪽으로 이동해야 할 경우
		if (start.x == max)
		{
			if (center.getX() > end.x + 1)
			{
				center.setX(center.getX() - 2*Speed * dt);
				return;
			}
		}
	}
	if (tempPoints.size() == 1)
	{
		IsReturning = false;
		tempPoints.clear();
		return;
	}

	else
	{
		//auto it = tempPoints.
		CurTempLine.first = CurTempLine.second;
		tempPoints.pop_back();
		printf("tempPoints : \n");
		for (auto e : tempPoints)
			printf("\t %d, %d\n", e.x, e.y);
		//if (!tempPoints.empty())
		CurTempLine.second = tempPoints.back();
		return;
	}
}


