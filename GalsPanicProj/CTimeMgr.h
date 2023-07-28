#pragma once
#include "pch.h"
class CTimeMgr
{
	SINGLE(CTimeMgr);
private:
	LARGE_INTEGER	curCount;
	LARGE_INTEGER	prevCount;
	LARGE_INTEGER	frequency;

	double			deltaTime;
	double			Acc;
	UINT			callCount;
	UINT			FPS;

public:
	void Init();
	void Update();
	void Render();

public:
	double GetDT() { return deltaTime; }
	float GetfDT() { return (float)deltaTime; }
};

