#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "Msg.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "CCAsyncDBJob.h"
#include "CCAsyncDBJob_GetLoginInfo.h"
#include "CCAsyncDBJob_InsertConnLog.h"
#include "RTypes.h"
#include "CCMatchUtil.h"
#include <winbase.h>
#include "CCMatchPremiumIPCache.h"
#include "CCCommandBuilder.h"
#include "CCMatchStatus.h"
#include "CCMatchLocale.h"

bool CCMatchServer::CheckOnLoginPre(const CCUID& CommUID, int nCmdVersion, bool& outbFreeIP, string& strCountryCode3)
{
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(CommUID);
	if (pCommObj == NULL) return false;

	// �������� ���� üũ
	if (nCmdVersion != MCOMMAND_VERSION)
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_COMMAND_INVALID_VERSION);
		Post(pCmd);	
		return false;
	}

	// free login ip�� �˻��ϱ����� debug������ debug ip�� �˻��Ѵ�.
	// ������ debugŸ������ �˻�.
	if( MGetServerConfig()->IsDebugServer() && MGetServerConfig()->IsDebugLoginIPList(pCommObj->GetIPString()) )
	{
		outbFreeIP = true;
		return true;
	}

	// �ִ��ο� üũ
	bool bFreeLoginIP = false;
	if (MGetServerConfig()->CheckFreeLoginIPList(pCommObj->GetIPString()) == true) {
		bFreeLoginIP = true;
		outbFreeIP = true;
		return true;
	} else {
		outbFreeIP = false;

		if ((int)m_Objects.size() >= MGetServerConfig()->GetMaxUser())
		{
			CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_CLIENT_FULL_PLAYERS);
			Post(pCmd);	
			return false;
		}
	}

	// ������ ���Ƴ��� ������ IP�ΰ�
	if( CheckIsValidIP(CommUID, pCommObj->GetIPString(), strCountryCode3, MGetServerConfig()->IsUseFilter()) )
		IncreaseNonBlockCount();
	else
	{
		IncreaseBlockCount();

		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_FAILED_BLOCK_IP);
		Post(pCmd);	
		return false;
	}

	return true;
}

void CCMatchServer::OnMatchLogin(CCUID CommUID, const char* szUserID, const char* szPassword, int nCommandVersion, unsigned long nChecksumPack, char *szEncryptMd5Value)
{
//	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(CommUID);
//	if (pCommObj == NULL) return;

	// �ʱ� ��ġ�� ���� �˻��ؼ� ����.
	int nMapID = 0;

	unsigned int nAID = 0;
	char szDBPassword[32] = "";
	string strCountryCode3;

	bool bFreeLoginIP = false;

	// ��������, �ִ��ο� üũ
	if (!CheckOnLoginPre(CommUID, nCommandVersion, bFreeLoginIP, strCountryCode3)) return;


	// ���� ������ �ݸ��� �����Ƿ� �ش� ������ ������ ���� �����Ѵ�. 
	if (!m_MatchDBMgr.GetLoginInfo(szUserID, &nAID, szDBPassword))
	{
#ifdef _DEBUG
		m_MatchDBMgr.CreateAccount(szUserID, szPassword, 0, szUserID, 20, 1);
		strcpy(szDBPassword, szPassword);

		m_MatchDBMgr.GetLoginInfo(szUserID, &nAID, szDBPassword);
#endif

		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_CLIENT_WRONG_PASSWORD);
		Post(pCmd);	

		return;
	}


	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(CommUID);
	if (pCommObj)
	{
		// ��� ���� ���ӽð��� ������Ʈ �Ѵ�.
		if (!m_MatchDBMgr.UpdateLastConnDate(szUserID, pCommObj->GetIPString()))
		{	
			cclog("DB Query(OnMatchLogin > UpdateLastConnDate) Failed");
		}

	}


	// �н����尡 Ʋ���� ��� ó��
	if (strcmp(szDBPassword, szPassword))
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_CLIENT_WRONG_PASSWORD);
		Post(pCmd);	

		return;
	}

	CCMatchAccountInfo accountInfo;
	if (!m_MatchDBMgr.GetAccountInfo(nAID, &accountInfo, MGetServerConfig()->GetServerID()))
	{
		// Notify Message �ʿ� -> �α��� ���� - �ذ�(Login Fail �޼��� �̿�)
		// Disconnect(CommUID);
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_FAILED_GETACCOUNTINFO);
		Post(pCmd);	
	}

	CCMatchAccountPenaltyInfo accountpenaltyInfo;
	if( !m_MatchDBMgr.GetAccountPenaltyInfo(nAID, &accountpenaltyInfo) ) 
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_FAILED_GETACCOUNTINFO);
		Post(pCmd);	
	}

