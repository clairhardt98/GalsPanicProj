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
	//�׸� �� ����ϴ� �ӽ� ����
	std::vector<POINT> tempPoints;
	//���� ���� ����
	std::vector<POINT> Points;
	//�׸� ���� ����ϴ� POINT �迭
	POINT* PointsArr;
	//���� ��� ���� ���� �ִ����� ���� ����
	std::pair<int, int> CurLine;
	int OnLineCnt;
	//���� ĳ����
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
//<���� �׸����� ����
public:
	//�׸��� ������ �ε����� ��
	BOOL IsCollidedWithDrawedLine();
	//�׵θ��� �ε����� ��
	BOOL IsCollidedWithBorderLine();
	//�̹� �����ϴ� �����￡ �ε����� ��
	BOOL IsCollidedWithMyPolygon();
	//�� ������ �����ϴ��� Ȯ��
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