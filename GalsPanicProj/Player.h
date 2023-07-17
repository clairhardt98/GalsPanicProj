#pragma once
class Player
{
private:
	enum MoveState{UP,DOWN,LEFT,RIGHT,STOP};
	MoveState mov;
	Vector2D center;
	LONG radius;
	Vector2D movement;

	POINT rectView[4];
	BOOL Line[4];
	std::vector<POINT> tempPoints;
	std::vector<POINT> Points;

	BOOL IsDrawing;
	BOOL CanMoveX;
	BOOL CanMoveY;
	BOOL NowDrawing;
	BOOL SuccessedDrawing;

	const float Speed = 200.0f;
	float dt;

public:
	Player();
	void SetRect(const RECT& rv);
	Vector2D GetCenter() { return center; }
	void SetCenter(const Vector2D& c) { center = c; }
	LONG GetRadius() { return radius;}
	void SetRadius(const LONG& l) { radius = l; }
	const Vector2D& GetMovement() { return movement; }
	void SetMovement(int movement, float dt);
	void CheckCanMove();
	BOOL CheckLine(const POINT& p1, const POINT& p2);

	void StartDrawing();
	void EndDrawing();
	void Turn();
	void Draw(HDC hdc);
	void DrawLine(HDC hdc);
	void Update();
//<도형 그릴때의 로직
public:
	//그리던 도형에 부딪혔을 때
	BOOL IsCollidedWithDrawedLine();
	//테두리에 부딪혔을 때
	BOOL IsCollidedWithBorderLine();
	//이미 존재하는 폴리곤에 부딪혔을 때
	BOOL IsCollidedWithMyPolygon();

};