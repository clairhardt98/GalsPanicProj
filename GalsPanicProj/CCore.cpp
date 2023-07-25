#include "pch.h"
#include "CCore.h"
#include "CTimeMgr.h"

//objects
Player player;
CCore::CCore()
	:hWnd(0)
	, ptResolution{}
	, hdc(0)
	, hBit(0)
	, memDC(0)
	
{}

CCore::~CCore()
{
	ReleaseDC(hWnd,hdc);

	DeleteDC(memDC);
	DeleteObject(hBit);
}

int CCore::Init(HWND hWnd, POINT res)
{
	this->hWnd = hWnd;
	this->ptResolution = res;

	RECT rt = { 0,0,ptResolution.x,ptResolution.y };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, true);
	SetWindowPos(hWnd, nullptr, 600, 100, rt.right - rt.left, rt.bottom - rt.top, 0);

	hdc = GetDC(hWnd);

	hBit = CreateCompatibleBitmap(hdc, ptResolution.x, ptResolution.y);
	memDC = CreateCompatibleDC(hdc);

	HBITMAP hOldBit = (HBITMAP)SelectObject(memDC, hBit);
	DeleteObject(hOldBit);
	//Init Mgr
	CTimeMgr::GetInst()->Init();

	//Init Player
	player.SetRect(rt);
	return S_OK;
}

void CCore::Progress()
{
	CTimeMgr::GetInst()->Update();
	Update();
	Render();
}

void CCore::Update()
{
	static bool spacePressed = false;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		player.SetMovement(0,fDT);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		player.SetMovement(1,fDT);
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		player.SetMovement(2, fDT);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		player.SetMovement(3, fDT);
	}
	else if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		player.StartDrawing();
		spacePressed = true;
	}
	else if (GetAsyncKeyState(VK_SPACE) & 0x0000)
	{
		player.EndDrawing();
		spacePressed = false;
	}
	else
	{
		player.SetMovement(4, fDT);
		player.EndDrawing();
	}
	
	player.Update();
}

void CCore::Render()
{
	Rectangle(memDC, -1, -1, ptResolution.x + 1, ptResolution.y + 1);
	player.Draw(memDC);
	BitBlt(hdc, 0, 0, ptResolution.x, ptResolution.y, memDC, 0, 0, SRCCOPY);
}
