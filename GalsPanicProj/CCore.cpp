#include "pch.h"
#include "CCore.h"
#include "CTimeMgr.h"
#include <PathCch.h>

//objects
Player player;
Enemy enemy;
HBITMAP hBitmap;
BITMAP BitBack;
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

	hBitmap = (HBITMAP)LoadImage(NULL, TEXT("수지.bmp"), 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (hBitmap==NULL)
		MessageBox(NULL, TEXT("이미지 로드 에러"), TEXT("에러"), MB_OK);

	GetObject(hBitmap, sizeof(BITMAP), &BitBack);
	//Init Player
	player.SetRect(rt);

	//init Enemy
	enemy.SetPosition(rt);
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
	player.SetDT(fDT);
	enemy.SetDT(fDT);


	if (GetKeyState(VK_UP) & 0x8000)
	{
		player.SetMovement(0);
	}
	else if (GetKeyState(VK_DOWN) & 0x8000)
	{
		player.SetMovement(1);
	}
	else if (GetKeyState(VK_LEFT) & 0x8000)
	{
		player.SetMovement(2);
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		player.SetMovement(3);
	}
	else if ((GetKeyState(VK_SPACE) & 0x8000))
	{
		player.StartDrawing();
	}
	else if (GetKeyState(VK_SPACE) & 0x0000)
	{
		player.EndDrawing();
	}
	else
	{
		player.SetMovement(4);
		player.EndDrawing();
	}

	player.Update();
	enemy.SetMovement();
	enemy.Update();
}

void CCore::Render()
{
	Rectangle(memDC, -1, -1, ptResolution.x + 1, ptResolution.y + 1);
	{
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		int bx = BitBack.bmWidth;
		int by = BitBack.bmHeight;
		BitBlt(memDC, 0, 0, bx, by, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	player.Draw(memDC);
	enemy.Draw(memDC);
	BitBlt(hdc, 0, 0, ptResolution.x, ptResolution.y, memDC, 0, 0, SRCCOPY);
	
}

void CCore::SetEnemyPoints(const std::vector<POINT>& points)
{
	enemy.GetPlayerPoints(points);
}
