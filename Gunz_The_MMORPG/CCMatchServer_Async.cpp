#include "stdafx.h"
#include "CCMatchServer.h"
#include "MAsyncDBJob.h"
#include "MAsyncDBJob_FriendList.h"
#include "MAsyncDBJob_GetLoginInfo.h"
#include "MAsyncDBJob_InsertConnLog.h"
#include "CCBlobArray.h"
#include "CCMatchFormula.h"
#include "MAsyncDBJob_Event.h"
#include "MAsyncDBJob_GetAccountItemList.h"
#include "CCAsyncDBJob_BuyQuestItem.h"
#include "CCAsyncDBJob_SurvivalMode.h"				// 2009. 6. 3 - Added By Hong KiJu

void CCMatchServer::PostAsyncJob(MAsyncJob* pJob )
{
	m_AsyncProxy.PostJob(pJob);
}

void CCMatchServer::ProcessAsyncJob()
{
	while(MAsyncJob* pJob = m_AsyncProxy.GetJobResult()) 
	{
		//////////////////////////////////////////////////////////////////////
		{
			CCMatchObject* pPlayer = GetObject( pJob->GetOwnerUID() );
			if( NULL != pPlayer )
			{
				pPlayer->m_DBJobQ.bIsRunningAsyncJob = false;
			}
		}
		//////////////////////////////////////////////////////////////////////

		switch(pJob->GetJobID()) {
		case MASYNCJOB_GETACCOUNTCHARLIST:
			{
				OnAsyncGetAccountCharList(pJob);
			}
			break;				
		//case MASYNCJOB_GETACCOUNTCHARINFO:
		//	{
		//		OnAsyncGetAccountCharInfo(pJob);
		//	}
		//	break;				
		//case MASYNCJOB_GETCHARINFO:
		//	{
		//		OnAsyncGetCharInfo(pJob);
		//	}
		//	break;
		case MASYNCJOB_FRIENDLIST:
			{
				OnAsyncGetFriendList(pJob);
			}
			break;			
		case MASYNCJOB_CREATECHAR:
			{
				OnAsyncCreateChar(pJob);
			}
			break;
		case MASYNCJOB_GETLOGININFO:
			{
				OnAsyncGetLoginInfo(pJob);
			}
			break;
		case MASYNCJOB_DELETECHAR:
			{
				OnAsyncDeleteChar(pJob);
			}
			break;
		case MASYNCJOB_WINTHECLANGAME:
			{
				OnAsyncWinTheClanGame(pJob);
			}
			break;
		case MASYNCJOB_UPDATECHARINFODATA:
			{
				OnAsyncUpdateCharInfoData(pJob);
			}
			break;
		case MASYNCJOB_CHARFINALIZE:
			{
				OnAsyncCharFinalize(pJob);
			}
			break;
		//case MASYNCJOB_BRINGACCOUNTITEM:
		//	{
		//		OnAsyncBringAccountItem(pJob);
		//	}
		//	break;
		case MASYNCJOB_INSERTCONNLOG:
			{
				OnAsyncInsertConnLog(pJob);
			}
			break;
		case MASYNCJOB_INSERTGAMELOG:
			{
				OnAsyncInsertGameLog(pJob);
			}
			break;
		case MASYNCJOB_INSERTGAMEPLAYERLOG:
			{

			}
			break;
		case MASYNCJOB_CREATECLAN:
			{
				OnAsyncCreateClan(pJob);
			}
			break;
		case MASYNCJOB_EXPELCLANMEMBER:
			{
				OnAsyncExpelClanMember(pJob);
			}
			break;
			
		case MASYNCJOB_INSERTQUESTGAMELOG :
			{

			}
			break;
		case MASYNCJOB_UPDATEQUESTITEMINFO :
			{
			}
			break;
			

		case MASYNCJOB_PROBABILITYEVENTPERTIME :
			{
				OnAsyncInsertEvent( pJob );
			}
			break;

		case MASYNCJOB_UPDATEIPTOCOUNTRYLIST :
			{
				OnAsyncUpdateIPtoCoutryList( pJob );
			};
			break;

		case MASYNCJOB_UPDATEBLOCKCOUNTRYCODELIST :
			{
				OnAsyncUpdateBlockCountryCodeList( pJob );
			}
			break;

		case MASYNCJOB_UPDATECUSTOMIPLIST :
			{
				OnAsyncUpdateCustomIPList( pJob );
			}
			break;

		case MASYNCJOB_GETACCOUNTITEMLIST :
			{
				OnAsyncGetAccountItemList( pJob );
			}
			break;

		case MASYNCJOB_BUYQUESTITEM :
			{
				OnAsyncBuyQuestItem( pJob );
			}
			break;


		//////////////////////////////////////////////////////////////////////////////////////////////
		// 2009. 6. 3 - Added By Hong KiJu
		case MASYNCJOB_INSERT_SURVIVALMODE_GAME_LOG :
			{
				OnAsyncSurvivalModeGameLog(pJob);				
			}
			break;
		case MASYNCJOB_GET_SURVIVALMODE_GROUP_RANKING :
			{
				OnAsyncSurvivalModeGroupRanking(pJob);
			}
			break;

		case MASYNCJOB_GET_SURVIVALMODE_PRIVATE_RANKING :
			{
				OnAsyncSurvivalModePrivateRanking(pJob);
			}
			break;

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Added By Hong KiJu(2009-09-25)
		case MASYNCJOB_GET_DUELTOURNAMENT_CHARINFO :
			{
				OnAsyncResponse_GetDuelTournamentCharacterInfo(pJob);
			}
			break;

		case MASYNCJOB_GET_DUELTOURNAMENT_PREVIOUS_CHARINFO :
			{
				OnAsyncResponse_GetDuelTournamentPreviousCharacterInfo(pJob);

			}
			break;

		case MASYNCJOB_GET_DUELTOURNAMENT_SIDERANKING :
			{
				OnAsyncResponse_GetDuelTournamentSideRanking(pJob);
			}
			break;
		
		case MASYNCJOB_GET_DUELTOURNAMENT_GROUPRANKING :
			{
				OnAsyncResponse_GetDuelTournamentGroupRanking(pJob);
			}
			break;

		case MASYNCJOB_GET_DUELTOURNAMENT_TIMESTAMP :
			{
				OnAsyncResponse_GetDuelTournamentTimeStamp(pJob);
			}
			break;

		case MASYNCJOB_UPDATE_DUELTOURNAMENT_CHARINFO :
		case MASYNCJOB_UPDATE_DUELTOURNAMENT_GAMELOG :
		case MASYNCJOB_INSERT_DUELTOURNAMENT_GAMELOGDETAIL :
			{

			}
			break;

		case MASYNCJOB_UPDATE_CHARITEM_COUNT :
			{

			}
			break;

		case MASYNCJOB_GET_BR_DESCRIPTION:
			{
				OnAsyncResponse_GetBR_Description(pJob);
			}
			break;

		case MASYNCJOB_GET_CHAR_BR_INFO:
			{
				OnAsyncResponse_GetCharBRInfo(pJob);
			}
			break;

		case MASYNCJOB_UPDATE_CHAR_BR_INFO:
			{
				OnAsyncResponse_UpdateCharBRInfo(pJob);
			}
			break;

		case MASYNCJOB_REWARD_CHAR_BR:
			{
				OnAsyncResponse_RewardCharBR(pJob);
			}
			break;

		default :
			{
				OnProcessAsyncJob( pJob );
			}
			break;
		};

		delete pJob;
	}
}



