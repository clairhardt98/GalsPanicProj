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
	borderLine[0] = { rv.left + 50, rv.top + 100 };
	borderLine[1] = { rv.left + 50,rv.bottom - 50 };
	borderLine[2] = { rv.right - 50,rv.bottom - 50 };
	borderLine[3] = { rv.right - 50, rv.top + 100 };
	//>>
	
	//기본 위치 초기화
	center = Vector2D(borderLine[0].x, borderLine[0].y);
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
	if (mov!= STOP && m!=STOP && m != mov)
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
	//테두리 사각형
	Polygon(hdc, borderLine, 4);
	DrawPoints(hdc);
	Ellipse(hdc, center.getX() - radius, center.getY() - radius,
		center.getX() + radius, center.getY() + radius);

	DrawLine(hdc);
}

void Player::Update()
{
	if (CanMoveX)
		center.setX(center.getX() + movement.getX() * Speed * dt);
	if (CanMoveY)
		center.setY(center.getY() + movement.getY() * Speed * dt);
	CheckCanMove();
	printf("canMoveX, canMoveY : %d, %d\n", CanMoveX, CanMoveY);
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
		//일단 다 움직일 수 있음
		CanMoveX = CanMoveY = true;
		//borderLine의 바깥으로 움직이는 것은 안됨
		if (center.getX() < borderLine[0].x)
			center.setX(borderLine[0].x);
		if (center.getY() > borderLine[1].y)
			center.setY(borderLine[1].y);
		if (center.getX() > borderLine[2].x)
			center.setX(borderLine[2].x);
		if (center.getY() < borderLine[0].y)
			center.setY(borderLine[0].y);
	}
	else
	{
		CanMoveX = CanMoveY = false;

		//points 폴리곤 위에서 움직이기
		if(Points.size()>1)
		{
			for (int i = 0; i < Points.size() - 1; i++)
			{
				if (Points[i].y == Points[i + 1].y && (int)center.getY() == Points[i].y)
				{
					LONG max = max(Points[i].x, Points[i + 1].x);
					LONG min = min(Points[i].x, Points[i + 1].x);

					
					if (center.getX() >= max)
						center.setX(max);
					if (center.getX() <= min)
						center.setX(min);
					
					CanMoveX = true;
					//return;
				}
				if (Points[i].x == Points[i + 1].x && (int)center.getX() == Points[i].x)
				{
					LONG max = max(Points[i].y, Points[i + 1].y);
					LONG min = min(Points[i].y, Points[i + 1].y);

					
					if (center.getY() >= max)
						center.setY(max);
					if (center.getY() <= min)
						center.setY(min);
					
					CanMoveY = true;
					//return;
				}
			}
		}
		
		//borderLine의 선 위에서 움직이기
		for (int i = 0; i < 4; i++)
		{
			if (borderLine[i].y == borderLine[(i + 1) % 4].y && (int)center.getY() == borderLine[i].y)
			{
				LONG max = max(borderLine[i].x, borderLine[(i + 1) % 4].x);
				LONG min = min(borderLine[i].x, borderLine[(i + 1) % 4].x);

				
				if (center.getX() >= max)
					center.setX(max);
				if (center.getX() <= min)
					center.setX(min);
				
				CanMoveX = true;
			}
			if (borderLine[i].x == borderLine[(i + 1) % 4].x && (int)center.getX() == borderLine[i].x)
			{
				LONG max = max(borderLine[i].y, borderLine[(i + 1) % 4].y);
				LONG min = min(borderLine[i].y, borderLine[(i + 1) % 4].y);

				
				if (center.getY() >= max)
					center.setY(max);
				if (center.getY() <= min)
					center.setY(min);
				
				CanMoveY = true;
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
	if (Points.size() > 1)
	{
		POINT* tempP = new POINT[Points.size()];
		for (int i = 0; i < Points.size(); i++)
			tempP[i] = Points[i];
		Polygon(hdc, tempP, Points.size());
		delete[] tempP;
	}
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
	//<<보더라인에 닿으면
	if (!NowDrawing)
		return false;
	for (int i = 0; i < 4; i++)
	{
		//x선분에 겹침
		if (borderLine[i].y == borderLine[(i + 1) % 4].y && (int)center.getY() == borderLine[i].y)
		{
			tempPoints.push_back({(int)center.getX(), (int)center.getY()});
			printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
			SuccessedDrawing = true;
			break;
		}
		if (borderLine[i].x == borderLine[(i + 1) % 4].x && (int)center.getX() == borderLine[i].x)
		{
			tempPoints.push_back({ (int)center.getX(), (int)center.getY() });
			printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
			SuccessedDrawing = true;
			break;
		}
	}
	//<<
	//>>성공적으로 그리기를 마쳤다면
	if (SuccessedDrawing)
	{
		//임시 배열 벡터를 points에 옮김
		printf("successedDrawing\n");
		Points = tempPoints;
		printf("Points : \n");
		for (auto e : Points)
			printf("\t (%d, %d)\n", e.x, e.y);
		NowDrawing = false;
		return true;
	}
	return false;
}

BOOL Player::IsCollidedWithMyPolygon()
{
	return 0;
}

void Player::EndDrawing()
{
	if (IsDrawing)
	{
		if(!SuccessedDrawing)
		{
			center = Vector2D(tempPoints[0].x, tempPoints[0].y);
		}
		tempPoints.clear();
		printf("tempPoints Cleared\n");
		IsDrawing = false;
		NowDrawing = false;
		SuccessedDrawing = false;
	}
}

//BOOL Player::CheckLine(const POINT& p1, const POINT& p2)
//{
//	if (p1.x == p2.x)
//	{
//		if (center.getX() == p1.x)
//			return true;
//	}
//	if (p1.y == p2.y)
//	{
//		if (center.getY() == p1.y)
//			return true;
//	}
//	return false;
//}