#ifndef _DEBUG
	// �ߺ� �α����̸� ������ �ִ� ����� ���������.
	CCMatchObject* pCopyObj = GetPlayerByAID(accountInfo.m_nAID);
 	if (pCopyObj != NULL) 
	{
		// ���� �α����϶� �̹� �α��� ���ִ� Ŭ���̾�Ʈ�� ������ �̹� �α��� Ŭ���̾�Ʈ�� 
		// �ߺ� �α����̶� �޼��� ������ ������ ����. - by kammir 2008.09.30
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(pCopyObj->GetUID(), MERR_MULTIPLE_LOGIN);
		Post(pCmd);	
		//Disconnect(pCopyObj->GetUID());
	}
#endif

	// ������� �������� Ȯ���Ѵ�.
	if ((accountInfo.m_nUGrade == CCMUGBLOCKED) || (accountInfo.m_nUGrade == CCMUGPENALTY))
	{
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_CLIENT_CCMUGBLOCKED);
		Post(pCmd);	
		return;
	}

#ifndef _DEBUG // debug���� �������. �׽�Ʈ�� �ʿ��ϸ� ���� ������ �ؾ� ��. - by SungE 2007-05-03
	// gunz.exe ���������� ���Ἲ�� Ȯ���Ѵ�. (��ȣȭ �Ǿ� �ִ�)
	// server.ini ���Ͽ��� ������ ���� ���� ������� ������ �˻����� �ʴ´�.
	if (MGetServerConfig()->IsUseMD5())				
	{
		unsigned char szMD5Value[ MAX_MD5LENGH ] = {0, };
		pCommObj->GetCrypter()->Decrypt(szEncryptMd5Value, MAX_MD5LENGH, (CCPacketCrypterKey*)pCommObj->GetCrypter()->GetKey());
		memcpy( szMD5Value, szEncryptMd5Value, MAX_MD5LENGH );

		if ((memcmp(m_szMD5Value, szMD5Value, MAX_MD5LENGH)) != 0)
		{
			// "�������� ���������� �ƴմϴ�." �̷� ���� ��Ŷ�� ��� ���� ����
			LOG(LOG_PROG, "MD5 error : AID(%u).\n \n", accountInfo.m_nAID);
			// ���� ���������
//			Disconnect(CommUID);
			return;
		}
	}
#endif

	// �α��μ����Ͽ� ������Ʈ(CCMatchObject) ����
	AddObjectOnMatchLogin(CommUID, &accountInfo, &accountpenaltyInfo, bFreeLoginIP, strCountryCode3, nChecksumPack);