void CCMatchServer::OnAsyncGetLoginInfo(MAsyncJob* pJobInput)
{
	MAsyncDBJob_GetLoginInfo* pJob = (MAsyncDBJob_GetLoginInfo*)pJobInput;


	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) 
	{		
		// Notify Message �ʿ� -> �α��� ���� - �ذ�(Login Fail �޼��� �̿�)
		// Disconnect(pJob->GetCommUID());
		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pJob->GetCommUID(), MERR_FAILED_GETACCOUNTINFO);
		Post(pCmd);

		pJob->DeleteMemory();
		return;
	}

	CCMatchAccountInfo* pAccountInfo = pJob->GetAccountInfo();
	if( pAccountInfo == 0 ) return;

	CCMatchAccountPenaltyInfo* pAccountPenaltyInfo = pJob->GetAccountPenaltyInfo();
	if( pAccountPenaltyInfo == 0 ) return;


#ifndef _DEBUG
	// �ߺ� �α����̸� ������ �ִ� ����� ���������.
	CCMatchObject* pCopyObj = GetPlayerByAID(pAccountInfo->m_nAID);
	if (pCopyObj != NULL) 
	{
		// Notify Message �ʿ� -> �α��� ���� - �ذ�(Ư���� �޼��� �ʿ� ����)
		// �ߺ� ���ӿ� ���� ���� ���� �������� ������ �����ϴ� ���̹Ƿ�,
		// Ư���� ���� ��Ŷ�� ������ �ʴ´�.
		Disconnect(pCopyObj->GetUID());
	}
#endif

	// ������� �������� Ȯ���Ѵ�.
	if ((pAccountInfo->m_nUGrade == MMUG_BLOCKED) || (pAccountInfo->m_nUGrade == MMUG_PENALTY))
	{
		MCommand* pCmd = CreateCmdMatchResponseLoginFailed(pJob->GetCommUID(), MERR_CLIENT_MMUG_BLOCKED);
		Post(pCmd);

		pJob->DeleteMemory();
		return;
	}

	AddObjectOnMatchLogin(pJob->GetCommUID(), pJob->GetAccountInfo(), pJob->GetAccountPenaltyInfo(), 
		pJob->IsFreeLoginIP(), pJob->GetCountryCode3(), pJob->GetChecksumPack());

