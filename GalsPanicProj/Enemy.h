#pragma once
#include "CTexture.h"
#include "CCore.h"
class Enemy
{
private:
	enum MoveState { UP, DOWN, LEFT, RIGHT};
	MoveState mov;
	Vector2D center;
	Vector2D movement;
	std::vector<POINT> playerPoints;
	
	const LONG radius = 50;
	const float moveSpeed = 400.0f;
	float dt;

	POINT LineCastStartPos;
	POINT LineCastEndPos;
	const LONG LineLength = 1000;

	double accum;

	CTexture* pTex;
	bool Cleared;

public:
	Enemy();
	~Enemy();
public:
	void Draw(HDC hdc);
	void Update();
public:
	void SetPosition(const RECT& rv);
	void SetMovement();
	void SetRandomMov();
	void SetDT(float _dt);
	void SetTexture();
public:
	Vector2D GetCenter() { return center; }
	LONG GetRadius() const{ return radius; }
public:
	int CheckMov();
	bool CheckCollision(Vector2D playerPos, LONG playerRadius);
public:
	void GetPlayerPoints(const std::vector<POINT>& points);
	void SetClear(bool b) { Cleared = b; }
};