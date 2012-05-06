#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "CCMatchItem.h"
#include "CCAgentObject.h"
#include "CCMatchNotify.h"
#include "Msg.h"
#include "CCMatchObjCache.h"
#include "CCMatchStage.h"
#include "CCMatchTransDataType.h"
#include "CCMatchFormula.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCMatchShop.h"
#include "CCMatchTransDataType.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "CCMatchStatus.h"
#include "CCAsyncDBJob.h"
#include "CCAsyncDBJob_WinTheClanGame.h"
#include "CCUtil.h"

// Ŭ�� ���� ���� �Լ� ////////////////////////////////////////////////////////////////////////////
void CopyClanMemberListNodeForTrans(CCTD_ClanMemberListNode* pDest, CCMatchObject* pSrcObject)
{
	pDest->uidPlayer = pSrcObject->GetUID();
	strcpy(pDest->szName, pSrcObject->GetCharInfo()->m_szName);
	pDest->nLevel = (char)pSrcObject->GetCharInfo()->m_nLevel;
	pDest->nPlace = pSrcObject->GetPlace();
	pDest->nClanGrade = pSrcObject->GetCharInfo()->m_ClanInfo.m_nGrade;
}


int CCMatchServer::ValidateCreateClan(const char* szClanName, CCMatchObject* pMasterObject, CCMatchObject** ppSponsorObject)
{
	int nResult = MOK;

	nResult = ValidateMakingName(szClanName, MIN_CLANNAME, MAX_CLANNAME);
	if (nResult != MOK)
	{
		return nResult;
	}

	// Ŭ�������� �ʿ��� �������� ���� �˻�
	if (pMasterObject->GetCharInfo()->m_nLevel < CLAN_CREATING_NEED_LEVEL)
	{
		return MERR_CLAN_CREATING_LESS_LEVEL;
	}

	// Ŭ�������� �ʿ��� �������� �ٿ�Ƽ�� ������� �˻�
	if (pMasterObject->GetCharInfo()->m_nBP < CLAN_CREATING_NEED_BOUNTY)
	{
		return MERR_CLAN_CREATING_LESS_BOUNTY;
	}


	// Ŭ�� �ߺ� �˻� - ��񿡼� ���� �˻��Ѵ�.
	int nTempCLID = 0;
	if (m_MatchDBMgr.GetClanIDFromName(szClanName, &nTempCLID))
	{
		return MERR_EXIST_CLAN;
	}

	
	for (int i = 0;i < CLAN_SPONSORS_COUNT; i++)
	{
		// Ŭ����������� Ŭ���� ������ �ʾҴ��� �˻�
		if (ppSponsorObject[i]->GetCharInfo() == NULL) return MERR_CLAN_NO_SPONSOR;
		if (ppSponsorObject[i]->GetCharInfo()->m_ClanInfo.m_nClanID != 0) return MERR_CLAN_SPONSOR_JOINED_OTHERCLAN;

		// �κ� �ִ��� Ȯ��
		if (ppSponsorObject[i]->GetPlace() != MMP_LOBBY) return MERR_CLAN_SPONSOR_NOT_LOBBY;
	}

	// Ŭ������������� �ߺ��Ǿ����� �˻�
	CCUID* tempUID = new CCUID[CLAN_SPONSORS_COUNT+1]; 

	tempUID[0] = pMasterObject->GetUID();

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		tempUID[i+1] = ppSponsorObject[i]->GetUID();
	}

	bool bExist = false;

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		for (int j = i+1; j < CLAN_SPONSORS_COUNT+1; j++)
		{
			if (tempUID[i] == tempUID[j]) 
			{
				delete [] tempUID;
				return MERR_CLAN_NO_SPONSOR;
			}
		}
	}
	delete [] tempUID;


	return MOK;
}


int ValidateJoinClan(CCMatchObject* pAdminObject, CCMatchObject* pJoinerObject, const char* szClanName)
{
	// Ŭ�� �����̻� ������� Ȯ��
	if (! IsUpperClanGrade(pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade, CCG_ADMIN))
	{
		return MERR_CLAN_NOT_MASTER_OR_ADMIN;
	}

	// �����ڰ� Ŭ���� �̹� ���ԵǾ��ִ��� Ȯ��
	if (pJoinerObject->GetCharInfo()->m_ClanInfo.IsJoined() == true)
	{
		return MERR_CLAN_JOINER_JOINED_ALREADY;
	}

	// �����ڰ� �κ� �ִ��� Ȯ��
	if (pJoinerObject->GetPlace() != MMP_LOBBY)
	{
		return MERR_CLAN_JOINER_NOT_LOBBY;
	}


	// Ŭ�� �̸��� �´��� Ȯ��
	if (stricmp(pAdminObject->GetCharInfo()->m_ClanInfo.m_szClanName, szClanName))
	{
		return MERR_CLAN_WRONG_CLANNAME;
	}
	
	return MOK;
}