/*
	CCUID AllocUID = CommUID;
	int nErrCode = ObjectAdd(CommUID);
	if(nErrCode!=MOK){
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	CCMatchObject* pObj = GetObject(AllocUID);
	pObj->AddCommListener(CommUID);
	pObj->SetObjectType(MOT_PC);
	memcpy(pObj->GetAccountInfo(), &accountInfo, sizeof(CCMatchAccountInfo));
	pObj->SetFreeLoginIP(bFreeLoginIP);
	pObj->SetCountryCode3( strCountryCode3 );
	pObj->UpdateTickLastPacketRecved();

	if (pCommObj != NULL)
	{
		pObj->SetPeerAddr(pCommObj->GetIP(), pCommObj->GetIPString(), pCommObj->GetPort());
	}
	
	SetClientClockSynchronize(CommUID);


	// �����̾� IP�� üũ�Ѵ�.
	if (MGetServerConfig()->CheckPremiumIP())
	{
		if (pCommObj)
		{
			bool bIsPremiumIP = false;
			bool bExistPremiumIPCache = false;
			
			bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(pCommObj->GetIP(), bIsPremiumIP);

			// ���� ĳ���� ������ ���� DB���� ã���� �Ѵ�.
			if (!bExistPremiumIPCache)
			{
				if (m_MatchDBMgr.CheckPremiumIP(pCommObj->GetIPString(), bIsPremiumIP))
				{
					// ����� ĳ���� ����
					MPremiumIPCache()->AddIP(pCommObj->GetIP(), bIsPremiumIP);
				}
				else
				{
					MPremiumIPCache()->OnDBFailed();
				}

			}

			if (bIsPremiumIP) pObj->GetAccountInfo()->m_nPGrade = MMPG_PREMIUM_IP;
		}		
	}


	CCCommand* pCmd = CreateCmdMatchResponseLoginOK(CommUID, 
												   AllocUID, 
												   pObj->GetAccountInfo()->m_szUserID,
												   pObj->GetAccountInfo()->m_nUGrade,
                                                   pObj->GetAccountInfo()->m_nPGrade);
	Post(pCmd);	

	// ���� �α׸� �����.
	m_MatchDBMgr.InsertConnLog(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );

#ifndef _DEBUG
	// Client DataFile Checksum�� �˻��Ѵ�.
	unsigned long nChecksum = nChecksumPack ^ CommUID.High ^ CommUID.Low;
	if (nChecksum != GetItemFileChecksum()) {
		LOG(LOG_PROG, "Invalid ZItemChecksum(%u) , UserID(%s) ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
		Disconnect(CommUID);
	}
#endif

*/
}

/*
void CCMatchServer::OnMatchLoginFromNetmarble(const CCUID& CommUID, const char* szCPCookie, const char* szSpareData, int nCmdVersion, unsigned long nChecksumPack)
{
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(CommUID);
	if (pCommObj == NULL) return;

	bool bFreeLoginIP = false;
	string strCountryCode3;

	// ��������, �ִ��ο� üũ
	if (!CheckOnLoginPre(CommUID, nCmdVersion, bFreeLoginIP, strCountryCode3)) return;


	CCMatchAuthBuilder* pAuthBuilder = GetAuthBuilder();
	if (pAuthBuilder == NULL) {
		LOG(LOG_PROG, "Critical Error : MatchAuthBuilder is not assigned.\n");
		return;
	}
	CCMatchAuthInfo* pAuthInfo = NULL;
	if (pAuthBuilder->ParseAuthInfo(szCPCookie, &pAuthInfo) == false) 
	{
		MGetServerStatusSingleton()->SetRunStatus(5);

		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_CLIENT_WRONG_PASSWORD);
		Post(pCmd);	

		LOG(LOG_PROG, "Netmarble Certification Failed\n");
		return;
	}

	const char* pUserID = pAuthInfo->GetUserID();
	const char* pUniqueID = pAuthInfo->GetUniqueID();
	const char* pCertificate = pAuthInfo->GetCertificate();
	const char* pName = pAuthInfo->GetName();
	int nAge = pAuthInfo->GetAge();
	int nSex = pAuthInfo->GetSex();
	bool bCheckPremiumIP = MGetServerConfig()->CheckPremiumIP();
	const char* szIP = pCommObj->GetIPString();
	DWORD dwIP = pCommObj->GetIP();

	// Async DB
	CCAsyncDBJob_GetLoginInfo* pNewJob = new CCAsyncDBJob_GetLoginInfo(CommUID);
	pNewJob->Input(new CCMatchAccountInfo(), 
					pUserID, 
					pUniqueID, 
					pCertificate, 
					pName, 
					nAge, 
					nSex, 
					bFreeLoginIP, 
					nChecksumPack,
					bCheckPremiumIP,
					szIP,
					dwIP,
					strCountryCode3);
	PostAsyncJob(pNewJob);

	if (pAuthInfo)
	{
		delete pAuthInfo; pAuthInfo = NULL;
	}
}
*/

