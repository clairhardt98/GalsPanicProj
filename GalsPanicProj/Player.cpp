#include "pch.h"
#include "Player.h"

Player::Player()
{
	radius = 10;
	movement = Vector2D(0, 0);

	for (int i = 0; i < 4; i++)
		Line[i] = false;


	IsDrawing = false;
	SuccessedDrawing = false;
	NowDrawing = false;
	mov = STOP;
	CanMoveX = CanMoveY = false;
	dt = 0;
}

void Player::SetRect(const RECT& rv)
{
	rectView[0] = { rv.left + 50, rv.top + 100 };
	rectView[1] = { rv.left + 50,rv.bottom - 50 };
	rectView[2] = { rv.right - 50,rv.bottom - 50 };
	rectView[3] = { rv.right - 50, rv.top + 100 };

	center = Vector2D(rectView[0].x, rectView[0].y);
}

void Player::SetMovement(int m, float _dt)
{

	if (IsDrawing && mov == UP && m == DOWN)
		return;
	else if (IsDrawing && mov == DOWN && m == UP)
		return;
	else if (IsDrawing && mov == LEFT && m == RIGHT)
		return;
	else if (IsDrawing && mov == RIGHT && m == LEFT)
		return;

	dt = _dt;

	if (mov!= STOP && m!=STOP && m != mov)
	{
		Turn();
	}
	switch (m)
	{
	case 0:
		if (!IsDrawing || (IsDrawing && (mov != DOWN)))
		{
			movement.setX(0);
			movement.setY(-1);
			mov = UP;
		}

		break;
	case 1:
		if (!IsDrawing || (IsDrawing && (mov != UP)))
		{
			movement.setX(0);
			movement.setY(1);
			mov = DOWN;
		}
		break;
	case 2:
		if (!IsDrawing || (IsDrawing && (mov != RIGHT)))
		{
			movement.setX(-1);
			movement.setY(0);
			mov = LEFT;
		}
		break;
	case 3:
		if (!IsDrawing || (IsDrawing && (mov != LEFT)))
		{
			movement.setX(1);
			movement.setY(0);
			mov = RIGHT;
		}
		break;
	default:
		movement.setX(0);
		movement.setY(0);
		mov = STOP;
		break;
	}

}

void Player::Draw(HDC hdc)
{
	Polygon(hdc, rectView, 4);
	Ellipse(hdc, center.getX() - radius, center.getY() - radius,
		center.getX() + radius, center.getY() + radius);

	DrawLine(hdc);
	if(Points.size()>1)
	{
		POINT* tempP = new POINT[Points.size()];
		for (int i = 0; i < Points.size(); i++)
			tempP[i] = Points[i];
		Polygon(hdc, tempP, Points.size());
		delete[] tempP;
	}

}

void Player::Update()
{
	if (CanMoveX)
		center.setX(center.getX() + movement.getX() * Speed * dt);
	if (CanMoveY)
		center.setY(center.getY() + movement.getY() * Speed * dt);
	CheckCanMove();
	if (IsCollidedWithDrawedLine())
	{
		printf("Collided\n");
		EndDrawing();
	}
	if (IsCollidedWithBorderLine())
	{
		printf("Collided with borderline\n");
	}
	

}

void Player::CheckCanMove()
{
	if (IsDrawing)
	{
		CanMoveX = CanMoveY = true;
		if (center.getX() < rectView[0].x)
			center.setX(rectView[0].x);
		if (center.getY() > rectView[1].y)
			center.setY(rectView[1].y);
		if (center.getX() > rectView[2].x)
			center.setX(rectView[2].x);
		if (center.getY() < rectView[0].y)
			center.setY(rectView[0].y);
	}
	else
	{
		CanMoveX = CanMoveY = false;
		for (int i = 0; i < 4; i++)
		{
			if (rectView[i].y == rectView[(i + 1) % 4].y && center.getY() == rectView[i].y)
			{
				LONG max = max(rectView[i].x, rectView[(i + 1) % 4].x);
				LONG min = min(rectView[i].x, rectView[(i + 1) % 4].x);

				if (center.getX() >= max)
					center.setX(max);
				if (center.getX() <= min)
					center.setX(min);
				CanMoveX = true;
			}
			if (rectView[i].x == rectView[(i + 1) % 4].x && center.getX() == rectView[i].x)
			{
				LONG max = max(rectView[i].y, rectView[(i + 1) % 4].y);
				LONG min = min(rectView[i].y, rectView[(i + 1) % 4].y);

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
	if (!IsDrawing)
	{
		tempPoints.push_back({ (int)center.getX(),(int)center.getY() });
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
	for (int i = 0; i < 4; i++)
	{
		//x선분에 겹침
		if (rectView[i].y == rectView[(i + 1) % 4].y && (int)center.getY() == rectView[i].y)
		{
			tempPoints.push_back({(int)center.getX(), (int)center.getY()});
			//printf("borderline\n");
			SuccessedDrawing = true;
		}
		if (rectView[i].x == rectView[(i + 1) % 4].x && (int)center.getX() == rectView[i].x)
		{
			tempPoints.push_back({ (int)center.getX(), (int)center.getY() });
			SuccessedDrawing = true;
		}
	}
	if (SuccessedDrawing)
	{
		printf("successedDrawing\n");
		Points = tempPoints;
		NowDrawing = false;
		//tempPoints.clear();
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
			tempPoints.clear();
			printf("tempPoints Cleared\n");

		}
		IsDrawing = false;
		NowDrawing = false;
		SuccessedDrawing = false;
	}
}

BOOL Player::CheckLine(const POINT& p1, const POINT& p2)
{
	if (p1.x == p2.x)
	{
		if (center.getX() == p1.x)
			return true;
	}
	if (p1.y == p2.y)
	{
		if (center.getY() == p1.y)
			return true;
	}
	return false;
}