/*
	// �Ҵ�...
	CCUID AllocUID = CommUID;
	int nErrCode = ObjectAdd(CommUID);
	if(nErrCode!=MOK) {
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	CCMatchObject* pObj = GetObject(AllocUID);
	if (pObj == NULL)
	{
		Disconnect(CommUID);
		delete pJob->GetAccountInfo();
		return;
	}

	pObj->AddCommListener(CommUID);
	pObj->SetObjectType(MOT_PC);
	memcpy(pObj->GetAccountInfo(), pAccountInfo, sizeof(CCMatchAccountInfo));
	pObj->SetFreeLoginIP(pJob->IsFreeLoginIP());
	pObj->SetCountryCode3( pJob->GetCountryCode3() );


	MCommObject* pCommObj = (MCommObject*)m_CommRefCache.GetRef(CommUID);
	if (pCommObj != NULL)
	{
		pObj->SetPeerAddr(pCommObj->GetIP(), pCommObj->GetIPString(), pCommObj->GetPort());
	}
	
	SetClientClockSynchronize(CommUID);

	MCommand* pCmd = CreateCmdMatchResponseLoginOK(CommUID, AllocUID, pAccountInfo->m_szUserID, pAccountInfo->m_nUGrade, pAccountInfo->m_nPGrade);
	Post(pCmd);	


	// ���� �α�
	MAsyncDBJob_InsertConnLog* pNewJob = new MAsyncDBJob_InsertConnLog();
	pNewJob->Input(pObj->GetAccountInfo()->m_nAID, pObj->GetIPString(), pObj->GetCountryCode3() );
	PostAsyncJob(pNewJob);

#ifndef _DEBUG
	// Client DataFile Checksum�� �˻��Ѵ�.
	unsigned long nChecksum = pJob->GetChecksumPack() ^ CommUID.High ^ CommUID.Low;
	if (nChecksum != GetItemFileChecksum()) {
		LOG(LOG_PROG, "Invalid ZItemChecksum(%u) , UserID(%s) ", nChecksum, pObj->GetAccountInfo()->m_szUserID);
		Disconnect(CommUID);
	}
#endif

	delete pJob->GetAccountInfo();
*/

}

void CCMatchServer::OnAsyncGetAccountCharList(MAsyncJob* pJobResult)
{
	MAsyncDBJob_GetAccountCharList* pJob = (MAsyncDBJob_GetAccountCharList*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(ResponseAccountCharList) Failed\n", szTime);
		return;
	}		

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) 
		return;

	const int					nCharCount		= pJob->GetCharCount();
	const MTD_AccountCharInfo * pCharList		= pJob->GetCharList();
	MTD_AccountCharInfo*		pTransCharInfo	= NULL;
	int							nCharMaxLevel	= 0;

	MCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_ACCOUNT_CHARLIST, CCUID(0,0));
	void* pCharArray = MMakeBlobArray(sizeof(MTD_AccountCharInfo), nCharCount);

	for (int i = 0; i < nCharCount; i++)
	{
		pTransCharInfo = (MTD_AccountCharInfo*)MGetBlobArrayElement(pCharArray, i);
		memcpy(pTransCharInfo, &pCharList[i], sizeof(MTD_AccountCharInfo));

		nCharMaxLevel = max(nCharMaxLevel, pTransCharInfo->nLevel);
	}

	pObj->CheckNewbie( nCharMaxLevel );

	pNewCmd->AddParameter(new MCommandParameterBlob(pCharArray, MGetBlobArraySize(pCharArray)));
	MEraseBlobArray(pCharArray);
    
	RouteToListener( pObj, pNewCmd );
}

//void CCMatchServer::OnAsyncGetAccountCharInfo(MAsyncJob* pJobResult)
//{
//	MAsyncDBJob_GetAccountCharInfo* pJob = (MAsyncDBJob_GetAccountCharInfo*)pJobResult;
//
//	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
//		char szTime[128]="";
//		_strtime(szTime);
//
//		cclog("[%s] Async DB Query(ResponseAccountCharInfo) Failed\n", szTime);
//		return;
//	}		
//
//	CCMatchObject* pObj = GetObject(pJob->GetUID());
//	if (pObj == NULL) return;
//	if (pJob->GetResultCommand() == NULL) return;
//
//	RouteToListener(pObj, pJob->GetResultCommand());
//}


