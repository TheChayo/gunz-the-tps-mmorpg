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
#include "CCAsyncDBJob_FriendList.h"
#include "CCAsyncDBJob_CharFinalize.h"
#include "CCMatchUtil.h"
#include "CCMatchRuleBaseQuest.h"
#include "CCMatchLocale.h"
#include "CCMatchObject.h"

#include "CCAsyncDBJob_UpdateAccountLastLoginTime.h"
#include "CCAsyncDBJob_UpdateCharBRInfo.h"

void CCMatchServer::OnRequestAccountCharInfo(const CCUID& uidPlayer, int nCharNum)
{
    // Async DB //////////////////////////////
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCAsyncDBJob_GetAccountCharInfo* pJob=new CCAsyncDBJob_GetAccountCharInfo(uidPlayer,pObj->GetAccountInfo()->m_nAID, nCharNum);
	
	pObj->m_DBJobQ.DBJobQ.push_back( pJob );
	// PostAsyncJob(pJob);
}


void CCMatchServer::OnRequestSelectChar(const CCUID& uidPlayer, const int nCharIndex)
{
	CCMatchObject* pObj = GetObject(uidPlayer);

	if ((pObj == NULL) || (pObj->GetAccountInfo()->m_nAID < 0)) return;
	if ((nCharIndex < 0) || (nCharIndex >= MAX_CHAR_COUNT)) return;

	// ��Ŀ�̸� ������ ���´�.
#if defined(_XTRAP) || defined(_HSHIELD)
	if( (MGetServerConfig()->IsUseXTrap() || MGetServerConfig()->IsUseHShield()) && pObj->IsHacker() )
	{
#ifdef _XTRAP
		if( MGetServerConfig()->IsUseXTrap() )
		{
//			pObj->SetXTrapHackerDisconnectWaitInfo();
			pObj->DisconnectHacker( CCMHT_XTRAP_HACKER );
			return;
		}
#endif

#ifdef _HSHIELD
		if( MGetServerConfig()->IsUseHShield() )
		{
//			pObj->SetHShieldHackerDisconnectWaitInfo();
			pObj->DisconnectHacker( CCMHT_HSHIELD_HACKER );
			return;
		}
#endif
	}
#endif

	// Async DB //////////////////////////////
	CCAsyncDBJob_GetCharInfo* pJob=new CCAsyncDBJob_GetCharInfo(uidPlayer, pObj->GetAccountInfo()->m_nAID, nCharIndex);
	pJob->SetCharInfo(new CCMatchCharInfo);
	pObj->m_DBJobQ.DBJobQ.push_back( pJob );
	//pJob->SetFriendInfo(new CCMatchFriendInfo);
	// PostAsyncJob(pJob);

}


void CCMatchServer::OnRequestDeleteChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName)
{
	ResponseDeleteChar(uidPlayer, nCharIndex, szCharName);
}

bool CCMatchServer::ResponseDeleteChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetAccountInfo()->m_nAID < 0)) return false;
	if ((nCharIndex < 0) || (nCharIndex >= MAX_CHAR_COUNT)) return false;

	// �������� ��� - Post AsyncJob
	CCAsyncDBJob_DeleteChar* pJob = new CCAsyncDBJob_DeleteChar(uidPlayer, pObj->GetAccountInfo()->m_nAID, nCharIndex, szCharName);
	pObj->m_DBJobQ.DBJobQ.push_back( pJob );
	// PostAsyncJob(pJob);

	return true;
}


void CCMatchServer::OnRequestCreateChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName,
						 const unsigned int nSex, const unsigned int nHair, const unsigned int nFace, const unsigned int nCostume)
{
	CCMatchSex sex = (nSex == 0) ? MMS_MALE : MMS_FEMALE;
	ResponseCreateChar(uidPlayer, nCharIndex, szCharName, CCMatchSex(nSex), nHair, nFace, nCostume);
}


