#include "pch.h"
#include "CPathMgr.h"
#include "CCore.h"

CPathMgr::CPathMgr()
	:m_szContentPath{} {}
CPathMgr::~CPathMgr(){}
void CPathMgr::init()
{
	GetCurrentDirectory(255, m_szContentPath);

	int len = wcslen(m_szContentPath);

	for (int i = len - 1; i >= 0; --i)
	{
		if ('\\' == m_szContentPath[i])
		{
			m_szContentPath[i] = '\0';
			break;
		}
	}

	wcscat_s(m_szContentPath, 255,L"\\bin\\content\\");
	//상위폴더로
	//+bin\\content\\붙여주기

	SetWindowText(CCore::GetInst()->GetMainHwnd(), m_szContentPath);
}