int ValidateLeaveClan(CCMatchObject* pLeaverObject)
{
	// Ŭ���� �����ִ��� Ȯ��
	if (!pLeaverObject->GetCharInfo()->m_ClanInfo.IsJoined())
	{
		return MERR_CLAN_NOT_JOINED;
	}

	// �����ʹ� Ż���� �� ����
	if (pLeaverObject->GetCharInfo()->m_ClanInfo.m_nGrade == CCG_MASTER)
	{
		return MERR_CLAN_CANNOT_LEAVE;
	}

	return MOK;
}

bool IsSameClan(CCMatchObject* pSrcObject, CCMatchObject* pTarObject)
{
	if ((pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID == 0) || (pTarObject->GetCharInfo()->m_ClanInfo.m_nClanID == 0) ||
	   (pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID != pTarObject->GetCharInfo()->m_ClanInfo.m_nClanID))
	{
		return false;
	}

	return true;
}

void CCMatchServer::UpdateCharClanInfo(CCMatchObject* pObject, const int nCLID, const char* szClanName, const CCMatchClanGrade nGrade)
{
	if (! IsEnabledObject(pObject)) return;

	bool bHasJoined = pObject->GetCharInfo()->m_ClanInfo.IsJoined();


	// m_ClanMap�� Join, Leave�� ���⼭ ���ش�.
	if ((bHasJoined) && (nCLID == 0))
	{
		m_ClanMap.RemoveObject(pObject->GetUID(), pObject);
	}

	// object�� ���� ����
	strcpy(pObject->GetCharInfo()->m_ClanInfo.m_szClanName, szClanName);
	pObject->GetCharInfo()->m_ClanInfo.m_nGrade = nGrade;
	pObject->GetCharInfo()->m_ClanInfo.m_nClanID = nCLID;


	if ((pObject->GetCharInfo()->m_ClanInfo.IsJoined()) && (!bHasJoined))
	{
		m_ClanMap.AddObject(pObject->GetUID(), pObject);

		// �ӽ��ڵ�... �߸��� Ŭ��ID �´ٸ� üũ�Ͽ� �������...20090224 by kammir
		if(pObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
			LOG(LOG_FILE, "[UpdateCharClanInfo()] %s's ClanID:%d.", pObject->GetAccountName(), pObject->GetCharInfo()->m_ClanInfo.GetClanID());
	}


	// route���� ���⼭ ���ش�.
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_UPDATE_CHAR_CLANINFO, CCUID(0,0));
	void* pClanInfoArray = CCMakeBlobArray(sizeof(CCTD_CharClanInfo), 1);

	CCTD_CharClanInfo* pClanInfo = (CCTD_CharClanInfo*)CCGetBlobArrayElement(pClanInfoArray, 0);
	
	strcpy(pClanInfo->szClanName, szClanName);
	pClanInfo->nGrade = nGrade;

	
	pNewCmd->AddParameter(new CCCommandParameterBlob(pClanInfoArray, CCGetBlobArraySize(pClanInfoArray)));
	CCEraseBlobArray(pClanInfoArray);

	RouteToListener(pObject, pNewCmd);

}





///////////////////////////////////////////////////////////////////////////////////////////////////
CCMatchClan* CCMatchServer::FindClan(const int nCLID)
{
	CCMatchClanMap::iterator i = m_ClanMap.find(nCLID);
	if(i==m_ClanMap.end()) return NULL;

	CCMatchClan* pClan = (*i).second;
	return pClan;
}