bool CCMatchServer::ResponseCreateChar(const CCUID& uidPlayer, const int nCharIndex, const char* szCharName,
						CCMatchSex nSex, const unsigned int nHair, const unsigned int nFace,	const unsigned int nCostume)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	if ((pObj->GetAccountInfo()->m_nAID < 0) ||
	   (nCharIndex < 0) || (nCharIndex >= MAX_CHAR_COUNT) || (nHair >= MAX_COSTUME_HAIR) ||
	   (nFace >= MAX_COSTUME_FACE) || (nCostume >= MAX_COSTUME_TEMPLATE)) 
	{
		int nResult = -1;	// false
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_CREATE_CHAR, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(nResult));			// result
		pNewCmd->AddParameter(new CCCommandParameterString(szCharName));		// ������� ĳ���� �̸�
		RouteToListener(pObj, pNewCmd);
		return false;
	}

	int nResult = -1;	// false

	nResult = ValidateMakingName(szCharName, MIN_CHARNAME, MAX_CHARNAME);
	if (nResult != MOK) {
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_CREATE_CHAR, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(nResult));			// result
		pNewCmd->AddParameter(new CCCommandParameterString(szCharName));		// ������� ĳ���� �̸�
		RouteToListener(pObj, pNewCmd);

		return false;
	}	

	// �������� ��� - Post AsyncJob
	CCAsyncDBJob_CreateChar* pJob = new CCAsyncDBJob_CreateChar(uidPlayer, pObj->GetAccountInfo()->m_nAID, 
											szCharName, nCharIndex, nSex, nHair, nFace, nCostume);
	

#ifdef _DEBUG
	// ���� �ּ�ó���߽��ϴ� - bird
	cclog( "Selected character name : %s (", szCharName);

	for ( int i = 0;  i < (int)strlen( szCharName);  i++)
	{
		cclog( "%02X ", szCharName[ i] & 0x00FF);
	}

	cclog( ")  (len = %d)\n", (int)strlen( szCharName));
#endif

	// PostAsyncJob(pJob);

	pObj->m_DBJobQ.DBJobQ.push_back( pJob );

	return true;
}


void CCMatchServer::OnCharClear(const CCUID& uidPlayer)
{
	// Network�̺�Ʈ������ �������� ���� �� �ִ�.
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj)
	{
		ObjectRemove(pObj->GetUID(), NULL);
	}
}

