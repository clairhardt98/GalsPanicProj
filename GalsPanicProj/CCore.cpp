#include "pch.h"
#include "CCore.h"
#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CTexture.h"
#include <PathCch.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment(lib,"Gdiplus.lib")

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
	, GameCleared(false)
	, GameStarted(false)
{}

CCore::~CCore()
{
	ReleaseDC(hWnd, hdc);

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
	CPathMgr::GetInst()->init();

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += L"Images\\shiba1.bmp";

	hBitmap = (HBITMAP)LoadImage(NULL, strFilePath.c_str(),
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (hBitmap == NULL)
		MessageBox(NULL, TEXT("이미지 로드 에러"), TEXT("에러"), MB_OK);

	GetObject(hBitmap, sizeof(BITMAP), &BitBack);
	//Init Player
	player.SetRect(rt);
	player.SetTexture();

	//init Enemy
	enemy.SetPosition(rt);
	enemy.SetTexture();

	StartScene();
	return S_OK;
}

void CCore::Progress()
{
	static float accum;
	if (GameCleared)
	{
		accum += fDT;
		if (accum > 2)
		{
			MessageBox(NULL, TEXT("GAME CLEAR!!!"), TEXT("GAME CLEAR!!!"), MB_OK);
			PostQuitMessage(0);
		}
	}
	CTimeMgr::GetInst()->Update();
	CTimeMgr::GetInst()->Render();
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


	if (player.GetIsDrawing() && enemy.CheckCollision(player.GetCenter(), player.GetRadius()))
	{
		player.Damage();
		if (player.GetHealth() <= 0)
		{
			MessageBox(NULL, TEXT("Game Over!!!"), TEXT("Game Over!!!"), MB_OK);
			PostQuitMessage(0);
		}
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

void CCore::GameClear()
{
	player.SetClear(true);
	enemy.SetClear(true);
	GameCleared = true;
}

void CCore::StartScene()
{
	HBRUSH hBrush, oldBrush;
	hBrush = CreateSolidBrush(RGB(255, 125, 0));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc, -1, -1, ptResolution.x + 1, ptResolution.y + 1);
	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);


	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Graphics graphics(hdc);
	wstring str(L"땅따먹기");
	FontFamily fontFamily(L"Arial");
	Font font(&fontFamily, 70, FontStyleRegular, UnitPixel);
	Font font2(&fontFamily, 24, FontStyleRegular, UnitPixel);

	RectF layoutRect((float)ptResolution.x / 2 - 160.0f, (float)ptResolution.y / 2 - 150.0f, 400.0f, 100.0f);

	SolidBrush solidBrush(Color(255, 128, 64, 64));
	graphics.DrawString(str.c_str(), -1, &font, layoutRect, NULL, &solidBrush);
	layoutRect = { (float)ptResolution.x / 2 - 140.0f, (float)ptResolution.y / 2 + 250.0f, 400.0f, 100.0f };
	graphics.DrawString(L"Press Space To Continue...", -1, &font2, layoutRect, NULL, &solidBrush);
	

	while (1)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			break;
	}
}
