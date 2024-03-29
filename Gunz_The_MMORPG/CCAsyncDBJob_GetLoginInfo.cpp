#include "stdafx.h"
#include "CCAsyncDBJob_GetLoginInfo.h"
#include "CCMatchPremiumIPCache.h"
#include "CCMatchConfig.h"

void CCAsyncDBJob_GetLoginInfo::Run(void* pContext)
{
	_ASSERT(m_pAccountInfo);

	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	// 원래 계정은 넷마블에 있으므로 해당 계정이 없으면 새로 생성한다. 
	if (!pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword))
	{
		int nGunzSex;	// 건즈디비의 성별값은 넷마블 성별값과 반대이다.
		if (m_nSex == 0) nGunzSex = 1; else nGunzSex = 0;

		int nCert = 0;
		if (strlen(m_szCertificate) > 0)
		{
			if (m_szCertificate[0] == '1') nCert =1;
		}

		pDBMgr->CreateAccount(m_szUserID, m_szUniqueID, nCert, m_szName, m_nAge, nGunzSex);
		pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword);
	}

	// 계정 정보를 읽는다.
	if (!pDBMgr->GetAccountInfo(m_nAID, m_pAccountInfo, CCGetServerConfig()->GetServerID()))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	// 계정 페널티 정보를 읽는다. - 2010-08-10 홍기주
	if( !pDBMgr->GetAccountPenaltyInfo(m_nAID, m_pAccountPenaltyInfo) ) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	// 프리미엄 IP를 체크한다.
	if (m_bCheckPremiumIP)
	{
		bool bIsPremiumIP = false;
		bool bExistPremiumIPCache = false;
		
		bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(m_dwIP, bIsPremiumIP);

		// 만약 캐쉬에 없으면 직접 DB에서 찾도록 한다.
		if (!bExistPremiumIPCache)
		{
			if (pDBMgr->CheckPremiumIP(m_szIP, bIsPremiumIP))
			{
				// 결과를 캐쉬에 저장
				MPremiumIPCache()->AddIP(m_dwIP, bIsPremiumIP);
			}
			else
			{
				MPremiumIPCache()->OnDBFailed();
			}

		}

		if (bIsPremiumIP) m_pAccountInfo->m_nPGrade = MMPG_PREMIUM_IP;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}


bool CCAsyncDBJob_GetLoginInfo::Input(CCMatchAccountInfo* pNewAccountInfo,
			CCMatchAccountPenaltyInfo* pNewAccountPenaltyInfo,
			const char* szUserID, 
			const char* szUniqueID, 
			const char* szCertificate, 
			const char* szName, 
			const int nAge, 
			const int nSex,
			const bool bFreeLoginIP,
			unsigned long nChecksumPack,
			const bool bCheckPremiumIP,
			const char* szIP,
            DWORD dwIP,
			const string& strCountryCode3)
{
	m_pAccountInfo = pNewAccountInfo;
	m_pAccountPenaltyInfo = pNewAccountPenaltyInfo;

	strcpy(m_szUserID, szUserID);
	strcpy(m_szUniqueID, szUniqueID);
	strcpy(m_szCertificate, szCertificate);
	strcpy(m_szName, szName);
	m_nAge = nAge;
	m_nSex = nSex;
	m_bFreeLoginIP = bFreeLoginIP;
	m_nChecksumPack = nChecksumPack;
	m_bCheckPremiumIP = bCheckPremiumIP;
	strcpy(m_szIP, szIP);
	m_dwIP = dwIP;
	if( 3 == strCountryCode3.length() )
		m_strCountryCode3 = strCountryCode3;
	else
		m_strCountryCode3 = "Err";

	return true;
}
