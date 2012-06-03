#include "stdafx.h"
#include "CCAsyncDBJob_GetLoginInfo.h"
#include "CCMatchPremiumIPCache.h"
#include "CCMatchConfig.h"

void CCAsyncDBJob_GetLoginInfo::Run(void* pContext)
{
	_ASSERT(m_pAccountInfo);

	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	// ���� ������ �ݸ��� �����Ƿ� �ش� ������ ������ ���� �����Ѵ�. 
	if (!pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword))
	{
		int nGunzSex;	// �������� �������� �ݸ��� �������� �ݴ��̴�.
		if (m_nSex == 0) nGunzSex = 1; else nGunzSex = 0;

		int nCert = 0;
		if (strlen(m_szCertificate) > 0)
		{
			if (m_szCertificate[0] == '1') nCert =1;
		}

		pDBMgr->CreateAccount(m_szUserID, m_szUniqueID, nCert, m_szName, m_nAge, nGunzSex);
		pDBMgr->GetLoginInfo(m_szUserID, &m_nAID, m_szDBPassword);
	}

	// ���� ������ �д´�.
	if (!pDBMgr->GetAccountInfo(m_nAID, m_pAccountInfo, CCGetServerConfig()->GetServerID()))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	// ���� ���Ƽ ������ �д´�. - 2010-08-10 ȫ����
	if( !pDBMgr->GetAccountPenaltyInfo(m_nAID, m_pAccountPenaltyInfo) ) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	// �����̾� IP�� üũ�Ѵ�.
	if (m_bCheckPremiumIP)
	{
		bool bIsPremiumIP = false;
		bool bExistPremiumIPCache = false;
		
		bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(m_dwIP, bIsPremiumIP);

		// ���� ĳ���� ������ ���� DB���� ã���� �Ѵ�.
		if (!bExistPremiumIPCache)
		{
			if (pDBMgr->CheckPremiumIP(m_szIP, bIsPremiumIP))
			{
				// ����� ĳ���� ����
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