void CCMatchServer::OnClanRequestCreateClan(const CCUID& uidPlayer, const int nRequestID, const char* szClanName, 
					char** szSponsorNames)
{
	CCMatchObject* pMasterObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pMasterObject)) return;

	CCMatchObject* pSponsorObjects[CLAN_SPONSORS_COUNT];

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		pSponsorObjects[i] = GetPlayerByName(szSponsorNames[i]);

		// Ŭ����������� �Ѹ��̶� �������� ������ �ȵȴ�
		if (pSponsorObjects[i] == NULL)
		{
			// �޼��� �����ְ� ��.
			CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN, CCUID(0,0));
			pNewCmd->AddParameter(new CCCommandParameterInt(MERR_CLAN_NO_SPONSOR));
			pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
			RouteToListener(pMasterObject, pNewCmd);

			return;
		}
	}

	
	// �����ܿ��� Ŭ���� ������ �� �ִ��� �˻��Ѵ�.
	int nRet = ValidateCreateClan(szClanName, pMasterObject, pSponsorObjects);

	if (nRet != MOK)
	{
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRet));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
		RouteToListener(pMasterObject, pNewCmd);
		return;
	}

	// Ŭ����������� ��û���� ����� �ִ��� ���캻��
	bool bCheckRejectInvite = false;
	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		if (pSponsorObjects[i]->CheckUserOption(MBITFLAG_USEROPTION_REJECT_INVITE) == true) {
			bCheckRejectInvite = true;
			NotifyMessage(pSponsorObjects[i]->GetUID(), MATCHNOTIFY_USER_INVITE_IGNORED);
		}
	}
	if (bCheckRejectInvite == true) {
		// �޼��� �����ְ� ��.
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_NO_SPONSOR);
		NotifyMessage(pMasterObject->GetUID(), MATCHNOTIFY_USER_INVITE_REJECTED);
		return;
	}

	// Ŭ������������� ���Ǹ� �����.
	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		// �޼��� ������� ��
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ASK_SPONSOR_AGREEMENT, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
		pNewCmd->AddParameter(new CCCommandParameterString((char*)szClanName));
		pNewCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
		pNewCmd->AddParameter(new CCCommandParameterString(pMasterObject->GetCharInfo()->m_szName));

		RouteToListener(pSponsorObjects[i], pNewCmd);
	}


	CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterInt(nRet));
	pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
	RouteToListener(pMasterObject, pNewCmd);
}



void CCMatchServer::OnClanAnswerSponsorAgreement(const int nRequestID, const CCUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer)
{
	CCMatchObject* pClanMasterObject = GetObject(uidClanMaster);
	if (! IsEnabledObject(pClanMasterObject)) return;

	
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterInt(nRequestID));
	pNewCmd->AddParameter(new CCCommandParameterUID(uidClanMaster));
	pNewCmd->AddParameter(new CCCommandParameterString(szSponsorCharName));
	pNewCmd->AddParameter(new CCCommandParameterBool(bAnswer));

	RouteToListener(pClanMasterObject, pNewCmd);	
}

void CCMatchServer::OnClanRequestAgreedCreateClan(const CCUID& uidPlayer, const char* szClanName, char** szSponsorNames)
{
	CCMatchObject* pMasterObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pMasterObject)) return;

	CCMatchObject* pSponsorObjects[CLAN_SPONSORS_COUNT];

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		pSponsorObjects[i] = GetPlayerByName(szSponsorNames[i]);

		// Ŭ����������� �Ѹ��̶� �������� ������ �ȵȴ�
		if (! IsEnabledObject(pSponsorObjects[i]))
		{
			// �޼��� �����ְ� ��.
			RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_NO_SPONSOR);

			return;
		}
	}
	
	// �����ܿ��� Ŭ���� ������ �� �ִ��� �˻��Ѵ�.
	int nRet = ValidateCreateClan(szClanName, pMasterObject, pSponsorObjects);

	if (nRet != MOK)
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, nRet);
		return;
	}

	int nMasterCID = 0;
	int nMemberCID[CLAN_SPONSORS_COUNT] = {0, };

	nMasterCID = pMasterObject->GetCharInfo()->m_nCID;
	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		nMemberCID[i] = pSponsorObjects[i]->GetCharInfo()->m_nCID;
	}

	int nNewCLID = 0;

	// ������ ��� �ִ´�.
	if (CLAN_SPONSORS_COUNT == 4)
	{
		CCAsyncDBJob_CreateClan* pNewJob = new CCAsyncDBJob_CreateClan(uidPlayer);
		pNewJob->Input(szClanName, 
					   nMasterCID, 
					   nMemberCID[0], 
					   nMemberCID[1], 
					   nMemberCID[2], 
					   nMemberCID[3],
					   pMasterObject->GetUID(),
					   pSponsorObjects[0]->GetUID(),
					   pSponsorObjects[1]->GetUID(),
					   pSponsorObjects[2]->GetUID(),
					   pSponsorObjects[3]->GetUID());
		// PostAsyncJob(pNewJob);
		pMasterObject->m_DBJobQ.DBJobQ.push_back( pNewJob );
	}
	else
	{
		_ASSERT(0);
	}
}

void CCMatchServer::OnClanRequestCloseClan(const CCUID& uidClanMaster, const char* szClanName)
{
	ResponseCloseClan(uidClanMaster, szClanName);
}

