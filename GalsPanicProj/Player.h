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

	POINT ImageRect[4];
	//그릴 때 사용하는 임시 벡터
	std::vector<POINT> tempPoints;
	//로직 수행 벡터
	std::vector<POINT> Points;
	//그릴 때만 사용하는 POINT 배열
	POINT* PointsArr;
	//현재 어느 선분 위에 있는지에 대한 정보
	std::pair<int, int> CurLine;
	int OnLineCnt;
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

	LONG ImageSize;
	LONG PointSize;
	POINT PercentageDisplayPos;

public:
	Player();
	~Player();
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
	//두 선분이 교차하는지 확인
	/*int CCW(POINT p1, POINT p2, POINT p3);
	BOOL IsIntersect(POINT p1, POINT p2, POINT p3, POINT p4);*/

public:
	int GetArea(const std::vector<POINT>& _polygon);
	void UpdatePoints();
	void SetPoints(std::vector<POINT>& p1, std::vector<POINT>& p2);
	BOOL CheckClockWise();

public:
	void CheckCurLine();
	void DeleteUnnecessaryPoints();
	void DisplayPercentage(HDC hdc);
	void GetBackToFirstPoint();
	void GetBackThroughLine(const POINT& start, const POINT& end);
};