//void CCMatchServer::OnAsyncGetCharInfo(MAsyncJob* pJobResult)
//{
//	MAsyncDBJob_GetCharInfo* pJob = (MAsyncDBJob_GetCharInfo*)pJobResult;
//
//	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
//		cclog("DB Query(OnAsyncGetCharInfo > GetCharInfoByAID) Failed\n");
//		return;
//	}
//
//	CCMatchObject* pObj = GetObject(pJob->GetUID());
//	if (pObj == NULL) return;
//
//	if (pObj->GetCharInfo())
//	{
//		// ������ ĳ���� ���õǾ� �־��ٸ� ĳ�������� �α� �����
//		if (pObj->GetCharInfo()->m_nCID != 0)
//		{
//			CharFinalize(pObj->GetUID());		// ĳ�������� ��� �α� �� ó��
//		}
//
//		pObj->FreeCharInfo();
//		pObj->FreeFriendInfo();
//	}
//
//	if (pJob->GetCharInfo() == NULL)
//	{
//		cclog("pJob->GetCharInfo() IS NULL\n");
//		return;
//	}
//	pObj->SetCharInfo(pJob->GetCharInfo());		// Save Async Result
////	pObj->SetFriendInfo(pJob->GetFriendInfo());	// Save Async Result
//
//	if (CharInitialize(pJob->GetUID()) == false)
//	{
//		cclog("OnAsyncGetCharInfo > CharInitialize failed");
//		return;
//	}
//
//	// Client�� ������ ĳ���� ���� ����
//	MTD_CharInfo trans_charinfo;
//	CopyCharInfoForTrans(&trans_charinfo, pJob->GetCharInfo(), pObj);
//	
//	MCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_SELECT_CHAR, CCUID(0,0));
//	pNewCmd->AddParameter(new MCommandParameterInt(MOK));		// result
//
//	void* pCharArray = MMakeBlobArray(sizeof(MTD_CharInfo), 1);
//	MTD_CharInfo* pTransCharInfo = (MTD_CharInfo*)MGetBlobArrayElement(pCharArray, 0);
//	memcpy(pTransCharInfo, &trans_charinfo, sizeof(MTD_CharInfo));
//	pNewCmd->AddParameter(new MCommandParameterBlob(pCharArray, MGetBlobArraySize(pCharArray)));
//	MEraseBlobArray(pCharArray);
//
//
//	// �� ĳ������ �߰� ����
//	void* pMyExtraInfoArray = MMakeBlobArray(sizeof(MTD_MyExtraCharInfo), 1);
//	MTD_MyExtraCharInfo* pMyExtraInfo = (MTD_MyExtraCharInfo*)MGetBlobArrayElement(pMyExtraInfoArray, 0);
//	int nPercent = CCMatchFormula::GetLevelPercent(trans_charinfo.nXP, (int)trans_charinfo.nLevel);
//	pMyExtraInfo->nLevelPercent = (char)nPercent;
//	pNewCmd->AddParameter(new MCommandParameterBlob(pMyExtraInfoArray, MGetBlobArraySize(pMyExtraInfoArray)));
//	MEraseBlobArray(pMyExtraInfoArray);
//
//	RouteToListener(pObj, pNewCmd);
//
//#ifdef _DELETE_CLAN
//	if( MMCDS_NORMAL != pJob->GetDeleteState() )
//	{
//		if( MMCDS_WAIT )
//		{
//			// �۷� ��� ��¥�� �˷���.
//			
//			MCommand* pCmdDelClan = CreateCommand( MC_MATCH_CLAN_ACCOUNCE_DELETE, pObj->GetUID() );
//			pCmdDelClan->AddParameter( new MCmdParamStr(pObj->GetCharInfo()->m_ClanInfo.m_strDeleteDate.c_str()) );
//			Post( pCmdDelClan );
//		}
//		else if( MMCDS_DELETE )
//		{
//			// Ŭ�� ��� ��Ŵ.
//		}
//	}
//#endif
//}

void CCMatchServer::OnAsyncGetFriendList(MAsyncJob* pJobInput)
{
	MAsyncDBJob_FriendList* pJob = (MAsyncDBJob_FriendList*)pJobInput;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) 
	{
		return;
	}

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (!IsEnabledObject(pObj)) return;

	pObj->SetFriendInfo(pJob->GetFriendInfo());	// Save Async Result

	FriendList(pObj->GetUID());
}

void CCMatchServer::OnAsyncCreateChar(MAsyncJob* pJobResult)
{
	MAsyncDBJob_CreateChar* pJob = (MAsyncDBJob_CreateChar*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(CreateChar) Failed\n", szTime);
		return;
	}		

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) return;

	// Make Result
	MCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_CREATE_CHAR, CCUID(0,0));
	pNewCmd->AddParameter(new MCommandParameterInt(pJob->GetDBResult()));			// result
	pNewCmd->AddParameter(new MCommandParameterString(pJob->GetCharName()));	// ������� ĳ���� �̸�

	RouteToListener( pObj, pNewCmd );
}

