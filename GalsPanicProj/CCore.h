#pragma once
#include "Player.h"
#include "Enemy.h"
class CCore
{
	SINGLE(CCore);
private:
	HWND	hWnd;			//메인 윈도우 핸들
	POINT	ptResolution;	//메인 윈도우 해상도
	HDC		hdc;			//메인 윈도우에 Draw할 DC

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