bool CCMatchServer::CharInitialize(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	CCMatchCharInfo*	pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return false;

	pCharInfo->m_nConnTime = GetTickTime();

	m_objectCommandHistory.SetCharacterInfo( uidPlayer, pCharInfo->m_szName, pCharInfo->m_nCID );

	if( !pCharInfo->EquipFromItemList() ) {
		cclog("CCMatchServer::CharInitialize - EquipFromItemList Failed(CID - %d)\n", pCharInfo->m_nCID);
		if (!m_MatchDBMgr.ClearAllEquipedItem(pCharInfo->m_nCID)) {
			cclog("CCMatchServer::CharInitialize - DB Query(ClearAllEquipedItem) Failed\n");
		}

		pCharInfo->m_EquipedItem.Clear();
	}

	CheckExpiredItems(pObj);


	// �ִ� ������ �ִ� ���Ժ��� �� ���� ��� ������ ��� ���⸦ ������Ų��.
	int nWeight, nMaxWeight;
	pCharInfo->GetTotalWeight(&nWeight, &nMaxWeight);
	if (nWeight > nMaxWeight) {
		if (!m_MatchDBMgr.ClearAllEquipedItem(pCharInfo->m_nCID)) {
			cclog("CCMatchServer::CharInitialize - DB Query(ClearAllEquipedItem) Failed\n");
		}

		pCharInfo->m_EquipedItem.Clear();
	}

	
	// Ŭ���� ���ԵǾ� ������ CCMatchMap�� ���
	if (pCharInfo->m_ClanInfo.IsJoined())
	{
		// Ŭ�������� ���Ӿ˸�
		CCCommand* pNew = CreateCommand(MC_MATCH_CLAN_MEMBER_CONNECTED, CCUID(0,0));
		pNew->AddParameter(new CCCommandParameterString((char*)pCharInfo->m_szName));
		RouteToClan(pCharInfo->m_ClanInfo.m_nClanID, pNew);


		m_ClanMap.AddObject(uidPlayer, pObj);

		// �ӽ��ڵ�... �߸��� Ŭ��ID �´ٸ� üũ�Ͽ� �������...20090224 by kammir
		if(pObj->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
            LOG(LOG_FILE, "[CharInitialize()] %s's ClanID:%d.", pObj->GetAccountName(), pObj->GetCharInfo()->m_ClanInfo.GetClanID());

		// ���� Ŭ���� ������ ��� Ŭ���� ������ �޾ƿ´�. - ���������� �׳� ������ Ŭ������ �о��
//		if (MGetServerConfig()->GetServerMode() == CSM_CLAN)
		{
			CCMatchClan* pClan = m_ClanMap.GetClan(pCharInfo->m_ClanInfo.m_nClanID);
			if (pClan)
			{
				if (!pClan->IsInitedClanInfoEx())
				{
					pClan->InitClanInfoFromDB();
				}
			}
		}
	}

	return true;
}

void CCMatchServer::CheckExpiredItems(CCMatchObject* pObj)
{
	CCMatchCharInfo*	pCharInfo = pObj->GetCharInfo();
	if( NULL == pCharInfo )
		return;

	if (!pCharInfo->m_ItemList.HasRentItem()) return;

	vector<unsigned long int> vecExpiredItemIDList;
	vector<CCUID> vecExpiredIteCCUIDList;				// ����� ������ UID
	const DWORD dwTick = GetTickTime();

	// �Ⱓ ���� �������� �ִ��� üũ�ϰ� ������ ������ �����ϰ� �����Ѵ�.
	for (CCMatchItemMap::iterator itor = pCharInfo->m_ItemList.begin(); itor != pCharInfo->m_ItemList.end(); ++itor)
	{
		CCMatchItem* pCheckItem = (*itor).second;
		if (pCheckItem->IsRentItem())
		{
			// �ν��Ͻ� �����ǰ��� ���� �ð�
			/// unsigned long int nPassTime = CCGetTimeDistance(pCheckItem->GetRentItemRegTime(), dwTick);
			// int nPassMinuteTime = nPassTime / (1000 * 60);

			// if ((pCheckItem->GetRentMinutePeriodRemainder()-nPassMinuteTime) <= 0) // �Լ� ȣ��� ����. - by SungE
			if( IsExpiredRentItem(pCheckItem, dwTick) )
			{
				// ������̸� �����.
				CCMatchCharItemParts nCheckParts = MMCIP_END;
				// if (pCharInfo->m_EquipedItem.IsEquipedItem(pCheckItem, nCheckParts))
				if (pCharInfo->m_EquipedItem.IsEquipedItem(itor->first, nCheckParts))
				{
					ResponseTakeoffItem(pObj->GetUID(), nCheckParts);
				}

				// ������ ����
				int nExpiredItemID = pCheckItem->GetDescID();
				if (nExpiredItemID != 0)
				{
					vecExpiredIteCCUIDList.push_back(pCheckItem->GetUID());
					vecExpiredItemIDList.push_back(nExpiredItemID);
				}
			}
		}
	}

	// ����� �������� ������ ����
	if (!vecExpiredItemIDList.empty())
	{
		int nExpiredIteCCUIDListCount = (int)vecExpiredIteCCUIDList.size();
		for (int i = 0; i < nExpiredIteCCUIDListCount; i++)
		{
			// ������ ���⼭ �������� �����.
			RemoveExpiredCharItem(pObj, vecExpiredIteCCUIDList[i]);
		}

		ResponseExpiredItemIDList(pObj, vecExpiredItemIDList);
	}
}

void CCMatchServer::ResponseExpiredItemIDList(CCMatchObject* pObj, vector<unsigned long int>& vecExpiredItemIDList)
{
	int nBlobSize = (int)vecExpiredItemIDList.size();
	CCCommand* pNewCmd = CreateCommand(MC_MATCH_EXPIRED_RENT_ITEM, CCUID(0,0));
	
	void* pExpiredItemIDArray = CCMakeBlobArray(sizeof(unsigned long int), nBlobSize);

	for (int i = 0; i < nBlobSize; i++)
	{
		unsigned long int *pItemID = (unsigned long int*)CCGetBlobArrayElement(pExpiredItemIDArray, i);
		*pItemID = vecExpiredItemIDList[i];
	}
	pNewCmd->AddParameter(new CCCommandParameterBlob(pExpiredItemIDArray, CCGetBlobArraySize(pExpiredItemIDArray)));
	CCEraseBlobArray(pExpiredItemIDArray);
	RouteToListener(pObj, pNewCmd);
}

// �����Ǹ� true
bool CCMatchServer::CorrectEquipmentByLevel(CCMatchObject* pPlayer, CCMatchCharItemParts nPart, int nLegalItemLevelDiff)	
{
	if (!IsEnabledObject(pPlayer)) return false;

	if (pPlayer->GetCharInfo()->m_EquipedItem.IsEmpty(nPart) == false) {
		CCMatchItem* pItem = pPlayer->GetCharInfo()->m_EquipedItem.GetItem(nPart);
		if (pItem->GetDesc())
		{
			if (pItem->GetDesc()->m_nResLevel.Ref() > (pPlayer->GetCharInfo()->m_nLevel+nLegalItemLevelDiff)) {
				ResponseTakeoffItem(pPlayer->GetUID(), nPart);
				return true;
			}
		}
	}
	return false;
}

bool CCMatchServer::CharFinalize(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return false;

	if (MGetServerConfig()->GetServerMode() == CSM_EVENT) 
	{	///< EVENT�� �������� ���� ������ �������� �����
		CorrectEquipmentByLevel(pObj, MMCIP_MELEE);
		CorrectEquipmentByLevel(pObj, MMCIP_PRIMARY);
		CorrectEquipmentByLevel(pObj, MMCIP_SECONDARY);
		CorrectEquipmentByLevel(pObj, MMCIP_CUSTOM1);
		CorrectEquipmentByLevel(pObj, MMCIP_CUSTOM2);
		CorrectEquipmentByLevel(pObj, MMCIP_COMMUNITY1);
		CorrectEquipmentByLevel(pObj, MMCIP_COMMUNITY2);
		CorrectEquipmentByLevel(pObj, MMCIP_LONGBUFF1);
		CorrectEquipmentByLevel(pObj, MMCIP_LONGBUFF2);
	}

	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
	UpdateCharItemDBCachingData(pObj);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ

	DWORD nAID = 0;
	if( NULL != pObj->GetAccountInfo() )
		nAID = pObj->GetAccountInfo()->m_nAID;

	CCAsyncDBJob_UpdateAccountLastLoginTime* pUpdateAccountLastLoginTimeJob = new CCAsyncDBJob_UpdateAccountLastLoginTime(uidPlayer);
	if( NULL != pUpdateAccountLastLoginTimeJob )
	{
		pUpdateAccountLastLoginTimeJob->Input( pObj->GetAccountInfo()->m_nAID );
		// PostAsyncJob( pUpdateAccountLastLoginTimeJob );
		pObj->m_DBJobQ.DBJobQ.push_back( pUpdateAccountLastLoginTimeJob );
	}

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if( NULL == pCharInfo ) return false;

	// Ŭ���� ���ԵǾ� ������ CCMatchMap���� ����
	if (pCharInfo->m_ClanInfo.IsJoined())
	{
		m_ClanMap.RemoveObject(uidPlayer, pObj);
	}

	// ĳ�������� �α� �����
	if (pCharInfo->m_nCID != 0)
	{
		unsigned long int nPlayTime = 0;		// 1 = 1��
		unsigned long int nNowTime = GetTickTime();

		if (pCharInfo->m_nConnTime != 0) {
			nPlayTime = CCGetTimeDistance(pCharInfo->m_nConnTime, nNowTime) / 1000;
		}

		CCAsyncDBJob_CharFinalize* pJob = new CCAsyncDBJob_CharFinalize(uidPlayer);
		pJob->Input(nAID,
			pCharInfo->m_nCID, 
			nPlayTime, 
			pCharInfo->m_nConnKillCount, 
			pCharInfo->m_nConnDeathCount,
			pCharInfo->m_nConnXP, 
			pCharInfo->m_nXP,
			pCharInfo->m_QuestItemList,
			pCharInfo->m_QMonsterBible,
			pCharInfo->m_DBQuestCachingData.IsRequestUpdateWhenLogout() );
		// PostAsyncJob(pJob);

		pObj->m_DBJobQ.DBJobQ.push_back( pJob );

		pCharInfo->m_DBQuestCachingData.Reset();
	}

	
	for(CCMatchCharBattleTimeRewardInfoMap::iterator iter = pCharInfo->GetBRInfoMap().begin();
		iter != pCharInfo->GetBRInfoMap().end(); iter++)
	{
		CCMatchCharBRInfo* pInfo = iter->second;
		OnAsyncRequest_UpdateCharBRInfo(uidPlayer, pInfo->GetBRID(), pInfo->GetBRTID(), pInfo->GetRewardCount(), pInfo->GetBattleTime(), pInfo->GetKillCount());
	}
		


	return true;
}

void CCMatchServer::OnRequestMySimpleCharInfo(const CCUID& uidPlayer)
{
	ResponseMySimpleCharInfo(uidPlayer);
}

void CCMatchServer::ResponseMySimpleCharInfo(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();

	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_MY_SIMPLE_CHARINFO, CCUID(0,0));

	void* pMyCharInfoArray = CCMakeBlobArray(sizeof(CCTD_MySimpleCharInfo), 1);
	CCTD_MySimpleCharInfo* pMyCharInfo = (CCTD_MySimpleCharInfo*)CCGetBlobArrayElement(pMyCharInfoArray, 0);

	pMyCharInfo->nXP = pCharInfo->m_nXP;
	pMyCharInfo->nBP = pCharInfo->m_nBP;
	pMyCharInfo->nLevel = pCharInfo->m_nLevel;

	pNewCmd->AddParameter(new CCCommandParameterBlob(pMyCharInfoArray, CCGetBlobArraySize(pMyCharInfoArray)));
	CCEraseBlobArray(pMyCharInfoArray);

	RouteToListener(pObj, pNewCmd);
}