void CCMatchServer::ResponseCloseClan(const CCUID& uidClanMaster, const char* szClanName)
{
	CCMatchObject* pMasterObject = GetObject(uidClanMaster);
	if (! IsEnabledObject(pMasterObject)) return;

	// Ŭ������������ Ȯ��
	if (pMasterObject->GetCharInfo()->m_ClanInfo.m_nGrade != CCG_MASTER)
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MERR_CLAN_NOT_MASTER);
		return;
	}

	// Ŭ�� �̸��� ����� �Ǿ����� Ȯ��
	if (stricmp(pMasterObject->GetCharInfo()->m_ClanInfo.m_szClanName, szClanName))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MERR_CLAN_WRONG_CLANNAME);
		return;
	}

	// ������ ��񿡼� ��� ����
	if (!m_MatchDBMgr.ReserveCloseClan(pMasterObject->GetCharInfo()->m_ClanInfo.m_nClanID,
										pMasterObject->GetCharInfo()->m_ClanInfo.m_szClanName,
										pMasterObject->GetCharInfo()->m_nCID,
										MGetStrLocalTime(DAY_OF_DELETE_CLAN, 0, 0, MDT_YMD)))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MERR_CLAN_CANNOT_CLOSE);
		return;
	}


	// ��⿹��Ǿ��ٴ� �޼����� ������.
	RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MOK);
}

void CCMatchServer::OnClanRequestJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ResponseJoinClan(uidClanAdmin, szClanName, szJoiner);
}


void CCMatchServer::ResponseJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	CCMatchObject* pAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pAdminObject)) return;

	CCMatchObject* pJoinerObject = GetPlayerByName(szJoiner);
	if (! IsEnabledObject(pJoinerObject))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_JOIN_CLAN, MERR_CLAN_OFFLINE_TARGET);
		return;
	}

	// ����� ��û�ź� �����̸� ��û���Ѵ�
	if (pJoinerObject->CheckUserOption(MBITFLAG_USEROPTION_REJECT_INVITE) == true) {
		NotifyMessage(pAdminObject->GetUID(), MATCHNOTIFY_USER_INVITE_REJECTED);
		return;
	}

	int nRet = ValidateJoinClan(pAdminObject, pJoinerObject, szClanName);
	if (nRet != MOK)
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_JOIN_CLAN, nRet);
		return;
	}

	// �����ڿ��� ���Ǹ� ���´�.
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ASK_JOIN_AGREEMENT, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterString((char*)szClanName));
	pNewCmd->AddParameter(new CCCommandParameterUID(uidClanAdmin));
	pNewCmd->AddParameter(new CCCommandParameterString(pAdminObject->GetCharInfo()->m_szName));
	RouteToListener(pJoinerObject, pNewCmd);


	RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_JOIN_CLAN, MOK);
}

void CCMatchServer::OnClanAnswerJoinAgreement(const CCUID& uidClanAdmin, const char* szJoiner, const bool bAnswer)
{
	CCMatchObject* pClanAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pClanAdminObject)) return;

	
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterUID(uidClanAdmin));
	pNewCmd->AddParameter(new CCCommandParameterString((char*)szJoiner));
	pNewCmd->AddParameter(new CCCommandParameterBool(bAnswer));

	RouteToListener(pClanAdminObject, pNewCmd);	
}

void CCMatchServer::OnClanRequestAgreedJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ResponseAgreedJoinClan(uidClanAdmin, szClanName, szJoiner);
}

void CCMatchServer::ResponseAgreedJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	CCMatchObject* pAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pAdminObject)) return;

	CCMatchObject* pJoinerObject = GetPlayerByName(szJoiner);
	if (! IsEnabledObject(pJoinerObject)) return;


	int nRet = ValidateJoinClan(pAdminObject, pJoinerObject, szClanName);
	if (nRet != MOK)
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, nRet);
		RouteResponseToListener(pJoinerObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, nRet);
		return;
	}


	int nCLID = pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nJoinerCID = pJoinerObject->GetCharInfo()->m_nCID;
	int nClanGrade = (int)CCG_MEMBER;

	bool bDBRet = false;

	// ���� ���󿡼� ����ó��
	if (!m_MatchDBMgr.AddClanMember(nCLID, nJoinerCID, nClanGrade, &bDBRet))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_DONT_JOINED);
		RouteResponseToListener(pJoinerObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_DONT_JOINED);
		return;
	}

	// �ο��� �ʰ��Ǹ� db return ���� false�̴�.
	if (!bDBRet)
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_MEMBER_FULL);
		RouteResponseToListener(pJoinerObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_MEMBER_FULL);
		return;
	}

	// Ŭ������ ������Ʈ�ϰ� Route����
	UpdateCharClanInfo(pJoinerObject, pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID, szClanName, CCG_MEMBER);
	// �ӽ��ڵ�... �߸��� CCMatchObject*�� �´ٸ� üũ�Ͽ� �������...20090224 by kammir
	if(pAdminObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
		LOG(LOG_FILE, "[ResponseAgreedJoinClan()] %s's ClanID:%d.", pAdminObject->GetAccountName(), pAdminObject->GetCharInfo()->m_ClanInfo.GetClanID());




	RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MOK);
	RouteResponseToListener(pJoinerObject, MC_MATCH_RESPONSE_RESULT, MRESULT_CLAN_JOINED);
}



