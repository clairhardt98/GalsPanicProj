#pragma once
#include <cmath>
class Vector2D
{
private:
	float x;
	float y;
	float mag;
public:
	Vector2D() { x = 0; y = 0; mag = 0; };
	Vector2D(const float x, const float y);
	float getX() { return x; };
	float getY() { return y; };
	float getMag() { return mag; };
	void setX(const float x) { this->x = x; setMag(); };
	void setY(const float y) { this->y = y; setMag(); };
	void setMag() { mag = sqrt(pow(x, 2) + pow(y, 2)); };
	Vector2D normalize();
	Vector2D operator+(const Vector2D&);
	Vector2D operator/(float t);
	bool operator==(const Vector2D&);

	friend Vector2D operator*(float t, Vector2D& v);

};