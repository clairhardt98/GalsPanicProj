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

	Points.push_back({ rv.left + 50, rv.top + 100 });
	Points.push_back({ rv.left + 50,rv.bottom - 50 });
	Points.push_back({ rv.right - 50,rv.bottom - 50 });
	Points.push_back({ rv.right - 50, rv.top + 100 });
	//>>

	//�⺻ ��ġ �ʱ�ȭ
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
	if (mov != STOP && m != STOP && m != mov)
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

		//points ������ ������ �����̱�

		for (int i = 0; i < Points.size(); i++)
		{
			if (Points[i].y == Points[(i + 1) % Points.size()].y && (int)center.getY() == Points[i].y)
			{
				LONG max = max(Points[i].x, Points[(i + 1) % Points.size()].x);
				LONG min = min(Points[i].x, Points[(i + 1) % Points.size()].x);

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
	Polygon(hdc, PointsArr, Points.size());
}

int Player::CastLine()
{
	int x, y;
	int cnt = 0;
	//�̵����⿡ ���� ���� ����
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

	for (int i = 0; i < Points.size(); i++)
	{
		//Points�� x����
		if (Points[i].y == Points[(i + 1) % Points.size()].y)
		{
			int min = min((int)center.getY(), CastedLinePoint.y);
			int max = max((int)center.getY(), CastedLinePoint.y);
			if (Points[i].y < max && Points[i].y>min)
				cnt++;
			//return;
		}
		if (Points[i].x == Points[(i + 1) % Points.size()].x)
		{
			int min = min((int)center.getX(), CastedLinePoint.x);
			int max = max((int)center.getX(), CastedLinePoint.x);
			if (Points[i].x < max && Points[i].x>min)
				cnt++;
			//return;
		}
	}

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

	if (!NowDrawing)
		return false;
	//<<Points�� ������
	for (int i = 0; i < Points.size(); i++)
	{
		//x���п� ��ħ
		if (Points[i].y == Points[(i + 1) % 4].y && (int)center.getY() == Points[i].y)
		{
			tempPoints.push_back({ (int)center.getX(), (int)center.getY() });
			printf("Pushed %d, %d\n", (int)center.getX(), (int)center.getY());
			SuccessedDrawing = true;
			break;
		}
		if (Points[i].x == Points[(i + 1) % 4].x && (int)center.getX() == Points[i].x)
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

int Player::GetArea(const std::vector<POINT>& _polygon)
{
	//�ŵ���� ����ؼ� ���� ���ϱ�
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
	//CurLine�� ������� � ���� �������� ������ ���� ����
	std::pair<int, int> prevLine = CurLine;
	std::pair<int, int> nextLine = CurLine;

	std::vector<POINT> tempPolygon1;
	std::vector<POINT> tempPolygon2;

	for (int i = 0; i < Points.size(); i++)
	{
		if (Points[i].y == Points[(i + 1) % Points.size()].y && (int)center.getY() == Points[i].y)
		{
			LONG max = max(Points[i].x, Points[(i + 1) % Points.size()].x);
			LONG min = min(Points[i].x, Points[(i + 1) % Points.size()].x);

			if (center.getX() >= max)
				center.setX(max);
			if (center.getX() <= min)
				center.setX(min);

			nextLine = { i,(i + 1) % Points.size() };
			
		}
		if (Points[i].x == Points[(i + 1) % Points.size()].x && (int)center.getX() == Points[i].x)
		{
			LONG max = max(Points[i].y, Points[(i + 1) % Points.size()].y);
			LONG min = min(Points[i].y, Points[(i + 1) % Points.size()].y);


			if (center.getY() >= max)
				center.setY(max);
			if (center.getY() <= min)
				center.setY(min);
			nextLine = { i,(i + 1) % Points.size() };
		}
	}
	//printf("prevLine : %d, %d , nextLine : %d, %d\n", prevLine.first, prevLine.second, nextLine.first, nextLine.second);

	//tempPolygon 1 ����
	int idx = prevLine.second;
	while(1)
	{
		if (idx == CurLine.first)break;
		tempPolygon1.push_back(Points[idx]);
		idx = (idx + 1) % Points.size();
	}
	for (auto it = tempPoints.rbegin(); it != tempPoints.rend(); it++)
		tempPolygon1.push_back(*it);
	
	printf("tempPoly 1 : \n");
	for (auto e : tempPolygon1)
		printf("\t %d, %d\n", e.x, e.y);
	//tempPolygon 2 ����
	idx = CurLine.second;
	while (1)
	{
		if (idx == prevLine.first)break;
		tempPolygon2.push_back(Points[idx]);
		idx = (idx + 1) % Points.size();
	}
	for (auto it = tempPoints.begin(); it != tempPoints.end(); it++)
		tempPolygon2.push_back(*it);

	printf("tempPoly 2 : \n");

	for (auto e : tempPolygon2)
		printf("\t %d, %d\n", e.x, e.y);
	/*if (GetArea(tempPolygon1) >= GetArea(tempPolygon2))
		Points = tempPolygon1;
	else
		Points = tempPolygon2;*/
}

void Player::EndDrawing()
{
	if (IsDrawing)
	{
		if (!SuccessedDrawing)
		{
			//�ǵ��ư��� ����
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

