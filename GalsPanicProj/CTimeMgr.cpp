#include "pch.h"
#include "CTimeMgr.h"
#include "CCore.h"

CTimeMgr::CTimeMgr()
	:curCount{}
	, prevCount{}
	, frequency{}
	,deltaTime(0)
	,Acc(0)
	,callCount(0)
	,FPS(0)
{}

CTimeMgr::~CTimeMgr()
{}

void CTimeMgr::Init()
{
	QueryPerformanceCounter(&prevCount);
	QueryPerformanceFrequency(&frequency);
}

void CTimeMgr::Update()
{
	QueryPerformanceCounter(&curCount);
	deltaTime = (double)(curCount.QuadPart - prevCount.QuadPart) / (double)frequency.QuadPart;

	prevCount = curCount;
	
}

void CTimeMgr::Render()
{
	++callCount;
	Acc += deltaTime;
	if (Acc >= 1.)
	{
		FPS = callCount;
		Acc = 0;
		callCount = 0;

		wchar_t szBuffer[255] = {};
		swprintf_s(szBuffer, L"FPS : %d, DT : %f", FPS, deltaTime);
		SetWindowText(CCore::GetInst()->GetMainHwnd(), szBuffer);
	}
}
