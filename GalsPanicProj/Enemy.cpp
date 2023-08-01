#include "pch.h"
#include "Enemy.h"

Enemy::Enemy()
{
	SetRandomMov();
	accum = 0;
	Cleared = false;
}

Enemy::~Enemy()
{
	if(pTex!=nullptr)
		delete pTex;
}
void Enemy::Draw(HDC hdc)
{
	if (Cleared)return;
	int Width = (int)pTex->Width();
	int Height = (int)pTex->Height();
	TransparentBlt(hdc, int(center.getX() - Width / 2), int(center.getY() - Height / 2),
		Width, Height,
		pTex->GetDC(),
		0, 0, Width, Height,
		RGB(255, 0, 255));
}

void Enemy::Update()
{
	accum += dt;
	if (CheckMov() %2 == 0)
	{
		switch (mov)
		{
		case UP:
			mov = DOWN;
			break;
		case DOWN:
			mov = UP;
			break;
		case LEFT:
			mov = RIGHT;
			break;
		case RIGHT:
			mov = LEFT;
			break;
		}
		SetMovement();
	}
	else
		SetRandomMov();
	center.setX(center.getX() + movement.getX() * moveSpeed * dt);
	center.setY(center.getY() + movement.getY() * moveSpeed * dt);
}

void Enemy::SetPosition(const RECT& rv)
{
	center.setX((int)(rv.right - rv.left) / 2);
	center.setY((int)(rv.bottom - rv.top) / 2);
}

void Enemy::SetMovement()
{
	switch (mov)
	{
	case UP:
		movement.setX(0);
		movement.setY(-1);
		LineCastStartPos = { (LONG)center.getX(), (LONG)center.getY() - radius - 1 };
		LineCastEndPos = { LineCastStartPos.x, LineCastStartPos.y - LineLength };
		break;
	case DOWN:
		movement.setX(0);
		movement.setY(1);
		LineCastStartPos = { (LONG)center.getX(), (LONG)center.getY() + radius + 1 };
		LineCastEndPos = { LineCastStartPos.x, LineCastStartPos.y + LineLength };
		break;
	case LEFT:
		movement.setX(-1);
		movement.setY(0);
		LineCastStartPos = { (LONG)center.getX() - radius - 1, (LONG)center.getY() };
		LineCastEndPos = { LineCastStartPos.x - LineLength, LineCastStartPos.y };
		break;
	case RIGHT:
		movement.setX(1);
		movement.setY(0);
		LineCastStartPos = { (LONG)center.getX() + radius + 1 , (LONG)center.getY()};
		LineCastEndPos = { LineCastStartPos.x + LineLength , LineCastStartPos.y};
		break;
	/*default:
		movement.setX(0);
		movement.setY(0);
		LineCastStartPos = { (LONG)center.getX(), (LONG)center.getY()};
		LineCastEndPos = { LineCastStartPos.x, LineCastStartPos.y };*/
	}
}

void Enemy::SetRandomMov()
{
	if (accum < 3)return;
	else accum = 0;
	srand(time(NULL));
	MoveState temp = static_cast<MoveState>(rand() % 4);
	while (temp == mov)
	{
		temp = static_cast<MoveState>(rand() % 4);
	}
	mov = temp;
}

void Enemy::SetDT(float _dt)
{
	dt = _dt;
}

void Enemy::SetTexture()
{
	pTex = new CTexture;
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += L"Images\\Enemy.bmp";
	pTex->Load(strFilePath);
}

int Enemy::CheckMov()
{
	int cnt = 0;
	for (int i = 0; i < playerPoints.size(); i++)
	{
		//Points의 x선분
		if (playerPoints[i].y == playerPoints[(i + 1) % playerPoints.size()].y)
		{
			int minX = min(playerPoints[i].x, playerPoints[(i + 1) % playerPoints.size()].x);
			int maxX = max(playerPoints[i].x, playerPoints[(i + 1) % playerPoints.size()].x);
			int minY = min(LineCastStartPos.y, LineCastEndPos.y);
			int maxY = max(LineCastStartPos.y, LineCastEndPos.y);
			if (playerPoints[i].y < maxY && playerPoints[i].y>minY && LineCastStartPos.x > minX && LineCastStartPos.x < maxX)
				cnt++;
			//return;
		}
		if (playerPoints[i].x == playerPoints[(i + 1) % playerPoints.size()].x)
		{
			int minX = min(LineCastStartPos.x, LineCastEndPos.x);
			int maxX = max(LineCastStartPos.x, LineCastEndPos.x);
			int minY = min(playerPoints[i].y, playerPoints[(i + 1) % playerPoints.size()].y);
			int maxY = max(playerPoints[i].y, playerPoints[(i + 1) % playerPoints.size()].y);
			if (playerPoints[i].x < maxX && playerPoints[i].x>minX && LineCastStartPos.y > minY && LineCastStartPos.y < maxY)
				cnt++;
			//return;
		}
	}
	//printf("cnt : %d\n", cnt);
	return cnt;
}

bool Enemy::CheckCollision(Vector2D playerPos, LONG playerRadius)
{
	//두 점 사이의 거리가 반지름의 합보다 크면 충돌아님
	LONG distSqr = pow(center.getX() - playerPos.getX(), 2) + pow(center.getY() - playerPos.getY(), 2);
	LONG radiusSqr = pow(radius + playerRadius, 2);
	if (radiusSqr > distSqr)return true;
	else
		return false;
}

void Enemy::GetPlayerPoints(const std::vector<POINT>& points)
{
	playerPoints = points;
}