void CCMatchServer::OnClanRequestLeaveClan(const CCUID& uidPlayer)
{
	ResponseLeaveClan(uidPlayer);
}

void CCMatchServer::ResponseLeaveClan(const CCUID& uidPlayer)
{
	CCMatchObject* pLeaverObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pLeaverObject)) return;

	int nRet = ValidateLeaveClan(pLeaverObject);

	if (nRet != MOK)
	{
		RouteResponseToListener(pLeaverObject, MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN, nRet);
		return;
	}


	int nCLID = pLeaverObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nLeaverCID = pLeaverObject->GetCharInfo()->m_nCID;

	// ������ ���󿡼� Ż��ó��
	if (!m_MatchDBMgr.RemoveClanMember(nCLID, nLeaverCID))
	{
		RouteResponseToListener(pLeaverObject, MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN, MERR_CLAN_CANNOT_LEAVE);
		return;
	}

	// Ŭ������ ������Ʈ�ϰ� Route����
	UpdateCharClanInfo(pLeaverObject, 0, "", CCG_NONE);


	RouteResponseToListener(pLeaverObject, MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN, MOK);
}

void CCMatchServer::OnClanRequestChangeClanGrade(const CCUID& uidClanMaster, const char* szMember, int nClanGrade)
{
	ResponseChangeClanGrade(uidClanMaster, szMember, nClanGrade);
}

int ValidateChangeClanGrade(CCMatchObject* pMasterObject, CCMatchObject* pTargetObject, int nClanGrade)
{
	// ���������� Ȯ��
	if (pMasterObject->GetCharInfo()->m_ClanInfo.m_nGrade != CCG_MASTER)
	{
		return MERR_CLAN_NOT_MASTER;
	}

	// ���� Ŭ������ Ȯ��
	if (!IsSameClan(pMasterObject, pTargetObject))
	{
		return MERR_CLAN_OTHER_CLAN;
	}


	// �����͸� ���Ѻ����� �� ����.
	if (pTargetObject->GetCharInfo()->m_ClanInfo.m_nGrade == CCG_MASTER)
	{
		return MERR_CLAN_NOT_MASTER;
	}

	CCMatchClanGrade grade = (CCMatchClanGrade)nClanGrade;
	if ((grade != CCG_ADMIN) && (grade != CCG_MEMBER))
	{
		return MERR_CLAN_CANNOT_CHANGE_GRADE;
	}

	return MOK;
}

void CCMatchServer::ResponseChangeClanGrade(const CCUID& uidClanMaster, const char* szMember, int nClanGrade)
{
	CCMatchObject* pMasterObject = GetObject(uidClanMaster);
	if (! IsEnabledObject(pMasterObject)) return;

	CCMatchObject* pTargetObject = GetPlayerByName(szMember);
	if (! IsEnabledObject(pTargetObject))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MERR_CLAN_OFFLINE_TARGET);
		return;
	}

	// ���� ���� �������� üũ
	int nRet = ValidateChangeClanGrade(pMasterObject, pTargetObject, nClanGrade);
	if (nRet != MOK)
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, nRet);
		return;
	}

	int nCLID = pMasterObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nMemberCID = pTargetObject->GetCharInfo()->m_nCID;
	
	// ������ ���󿡼� ���� ����
	if (!m_MatchDBMgr.UpdateClanGrade(nCLID, nMemberCID, nClanGrade))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MERR_CLAN_CANNOT_CHANGE_GRADE);
		return;
	}

	// Ŭ������ ������Ʈ�ϰ� Route����
	UpdateCharClanInfo(pTargetObject, pTargetObject->GetCharInfo()->m_ClanInfo.m_nClanID, 
						pTargetObject->GetCharInfo()->m_ClanInfo.m_szClanName, (CCMatchClanGrade)nClanGrade);
	// �ӽ��ڵ�... �߸��� CCMatchObject*�� �´ٸ� üũ�Ͽ� �������...20090224 by kammir
	if(pTargetObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
		LOG(LOG_FILE, "[ResponseChangeClanGrade()] %s's ClanID:%d.", pTargetObject->GetAccountName(), pTargetObject->GetCharInfo()->m_ClanInfo.GetClanID());


	RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MOK);
}


void CCMatchServer::OnClanRequestExpelMember(const CCUID& uidClanAdmin, const char* szMember)
{
	ResponseExpelMember(uidClanAdmin, szMember);
}

void CCMatchServer::ResponseExpelMember(const CCUID& uidClanAdmin, const char* szMember)
{
	CCMatchObject* pAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pAdminObject)) return;

	// Ż��ó���� �� �ִ� �������� �˻�
	if (!IsUpperClanGrade(pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade, CCG_ADMIN))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_NOT_MASTER_OR_ADMIN);
		return;
	}

	int nCLID = pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nClanGrade = pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade;

	CCAsyncDBJob_ExpelClanMember* pNewJob = new CCAsyncDBJob_ExpelClanMember(uidClanAdmin);
	pNewJob->Input(uidClanAdmin,
					nCLID,
					nClanGrade,
					szMember);
	// PostAsyncJob(pNewJob);
	pAdminObject->m_DBJobQ.DBJobQ.push_back( pNewJob );