void CCMatchServer::OnAsyncDeleteChar(MAsyncJob* pJobResult)
{
	MAsyncDBJob_DeleteChar* pJob = (MAsyncDBJob_DeleteChar*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(DeleteChar) Failed\n", szTime);
	}		

	CCMatchObject* pObj = GetObject(pJob->GetUID());
	if (pObj == NULL) return;

	RouteResponseToListener(pObj, MC_MATCH_RESPONSE_DELETE_CHAR, pJob->GetDeleteResult());
}

void CCMatchServer::OnAsyncWinTheClanGame(MAsyncJob* pJobInput)
{
	if (pJobInput->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncWinTheClanGame) Failed\n", szTime);
		return;
	}		

}


void CCMatchServer::OnAsyncUpdateCharInfoData(MAsyncJob* pJobInput)
{
	if (pJobInput->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncUpdateCharInfoData) Failed\n", szTime);
		return;
	}		

}

void CCMatchServer::OnAsyncCharFinalize(MAsyncJob* pJobInput)
{
	if (pJobInput->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncCharFinalize) Failed\n", szTime);
		return;
	}

}

//void CCMatchServer::OnAsyncBringAccountItem(MAsyncJob* pJobResult)
//{
//	MAsyncDBJob_BringAccountItem* pJob = (MAsyncDBJob_BringAccountItem*)pJobResult;
//
//	CCMatchObject* pObj = GetObject(pJob->GetUID());
//	if (!IsEnabledObject(pObj)) return;
//
//	int nRet = MERR_UNKNOWN;
//
//	if (pJob->GetResult() == MASYNC_RESULT_SUCCEED) 
//	{
//		unsigned long int nNewCIID =	pJob->GetNewCIID();
//		unsigned long int nNewItemID =	pJob->GetNewItemID();
//		bool bIsRentItem =				pJob->GetRentItem();
//		int nRentMinutePeriodRemainder = pJob->GetRentMinutePeriodRemainder();
//
//
//
//
//		// ������Ʈ�� ������ �߰�
//		CCUID uidNew = CCMatchItemMap::UseUID();
//		pObj->GetCharInfo()->m_ItemList.CreateItem(uidNew, nNewCIID, nNewItemID, bIsRentItem, nRentMinutePeriodRemainder);
//
//		nRet = MOK;
//	}		
//
//	ResponseCharacterItemList(pJob->GetUID());	// ���� �ٲ� ������ ����Ʈ�� �ٽ� �ѷ��ش�.
//
//
//	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM, CCUID(0,0));
//	pNew->AddParameter(new MCmdParamInt(nRet));
//	RouteToListener(pObj, pNew);
//
//
//}

void CCMatchServer::OnAsyncInsertConnLog(MAsyncJob* pJobResult)
{
	if (pJobResult->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]="";
		_strtime(szTime);

		cclog("[%s] Async DB Query(OnAsyncInsertConnLog) Failed\n", szTime);
		return;
	}		

}

void CCMatchServer::OnAsyncInsertGameLog(MAsyncJob* pJobResult)
{
	MAsyncDBJob_InsertGameLog* pJob = (MAsyncDBJob_InsertGameLog*)pJobResult;

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		char szTime[128]=""; _strtime(szTime);
		cclog("[%s] Async DB Query(OnAsyncInsertGameLog) Failed\n", szTime);
		return;
	}

	CCMatchStage* pStage = FindStage(pJob->GetOwnerUID());
	if( pStage == NULL ) return;

	pStage->SetGameLogID(pJob->GetID());
}

void CCMatchServer::OnAsyncCreateClan(MAsyncJob* pJobResult)
{
	MAsyncDBJob_CreateClan* pJob = (MAsyncDBJob_CreateClan*)pJobResult;

	CCUID uidMaster = pJob->GetMasterUID();
	CCMatchObject* pMasterObject = GetObject(uidMaster);
	

	if (pJob->GetResult() != MASYNC_RESULT_SUCCEED) {
		if (IsEnabledObject(pMasterObject))
		{
			RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_CANNOT_CREATE);
		}
		return;
	}		

	int nNewCLID = pJob->GetNewCLID();

	if ( (pJob->GetDBResult() == false) || (nNewCLID ==0) )
	{
		if (IsEnabledObject(pMasterObject))
		{
			RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_CANNOT_CREATE);
		}
		return;
	}


	// �������� �ٿ�Ƽ�� ��´�.
	if (IsEnabledObject(pMasterObject))
	{
		pMasterObject->GetCharInfo()->IncBP(-CLAN_CREATING_NEED_BOUNTY);
		ResponseMySimpleCharInfo(pMasterObject->GetUID());
	
		UpdateCharClanInfo(pMasterObject, nNewCLID, pJob->GetClanName(), MCG_MASTER);
	
		// �ӽ��ڵ�... �߸��� CCMatchObject*�� �´ٸ� üũ�Ͽ� �������...20090224 by kammir
		if(pMasterObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
			LOG(LOG_FILE, "[OnAsyncCreateClan()] %s's ClanID:%d.", pMasterObject->GetAccountName(), pMasterObject->GetCharInfo()->m_ClanInfo.GetClanID());

	}


	CCMatchObject* pSponsorObjects[CLAN_SPONSORS_COUNT];
	_ASSERT(CLAN_SPONSORS_COUNT == 4);

	pSponsorObjects[0] = GetObject(pJob->GetMember1UID());
	pSponsorObjects[1] = GetObject(pJob->GetMember2UID());
	pSponsorObjects[2] = GetObject(pJob->GetMember3UID());
	pSponsorObjects[3] = GetObject(pJob->GetMember4UID());

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		if (IsEnabledObject(pSponsorObjects[i]))
		{
			UpdateCharClanInfo(pSponsorObjects[i], nNewCLID, pJob->GetClanName(), MCG_MEMBER);
			// �ӽ��ڵ�... �߸��� CCMatchObject*�� �´ٸ� üũ�Ͽ� �������...20090224 by kammir
			if(pSponsorObjects[i]->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
				LOG(LOG_FILE, "[OnAsyncCreateClan()] %s's ClanID:%d.", pSponsorObjects[i]->GetAccountName(), pSponsorObjects[i]->GetCharInfo()->m_ClanInfo.GetClanID());

			RouteResponseToListener(pSponsorObjects[i], MC_MATCH_RESPONSE_RESULT, MRESULT_CLAN_CREATED);
		}
	}

	if (IsEnabledObject(pMasterObject))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MOK);
	}
}

