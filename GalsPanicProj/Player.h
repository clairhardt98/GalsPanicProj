#pragma once
#include <list>
class Player
{
private:
	enum MoveState{UP,DOWN,LEFT,RIGHT,STOP};
	MoveState mov;
	Vector2D center;
	LONG radius;
	Vector2D movement;

	//그릴 때 사용하는 임시 벡터
	std::vector<POINT> tempPoints;
	//로직 수행 벡터
	std::vector<POINT> Points;
	//그릴 때만 사용하는 POINT 배열
	POINT* PointsArr;
	//현재 어느 선분 위에 있는지에 대한 정보
	std::pair<int, int> CurLine;
	//라인 캐스팅
	POINT CastedLinePoint;
	const LONG LineLength = 1000;

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
	void DrawPoints(HDC hdc);
	
	int CastLine();

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

public:
	int GetArea(const std::vector<POINT>& _polygon);
	void UpdatePoints();
};