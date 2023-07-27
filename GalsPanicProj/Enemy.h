#pragma once
class Enemy
{
private:
	enum MoveState { UP, DOWN, LEFT, RIGHT};
	MoveState mov;
	Vector2D center;
	Vector2D movement;
	std::vector<POINT> playerPoints;
	
	const LONG radius = 50;
	const float moveSpeed = 200.0f;
	float dt;

	POINT LineCastStartPos;
	POINT LineCastEndPos;
	const LONG LineLength = 1000;

	double accum;

public:
	Enemy();
	//~Enemy();
public:
	void Draw(HDC hdc);
	void Update();
public:
	void SetPosition(const RECT& rv);
	void SetMovement();
	void SetRandomMov();
	void SetDT(float _dt);
public:
	int CheckMov();
public:
	void GetPlayerPoints(const std::vector<POINT>& points);
};