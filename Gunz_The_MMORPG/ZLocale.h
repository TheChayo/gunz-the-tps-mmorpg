#ifndef _ZLOCALE_H
#define _ZLOCALE_H

#include "CCBaseLocale.h"
#include "ZBaseAuthInfo.h"
#include "ZLocaleDefine.h"


//#define ZTOK_LANGUAGE_KOREA					"kor"
//#define ZTOK_LANGUAGE_INTERNATIONAL			"international"
//#define ZTOK_LANGUAGE_JAPAN					"jpn"

class ZConfiguration;





/// ����ȭ ���� ���� Ŭ���� 
class ZLocale : public CCBaseLocale
{
private:
	bool				m_bTeenMode;			// �ѱ������� ����ϴ� ƾ������� ����
	ZBaseAuthInfo*		m_pAuthInfo;			// ���� ����
protected:
	virtual bool OnInit();
	bool CreateAuthInfo();
public:
	ZLocale();
	virtual ~ZLocale();
	static ZLocale* GetInstance();

	bool IsTeenMode();
	void SetTeenMode(bool bTeenMode);

	/// Ȩ������������ �����ؾ��ϴ� �������� ���������� �ٷ� �������� ��쿡�� Ȩ�������� ����ش�.
	void RouteToWebsite();
	void PostLoginViaHomepage(CCUID* pAllocUID);

	bool ParseArguments(const char* pszArgs);
	ZBaseAuthInfo* GetAuthInfo()			{ return m_pAuthInfo; }
};

inline ZLocale* ZGetLocale()
{
	return ZLocale::GetInstance();
}





#endif