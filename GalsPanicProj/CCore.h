#pragma once
#include "Player.h"
#include "Enemy.h"
class CCore
{
	SINGLE(CCore);
private:
	HWND	hWnd;			//���� ������ �ڵ�
	POINT	ptResolution;	//���� ������ �ػ�
	HDC		hdc;			//���� �����쿡 Draw�� DC

	HBITMAP hBit;
	HDC		memDC;

private:
	bool GameCleared;
	bool GameStarted;


public:
	int Init(HWND hWnd, POINT res);
	void Progress();
private:
	void Update();
	void Render();

public:
	HWND GetMainHwnd() { return hWnd; }
	HDC GetMainDC() { return hdc; }

public:
	void SetEnemyPoints(const std::vector<POINT>& points);
	void GameClear();
	void StartScene();
};