/*
	// ���󿡼� Ż��ó��
	int nDBRet = 0;
	int nCLID = pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nClanGrade = pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade;
	char szTarMember[256]; 
	sprintf(szTarMember, szMember);

	// ������ ���󿡼� ���� ����
	if (!m_MatchDBMgr.ExpelClanMember(nCLID, nClanGrade, szTarMember, &nDBRet))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
		return;
	}


	switch (nDBRet)
	{
	case CCMatchDBMgr::ER_NO_MEMBER:
		{
			RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
			return;
		}
		break;
	case CCMatchDBMgr::ER_WRONG_GRADE:
		{
			RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_CHANGE_GRADE);
			return;
		}
		break;
	}


	// ���� ����ڰ� ������������ Ŭ��Ż��Ǿ��ٰ� �˷�����Ѵ�.
	CCMatchObject* pMemberObject = GetPlayerByName(szMember);
	if (IsEnabledObject(pMemberObject))
	{
		UpdateCharClanInfo(pMemberObject, 0, "", CCG_NONE);
	}


	RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MOK);
*/
}


void CCMatchServer::OnClanRequestMsg(const CCUID& uidSender, const char* szMsg)
{
	CCMatchObject* pSenderObject = GetObject(uidSender);
	if (! IsEnabledObject(pSenderObject)) return;
	
	if (!pSenderObject->GetCharInfo()->m_ClanInfo.IsJoined()) return;

	CCCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_MSG, CCUID(0,0));

	char szSenderName[256];
	char szTransMsg[512];
	strcpy(szSenderName, pSenderObject->GetCharInfo()->m_szName);
	strcpy(szTransMsg, szMsg);

	pNewCmd->AddParameter(new CCCommandParameterString(szSenderName));
	pNewCmd->AddParameter(new CCCommandParameterString(szTransMsg));


	int nCLID = pSenderObject->GetCharInfo()->m_ClanInfo.m_nClanID;

	RouteToClan(nCLID, pNewCmd);	
}

//void CCMatchServer::OnClanRequestMemberList(const CCUID& uidChar)
//{
//	ResponseClanMemberList(uidChar);
//}

void CCMatchServer::OnClanRequestMemberList(const CCUID& uidChar)
{
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidChar);
	if (! IsEnabledObject(pObj)) return;

	CCMatchClan* pClan = FindClan(pObj->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan == NULL) return;

	CCRefreshClientClanMemberImpl* pImpl = pObj->GetRefreshClientClanMemberImplement();
	pImpl->SetCategory(0);
	pImpl->SetChecksum(0);
	pImpl->Enable(true);
	pClan->SyncPlayerList(pObj, 0);
}

void CCMatchServer::ResponseClanMemberList(const CCUID& uidChar)
{
	CCMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	if (!pObject->GetCharInfo()->m_ClanInfo.IsJoined()) return;

	CCMatchClan* pClan = FindClan(pObject->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan == NULL) return;

	int nNodeCount = pClan->GetMemberCount();
	if (nNodeCount <= 0) return;

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CLAN_RESPONSE_MEMBER_LIST), CCUID(0,0), m_This);

	void* pMemberArray = CCMakeBlobArray(sizeof(CCTD_ClanMemberListNode), nNodeCount);

	int nArrayIndex=0;
	for (CCUIDRefCache::iterator itor= pClan->GetMemberBegin(); itor != pClan->GetMemberEnd(); ++itor) 
	{
		CCMatchObject* pScanObj = (CCMatchObject*)(*itor).second;

		CCTD_ClanMemberListNode* pNode = (CCTD_ClanMemberListNode*)CCGetBlobArrayElement(pMemberArray, nArrayIndex++);

		if (IsEnabledObject(pScanObj))
		{
			CopyClanMemberListNodeForTrans(pNode, pScanObj);		
		}

		if (nArrayIndex >= nNodeCount) break;
	}

	pNew->AddParameter(new CCCommandParameterBlob(pMemberArray, CCGetBlobArraySize(pMemberArray)));
	CCEraseBlobArray(pMemberArray);
	RouteToListener(pObject, pNew);
}