void CCMatchServer::OnRequestCopyToTestServer(const CCUID& uidPlayer)
{
#ifndef _DEBUG
	return;
#endif

	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchCharInfo*	pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;


	int nResult = MERR_UNKNOWN;

	ResponseCopyToTestServer(uidPlayer, nResult);
}

void CCMatchServer::ResponseCopyToTestServer(const CCUID& uidPlayer, const int nResult)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_COPY_TO_TESTSERVER, CCUID(0,0));
	pNewCmd->AddParameter(new CCCommandParameterInt(nResult));		// result
	RouteToListener(pObj, pNewCmd);
}

void CCMatchServer::OnFriendAdd(const CCUID& uidPlayer, const char* pszName)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

	if (pObj->GetFriendInfo() == NULL) return;

	CCMatchObject* pTargetObj = GetPlayerByName(pszName);
	if (!IsEnabledObject(pTargetObj)) {
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	const int nTargetCID = pTargetObj->GetCharInfo()->m_nCID;

	if ((IsAdminGrade(pObj) == false) && (IsAdminGrade(pTargetObj) == true)) {
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	CCMatchFriendNode* pNode = pObj->GetFriendInfo()->Find(pszName);
	if (pNode) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_FRIEND_ALREADY_EXIST);
		return;
	}
	if (pObj->GetFriendInfo()->m_FriendList.size() > MAX_FRIEND_COUNT) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_FRIEND_TOO_MANY_ADDED);
		return;
	}

	int nCID = pObj->GetCharInfo()->m_nCID;
	int nFriendCID = 0;
	if (m_MatchDBMgr.GetCharCID(pszName, &nFriendCID) == false) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHARACTER_NOT_EXIST);
		return;
	}

	// ���� ���߿� ���� ���ؼ� �̸��� �ٲٸ� �ٸ� �� �ִ�.
	if( nTargetCID != nFriendCID )
	{
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHARACTER_NOT_EXIST);
		return;
	}

	if (m_MatchDBMgr.FriendAdd(nCID, nFriendCID, 0) == false) {
		cclog("DB Query(FriendAdd) Failed\n");
		return;
	}

	pObj->GetFriendInfo()->Add(nFriendCID, 0, pszName);
	NotifyMessage(uidPlayer, MATCHNOTIFY_FRIEND_ADD_SUCCEED);
}