void CCMatchServer::OnAsyncExpelClanMember(MAsyncJob* pJobResult)
{
	MAsyncDBJob_ExpelClanMember* pJob = (MAsyncDBJob_ExpelClanMember*)pJobResult;

	CCMatchObject* pAdminObject = GetObject(pJob->GetAdminUID());

	if (pJobResult->GetResult() != MASYNC_RESULT_SUCCEED) 
	{
		if (IsEnabledObject(pAdminObject))
		{
			RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
		}
		return;
	}		

	int nDBRet = pJob->GetDBResult();
	switch (nDBRet)
	{
	case CCMatchDBMgr::ER_NO_MEMBER:
		{
			if (IsEnabledObject(pAdminObject))
			{
				RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
			}
			return;
		}
		break;
	case CCMatchDBMgr::ER_WRONG_GRADE:
		{
			if (IsEnabledObject(pAdminObject))
			{
				RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_CHANGE_GRADE);
			}
			return;
		}
		break;
	}


	// ���� ����ڰ� ������������ Ŭ��Ż��Ǿ��ٰ� �˷�����Ѵ�.
	CCMatchObject* pMemberObject = GetPlayerByName(pJob->GetTarMember());
	if (IsEnabledObject(pMemberObject))
	{
		UpdateCharClanInfo(pMemberObject, 0, "", MCG_NONE);
		// �ӽ��ڵ�... �߸��� CCMatchObject*�� �´ٸ� üũ�Ͽ� �������...20090224 by kammir
		if(pMemberObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
			LOG(LOG_FILE, "[OnAsyncExpelClanMember()] %s's ClanID:%d.", pMemberObject->GetAccountName(), pMemberObject->GetCharInfo()->m_ClanInfo.GetClanID());

	}

	if (IsEnabledObject(pAdminObject))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MOK);
	}
}



void CCMatchServer::OnAsyncInsertEvent( MAsyncJob* pJobResult )
{
	if( 0 == pJobResult )
		return;

	MAsyncDBJob_EventLog* pEventJob = 
		reinterpret_cast< MAsyncDBJob_EventLog* >( pJobResult );

	if( pEventJob->GetAnnounce().empty() )
		return;

	if( MASYNC_RESULT_SUCCEED == pJobResult->GetResult() )
	{
		MCommand* pCmd;
		AsyncEventObjVec::const_iterator it, end;
		const AsyncEventObjVec& EventObjList = pEventJob->GetEventObjList();

		end = EventObjList.end();
		for( it = EventObjList.begin(); it != end; ++it )
		{
			if( CCUID(0, 0) != it->uidUser )
			{
				pCmd = CreateCommand( MC_MATCH_ANNOUNCE, it->uidUser );
				if( 0 != pCmd )
				{
					pCmd->AddParameter( new MCmdParamUInt(0) );
					pCmd->AddParameter( new MCmdParamStr(pEventJob->GetAnnounce().c_str()) );
					Post( pCmd );
				}
			}
		}
	}
}


