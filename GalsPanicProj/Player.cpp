#include "pch.h"
#include "Player.h"

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
	//>>

	//<<이동 상태 및 이동 속도
	mov = STOP;
	dt = 0;

}

void Player::SetRect(const RECT& rv)
{
	//<<기본 외곽 사각형 정의

	Points.push_back({ rv.left + 50, rv.top + 100 });
	Points.push_back({ rv.left + 50,rv.bottom - 50 });
	Points.push_back({ rv.right - 50,rv.bottom - 50 });
	Points.push_back({ rv.right - 50, rv.top + 100 });
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
}

void Player::SetMovement(int m, float _dt)
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
	dt = _dt;

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

	DrawPoints(hdc);

	//printf("Cur Line : %d, %d\n", CurLine.first, CurLine.second);
	DrawLine(hdc);
	Ellipse(hdc, center.getX() - radius, center.getY() - radius,
		center.getX() + radius, center.getY() + radius);
	for (int i = 0; i < tempPoints.size(); i++)
		Ellipse(hdc, tempPoints[i].x - 5, tempPoints[i].y - 5, tempPoints[i].x + 5, tempPoints[i].y + 5);
}

void Player::Update()
{
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

		//points 폴리곤 위에서 움직이기

		for (int i = 0; i < Points.size(); i++)
		{
			if (Points[i].y == Points[(i + 1) % Points.size()].y && (int)center.getY() == Points[i].y)
			{
				LONG max = max(Points[i].x, Points[(i + 1) % Points.size()].x);
				LONG min = min(Points[i].x, Points[(i + 1) % Points.size()].x);

				//if (center.getX() < min || center.getX() > max)continue;
				if (center.getX() >= max)
					center.setX(max);
				if (center.getX() <= min)
					center.setX(min);

				CurLine = { i,(i + 1) % Points.size() };
				CanMoveX = true;
				//return;
			}
			if (Points[i].x == Points[(i + 1) % Points.size()].x && (int)center.getX() == Points[i].x)
			{
				LONG max = max(Points[i].y, Points[(i + 1) % Points.size()].y);
				LONG min = min(Points[i].y, Points[(i + 1) % Points.size()].y);

				//if (center.getY() < min || center.getY() > max)continue;
				if (center.getY() >= max)
					center.setY(max);
				if (center.getY() <= min)
					center.setY(min);
				CurLine = { i,(i + 1) % Points.size() };
				CanMoveY = true;
				//return;
			}
		}
	}
}

void Player::StartDrawing()
{
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
	Polygon(hdc, PointsArr, Points.size());
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
	POINT tempCenter = { center.getX(), center.getY()};

	for (int i = 0; i < Points.size(); i++)
	{
		//Points의 x선분
		if (Points[i].y == Points[(i + 1) % Points.size()].y)
		{
			int minX = min((int)Points[i].x, Points[(i + 1) % Points.size()].x);
			int maxX = max((int)Points[i].x, Points[(i + 1) % Points.size()].x);
			int minY = min((int)center.getY(), CastedLinePoint.y);
			int maxY = max((int)center.getY(), CastedLinePoint.y);
			if (Points[i].y < maxY && Points[i].y>minY && tempCenter.x>minX && tempCenter.x<maxX)
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
	if (IsDrawing && tempPoints.size() > 0)
	{
		for (int i = 0; i < tempPoints.size() - 1; i++)
		{
			MoveToEx(hdc, tempPoints[i].x, tempPoints[i].y, nullptr);
			LineTo(hdc, tempPoints[i + 1].x, tempPoints[i + 1].y);
		}
	}
	if (IsDrawing)
	{
		MoveToEx(hdc, tempPoints.back().x, tempPoints.back().y, nullptr);
		LineTo(hdc, center.getX(), center.getY());
	}
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
				tempPoints.push_back({ (int)center.getX(), (int)center.getY() });
				printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
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
				tempPoints.push_back({ (int)center.getX(), (int)center.getY() });
				printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
				SuccessedDrawing = true;
				break;
			}
		}
	}
	//<<
	//>>성공적으로 그리기를 마쳤다면
	if (SuccessedDrawing)
	{

		//printf("TempPoints : \n");
		//for (auto e : tempPoints)
		//	printf("\t (%d, %d)\n", e.x, e.y);
		////NowDrawing = false;
		return true;
	}
	return false;
}

BOOL Player::IsCollidedWithMyPolygon()
{
	return 0;
}

//int Player::CCW(POINT p1, POINT p2, POINT p3)
//{
//	int s = p1.x * p2.y + p2.x * p3.y + p3.x * p1.y;
//	s -= (p1.y * p2.x + p2.y * p3.x + p3.y * p1.x);
//
//	if (s > 0)return 1;
//	if (s == 0)return 0;
//	if (s < 0)return -1;
//}
//
//BOOL Player::IsIntersect(POINT p1, POINT p2, POINT p3, POINT p4)
//{
//	int p1p2 = CCW(p1, p2, p3) * CCW(p1, p2, p4);
//	int p3p4 = CCW(p3, p4, p1) * CCW(p3, p4, p2);
//
//	if (p1p2 == 0 && p3p4 == 0)
//		return false;
//
//	return p1p2 <= 0 && p3p4 <= 0;
//}

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
	if (tempPoints.size() < 3) return;
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

			if(center.getX()<=max && center.getX()>=min)
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
		tempPolygon1 = tempPoints;
		printf("tempPoly 1 : \n");
		for (auto e : tempPolygon1)
			printf("\t %d, %d\n", e.x, e.y);

		Points.insert(Points.begin()+prevLine.first+1, tempPoints.begin(), tempPoints.end());
		tempPolygon2 = Points;
		printf("tempPoly 2 : \n");
		for (auto e : tempPolygon2)
			printf("\t %d, %d\n", e.x, e.y);
		SetPoints(tempPolygon1, tempPolygon2);
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
}

void Player::EndDrawing()
{
	if (IsDrawing)
	{
		if (!SuccessedDrawing)
		{
			//되돌아가는 로직
			center = Vector2D(tempPoints[0].x, tempPoints[0].y);
		}
		//CheckCanMove();
		UpdatePoints();
		tempPoints.clear();
		printf("tempPoints Cleared\n");
		IsDrawing = false;
		NowDrawing = false;
		SuccessedDrawing = false;
	}
}

void Player::SetPoints(std::vector<POINT>& p1, std::vector<POINT>& p2)
{
	printf("Polygon1 area : %d, Polygon2 Area : %d\n ", (GetArea(p1)), (GetArea(p2)));
	if (GetArea(p1) > GetArea(p2))
		Points = p1;
	else
		Points = p2;

	delete[] PointsArr;
	PointsArr = new POINT[Points.size()];
	for (int i = 0; i < Points.size(); i++)
	{
		PointsArr[i] = Points[i];
	}
}
