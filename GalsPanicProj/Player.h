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

	//�׸� �� ����ϴ� �ӽ� ����
	std::vector<POINT> tempPoints;
	//���� ���� ����
	std::vector<POINT> Points;
	//�׸� ���� ����ϴ� POINT �迭
	POINT* PointsArr;
	//���� ��� ���� ���� �ִ����� ���� ����
	std::pair<int, int> CurLine;
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
//<���� �׸����� ����
public:
	//�׸��� ������ �ε����� ��
	BOOL IsCollidedWithDrawedLine();
	//�׵θ��� �ε����� ��
	BOOL IsCollidedWithBorderLine();
	//�̹� �����ϴ� �����￡ �ε����� ��
	BOOL IsCollidedWithMyPolygon();

public:
	int GetArea(const std::vector<POINT>& _polygon);
	void UpdatePoints();
};