void CopyClanInfoForTrans(CCTD_ClanInfo* pDest, CCMatchClan* pClan)
{
	strcpy(pDest->szClanName, pClan->GetName());
	strcpy(pDest->szMaster, pClan->GetClanInfoEx()->szMaster);
	pDest->nLevel = pClan->GetClanInfoEx()->nLevel;
	pDest->nTotalPoint = pClan->GetClanInfoEx()->nTotalPoint;
	pDest->nRanking = pClan->GetClanInfoEx()->nRanking;
	pDest->nWins = pClan->GetClanInfoEx()->nWins;
	pDest->nLosses = pClan->GetClanInfoEx()->nLosses;
	pDest->nPoint = pClan->GetClanInfoEx()->nPoint;
	pDest->nTotalMemberCount = pClan->GetClanInfoEx()->nTotalMemberCount;
	pDest->nConnedMember = pClan->GetMemberCount();
	pDest->nCLID = pClan->GetCLID();
	pDest->nEmblemChecksum = pClan->GetEmblemChecksum();
}

void CCMatchServer::OnClanRequestClanInfo(const CCUID& uidChar, const char* szClanName)
{
	CCMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	CCMatchClan* pClan = m_ClanMap.GetClan(szClanName);
	if ((pClan == NULL) || (!pClan->IsInitedClanInfoEx())) return;

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CLAN_RESPONSE_CLAN_INFO), CCUID(0,0), m_This);

	void* pClanInfoArray = CCMakeBlobArray(sizeof(CCTD_ClanInfo), 1);
	CCTD_ClanInfo* pClanInfo = (CCTD_ClanInfo*)CCGetBlobArrayElement(pClanInfoArray, 0);
	CopyClanInfoForTrans(pClanInfo, pClan);

	pNew->AddParameter(new CCCommandParameterBlob(pClanInfoArray, CCGetBlobArraySize(pClanInfoArray)));
	CCEraseBlobArray(pClanInfoArray);
	RouteToListener(pObject, pNew);
}

void CCMatchServer::OnClanRequestEmblemURL(const CCUID& uidChar, void* pEmblemURLListBlob)
{
	CCMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	int nClanURLCount = CCGetBlobArrayCount(pEmblemURLListBlob);
	if (nClanURLCount < 1) return;

	for (int i = 0; i < nClanURLCount; i++)
	{
		int* pClanID = (int*)CCGetBlobArrayElement(pEmblemURLListBlob, i);
		CCMatchClan* pClan = m_ClanMap.GetClan(*pClanID);
		if (pClan == NULL) continue;

		CCCommand* pNew = CreateCommand(MC_MATCH_CLAN_RESPONSE_EMBLEMURL, CCUID(0,0));
		pNew->AddParameter(new MCmdParamInt(pClan->GetCLID()));
		pNew->AddParameter(new MCmdParamInt(pClan->GetEmblemChecksum()));
		pNew->AddParameter(new MCmdParamStr(pClan->GetEmblemURL()));
		RouteToListener(pObject, pNew);
	}
}

void CCMatchServer::StandbyClanList(const CCUID& uidPlayer, int nClanListStartIndex, bool bCacheUpdate)
{
	CCMatchObject* pObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pObject)) return;

	int nPrevClanListCount = -1, nNextClanListCount = -1;
	int nGroupCount = (int)GetLadderMgr()->GetGroupCount();
	if (nClanListStartIndex < 0) nClanListStartIndex = 0;
	if (nClanListStartIndex > nGroupCount) nClanListStartIndex = nGroupCount;


	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CLAN_STANDBY_CLAN_LIST), CCUID(0,0), m_This);


	nPrevClanListCount = nClanListStartIndex;
	if ((nPrevClanListCount < 0) || (nPrevClanListCount > nGroupCount)) nPrevClanListCount = 0;
	nNextClanListCount = nGroupCount - (nClanListStartIndex + MAX_LADDER_TEAM_MEMBER);
	if ((nNextClanListCount < 0) || (nNextClanListCount > nGroupCount)) nNextClanListCount = 0;

	pNew->AddParameter(new CCCommandParameterInt(nPrevClanListCount));
	pNew->AddParameter(new CCCommandParameterInt(nNextClanListCount));


	int nRealCount = max(0, min(MAX_LADDER_TEAM_MEMBER, (nGroupCount - nClanListStartIndex)));

	list<MLadderGroup*>::iterator itorGroup = GetLadderMgr()->GetGroupListBegin();
	for (int i = 0; i < nClanListStartIndex; i++)
	{
		if (itorGroup == GetLadderMgr()->GetGroupListEnd()) break;
		itorGroup++;
	}

	int nArrayIndex=0;
	void* pClanListArray = CCMakeBlobArray(sizeof(CCTD_StandbyClanList), nRealCount);

	for (int i = 0; i < nRealCount; i++)
	{
		if (itorGroup == GetLadderMgr()->GetGroupListEnd()) break;

		CCTD_StandbyClanList* pNode = (CCTD_StandbyClanList*)CCGetBlobArrayElement(pClanListArray, i);
		memset(pNode, 0, sizeof(CCTD_StandbyClanList));

		MLadderGroup* pLadderGroup = *itorGroup;
		
		if (pLadderGroup->GetPlayerCount() > 0)
		{
			CCUID uidMember = *pLadderGroup->GetPlayerListBegin();
			CCMatchObject* pMember = GetObject(uidMember);
			if ((IsEnabledObject(pMember)) && (pMember->GetCharInfo()->m_ClanInfo.IsJoined()))
			{
				strcpy(pNode->szClanName, pMember->GetCharInfo()->m_ClanInfo.m_szClanName);
			}

			pNode->nPlayers = (int)pLadderGroup->GetPlayerCount();
			pNode->nCLID = pLadderGroup->GetCLID();

			CCMatchClan* pClan = FindClan(pLadderGroup->GetCLID());
			if (pClan)
				pNode->nEmblemChecksum = pClan->GetEmblemChecksum();
			else
				pNode->nEmblemChecksum = 0;
		}
		itorGroup++;
	}

	pNew->AddParameter(new CCCommandParameterBlob(pClanListArray, CCGetBlobArraySize(pClanListArray)));
	CCEraseBlobArray(pClanListArray);
	
	RouteToListener(pObject, pNew);	
}


