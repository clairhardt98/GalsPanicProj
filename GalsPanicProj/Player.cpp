#include "Player.h"

void Player::SetRectView(const RECT& rv)
{
	rectView[0] = { rv.left, rv.top };
	rectView[1] = { rv.left,rv.bottom };
	rectView[2] = { rv.right,rv.bottom };
	rectView[3] = { rv.right, rv.top };
	center = rectView[0];
}

void Player::SetMovement(int m)
{
	switch(m)
	{
	case 0:
		movement.setX(0);
		movement.setY(-1);
		break;
	case 1:
		movement.setX(0);
		movement.setY(1);
		break;
	case 2:
		movement.setX(-1);
		movement.setY(0);
		break;
	case 3:
		movement.setX(1);
		movement.setY(0);
		break;
	default:
		movement.setX(0);
		movement.setY(0);
		break;
	}
	movement = 3*movement;
}

void Player::Draw(HDC hdc)
{
	Polygon(hdc, rectView, 4);
	Ellipse(hdc, center.x - radius, center.y - radius, center.x + radius, center.y + radius);
}

void Player::Update()
{
	Line = 0;
	for (int i = 0; i < 4; i++)
	{
		
		if (OnLine(rectView[i], rectView[(i + 1) % 4]))
		{
			printf("Player canMove : (%d, %d), Line : %d\n", X,Y,Line);
			
		}
	}
	if(CheckCanMove())
	{
		center.x += movement.getX();
		center.y += movement.getY();
	}
}

BOOL Player::CheckCanMove()
{
	if (center.x + movement.getX() < rectView[0].x)return false;
	if (center.x + movement.getX() > rectView[3].x)return false;
	if (center.y + movement.getY() < rectView[0].y)return false;
	if (center.y + movement.getY() > rectView[1].y)return false;
	if ((Line == 1) && X)movement.setX(0);
	if ((Line == 1) && Y)movement.setY(0);
	return true;

}

BOOL Player::OnLine(const POINT& p1, const POINT& p2)
{
	
	//y축 방향 선분일 때
	if (p1.x == p2.x)
	{
		if (center.x + radius > p1.x && center.x - radius < p1.x)
		{
			X = true;
			Line++;
			
		}
	}
	if (p1.y == p2.y)
	{
		if (center.y + radius > p1.y && center.y - radius < p1.y)
		{
			Y = true;
			Line++;
			
		}
	
	}
	if (Line)return true;
	else return false;
}