void CCMatchServer::OnFriendRemove(const CCUID& uidPlayer, const char* pszName)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;
	if (pObj->GetFriendInfo() == NULL) return;

	CCMatchFriendNode* pNode = pObj->GetFriendInfo()->Find(pszName);
	if (pNode == NULL) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_FRIEND_NOT_EXIST);
		return;
	}

	const int nNodeCID = pNode->nFriendCID;

	int nCID = pObj->GetCharInfo()->m_nCID;
	int nFriendCID = 0;
	if (m_MatchDBMgr.GetCharCID(pszName, &nFriendCID) == false) {
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHARACTER_NOT_EXIST);
		return;
	}

	if( nNodeCID != nFriendCID )
	{
		NotifyMessage(uidPlayer, MATCHNOTIFY_CHARACTER_NOT_EXIST);
		return;
	}

	if (m_MatchDBMgr.FriendRemove(nCID, nFriendCID) == false) {
		cclog("DB Query(FriendRemove) Failed\n");
		return;
	}

	pObj->GetFriendInfo()->Remove(pszName);
	NotifyMessage(uidPlayer, MATCHNOTIFY_FRIEND_REMOVE_SUCCEED);
}

void CCMatchServer::OnFriendList(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

	// ASync DB
	if (!pObj->DBFriendListRequested())
	{
		CCAsyncDBJob_FriendList* pJob=new CCAsyncDBJob_FriendList(uidPlayer, pObj->GetCharInfo()->m_nCID);
		pJob->SetFriendInfo(new CCMatchFriendInfo);
		pObj->m_DBJobQ.DBJobQ.push_back( pJob );
		// PostAsyncJob(pJob);
	}
	else if (!pObj->GetFriendInfo())
	{
		// ���� DB���� FriendList�� �ȹ޾ƿ����� �׳� ����
		return;
	}

	FriendList(uidPlayer);
}