void CCMatchServer::OnMatchLoginFromNetmarbleJP(const CCUID& CommUID, const char* szLoginID, const char* szLoginPW, int nCmdVersion, unsigned long nChecksumPack)
{
	bool bFreeLoginIP = false;
	string strCountryCode3;

	// ��������, �ִ��ο� üũ
	if (!CheckOnLoginPre(CommUID, nCmdVersion, bFreeLoginIP, strCountryCode3)) return;

	// DBAgent�� ���� ������ ������ ������ �α��� ���μ����� �����Ѵ�.
	if (!MGetLocale()->PostLoginInfoToDBAgent(CommUID, szLoginID, szLoginPW, bFreeLoginIP, nChecksumPack, GetClientCount()))
	{
		cclog( "Server user full(DB agent error).\n" );
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, MERR_CLIENT_FULL_PLAYERS);
		Post(pCmd);
		return;
	}
}

void CCMatchServer::OnMatchLoginFromDBAgent(const CCUID& CommUID, const char* szLoginID, const char* szName, int nSex, bool bFreeLoginIP, unsigned long nChecksumPack)
{
#ifndef LOCALE_NHNUSA
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(CommUID);
	if (pCommObj == NULL) return;

	string strCountryCode3;
	CheckIsValidIP( CommUID, pCommObj->GetIPString(), strCountryCode3, false );

	const char* pUserID = szLoginID;
	char szPassword[16] = "";			// �н������ ����
	char szCertificate[16] = "";
	const char* pName = szName;
	int nAge = 20;

	bool bCheckPremiumIP = MGetServerConfig()->CheckPremiumIP();
	const char* szIP = pCommObj->GetIPString();
	DWORD dwIP = pCommObj->GetIP();

	// Async DB
	CCAsyncDBJob_GetLoginInfo* pNewJob = new CCAsyncDBJob_GetLoginInfo(CommUID);
	pNewJob->Input(new CCMatchAccountInfo,
					new CCMatchAccountPenaltyInfo,
					pUserID, 
					szPassword, 
					szCertificate, 
					pName, 
					nAge, 
					nSex, 
					bFreeLoginIP, 
					nChecksumPack,
					bCheckPremiumIP,
					szIP,
					dwIP,
					strCountryCode3);
	PostAsyncJob(pNewJob);
#endif
}

void CCMatchServer::OnMatchLoginFailedFromDBAgent(const CCUID& CommUID, int nResult)
{
#ifndef LOCALE_NHNUSA
	// �������� ���� üũ
	CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(CommUID, nResult);
	Post(pCmd);	
#endif
}

CCCommand* CCMatchServer::CreateCmdMatchResponseLoginOK(const CCUID& uidComm, 
													  CCUID& uidPlayer, 
													  const char* szUserID, 
													  CCMatchUserGradeID nUGradeID, 
													  CCMatchPremiumGradeID nPGradeID,
//													  const unsigned char* szRandomValue,
													  const unsigned char* pbyGuidReqMsg)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_LOGIN, uidComm);
	pCmd->AddParameter(new CCCommandParameterInt(MOK));
	pCmd->AddParameter(new CCCommandParameterString(MGetServerConfig()->GetServerName()));
	pCmd->AddParameter(new CCCommandParameterChar((char)MGetServerConfig()->GetServerMode()));
	pCmd->AddParameter(new CCCommandParameterString(szUserID));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)nUGradeID));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)nPGradeID));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new CCCommandParameterBool((bool)MGetServerConfig()->IsEnabledSurvivalMode()));
	pCmd->AddParameter(new CCCommandParameterBool((bool)MGetServerConfig()->IsEnabledDuelTournament()));
