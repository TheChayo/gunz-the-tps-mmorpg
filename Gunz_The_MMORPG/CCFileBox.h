#pragma once
#pragma comment(lib, "Shlwapi.lib")

#include "CCListBox.h"

#define CCFILEBOX	"FileBox"

/// ���� ����Ʈ �ڽ�
class CCFileBox : public CCListBox{
	char	m_szFilter[256];
	char	m_szBaseDir[256];
public:
	/// @param	szFilter	���� ���� ( ��: *.exe )
	CCFileBox(const char* szFilter, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCFileBox(void);

	/// ���Ͱ����� ��� ��������
	/// @param	szFilter	���� ���� ( ��: *.exe )
	void Refresh(const char* szFilter=NULL);
	const char* GetBaseDir(void);

	virtual const char* GetClassName(void){ return CCFILEBOX; }
};
