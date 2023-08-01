#pragma once
using namespace std;

class CScene
{

private:
	wstring m_strName;
public:
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return m_strName; }

public:
	CScene();
	virtual ~CScene();

};