void CCMatchServer::OnAsyncUpdateIPtoCoutryList( MAsyncJob* pJobResult )
{
	MCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_IP_TO_COUNTRY, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::OnAsyncUpdateBlockCountryCodeList( MAsyncJob* pJobResult )
{
	MCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_BLOCK_COUTRYCODE, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::OnAsyncUpdateCustomIPList( MAsyncJob* pJobResult )
{
	MCommand* pCmd = CreateCommand( MC_LOCAL_UPDATE_CUSTOM_IP, GetUID() );
	if( 0 != pCmd )
		Post( pCmd );
}


void CCMatchServer::OnAsyncGetAccountItemList( MAsyncJob* pJobResult )
{
	MAsyncDBJob_GetAccountItemList* pJob = (MAsyncDBJob_GetAccountItemList*)pJobResult;

	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		cclog("GetAccountItemList Failed\n");
		return;
	}

	CCMatchObject* pObj = GetObject( pJob->GetPlayerUID() );
	if( NULL == pObj ) return;

	if( !pJob->GetExpiredAccountItems().empty() ) {
		ResponseExpiredItemIDList(pObj, pJob->GetExpiredAccountItems());
	}

	const int nAccountItemCount = pJob->GetAccountItemCount();

	if (nAccountItemCount > 0) {
		MAccountItemNode* accountItems = pJob->GetAccountItemList();
		if( NULL == accountItems ) return;

		MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_ACCOUNT_ITEMLIST, pObj->GetUID());

		// ���� �ִ� ������ ����Ʈ ����
		int nCountableAccountItemCount = 0;
		for(int i = 0; i < nAccountItemCount; i++ ) {
			if( accountItems[i].nCount > 0 && accountItems[i].nItemID > 0 ) { 
				nCountableAccountItemCount++; 
			}
		}

		void* pItemArray = MMakeBlobArray(sizeof(MTD_AccountItemNode), nCountableAccountItemCount);		

		int nIndex = 0;
		for (int i = 0; i < nAccountItemCount; i++)
		{
			if( accountItems[i].nItemID == 0 ) continue;
			MTD_AccountItemNode* pItemNode = (MTD_AccountItemNode*)MGetBlobArrayElement(pItemArray, nIndex);			

			_ASSERTE( ((NULL != MGetMatchItemDescMgr()->GetItemDesc(accountItems[i].nItemID)) 
				|| (NULL != m_GambleMachine.GetGambleItemByGambleItemID(accountItems[i].nItemID)))
				&& "zitem.xml or GambleItem�� ����Ǿ� ���� �ʴ� �������Դϴ�." );

			if( accountItems[i].nCount > 0 ) {
				Make_MTDAccountItemNode(pItemNode, accountItems[i].nAIID, accountItems[i].nItemID
					, accountItems[i].nRentMinutePeriodRemainder, accountItems[i].nCount);

				nIndex++;

				if( nIndex == nCountableAccountItemCount ) { break;	}
			}			
		}

		pNew->AddParameter(new MCommandParameterBlob(pItemArray, MGetBlobArraySize(pItemArray)));
		MEraseBlobArray(pItemArray);

		PostSafeQueue( pNew );
	}
}


void CCMatchServer::OnAsyncBuyQuestItem( MAsyncJob* pJobReslt )
{
	MAsyncDBJob_BuyQuestItem* pJob = (MAsyncDBJob_BuyQuestItem*)pJobReslt;
	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() ){ return; }

	CCMatchObject* pPlayer = GetObject( pJob->GetPlayerUID() );
	if( NULL == pPlayer ) {	return; }

	CCMatchCharInfo* pCharInfo = pPlayer->GetCharInfo();
	if( NULL == pCharInfo ) { return; }

	// ������ �ŷ� ī��Ʈ ����. ���ο��� ��� ������Ʈ ����.
	pCharInfo->GetDBQuestCachingData().IncreaseShopTradeCount(pJob->GetItemCount());
	pCharInfo->m_nBP -= pJob->GetPrice();

	
	MCommand* pNewCmd = CreateCommand( MC_MATCH_RESPONSE_BUY_QUEST_ITEM, pJob->GetPlayerUID() );
	if( 0 == pNewCmd ) {
		cclog( "CCMatchServer::OnResponseBuyQuestItem - new Command����.\n" );
		return;
	}
	
	pNewCmd->AddParameter( new MCmdParamInt(MOK) );
	pNewCmd->AddParameter( new MCmdParamInt(pCharInfo->m_nBP) );
	PostSafeQueue( pNewCmd );

	// ����Ʈ ������ ����Ʈ�� �ٽ� ������.
	OnRequestCharQuestItemList( pJob->GetPlayerUID() );
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu

void CCMatchServer::OnAsyncSurvivalModeGameLog( MAsyncJob* pJobResult )
{
	CCAsyncDBJob_InsertSurvivalModeGameLog *pJob = (CCAsyncDBJob_InsertSurvivalModeGameLog *)pJobResult;

	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() )
	{
		return;
	}
}

void CCMatchServer::OnAsyncSurvivalModeGroupRanking(MAsyncJob* pJobResult)
{
	CCAsyncDBJob_GetSurvivalModeGroupRanking* pJob = (CCAsyncDBJob_GetSurvivalModeGroupRanking*)pJobResult;
	
	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() )
	{
		return;
	}
