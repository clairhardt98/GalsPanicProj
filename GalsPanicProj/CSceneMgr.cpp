#include "pch.h"
#include "CSceneMgr.h"


CSceneMgr::CSceneMgr()
	:m_arrScene{}
	,m_pCurScene(0)
{

}

CSceneMgr::~CSceneMgr() 
{
	for (UINT i = 0; i < (UINT)SCENE_TYPE::END; i++)
	{
		if (nullptr != m_arrScene[i])
			delete m_arrScene[i];
	}
}

void CSceneMgr::Init()
{
	m_arrScene[(UINT)SCENE_TYPE::START] = new CScene_Start;
	m_arrScene[(UINT)SCENE_TYPE::MAIN] = new CScene_Main;
	m_arrScene[(UINT)SCENE_TYPE::END] = new CScene_End;
	m_pCurScene = m_arrScene[(UINT)SCENE_TYPE::START];
}