//	pCmd->AddParameter(new CCCommandParameterString(szRandomValue));

//	void* pBlob1 = CCMakeBlobArray(sizeof(unsigned char), 64);
//	unsigned char *pCmdBlock1 = (unsigned char*)CCGetBlobArrayElement(pBlob1, 0);
//	CopyMemory(pCmdBlock1, szRandomValue, 64);

//	pCmd->AddParameter(new CCCommandParameterBlob(pBlob1, CCGetBlobArraySize(pBlob1)));
//	CCEraseBlobArray(pBlob1);
	
	void* pBlob = CCMakeBlobArray(sizeof(unsigned char), SIZEOF_GUIDREQMSG);
	unsigned char* pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, pbyGuidReqMsg, SIZEOF_GUIDREQMSG);

	pCmd->AddParameter(new CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);

	return pCmd;
}

CCCommand* CCMatchServer::CreateCmdMatchResponseLoginFailed(const CCUID& uidComm, const int nResult)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_LOGIN, uidComm);
	pCmd->AddParameter(new CCCommandParameterInt(nResult));
	pCmd->AddParameter(new CCCommandParameterString(MGetServerConfig()->GetServerName()));
	pCmd->AddParameter(new CCCommandParameterChar((char)MGetServerConfig()->GetServerMode()));
	pCmd->AddParameter(new CCCommandParameterString("Ana"));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)CCMUGFREE));
	pCmd->AddParameter(new CCCommandParameterUChar((unsigned char)MMPG_FREE));
	pCmd->AddParameter(new CCCommandParameterUID(CCUID(0,0)));
	pCmd->AddParameter(new CCCommandParameterBool((bool)MGetServerConfig()->IsEnabledSurvivalMode()));
	pCmd->AddParameter(new CCCommandParameterBool((bool)MGetServerConfig()->IsEnabledDuelTournament()));
//	pCmd->AddParameter(new CCCommandParameterString("A"));
	
//	unsigned char tmp1 = 'A';
//	void* pBlob1 = CCMakeBlobArray(sizeof(unsigned char), sizeof(unsigned char));
//	unsigned char* pCmdBlock1 = (unsigned char*)CCGetBlobArrayElement(pBlob1, 0);
//	CopyMemory(pCmdBlock1, &tmp1, sizeof(unsigned char));
//	pCmd->AddParameter(new CCCommandParameterBlob(pBlob1, CCGetBlobArraySize(pBlob1)));
//	CCEraseBlobArray(pBlob1);

	unsigned char tmp = 0;
	void* pBlob = CCMakeBlobArray(sizeof(unsigned char), sizeof(unsigned char));
	unsigned char* pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, &tmp, sizeof(unsigned char));

	pCmd->AddParameter(new CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);

	return pCmd;
}