/*#ifdef _DEBUG
	// ������ CCAsyncDBJob_GetSurvivalModeGroupRanking Class�� ��� ����Դϴ�.
	// Class���� �� �ʿ��� �κ��� ������ �����Ͻø� �˴ϴ�.
	for(DWORD dwScenarioID = 1; dwScenarioID <= MAX_SURVIVAL_SCENARIO_COUNT; dwScenarioID++)
	{
		cclog("-------- Scenario ID = %d, Top 100 --------\n", dwScenarioID);

		for(int i = 1; i <= 100; i++)
		{
			RANKINGINFO *pRankingInfo = pJob->GetRankingInfo(dwScenarioID, i);
			
			if( NULL != pRankingInfo )
			{
				cclog("CID = %d, Ranking = %d, Ranking Point = %d\n", 
					pRankingInfo->dwCID, pRankingInfo->dwRanking, pRankingInfo->dwRankingPoint);
			}

		}
		
	}
#endif*/

	//��¥ ��� ����
	/*char sz[256];
	for (DWORD s=0; s<MAX_SURVIVAL_SCENARIO_COUNT; ++s)
	{
		for (int i=0; i<MAX_SURVIVAL_RANKING_LIST; ++i)
		{
			RANKINGINFO *pRankingInfo = pJob->GetRankingInfo(s, i);
			if (pRankingInfo)
			{
				pSurvivalRankInfo->SetRanking(s, i, 
					pRankingInfo->dwRanking, pRankingInfo->szCharName, pRankingInfo->dwRankingPoint);
			}
		}
	}*/

	MSurvivalRankInfo* pSurvivalRankInfo = CCMatchServer::GetInstance()->GetQuest()->GetSurvivalRankInfo();
	pSurvivalRankInfo->ClearRanking();

	for (DWORD s=0; s<MAX_SURVIVAL_SCENARIO_COUNT; ++s)
	{
		for (int i=0; i<MAX_SURVIVAL_RANKING_LIST; ++i)
		{
			RANKINGINFO *pRankingInfo = pJob->GetRankingInfo(s, i);
			if (pRankingInfo)
			{
				pSurvivalRankInfo->SetRanking(s, i, 
					pRankingInfo->dwRanking, pRankingInfo->szCharName, pRankingInfo->dwRankingPoint);
			}
		}
	}
}

void CCMatchServer::OnAsyncSurvivalModePrivateRanking(MAsyncJob* pJobResult)
{
	CCAsyncDBJob_GetSurvivalModePrivateRanking* pJob = (CCAsyncDBJob_GetSurvivalModePrivateRanking*)pJobResult;

	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() )
	{
		cclog("CCMatchServer::OnAsyncSurvivalModePrivateRanking - ����! stageUID[%d] playerCID[%d]\n", pJob->GetStageUID(), pJob->GetCID());
		return;
	}
#ifdef _DEBUG
	cclog("CCMatchServer::OnAsyncSurvivalModePrivateRanking - Test Log�Դϴ�. ����!\n");

	// ������ CCAsyncDBJob_GetSurvivalModePrivateRanking Class�� ��� ����Դϴ�.
	// Class���� �� �ʿ��� �κ��� ������ �����Ͻø� �˴ϴ�.
	cclog("-------- User Ranking Info --------\n");
	cclog("User CID = %d\n", pJob->GetCID());

	for(DWORD dwScenarioID = 1; dwScenarioID < MAX_SURVIVAL_SCENARIO_COUNT + 1; dwScenarioID++)
	{
		RANKINGINFO* pRankingInfo = pJob->GetPrivateRankingInfo(dwScenarioID);

		cclog("Scenario ID = %01d, Ranking = %d, Ranking Point = %d\n", 
			dwScenarioID, pRankingInfo->dwRanking, pRankingInfo->dwRankingPoint);
	}
#endif

	_ASSERT( pJob->GetScenarioID()-1 < MAX_SURVIVAL_SCENARIO_COUNT );
	RANKINGINFO* pRankingInfo = pJob->GetPrivateRankingInfo( pJob->GetScenarioID() );
	if (pRankingInfo)
	{
		// �÷��̾�� ��ŷ ������ ������		
		MCommand* pCmdPrivateRanking = MGetMatchServer()->CreateCommand( MC_SURVIVAL_PRIVATERANKING, CCUID(0, 0) );
		if( NULL == pCmdPrivateRanking )
			return;

		pCmdPrivateRanking->AddParameter( new MCommandParameterUInt(pRankingInfo->dwRanking) );
		pCmdPrivateRanking->AddParameter( new MCommandParameterUInt(pRankingInfo->dwRankingPoint) );
		
		RouteToObjInStage(pJob->GetStageUID(), pJob->GetPlayerUID(), pCmdPrivateRanking);
	}
	else _ASSERT(0);
	
}