void CCMatchServer::SaveClanPoint(CCMatchClan* pWinnerClan, CCMatchClan* pLoserClan, const bool bIsDrawGame,
								 const int nRoundWins, const int nRoundLosses, const int nMapID, const int nGameType,
								 const int nOneTeamMemberCount, list<CCUID>& WinnerObjUIDs,
								 const char* szWinnerMemberNames, const char* szLoserMemberNames,
								 float fPointRatio)
{
	if (bIsDrawGame) return;
	if ((!pWinnerClan) || (!pLoserClan)) return;

	int nAddedWinnerPoint = 0, nAddedLoserPoint = 0;

	int nWinnerClanPoint = pWinnerClan->GetClanInfoEx()->nPoint;
	int nLoserClanPoint = pLoserClan->GetClanInfoEx()->nPoint;

	int nWinnerCLID = pWinnerClan->GetCLID();
	int nLoserCLID = pLoserClan->GetCLID();

	int nPoint = CCMatchFormula::GetClanBattlePoint(nWinnerClanPoint, nLoserClanPoint, nOneTeamMemberCount);

	nAddedWinnerPoint = nPoint;

	if (nOneTeamMemberCount == CCLADDERTYPE_NORMAL_4VS4) // 4v4���� ����Ʈ�� �ι�
		nAddedWinnerPoint = nAddedWinnerPoint * 2;	
	else if (nOneTeamMemberCount == CCLADDERTYPE_NORMAL_3VS3) 
		nAddedWinnerPoint = (int)(nAddedWinnerPoint * 1.5f);	

	nAddedWinnerPoint = int(nAddedWinnerPoint * fPointRatio);

	nAddedLoserPoint = -(nPoint / 2);


	if (pWinnerClan)
	{
		pWinnerClan->IncWins(1);
		pWinnerClan->IncPoint(nAddedWinnerPoint);
		pWinnerClan->InsertMatchedClanID(nLoserCLID);
	}

	if (pLoserClan)
	{
		pLoserClan->IncLosses(1);
		pLoserClan->IncPoint(nAddedLoserPoint);
		pLoserClan->InsertMatchedClanID(nWinnerCLID);
	}

	CCAsyncDBJob_WinTheClanGame* pNewJob = new CCAsyncDBJob_WinTheClanGame();
	pNewJob->Input(nWinnerCLID, 
				   nLoserCLID, 
				   bIsDrawGame,
                   nAddedWinnerPoint, 
				   nAddedLoserPoint,
                   pWinnerClan->GetName(), 
				   pLoserClan->GetName(),
                   nRoundWins, 
				   nRoundLosses, 
				   nMapID, 
				   nGameType, 
				   szWinnerMemberNames, 
				   szLoserMemberNames );
	PostAsyncJob(pNewJob);


	// ĳ������ Ŭ�� �⿩�� ������Ʈ
	for (list<CCUID>::iterator itor=WinnerObjUIDs.begin(); itor!=WinnerObjUIDs.end(); itor++) 
	{
		CCUID uidObject = (*itor);

		CCMatchObject* pObject = GetObject(uidObject);
		if (IsEnabledObject(pObject))
		{
			int nCID = pObject->GetCharInfo()->m_nCID;
			pObject->GetCharInfo()->m_ClanInfo.m_nContPoint += nAddedWinnerPoint;

			CCAsyncDBJob_UpdateCharClanContPoint* pJob=new CCAsyncDBJob_UpdateCharClanContPoint(nCID, nWinnerCLID, nAddedWinnerPoint);
			PostAsyncJob(pJob);
		}
	}

	
}



