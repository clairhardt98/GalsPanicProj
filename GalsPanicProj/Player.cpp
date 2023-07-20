#include "pch.h"
#include "Player.h"

Player::Player()
{
	//<< �⺻ ��ġ
	radius = 10;
	movement = Vector2D(0, 0);
	//>>

	//<<�׸��� BOOL ����
	IsDrawing = false;
	SuccessedDrawing = false;
	NowDrawing = false;
	CanMoveX = CanMoveY = false;
	//>>
	
	//<<�̵� ���� �� �̵� �ӵ�
	mov = STOP;
	dt = 0;
}

void Player::SetRect(const RECT& rv)
{
	//<<�⺻ �ܰ� �簢�� ����
	borderLine[0] = { rv.left + 50, rv.top + 100 };
	borderLine[1] = { rv.left + 50,rv.bottom - 50 };
	borderLine[2] = { rv.right - 50,rv.bottom - 50 };
	borderLine[3] = { rv.right - 50, rv.top + 100 };
	//>>
	
	//�⺻ ��ġ �ʱ�ȭ
	center = Vector2D(borderLine[0].x, borderLine[0].y);
}

void Player::SetMovement(int m, float _dt)
{
	//<<�����̽��� ������ ���� �� �ݴ� �������� �̵� ���ϰ�
	if (IsDrawing && mov == UP && m == DOWN)
		return;
	else if (IsDrawing && mov == DOWN && m == UP)
		return;
	else if (IsDrawing && mov == LEFT && m == RIGHT)
		return;
	else if (IsDrawing && mov == RIGHT && m == LEFT)
		return;
	//<<

	//deltaTime ����
	dt = _dt;

	//>>�̵� ������ ������ ��
	if (mov!= STOP && m!=STOP && m != mov)
	{
		Turn();
	}
	//<<

	//>>���� �Էµ� �̵����⿡ ���� ����
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
	//�׵θ� �簢��
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
		//�ϴ� �� ������ �� ����
		CanMoveX = CanMoveY = true;
		//borderLine�� �ٱ����� �����̴� ���� �ȵ�
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

		//points ������ ������ �����̱�
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
		
		//borderLine�� �� ������ �����̱�
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
	//<<�׸��� ����, IsDrawing�� True ��
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
	//���� �׸��� �ִ� ������ ������ true ��ȯ
	if (IsDrawing && tempPoints.size() > 2)
	{
		for (int i = 0; i < tempPoints.size() - 2; i++)
		{
			//y�� ����
			if (tempPoints[i].x == tempPoints[(i + 1) % tempPoints.size()].x && (int)center.getX() == tempPoints[i].x)
			{
				int Max = max(tempPoints[i].y, tempPoints[(i + 1) % tempPoints.size()].y);
				int Min = min(tempPoints[i].y, tempPoints[(i + 1) % tempPoints.size()].y);
				if (center.getY() > Min && center.getY() < Max)
				{
					return true;
				}
			}

			//x�� ����
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
	//<<�������ο� ������
	if (!NowDrawing)
		return false;
	for (int i = 0; i < 4; i++)
	{
		//x���п� ��ħ
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
	//>>���������� �׸��⸦ ���ƴٸ�
	if (SuccessedDrawing)
	{
		//�ӽ� �迭 ���͸� points�� �ű�
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