bool CCMatchServer::AddObjectOnMatchLogin(const CCUID& uidComm, 
										const CCMatchAccountInfo* pSrcAccountInfo,
										const CCMatchAccountPenaltyInfo* pSrcAccountPenaltyInfo,
										bool bFreeLoginIP, string strCountryCode3, unsigned long nChecksumPack)
{
	CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(uidComm);
	if (pCommObj == NULL) return false;

	CCUID AllocUID = uidComm;
	int nErrCode = ObjectAdd(uidComm);
	if(nErrCode!=MOK) {
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	CCMatchObject* pObj = GetObject(AllocUID);
	if (pObj == NULL) {
		// Notify Message �ʿ� -> �α��� ���� - �ذ�(Login Fail �޼��� �̿�)
		// Disconnect(uidComm);
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(AllocUID, MERR_FAILED_LOGIN_RETRY);
		Post(pCmd);	
		return false;
	}

	pObj->AddCommListener(uidComm);
	pObj->SetObjectType(MOT_PC);

	memcpy(pObj->GetAccountInfo(), pSrcAccountInfo, sizeof(CCMatchAccountInfo));
	memcpy(pObj->GetAccountPenaltyInfo(), pSrcAccountPenaltyInfo, sizeof(CCMatchAccountPenaltyInfo));
		
	pObj->SetFreeLoginIP(bFreeLoginIP);
	pObj->SetCountryCode3( strCountryCode3 );
	pObj->UpdateTickLastPacketRecved();
	pObj->UpdateLastHShieldMsgRecved();

	if (pCommObj != NULL)
	{
		pObj->SetPeerAddr(pCommObj->GetIP(), pCommObj->GetIPString(), pCommObj->GetPort());
	}
	
	SetClientClockSynchronize(uidComm);

	// �����̾� IP�� üũ�Ѵ�.
	if (MGetServerConfig()->CheckPremiumIP())
	{
		if (pCommObj)
		{
			bool bIsPremiumIP = false;
			bool bExistPremiumIPCache = false;
			
			bExistPremiumIPCache = MPremiumIPCache()->CheckPremiumIP(pCommObj->GetIP(), bIsPremiumIP);

			// ���� ĳ���� ������ ���� DB���� ã���� �Ѵ�.
			if (!bExistPremiumIPCache)
			{
				if (m_MatchDBMgr.CheckPremiumIP(pCommObj->GetIPString(), bIsPremiumIP))
				{
					// ����� ĳ���� ����
					MPremiumIPCache()->AddIP(pCommObj->GetIP(), bIsPremiumIP);
				}
				else
				{
					MPremiumIPCache()->OnDBFailed();
				}

			}

			if (bIsPremiumIP) pObj->GetAccountInfo()->m_nPGrade = MMPG_PREMIUM_IP;
		}		
	}

	if (!PreCheckAddObj(uidComm))
	{
		// ���� ���� �ʱ�ȭ ���� ������ ������ ����ٰ� �α��� ���и� �����Ѵ�. //
		CCCommand* pCmd = CreateCmdMatchResponseLoginFailed(uidComm, MERR_FAILED_AUTHENTICATION);
		Post(pCmd);	
		return false;
	}

	CCCommand* pCmd = CreateCmdMatchResponseLoginOK(uidComm, 
												   AllocUID, 
												   pObj->GetAccountInfo()->m_szUserID,
												   pObj->GetAccountInfo()->m_nUGrade,
                                                   pObj->GetAccountInfo()->m_nPGrade,
//												   pObj->GetAntiHackInfo()->m_szRandomValue,
												   pObj->GetHShieldInfo()->m_pbyGuidReqMsg);
	Post(pCmd);	

	// ���� �α׸� �����.
	//m_MatchDBMgr.InsertConnLog(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );

	// ���� �α�
	CCAsyncDBJob_InsertConnLog* pNewJob = new CCAsyncDBJob_InsertConnLog(uidComm);
	pNewJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );
	PostAsyncJob(pNewJob);

	// Client DataFile Checksum�� �˻��Ѵ�.
	// 2006.2.20 dubble. filelist checksum���� ����
	unsigned long nChecksum = nChecksumPack ^ uidComm.High ^ uidComm.Low;
	if( MGetServerConfig()->IsUseFileCrc() && !CCMatchAntiHack::CheckClientFileListCRC(nChecksum, pObj->GetUID()) && 
		!MGetServerConfig()->IsDebugLoginIPList(pObj->GetIPString()) )
	{
		LOG(LOG_PROG, "Invalid filelist crc (%u) , UserID(%s)\n ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
//		pObj->SetBadFileCRCDisconnectWaitInfo();
		pObj->DisconnectHacker( CCMHT_BADFILECRC);
	}
	/*
	if (nChecksum != GetItemFileChecksum()) {
		LOG(LOG_PROG, "Invalid ZItemChecksum(%u) , UserID(%s) ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
		Disconnect(uidComm);
		return false;
	}
	*/

	pObj->LoginCompleted();

	return true;
}
