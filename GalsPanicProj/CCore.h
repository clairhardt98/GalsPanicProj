#pragma once
#include "Player.h"
class CCore
{
	SINGLE(CCore);
private:
	HWND	hWnd;			//���� ������ �ڵ�
	POINT	ptResolution;	//���� ������ �ػ�
	HDC		hdc;			//���� �����쿡 Draw�� DC

	HBITMAP hBit;
	HDC		memDC;


public:
	int Init(HWND hWnd, POINT res);
	void Progress();
private:
	void Update();
	void Render();

public:
	HWND GetMainHwnd() { return hWnd; }
};

