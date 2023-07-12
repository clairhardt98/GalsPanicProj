#pragma once
#include "framework.h"
class Player
{
private:
	const int speed = 3;
	POINT center;
	LONG radius;
	Vector2D movement;

	POINT rectView[4];
	int Line;
	BOOL X, Y;
public:
	Player() { radius = 10; center = { radius,radius }; movement = Vector2D(0, 0); X = Y = false; }
	void SetRectView(const RECT& rv);
	POINT GetCenter() { return center; }
	void SetCenter(const POINT& c) { center = c; }
	LONG GetRadius() { return radius;}
	void SetRadius(const LONG& l) { radius = l; }
	const Vector2D& GetMovement() { return movement; }
	void SetMovement(int movement);

	void Draw(HDC hdc);
	void Update();

	BOOL CheckCanMove();
	BOOL OnLine(const POINT& p1, const POINT&p2);
};