void CCMatchServer::FriendList(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;
	if (pObj->GetFriendInfo() == NULL) return;

	// Update Friends Status and Description
	pObj->GetFriendInfo()->UpdateDesc();

	CCMatchFriendList* pList = &pObj->GetFriendInfo()->m_FriendList;

	void* pListArray = CCMakeBlobArray(sizeof(MFRIENDLISTNODE), (int)pList->size());
	int nIndex=0;
	for (CCMatchFriendList::iterator i=pList->begin(); i!=pList->end(); i++) {
		CCMatchFriendNode* pNode = (*i);
		MFRIENDLISTNODE* pListNode = (MFRIENDLISTNODE*)CCGetBlobArrayElement(pListArray, nIndex++);
		pListNode->nState = pNode->nState;
		strcpy(pListNode->szName, pNode->szName);
		strcpy(pListNode->szDescription, pNode->szDescription);
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_FRIENDLIST, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterBlob(pListArray, CCGetBlobArraySize(pListArray)));
	CCEraseBlobArray(pListArray);
	RouteToListener(pObj, pCmd);
}

void CCMatchServer::OnFriendMsg(const CCUID& uidPlayer, const char* szMsg)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

}

void CCMatchServer::OnRequestCharInfoDetail(const CCUID& uidChar, const char* szCharName)
{
	ResponseCharInfoDetail(uidChar, szCharName);
}

void CCMatchServer::ResponseCharInfoDetail(const CCUID& uidChar, const char* szCharName)
{
	CCMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	CCMatchObject* pTarObject = GetPlayerByName(szCharName);
	if (! IsEnabledObject(pTarObject))
	{
		RouteResponseToListener(pObject, MC_MATCH_RESPONSE_RESULT, MERR_NO_TARGET);
		return;
	}


	// Client�� ������ ĳ���� ���� ����
	CCTD_CharInfo_Detail trans_charinfo_detail;
	CopyCharInfoDetailForTrans(&trans_charinfo_detail, pTarObject->GetCharInfo(), pTarObject);
	

	CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_CHARINFO_DETAIL, CCUID(0,0));

	void* pCharInfoArray = CCMakeBlobArray(sizeof(CCTD_CharInfo_Detail), 1);
	CCTD_CharInfo_Detail* pTransCharInfoDetail = (CCTD_CharInfo_Detail*)CCGetBlobArrayElement(pCharInfoArray, 0);
	memcpy(pTransCharInfoDetail, &trans_charinfo_detail, sizeof(CCTD_CharInfo_Detail));
	pNewCmd->AddParameter(new CCCommandParameterBlob(pCharInfoArray, CCGetBlobArraySize(pCharInfoArray)));
	CCEraseBlobArray(pCharInfoArray);

	RouteToListener(pObject, pNewCmd);
}

