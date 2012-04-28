#pragma once

#include "CCLocaleDefine.h"

// ���� �ڵ�
enum CCCountry
{
	CCC_INVALID			= 0,
	CCC_KOREA			= 82,		// �ѱ�
	CCC_US				= 1,		// �̱�(���ͳ׼ų�)
	CCC_JAPAN			= 81,		// �Ϻ�
	CCC_BRAZIL			= 55,		// �����
	CCC_INDIA			= 91,		// �ε�

	// 10000�̻���� CustomID.
	CCC_NHNUSA			= 10001		// NHN USA.
};

//
//#ifdef LOCALE_US
//	#define DEFAULT_COUNTRY			CCC_US
//#elif LOCALE_JAPAN
//	#define DEFAULT_COUNTRY			CCC_JAPAN
//#elif LOCALE_BRAZIL
//	#define DEFAULT_COUNTRY			CCC_BRAZIL
//#elif LOCALE_INDIA
//	#define DEFAULT_COUNTRY			CCC_INDIA
//#else
//	#define DEFAULT_COUNTRY			CCC_KOREA
//#endif
//

/* TODO: MLangageConf.h �� �����ؾ��� - bird */

// LANG_xxx�� winnt.h�� ���ǵǾ� ����. - by SungE.
enum CCLanguage
{
	CCL_INVALID				= 0x00,
	CCL_CHINESE				= LANG_CHINESE,					// �߱���
	CCL_CHINESE_TRADITIONAL	= SUBLANG_CHINESE_TRADITIONAL,	// �߱��� ������ -  _-)a;; 
	CCL_KOREAN				= LANG_KOREAN,					// �ѱ��� (LANG_KOREAN���� ����)
	CCL_ENGLISH				= LANG_ENGLISH,					// ���� (LANG_ENGLISH���� ����)
	CCL_JAPANESE			= LANG_JAPANESE,				// �Ϻ��� (LANG_JAPANESE���� ����)
	CCL_BRAZIL				= LANG_PORTUGUESE,				// ����� (LANG_BRAZIL���� ����)
	CCL_INDIA				= LANG_INDONESIAN,				// �ε� (LANG_INDONESIAN���� ����)
	CCL_GERMAN				= LANG_GERMAN,					// ���Ͼ� (LANG_GERMAN���� ����)
	CCL_SPANISH				= LANG_SPANISH,					// �����ξ� (LANG_SPANISH���� ����)
};

/// ����ȭ ���� �ֻ��� ���� Ŭ���� 
class CCBaseLocale
{
private:
	void InitLanguageFromCountry();
protected:
	CCCountry			m_nCountry;
	CCLanguage			m_nLanguage;

	bool				m_bIsComplete;

	virtual bool OnInit() = 0;
public:
	CCBaseLocale();
	virtual ~CCBaseLocale();
	bool Init(CCCountry nCountry);

	const CCCountry	GetCountry()		{ return m_nCountry; }
	const CCLanguage GetLanguage()		{ return m_nLanguage; }
	void SetLanguage(CCLanguage langID)	{ m_nLanguage = langID; }	// ������ ���� �ʱ�ȭ�� �� �ٸ� ������ �ٲٰ� ������

	const bool bIsComplete()			{ return m_bIsComplete; }
};

const CCCountry GetCountryID( const char* pCountry );
const CCLanguage GetLanguageID( const char* pLanguage );
