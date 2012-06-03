// FIX THIS
#pragma warning (disable : 4509)

#include "stdafx.h"
#include "CCMatrix.h"
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
#include "CCVoteDiscussImpl.h"
#include "CCUtil.h"
#include "CCMatchGameType.h"
#include "CCMatchRuleBaseQuest.h"
#include "CCMatchRuleQuest.h"
#include "CCMatchRuleBerserker.h"
#include "CCMatchRuleDuel.h"
#include "CCCrashDump.h"

#include "CCAsyncDBJob_InsertGamePlayerLog.h"

static bool StageShowInfo(CCMatchServer* pServer, const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat);


CCMatchStage* CCMatchServer::FindStage(const CCUID& uidStage)
{
	CCMatchStageMap::iterator i = m_StageMap.find(uidStage);
	if(i==m_StageMap.end()) return NULL;

	CCMatchStage* pStage = (*i).second;
	return pStage;
}

bool CCMatchServer::StageAdd(CCMatchChannel* pChannel, const char* pszStageName, bool bPrivate, const char* pszStagePassword, CCUID* pAllocUID, bool bIsAllowNullChannel)
{
	// Ŭ������ pChannel�� NULL�̴�.

	CCUID uidStage = m_StageMap.UseUID();
	
	CCMatchStage* pStage= new CCMatchStage;
	if (pChannel && !pChannel->AddStage(pStage)) {
		delete pStage;
		return false;
	}


	CCMATCH_GAMETYPE GameType = CCMATCH_GAMETYPE_DEFAULT;
	bool bIsCheckTicket = false;
	DWORD dwTicketID = 0;

	if ( (NULL != pChannel) && CCGetServerConfig()->IsUseTicket()) {
		bIsCheckTicket = (pChannel != 0) && pChannel->IsUseTicket() && pChannel->IsTicketChannel();
		dwTicketID = pChannel->GetTicketItemID();

		// Ƽ�� �������� �缳 ä���� ������ Ƽ�� �˻� - Ƽ���� Ŭ���� Ƽ�ϰ� �����ϴ�.
		if ( pChannel->GetChannelType() == CCCHANNEL_TYPE_USER) {
			bIsCheckTicket = true;
			dwTicketID = GetChannelMap()->GetClanChannelTicketInfo().m_dwTicketItemID;
		}
	}

	if (!pStage->Create( uidStage, pszStageName, bPrivate, pszStagePassword, bIsAllowNullChannel, GameType, bIsCheckTicket, dwTicketID) ) {
		if (pChannel) {
			pChannel->RemoveStage(pStage);
		}

		delete pStage;
		return false;
	}

	m_StageMap.Insert(uidStage, pStage);

	*pAllocUID = uidStage;

	return true;
}


bool CCMatchServer::StageRemove(const CCUID& uidStage, CCMatchStageMap::iterator* pNextItor)
{
	CCMatchStageMap::iterator i = m_StageMap.find(uidStage);
	if(i==m_StageMap.end()) {
		return false;
	}

	CCMatchStage* pStage = (*i).second;

	CCMatchChannel* pChannel = FindChannel(pStage->GetOwnerChannel());
	if (pChannel) {
		pChannel->RemoveStage(pStage);
	}

	pStage->Destroy();
	delete pStage;

	CCMatchStageMap::iterator itorTemp = m_StageMap.erase(i);
	if (pNextItor) *pNextItor = itorTemp;

	return true;
}


bool CCMatchServer::StageJoin(const CCUID& uidPlayer, const CCUID& uidStage)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;

	if (pObj->GetStageUID() != CCUID(0,0))
		StageLeave(pObj->GetUID());//, pObj->GetStageUID());

	CCMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pChannel == NULL) return false;
	if (pChannel->GetChannelType() == CCCHANNEL_TYPE_DUELTOURNAMENT) return false;

	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	int ret = ValidateStageJoin(uidPlayer, uidStage);
	if (ret != MOK) {
		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, ret);
		return false;
	}
	pObj->OnStageJoin();

	// Cache Add
	CCMatchObjectCacheBuilder CacheBuilder;
	CacheBuilder.AddObject(pObj);
	CCCommand* pCmdCacheAdd = CacheBuilder.GetResultCmd(MATCHCACHEMODE_ADD, this);
	RouteToStage(pStage->GetUID(), pCmdCacheAdd);

	// Join
	pStage->AddObject(uidPlayer, pObj);
		// �ӽ��ڵ�... �߸��� Ŭ��ID �´ٸ� üũ�Ͽ� �������...20090224 by kammir
	if(pObj->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
		LOG(LOG_FILE, "[UpdateCharClanInfo()] %s's ClanID:%d.", pObj->GetAccountName(), pObj->GetCharInfo()->m_ClanInfo.GetClanID());

	pObj->SetStageUID(uidStage);
	pObj->SetStageState(MOSS_NONREADY);
	pObj->SetTeam(pStage->GetRecommandedTeam());

	// Cast Join
	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_JOIN), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
	pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
	pNew->AddParameter(new CCCommandParameterUInt(pStage->GetIndex()+1));
	pNew->AddParameter(new CCCommandParameterString((char*)pStage->GetName()));
	
	if (pStage->GetState() == STAGE_STATE_STANDBY)  RouteToStage(pStage->GetUID(), pNew);
	else											RouteToListener(pObj, pNew);


	// Cache Update
	CacheBuilder.Reset();
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		CCUID uidObj = (CCUID)(*i).first;
		CCMatchObject* pScanObj = (CCMatchObject*)GetObject(uidObj);
		if (pScanObj) {
			CacheBuilder.AddObject(pScanObj);
		} else {
			LOG(LOG_PROG, "CCMatchServer::StageJoin - Invalid ObjectCCUID(%u:%u) exist in Stage(%s)\n",
				uidObj.High, uidObj.Low, pStage->GetName());
			pStage->RemoveObject(uidObj);
			return false;
		}
	}
    CCCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
	RouteToListener(pObj, pCmdCacheUpdate);


	// Cast Master(����)
	CCUID uidMaster = pStage->GetMasterUID();
	CCCommand* pMasterCmd = CreateCommand(MC_MATCH_STAGE_MASTER, CCUID(0,0));
	pMasterCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pMasterCmd->AddParameter(new CCCommandParameterUID(uidMaster));
	RouteToListener(pObj, pMasterCmd);


#ifdef _QUEST_ITEM
	if (CCGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
		if( 0 == pNode )
		{
			cclog( "CCMatchServer::StageJoin - �������� ���� ��� ã�� ����.\n" );
			return false;
		}

		if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
		{
			CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
			if( 0 == pRuleQuest )
			{
				cclog( "CCMatchServer::StageJoin - ������ ����ȯ ����.\n" );
				return false;
			}

			pRuleQuest->OnChangeCondition();
			//pRuleQuest->OnResponseQL_ToStage( pObj->GetStageUID() );
			// ��ȯ������ ó�� �������� ���νô� ������ ������ ����Ʈ�� ���־ 
			//  ó�� ������ ������ ����Ʈ Ÿ������ �˼��� ���⿡,
			//	Ŭ���̾�Ʈ�� �������� Ÿ���� ����Ʈ������ �ν��ϴ� ��������
			//  �� ������ ��û�� �ϴ� �������� ������. - 05/04/14 by �߱���.
			// pStage->GetRule()->OnResponseSacrificeSlotInfoToStage( uidPlayer );
		}
	}
#endif


	// Cast Character Setting
	StageTeam(uidPlayer, uidStage, pObj->GetTeam());
	StagePlayerState(uidPlayer, uidStage, pObj->GetStageState());


	// ��� �����ڸ� ��������� �ڵ����� ���Ѵ´�. - �°��ӳ� ���� ��û
	if (CCMUGEVENTMASTER == pObj->GetAccountInfo()->m_nUGrade) {
		OnEventChangeMaster(pObj->GetUID());
	}

	return true;
}

bool CCMatchServer::StageLeave(const CCUID& uidPlayer)//, const CCUID& uidStage)
{
	CCMatchObject* pObj = GetObject( uidPlayer );
	if( !IsEnabledObject(pObj) ) return false;
	// CCMatchStage* pStage = FindStage(uidStage);

	//if(pObj->GetStageUID()!=uidStage)
	//	cclog(" stage leave hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return false;

	bool bLeaverMaster = false;
	if (uidPlayer == pStage->GetMasterUID()) bLeaverMaster = true;

#ifdef _QUEST_ITEM
	if (CCGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
		if( 0 != pNode )
		{
			if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
			{
				CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
				if(pRuleQuest)
				{
					pRuleQuest->PreProcessLeaveStage( uidPlayer );
				} else {
					LOG(LOG_PROG, "StageLeave:: CCMatchRule to CCMatchRuleBaseQuest FAILED \n");
				}
			}
		}
	}
#endif

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LEAVE), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
	// pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
	RouteToStage(pStage->GetUID(), pNew);

	pStage->RemoveObject(uidPlayer);

	//CCMatchObject* pObj = GetObject(uidPlayer);
	//if (pObj)
	{
		CCMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pObj);
		CCCommand* pCmdCache = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REMOVE, this);
		RouteToStage(pStage->GetUID(), pCmdCache);
	}

	// cast Master
	if (bLeaverMaster) StageMaster(pStage->GetUID());

#ifdef _QUEST_ITEM
	// ������ ������������ �����Ŀ� QL�� �ٽ� ����� ��� ��.
	if (CCGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
		if( 0 == pNode )
		{
			cclog( "CCMatchServer::StageLeave - �������� ���� ��� ã�� ����.\n" );
			return false;
		}

		if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
		{
			CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
			if( 0 == pRuleQuest )
			{
				cclog( "CCMatchServer::StageLeave - ������ ����ȯ ����.\n" );
				return false;
			}

			if( STAGE_STATE_STANDBY == pStage->GetState() )
				pRuleQuest->OnChangeCondition();
				//pRuleQuest->OnResponseQL_ToStage( uidStage );
		}
	}
#endif


	return true;
}



DWORD StageEnterBattleExceptionHandler( PEXCEPTION_POINTERS ExceptionInfo )
{
	char szStageDumpFileName[ _MAX_DIR ]= {0,};
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	sprintf( szStageDumpFileName, "Log/StageDump_%d-%d-%d_%d-%d-%d.dmp"
		, SystemTime.wYear
		, SystemTime.wMonth
		, SystemTime.wDay
		, SystemTime.wHour
		, SystemTime.wMinute
		, SystemTime.wSecond );

	return CrashExceptionDump( ExceptionInfo, szStageDumpFileName, true );
}



bool ExceptionTraceStageEnterBattle( CCMatchObject* pObj, CCMatchStage* pStage )
{
	if( NULL == pObj )
	{
		return false;
	}

	if( NULL == pStage )
	{
		return false;
	}

//	__try
	{
		pStage->EnterBattle(pObj);
	}
/*	__except( StageEnterBattleExceptionHandler(GetExceptionInformation()) )
	{
		cclog( "\nexception : stage enter battle =====================\n" );


		CCMatchObject* pMaster = MGetMatchServer()->GetObject( pStage->GetMasterUID() );
		if( NULL != pMaster )  
		{
			if( NULL != pMaster->GetCharInfo() )
			{
				cclog( "stage master cid : %d\n", pMaster->GetCharInfo()->m_nCID );
			}
		}
		else
		{
			cclog( "stage master hass problem.\n" );				
		}
		
		
		if( NULL != pObj->GetCharInfo() )
		{
			cclog( "cmd sender cid : %d\n", pObj->GetCharInfo()->m_nCID );
		}
		else
		{
			cclog( "cmd send char info null point.\n" );
		}

		
		CCMatchStageSetting*	pStageSetting = pStage->GetStageSetting();
		if( NULL != pStageSetting )
		{
			cclog( "stage state : %d\n", pStage->GetStageSetting()->GetStageState() );

			const MSTAGE_SETTING_NODE* pExStageSettingNode = pStageSetting->GetStageSetting();
			if( NULL != pExStageSettingNode )
			{
				cclog( "stage name : %s\n", pExStageSettingNode->szMapName );
				cclog( "stage game type : %d\n", pExStageSettingNode->nGameType );
				cclog( "stage max player : %d\n", pExStageSettingNode->nMaxPlayers );
				cclog( "stage current player : %d\n", pStage->GetPlayers() );
				cclog( "stage force entry enable : %d\n", pExStageSettingNode->bForcedEntryEnabled );
				cclog( "stage rule pointer : %x\n", pStage->GetRule() );
			}
		}

		CCUIDRefCache::iterator itStage, endStage;
		endStage = pStage->GetObjEnd();
		itStage = pStage->GetObjBegin();
		CCMatchObject* pObj = NULL;
		for( ; endStage != itStage; ++itStage )
		{
			pObj = MGetMatchServer()->GetObject( itStage->first );
			if( NULL == pObj )
			{
				cclog( "!!!!stage can't find player!!!!\n" );
				continue;
			}

			cclog( "stage player name : %s\n", pObj->GetName() );
		}

		cclog( "=====================\n\n" );

		return false;
	}
*/
	return true;
}



bool CCMatchServer::StageEnterBattle(const CCUID& uidPlayer, const CCUID& uidStage)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;
	// CCMatchStage* pStage = FindStage(uidStage);
	
	if(pObj->GetStageUID()!=uidStage)
		cclog(" stage enter battle hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return false;

	pObj->SetPlace(MMP_BATTLE);

	CCCommand* pNew = CreateCommand(MC_MATCH_STAGE_ENTERBATTLE, CCUID(0,0));
	//pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
	//pNew->AddParameter(new CCCommandParameterUID(uidStage));

	unsigned char nParam = MCEP_NORMAL;
	if (pObj->IsForcedEntried()) nParam = MCEP_FORCED;
	pNew->AddParameter(new CCCommandParameterUChar(nParam));

	void* pPeerArray = CCMakeBlobArray(sizeof(CCTD_PeerListNode), 1);
	CCTD_PeerListNode* pNode = (CCTD_PeerListNode*)CCGetBlobArrayElement(pPeerArray, 0);
	memset(pNode, 0, sizeof(CCTD_PeerListNode));
	
	pNode->uidChar	= pObj->GetUID();
	pNode->dwIP		= pObj->GetIP();
	pNode->nPort	= pObj->GetPort();

	CopyCharInfoForTrans(&pNode->CharInfo, pObj->GetCharInfo(), pObj);
	//���������ӽ��ּ� 	CopyCharBuffInfoForTrans(&pNode->CharBuffInfo, pObj->GetCharInfo(), pObj);

	pNode->ExtendInfo.nPlayerFlags = pObj->GetPlayerFlags();
	if (pStage->GetStageSetting()->IsTeamPlay())	pNode->ExtendInfo.nTeam = (char)pObj->GetTeam();
	else											pNode->ExtendInfo.nTeam = 0;	

	pNew->AddParameter(new CCCommandParameterBlob(pPeerArray, CCGetBlobArraySize(pPeerArray)));
	CCEraseBlobArray(pPeerArray);

	RouteToStage(uidStage, pNew);

	// ��Ʋ ���۽ð� ����
	pObj->GetCharInfo()->m_nBattleStartTime = CCMatchServer::GetInstance()->GetGlobalClockCount();
	pObj->GetCharInfo()->m_nBattleStartXP = pObj->GetCharInfo()->m_nXP;

	// ����� �Ŀ� �־�� �Ѵ�.
	return ExceptionTraceStageEnterBattle( pObj, pStage );
}

bool CCMatchServer::StageLeaveBattle(const CCUID& uidPlayer, bool bGameFinishLeaveBattle, bool bForcedLeave)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;
	if (pObj->GetPlace() != MMP_BATTLE) { return false; }

	// CCMatchStage* pStage = FindStage(uidStage);

	//if(pObj->GetStageUID()!=uidStage)
	//	cclog(" stage leave battle hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);

	const CCUID uidStage = pObj->GetStageUID();

	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL)
	{	// Ŭ������ ������ �ٳ����� ���������� �������Ƿ� ���⼭ agent�� �����ش�. 
		if (pObj->GetRelayPeer()) {
			CCAgentObject* pAgent = GetAgent(pObj->GetAgentUID());
			if (pAgent) {
				CCCommand* pCmd = CreateCommand(MC_AGENT_PEER_UNBIND, pAgent->GetCommListener());
				pCmd->AddParameter(new CCCmdParaCCUID(uidPlayer));
				Post(pCmd);
			}
		}

		UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
		UpdateCharItemDBCachingData(pObj);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ
		//CheckSpendableItemCounts(pObj);		///< �׻� UpdateCharItemDBCachingData �ڿ� �־�� �մϴ�.
		
		ProcessCharPlayInfo(pObj);			///< ĳ���� �÷����� ���� ������Ʈ 
		return false;
	}
	else
	{
		// �÷��� ���� ���ǿ��� ����ġ, ��/��, �·�, �ٿ�Ƽ�� �ݿ����� �ʽ��ϴ�. - by kammir 2008.09.19
		// LeaveBattle�� �Ǹ鼭 ĳ���� �����͸� ������Ʈ ���ش�.
		CCMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.Reset();
		for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
			CCMatchObject* pScanObj = (CCMatchObject*)(*i).second;
			CacheBuilder.AddObject(pScanObj);
		}
		CCCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
		RouteToListener(pObj, pCmdCacheUpdate);
	}

	pStage->LeaveBattle(pObj);
	pObj->SetPlace(MMP_STAGE);


	// ������ �ȸ´� �������� üũ
#define LEGAL_ITEMLEVEL_DIFF		3
	bool bIsCorrect = true;
	for (int i = 0; i < MMCIP_END; i++) {
		if (CorrectEquipmentByLevel(pObj, CCMatchCharItemParts(i), LEGAL_ITEMLEVEL_DIFF)) {
			bIsCorrect = false;
		}
	}

	if (!bIsCorrect) {
		CCCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_RESULT, CCUID(0,0));
		pNewCmd->AddParameter(new CCCommandParameterInt(MERR_TAKEOFF_ITEM_BY_LEVELDOWN));
		RouteToListener(pObj, pNewCmd);
	}
	
	CheckExpiredItems(pObj);		//< �Ⱓ ���� �������� �ִ��� üũ

	if (pObj->GetRelayPeer()) {
		CCAgentObject* pAgent = GetAgent(pObj->GetAgentUID());
		if (pAgent) {
			CCCommand* pCmd = CreateCommand(MC_AGENT_PEER_UNBIND, pAgent->GetCommListener());
			pCmd->AddParameter(new CCCmdParaCCUID(uidPlayer));
			Post(pCmd);
		}
	}	

	// ĳ���� �÷����� ���� ������Ʈ 
	ProcessCharPlayInfo(pObj);

	//=======================================================================================================================================
	
	bool bIsLeaveAllBattle = true;
	
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		CCUID uidObj = (CCUID)(*i).first;
		CCMatchObject* pAllObj = (CCMatchObject*)GetObject(uidObj);
		if(NULL == pAllObj) continue;
		if(MMP_STAGE != pAllObj->GetPlace()) { 
			bIsLeaveAllBattle = false; 
			break; 
		}
	}


	if(pStage->IsRelayMap())
	{
		if(bGameFinishLeaveBattle)
		{	// �����̸�, ��Ʋ ����� ���������� ��������
			if(!pStage->m_bIsLastRelayMap)
			{	// �������� �ִٸ� �ٷ� ���� �ʽ��� ó��		

				if( !bForcedLeave ) 
				{
					pObj->SetStageState(MOSS_READY);
				}

				if( bIsLeaveAllBattle ) 
				{					
					OnStageRelayStart(uidStage);
				} 

				CCCommand* pNew = CreateCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT, CCUID(0,0));
				pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
				pNew->AddParameter(new CCCommandParameterBool(true));
				RouteToStage(uidStage, pNew);
			}
		}
		else
		{	///< ���� �޴��� ���������� ����		
			CCCommand* pNew = CreateCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT, CCUID(0,0));
			pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
			pNew->AddParameter(new CCCommandParameterBool(false));
			RouteToStage(uidStage, pNew);			

			if(bIsLeaveAllBattle) 
			{	///< ��� ���������� �ִٸ� �����̸� ������ �ٽ� ���ش�.
				pStage->m_bIsLastRelayMap = true;//�����̸��� ������
				pStage->GetStageSetting()->SetMapName(CCMATCH_MAPNAME_RELAYMAP);
				pStage->SetRelayMapCurrList(pStage->GetRelayMapList());
				pStage->m_RelayMapRepeatCountRemained = pStage->GetRelayMapRepeatCount();
			}
		}
	} 
	else 
	{
		CCCommand* pNew = CreateCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT, CCUID(0,0));
		pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
		pNew->AddParameter(new CCCommandParameterBool(false));
		RouteToStage(uidStage, pNew);
	}

	//=======================================================================================================================================

	// �濡�� ������ noready���·� ����ȴ�. 
	// ����� ������ ���������� ��� Ŭ���̾�Ʈ�� ������. - by SungE 2007-06-04
	StagePlayerState( uidPlayer, pStage->GetUID(), pObj->GetStageState() );	
	
	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
	UpdateCharItemDBCachingData(pObj);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ
	//CheckSpendableItemCounts(pObj);		///< �׻� UpdateCharItemDBCachingData �ڿ� �־�� �մϴ�.

	return true;
}

bool CCMatchServer::StageChat(const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL)	return false;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) return false;

	if (pObj->GetAccountInfo()->m_nUGrade == CCMUGCHAT_LIMITED) return false;

//	InsertChatDBLog(uidPlayer, pszChat);

	///< ȫ����(2009.08.04)
	///< ���� �ش� ����ڰ� �ִ� Stage�� ������ Stage�� UID�� �ٸ� ���!
	///< �ٸ� Stage�鿡�Ե� Msg�� ���� �� �ִ� ������ ���� (��ŷ ���α׷� ����)
	if( uidStage != pObj->GetStageUID() )
	{
		//LOG(LOG_FILE,"CCMatchServer::StageChat - Different Stage(S:%d, P:%d)", uidStage, pObj->GetStageUID());
		return false;
	}


	CCCommand* pCmd = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_CHAT), CCUID(0,0), m_This);
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pCmd->AddParameter(new CCCommandParameterString(pszChat));
	RouteToStage(uidStage, pCmd);
	return true;
}

bool CCMatchServer::StageTeam(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchTeam nTeam)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	pStage->PlayerTeam(uidPlayer, nTeam);

	CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_TEAM, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pCmd->AddParameter(new CCCommandParameterUInt(nTeam));

	RouteToStageWaitRoom(uidStage, pCmd);
	return true;
}

bool CCMatchServer::StagePlayerState(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchObjectStageState nStageState)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;
	// CCMatchStage* pStage = FindStage(uidStage);
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return false;

	pStage->PlayerState(uidPlayer, nStageState);
	
	CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_PLAYER_STATE, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pCmd->AddParameter(new CCCommandParameterInt(nStageState));
	RouteToStage(uidStage, pCmd);
	return true;
}

bool CCMatchServer::StageMaster(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	CCUID uidMaster = pStage->GetMasterUID();

	CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_MASTER, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pCmd->AddParameter(new CCCommandParameterUID(uidMaster));
	RouteToStage(uidStage, pCmd);

	return true;
}

void CCMatchServer::StageLaunch(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	ReserveAgent(pStage);

	CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_LAUNCH, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(uidStage));
	pCmd->AddParameter(new CCCmdParamStr( const_cast<char*>(pStage->GetMapName()) ));
	RouteToStage(uidStage, pCmd);
}

void CCMatchServer::StageRelayLaunch(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	ReserveAgent(pStage);

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		CCUID uidObj = (CCUID)(*i).first;
		CCMatchObject* pObj = (CCMatchObject*)GetObject(uidObj);
		if (pObj) {
			if( pObj->GetStageState() == MOSS_READY) {
				CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_RELAY_LAUNCH, CCUID(0,0));
				pCmd->AddParameter(new CCCmdParaCCUID(uidStage));
				pCmd->AddParameter(new CCCmdParamStr(const_cast<char*>(pStage->GetMapName())));
				pCmd->AddParameter(new CCCmdParamBool(false));
				RouteToListener(pObj, pCmd);
			} else {
				CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_RELAY_LAUNCH, CCUID(0,0));
				pCmd->AddParameter(new CCCmdParaCCUID(uidStage));
				pCmd->AddParameter(new CCCmdParamStr(const_cast<char*>(pStage->GetMapName())));
				pCmd->AddParameter(new CCCmdParamBool(true));
				RouteToListener(pObj, pCmd);
			}
		} else {
			LOG(LOG_PROG, "WARNING(StageRelayLaunch) : Not Existing Obj(%u:%u)\n", uidObj.High, uidObj.Low);
			i=pStage->RemoveObject(uidObj);
			LogObjectCommandHistory(uidObj);
		}
	}
}

void CCMatchServer::StageFinishGame(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	bool bIsRelayMapUnFinish = true;

	if(pStage->IsRelayMap())
	{ // ������ ���϶����� ��Ʋ�� �ٽ� �������ش�. 
		if((int)pStage->m_vecRelayMapsRemained.size() <= 0)
		{	// ���� ���� ������
			int nRepeatCount = (int)pStage->m_RelayMapRepeatCountRemained - 1;
			if(nRepeatCount < 0)
			{
				bIsRelayMapUnFinish = false;

				pStage->m_bIsLastRelayMap = true;//�����̸��� ������				
				nRepeatCount = 0;
				pStage->GetStageSetting()->SetMapName(CCMATCH_MAPNAME_RELAYMAP);	//"RelayMap" ����
			}
			pStage->m_RelayMapRepeatCountRemained = (RELAY_MAP_REPEAT_COUNT)nRepeatCount;
			pStage->SetRelayMapCurrList(pStage->GetRelayMapList());
		}

		if(!pStage->m_bIsLastRelayMap) {
			// ó�� ���۽�, Flag�� On�����ش�. 
			if( pStage->IsStartRelayMap() == false ) {
				pStage->SetIsStartRelayMap(true);
			}			

			if((int)pStage->m_vecRelayMapsRemained.size() > 0) { // �������� �ִٸ�
				int nRelayMapIndex = 0;

				if(pStage->GetRelayMapType() == RELAY_MAP_TURN) {	//< ���� ���߿��� ù ��°���� ����(������)
					nRelayMapIndex = 0; 
				} else if(pStage->GetRelayMapType() == RELAY_MAP_RANDOM) {
					nRelayMapIndex = rand() % (int)pStage->m_vecRelayMapsRemained.size();
				}

				if(nRelayMapIndex >= MAX_RELAYMAP_LIST_COUNT) { //< �� ������ 20��
					cclog("StageFinishGame RelayMap Fail RelayMapList MIsCorrect MaxCount[%d] \n", (int)nRelayMapIndex);
					return;
				}

				char* szMapName = (char*)MGetMapDescMgr()->GetMapName(pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
				if (!szMapName)
				{
					cclog("RelayMapBattleStart Fail MapID[%d] \n", (int)pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
					return;
				}

				pStage->GetStageSetting()->SetMapName(szMapName);

				// ������ �����̸��� �������ش�.
				vector<RelayMap>::iterator itor = pStage->m_vecRelayMapsRemained.begin();
				for(int i=0 ; nRelayMapIndex > i ; ++itor, ++i);// �ش� �ε������� �̵�
				pStage->m_vecRelayMapsRemained.erase(itor);
			} 
			else {
				cclog("CCMatchServer::StageFinishGame::IsRelayMap() - m_vecRelayMapsRemained.size() == 0\n");
			}
		} else {
			pStage->SetIsStartRelayMap(false);
			bIsRelayMapUnFinish = false; // �����̸� ������ ������
		}
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_FINISH_GAME, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pCmd->AddParameter(new CCCommandParameterBool(bIsRelayMapUnFinish));
	RouteToStage(uidStage, pCmd);

	return;
}

CCCommand* CCMatchServer::CreateCmdResponseStageSetting(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return NULL;

	CCCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_STAGESETTING, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(pStage->GetUID()));

	CCMatchStageSetting* pSetting = pStage->GetStageSetting();

	// Param 1 : Stage Settings
	void* pStageSettingArray = CCMakeBlobArray(sizeof(MSTAGE_SETTING_NODE), 1);
	MSTAGE_SETTING_NODE* pNode = (MSTAGE_SETTING_NODE*)CCGetBlobArrayElement(pStageSettingArray, 0);
	CopyMemory(pNode, pSetting->GetStageSetting(), sizeof(MSTAGE_SETTING_NODE));
	pCmd->AddParameter(new CCCommandParameterBlob(pStageSettingArray, CCGetBlobArraySize(pStageSettingArray)));
	CCEraseBlobArray(pStageSettingArray);

	// Param 2 : Char Settings
	int nCharCount = (int)pStage->GetObjCount();
	void* pCharArray = CCMakeBlobArray(sizeof(MSTAGE_CHAR_SETTING_NODE), nCharCount);
	int nIndex=0;
	for (CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		MSTAGE_CHAR_SETTING_NODE* pCharNode = (MSTAGE_CHAR_SETTING_NODE*)CCGetBlobArrayElement(pCharArray, nIndex++);
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
		pCharNode->uidChar = pObj->GetUID();
		pCharNode->nTeam = pObj->GetTeam();
		pCharNode->nState = pObj->GetStageState();
	}
	pCmd->AddParameter(new CCCommandParameterBlob(pCharArray, CCGetBlobArraySize(pCharArray)));
	CCEraseBlobArray(pCharArray);

	// Param 3 : Stage State
	pCmd->AddParameter(new CCCommandParameterInt((int)pStage->GetState()));

	// Param 4 : Stage Master
	pCmd->AddParameter(new CCCommandParameterUID(pStage->GetMasterUID()));

	return pCmd;
}



void CCMatchServer::OnStageCreate(const CCUID& uidChar, char* pszStageName, bool bPrivate, char* pszStagePassword)
{
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	CCMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pChannel == NULL) return;

	if ((CCGetServerConfig()->GetServerMode() == CSM_CLAN) && (pChannel->GetChannelType() == CCCHANNEL_TYPE_CLAN)
		&& (pChannel->GetChannelType() == CCCHANNEL_TYPE_DUELTOURNAMENT)) {
		return;
	}
	
	CCUID uidStage;

	if (!StageAdd(pChannel, pszStageName, bPrivate, pszStagePassword, &uidStage))
	{
		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_CREATE, MERR_CANNOT_CREATE_STAGE);
		return;
	}
	StageJoin(uidChar, uidStage);

	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage)
		pStage->SetFirstMasterName(pObj->GetCharInfo()->m_szName);
}


//void CCMatchServer::OnStageJoin(const CCUID& uidChar, const CCUID& uidStage)
//{
//	CCMatchObject* pObj = GetObject(uidChar);
//	if (pObj == NULL) return;
//
//	CCMatchStage* pStage = NULL;
//
//	if (uidStage == CCUID(0,0)) {
//		return;
//	} else {
//		pStage = FindStage(uidStage);
//	}
//
//	if (pStage == NULL) {
//		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_STAGE_NOT_EXIST);
//		return;
//	}
//
//	if ((IsAdminGrade(pObj) == false) && pStage->IsPrivate())
//	{
//		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_CANNOT_JOIN_STAGE_BY_PASSWORD);
//		return;
//	}
//
//	StageJoin(uidChar, pStage->GetUID());
//}

void CCMatchServer::OnPrivateStageJoin(const CCUID& uidPlayer, const CCUID& uidStage, char* pszPassword)
{
	if (strlen(pszPassword) > STAGEPASSWD_LENGTH) return;

	CCMatchStage* pStage = NULL;

	if (uidStage == CCUID(0,0)) 
	{
		return;
	} 
	else 
	{
		pStage = FindStage(uidStage);
	}

	if (pStage == NULL) 
	{
		CCMatchObject* pObj = GetObject(uidPlayer);
		if (pObj != NULL)
		{
			RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_STAGE_NOT_EXIST);
		}

		return;
	}

	// ���ڳ� �����ڸ� ����..

	bool bSkipPassword = false;

	CCMatchObject* pObj = GetObject(uidPlayer);

	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) 
		return;

	CCMatchUserGradeID ugid = pObj->GetAccountInfo()->m_nUGrade;

	if (ugid == CCMUGDEVELOPER || ugid == CCMUGADMIN) 
		bSkipPassword = true;

	// ��й��� �ƴϰų� �н����尡 Ʋ���� �н����尡 Ʋ�ȴٰ� �����Ѵ�.
	if(bSkipPassword==false) {
		if ((!pStage->IsPrivate()) || (strcmp(pStage->GetPassword(), pszPassword)))
		{
			CCMatchObject* pObj = GetObject(uidPlayer);
			if (pObj != NULL)
			{
				RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_CANNOT_JOIN_STAGE_BY_PASSWORD);
			}

			return;
		}
	}

	StageJoin(uidPlayer, pStage->GetUID());
}

void CCMatchServer::OnStageFollow(const CCUID& uidPlayer, const char* pszTargetName)
{
	CCMatchObject* pPlayerObj = GetObject(uidPlayer);
	if (pPlayerObj == NULL) return;

	CCMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;

	// �ڱ� �ڽ��� ���� ������ ������� �˻�.
	if (pPlayerObj->GetUID() == pTargetObj->GetUID()) return;

	// ������Ʈ�� �߸��Ǿ� �ִ��� �˻�.
	if (!pPlayerObj->CheckEnableAction(CCMatchObject::CCMOA_STAGE_FOLLOW)) return;


	// ���� �ٸ� ä������ �˻�.
	if (pPlayerObj->GetChannelUID() != pTargetObj->GetChannelUID()) {

#ifdef _VOTESETTING
		RouteResponseToListener( pPlayerObj, MC_MATCH_RESPONSE_STAGE_FOLLOW, MERR_CANNOT_FOLLOW );
#endif
		return;
	}

	if ((IsAdminGrade(pTargetObj) == true)) {
		NotifyMessage(pPlayerObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	CCMatchStage* pStage = FindStage(pTargetObj->GetStageUID());
	if (pStage == NULL) return;

	// Ŭ���������� ���� �� ����
	if (pStage->GetStageType() != CCST_NORMAL) return;

	if (pStage->IsPrivate()==false) {
		if ((pStage->GetStageSetting()->GetForcedEntry()==false) && pStage->GetState() != STAGE_STATE_STANDBY) {
			// Deny Join

#ifdef _VOTESETTING
			RouteResponseToListener( pPlayerObj, MC_MATCH_RESPONSE_STAGE_FOLLOW, MERR_CANNOT_FOLLOW );
#endif
		} else {
			StageJoin(uidPlayer, pTargetObj->GetStageUID());
		}
	}
	else {
		// ���󰡷��� ���� ��й�ȣ�� �ʿ�� �Ұ��� ���󰥼� ����.
		//RouteResponseToListener( pPlayerObj, MC_MATCH_RESPONSE_STAGE_FOLLOW, MERR_CANNOT_FOLLOW_BY_PASSWORD );

		// �ش���� ��й��̸� ��й�ȣ�� �䱸�Ѵ�.
		CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_REQUIRE_PASSWORD), CCUID(0,0), m_This);
		pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
		pNew->AddParameter(new CCCommandParameterString((char*)pStage->GetName()));
		RouteToListener(pPlayerObj, pNew);
	}
}

void CCMatchServer::OnStageLeave(const CCUID& uidPlayer)//, const CCUID& uidStage)
{
	CCMatchObject* pObj = GetObject( uidPlayer );
	if( !IsEnabledObject(pObj) ) return;
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if( !IsEnabledObject(GetObject(uidPlayer)) )
	{
		return;
	}

	StageLeave(uidPlayer);// , uidStage);
}

void CCMatchServer::OnStageRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidStage)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	// CCMatchStage* pStage = FindStage(uidStage);
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ���ο� ���
	CCMatchObjectCacheBuilder CacheBuilder;
	CacheBuilder.Reset();
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		CCMatchObject* pScanObj = (CCMatchObject*)(*i).second;
		CacheBuilder.AddObject(pScanObj);
	}
    CCCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
	RouteToListener(pObj, pCmdCacheUpdate);

	// Cast Master(����)
	CCUID uidMaster = pStage->GetMasterUID();
	CCCommand* pMasterCmd = CreateCommand(MC_MATCH_STAGE_MASTER, CCUID(0,0));
	pMasterCmd->AddParameter(new CCCommandParameterUID(uidStage));
	pMasterCmd->AddParameter(new CCCommandParameterUID(uidMaster));
	RouteToListener(pObj, pMasterCmd);

	// Cast Character Setting
	StageTeam(uidPlayer, uidStage, pObj->GetTeam());
	StagePlayerState(uidPlayer, uidStage, pObj->GetStageState());
}

void CCMatchServer::OnStageEnterBattle(const CCUID& uidPlayer, const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	StageEnterBattle(uidPlayer, uidStage);
}

void CCMatchServer::OnStageLeaveBattle(const CCUID& uidPlayer, bool bGameFinishLeaveBattle)//, const CCUID& uidStage)
{
	if( !IsEnabledObject(GetObject(uidPlayer)) )
	{
		return;
	}

	StageLeaveBattle(uidPlayer, bGameFinishLeaveBattle, false);//, uidStage);
}


#include "CMLexicalAnalyzer.h"
// ���� �ӽ��ڵ�
bool StageKick(CCMatchServer* pServer, const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat)
{
	CCMatchObject* pChar = pServer->GetObject(uidPlayer);
	if (pChar == NULL)	return false;
	CCMatchStage* pStage = pServer->FindStage(uidStage);
	if (pStage == NULL) return false;
	if (uidPlayer != pStage->GetMasterUID()) return false;

	bool bResult = false;
	CCLexicalAnalyzer lex;
	lex.Create(pszChat);

	if (lex.GetCount() >= 1) {
		char* pszCmd = lex.GetByStr(0);
		if (pszCmd) {
			if (stricmp(pszCmd, "/kick") == 0) {
				if (lex.GetCount() >= 2) {
					char* pszTarget = lex.GetByStr(1);
					if (pszTarget) {
						for (CCUIDRefCache::iterator itor = pStage->GetObjBegin(); 
							itor != pStage->GetObjEnd(); ++itor)
						{
							CCMatchObject* pTarget = (CCMatchObject*)((*itor).second);
							if (stricmp(pszTarget, pTarget->GetName()) == 0) {
								if (pTarget->GetPlace() != MMP_BATTLE) {
									pServer->StageLeave(pTarget->GetUID());//, uidStage);
									bResult = true;
								}
								break;
							}
						}
					}
				}
			}	// Kick
		}
	}

	lex.Destroy();
	return bResult;
}

// ����Ȯ�� �ӽ��ڵ�
bool StageShowInfo(CCMatchServer* pServer, const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat)
{
	CCMatchObject* pChar = pServer->GetObject(uidPlayer);
	if (pChar == NULL)	return false;
	CCMatchStage* pStage = pServer->FindStage(uidStage);
	if (pStage == NULL) return false;
	if (uidPlayer != pStage->GetMasterUID()) return false;

	bool bResult = false;
	CCLexicalAnalyzer lex;
	lex.Create(pszChat);

	if (lex.GetCount() >= 1) {
		char* pszCmd = lex.GetByStr(0);
		if (pszCmd) {
			if (stricmp(pszCmd, "/showinfo") == 0) {
				char szMsg[256]="";
				sprintf(szMsg, "FirstMaster : (%s)", pStage->GetFirstMasterName());
				pServer->Announce(pChar, szMsg);
				bResult = true;
			}	// ShowInfo
		}
	}

	lex.Destroy();
	return bResult;
}
void CCMatchServer::OnStageChat(const CCUID& uidPlayer, const CCUID& uidStage, char* pszChat)
{
	// RAONHAJE : ���� �ӽ��ڵ�
	if (pszChat[0] == '/') {
		if (StageKick(this, uidPlayer, uidStage, pszChat))
			return;
		if (StageShowInfo(this, uidPlayer, uidStage, pszChat))
			return;
	}

	StageChat(uidPlayer, uidStage, pszChat);
}

void CCMatchServer::OnStageStart(const CCUID& uidPlayer, const CCUID& uidStage, int nCountdown)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetMasterUID() != uidPlayer) return;

	if (pStage->StartGame(CCGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {
		StageRelayMapBattleStart(uidPlayer, uidStage);

		CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_START), CCUID(0,0), m_This);
		pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
		pNew->AddParameter(new CCCommandParameterUID(uidStage));
		pNew->AddParameter(new CCCommandParameterInt(min(nCountdown,3)));
		RouteToStage(uidStage, pNew);

		// ��� �α׸� �����.
		SaveGameLog(uidStage);
	}
}

void CCMatchServer::OnStageRelayStart(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	
	if (pStage->StartRelayGame(CCGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {
		// ��� �α׸� �����.
		SaveGameLog(uidStage);
	}
}

void CCMatchServer::OnStartStageList(const CCUID& uidComm)
{
	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	pObj->SetStageListTransfer(true);
}

void CCMatchServer::OnStopStageList(const CCUID& uidComm)
{
	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	pObj->SetStageListTransfer(false);
}

void CCMatchServer::OnStagePlayerState(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchObjectStageState nStageState)
{
	StagePlayerState(uidPlayer, uidStage, nStageState);
}


void CCMatchServer::OnStageTeam(const CCUID& uidPlayer, const CCUID& uidStage, CCMatchTeam nTeam)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	CCMatchObject* pChar = GetObject(uidPlayer);
	if (pChar == NULL) return;

	StageTeam(uidPlayer, uidStage, nTeam);
}

void CCMatchServer::OnStageMap(const CCUID& uidStage, char* pszMapName)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetState() != STAGE_STATE_STANDBY) return;	// �����¿����� �ٲܼ� �ִ�
	if (strlen(pszMapName) < 2) return;

	pStage->SetMapName( pszMapName );
	pStage->SetIsRelayMap(strcmp(CCMATCH_MAPNAME_RELAYMAP, pszMapName) == 0);
	
	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_MAP), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidStage));
	pNew->AddParameter(new CCCommandParameterString(pStage->GetMapName()));

	if ( MGetGameTypeMgr()->IsQuestDerived( pStage->GetStageSetting()->GetGameType()))
	{
		CCMatchRuleBaseQuest* pQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
		pQuest->RefreshStageGameInfo();
	}

    RouteToStage(uidStage, pNew);
}

void CCMatchServer::StageRelayMapBattleStart(const CCUID& uidPlayer, const CCUID& uidStage)
{// �����̸� �����ϰ� ���� ���� ��ư ������ ������ �����Ѵ�
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetMasterUID() != uidPlayer) return;
	if(!pStage->IsRelayMap()) return;
	
	// ���� ó�����۽� �ʱ�ȭ ���ֱ�
	pStage->InitCurrRelayMap();

	if (pStage->m_vecRelayMapsRemained.empty()) return;

	if((int)pStage->m_vecRelayMapsRemained.size() > MAX_RELAYMAP_LIST_COUNT)
	{// �� ������ 20�� �ʰ��ϸ� ����
		cclog("RelayMapBattleStart Fail RelayMapList MIsCorrect OverCount[%d] \n", (int)pStage->m_vecRelayMapsRemained.size());
		return;
	}

	if (pStage->m_vecRelayMapsRemained.size() != pStage->GetRelayMapListCount())
	{
		cclog("m_vecRelayMapsRemained[%d] != GetRelayMapListCount[%d]\n", (int)pStage->m_vecRelayMapsRemained.size(), pStage->GetRelayMapListCount());
		return;
	}

	// ó�� ������ ���� ���Ѵ�
	int nRelayMapIndex = 0;
	if(pStage->GetRelayMapType() == RELAY_MAP_TURN )
		nRelayMapIndex = 0; // �������߿��� ó����° ���� ����(������)
	else if(pStage->GetRelayMapType() == RELAY_MAP_RANDOM)
		nRelayMapIndex = rand() % int(pStage->m_vecRelayMapsRemained.size());

	if(CCMATCH_MAP_RELAYMAP == pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID)
	{
		cclog("RelayMapBattleStart Fail Type[%d], RoundCount[Curr:%d][%d], ListCount[Curr:%d][%d] \n",  
			pStage->GetRelayMapType(), pStage->m_RelayMapRepeatCountRemained, pStage->GetRelayMapRepeatCount(), (int)pStage->m_vecRelayMapsRemained.size(), pStage->GetRelayMapListCount());
		return;
	}

	char* szMapName = (char*)MGetMapDescMgr()->GetMapName(pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
	if (!szMapName)
	{
		cclog("RelayMapBattleStart Fail MapID[%d] \n", (int)pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
		return;
	}

	pStage->GetStageSetting()->SetMapName(szMapName);

	// ������ �����̸��� �������ش�.
	vector<RelayMap>::iterator itor = pStage->m_vecRelayMapsRemained.begin();
	for(int i=0 ; nRelayMapIndex > i ; ++itor, ++i);// �ش� �ε������� �̵�
	pStage->m_vecRelayMapsRemained.erase(itor);
}

void CCMatchServer::OnStageRelayMapElementUpdate(const CCUID& uidStage, int nType, int nRepeatCount)
{
	CCMatchStage* pStage = FindStage(uidStage);
	pStage->SetRelayMapType((RELAY_MAP_TYPE)nType);
	pStage->SetRelayMapRepeatCount((RELAY_MAP_REPEAT_COUNT)nRepeatCount);

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidStage));
	pNew->AddParameter(new CCCommandParameterInt((int)pStage->GetRelayMapType()));
	pNew->AddParameter(new CCCommandParameterInt((int)pStage->GetRelayMapRepeatCount()));
	RouteToStage(uidStage, pNew);
}

void CCMatchServer::OnStageRelayMapListUpdate(const CCUID& uidStage, int nRelayMapType, int nRelayMapRepeatCount, void* pRelayMapListBlob)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if(!pStage->IsRelayMap()) return;
	if (pStage->GetState() != STAGE_STATE_STANDBY) return;	// �����¿����� �ٲܼ� �ִ�

	// �����̸� ������ ������ ���������� ����
	RelayMap relayMapList[MAX_RELAYMAP_LIST_COUNT];
	for (int i = 0; i < MAX_RELAYMAP_LIST_COUNT; i++)
		relayMapList[i].nMapID = -1;
	int nRelayMapListCount = CCGetBlobArrayCount(pRelayMapListBlob);
	if(nRelayMapListCount > MAX_RELAYMAP_LIST_COUNT)
		nRelayMapListCount = MAX_RELAYMAP_LIST_COUNT;
	for (int i = 0; i < nRelayMapListCount; i++)
	{
		CCTD_RelayMap* pRelayMap = (CCTD_RelayMap*)CCGetBlobArrayElement(pRelayMapListBlob, i);
		if(!MGetMapDescMgr()->MIsCorrectMap(pRelayMap->nMapID))
		{
			cclog("OnStageRelayMapListUpdate Fail MIsCorrectMap ID[%d] \n", (int)pRelayMap->nMapID);
			break;
		}
		relayMapList[i].nMapID = pRelayMap->nMapID;
	}

	pStage->SetRelayMapType((RELAY_MAP_TYPE)nRelayMapType);
	pStage->SetRelayMapRepeatCount((RELAY_MAP_REPEAT_COUNT)nRelayMapRepeatCount);
	pStage->SetRelayMapList(relayMapList);
	pStage->InitCurrRelayMap();


	// �� �����, �ʸ���Ʈ ����
	void* pRelayMapListBlob = CCMakeBlobArray(sizeof(CCTD_RelayMap), pStage->GetRelayMapListCount());
	RelayMap RelayMapList[MAX_RELAYMAP_LIST_COUNT];
	memcpy(RelayMapList, pStage->GetRelayMapList(), sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT);
	for (int i = 0; i < pStage->GetRelayMapListCount(); i++)
	{
		CCTD_RelayMap* pRelayMapList = (CCTD_RelayMap*)CCGetBlobArrayElement(pRelayMapListBlob, i);
		pRelayMapList->nMapID = RelayMapList[i].nMapID;
	}

	// ������ ���� �����̸� ������ ����鿡�� ����
	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidStage));
	pNew->AddParameter(new CCCommandParameterInt((int)pStage->GetRelayMapType()));
	pNew->AddParameter(new CCCommandParameterInt((int)pStage->GetRelayMapRepeatCount()));
	pNew->AddParameter(new CCCommandParameterBlob(pRelayMapListBlob, CCGetBlobArraySize(pRelayMapListBlob)));
	RouteToStage(uidStage, pNew);
}
void CCMatchServer::OnStageRelayMapListInfo(const CCUID& uidStage, const CCUID& uidChar)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if(pStage == NULL) return;
	if(!pStage->IsRelayMap()) return;
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;
	// �������϶� ������ ó�� ������(�����̸� �ۼ����ϼ��� ����)
	if(pStage->GetState() == STAGE_STATE_STANDBY && pStage->GetMasterUID() == uidChar) return;	

	// �� �����, �ʸ���Ʈ ����
	void* pRelayMapListBlob = CCMakeBlobArray(sizeof(CCTD_RelayMap), pStage->GetRelayMapListCount());
	RelayMap RelayMapList[MAX_RELAYMAP_LIST_COUNT];
	memcpy(RelayMapList, pStage->GetRelayMapList(), sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT);
	for (int i = 0; i < pStage->GetRelayMapListCount(); i++)
	{
		CCTD_RelayMap* pRelayMapList = (CCTD_RelayMap*)CCGetBlobArrayElement(pRelayMapListBlob, i);
		pRelayMapList->nMapID = RelayMapList[i].nMapID;
	}
	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidStage));
	pNew->AddParameter(new CCCommandParameterInt((int)pStage->GetRelayMapType()));
	pNew->AddParameter(new CCCommandParameterInt((int)pStage->GetRelayMapRepeatCount()));
	pNew->AddParameter(new CCCommandParameterBlob(pRelayMapListBlob, CCGetBlobArraySize(pRelayMapListBlob)));
	CCEraseBlobArray(pRelayMapListBlob);

	RouteToListener(pObj, pNew); // ������ �����̸� �����߿� ������Ʈ�� �������� ���� �ɼ��� ����
}

void CCMatchServer::OnStageSetting(const CCUID& uidPlayer, const CCUID& uidStage, void* pStageBlob, int nStageCount)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetState() != STAGE_STATE_STANDBY) return;	// �����¿����� �ٲܼ� �ִ�
	if (nStageCount <= 0) return;

	// validate
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) {
		cclog(" stage setting invalid object (%d, %d) ignore\n", uidPlayer.High, uidPlayer.Low);
		return;
	}

	if( pObj->GetStageUID()!=uidStage ||  nStageCount!=1 ||
		CCGetBlobArraySize(pStageBlob) != (sizeof(MSTAGE_SETTING_NODE)+sizeof(int)*2) )
	{
		cclog(" stage setting hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);
		LogObjectCommandHistory( uidPlayer );
		return;
	}

	// �����̰ų� ��ڰ� �ƴѵ� ������ �ٲٸ� �׳� ����
	if (pStage->GetMasterUID() != uidPlayer)
	{
		CCMatchObject* pObjMaster = GetObject(uidPlayer);
		if (!IsAdminGrade(pObjMaster)) return;
	}


	MSTAGE_SETTING_NODE* pNode = (MSTAGE_SETTING_NODE*)CCGetBlobArrayElement(pStageBlob, 0);

	// let's refactor
	if( (pNode->nGameType < CCMATCH_GAMETYPE_DEATHMATCH_SOLO) || (pNode->nGameType >= CCMATCH_GAMETYPE_MAX)) {
		cclog(" stage setting game mode hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);
		LogObjectCommandHistory( uidPlayer );

		// ��� ������.
//		pObj->SetInvalidStageSettingDisconnectWaitInfo();
		pObj->DisconnectHacker( CCMHT_INVALIDSTAGESETTING );

		return;
	}

	// �����̹��� ��Ȱ�� �����ε� �����̹� ��û��
	if( CCGetServerConfig()->IsEnabledSurvivalMode()==false && pNode->nGameType==CCMATCH_GAMETYPE_SURVIVAL) {
		cclog(" stage setting game mode hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);
		LogObjectCommandHistory( uidPlayer );
		pObj->DisconnectHacker( CCMHT_INVALIDSTAGESETTING );
		return;
	}

	// �⺻������ �ִ� �ο��� STAGE_BASIC_MAX_PLAYERCOUNT�� ������ STAGE_BASIC_MAX_PLAYERCOUNT�� ������.
	// ���� �۾��� �����ϸ��� �� ���ӿ� �´� �ο����� ������ ��. - by SungE 2007-05-14
	if( STAGE_MAX_PLAYERCOUNT < pNode->nMaxPlayers )
		pNode->nMaxPlayers = STAGE_MAX_PLAYERCOUNT;

	// �� �̻��� ���� ������ �Ұ��� �ϴ�. ������ �����Ѵ�. - By SungE 2007-11-07
	if( STAGE__MAX_ROUND < pNode->nRoundMax )
		pNode->nRoundMax = STAGE__MAX_ROUND;

	CCMatchStageSetting* pSetting = pStage->GetStageSetting();
	CCMatchChannel* pChannel = FindChannel(pStage->GetOwnerChannel());

	bool bCheckChannelRule = true;

	if (QuestTestServer())
	{
		if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
		{
			bCheckChannelRule = false;
		}
	}

	if ((pChannel) && (bCheckChannelRule))
	{
		// ������ �� �ִ� ��, ����Ÿ������ üũ
		CCChannelRule* pRule = MGetChannelRuleMgr()->GetRule(pChannel->GetRuleType());
		if (pRule)
		{
			if (!pRule->CheckGameType(pNode->nGameType))
			{
				pNode->nGameType = CCMATCH_GAMETYPE_DEATHMATCH_SOLO;
			}

			bool bDuelMode = false;
			if ( pNode->nGameType == CCMATCH_GAMETYPE_DUEL)
				bDuelMode = true;

			if (!pRule->CheckMap(pNode->nMapIndex, bDuelMode))
			{
				strcpy(pNode->szMapName, MGetMapDescMgr()->GetMapName(CCMATCH_MAP_MANSION));
				pNode->nMapIndex = 0;
			}
			else
			{
				strcpy(pNode->szMapName, pSetting->GetMapName());
				pNode->nMapIndex = pSetting->GetMapIndex();
			}
		}
	}

	CCMATCH_GAMETYPE nLastGameType = pSetting->GetGameType();

	// ����Ʈ ����̸� ������ ���ԺҰ�, �ִ��ο� 4������ �����Ѵ�.
	if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
	{
		if (pNode->bForcedEntryEnabled == true) pNode->bForcedEntryEnabled = false;
		pNode->nMaxPlayers = STAGE_QUEST_MAX_PLAYER;
		pNode->nLimitTime = STAGESETTING_LIMITTIME_UNLIMITED;


		// ����Ʈ ������ �ƴѵ� ����Ʈ �����̸� �ַε�����ġ�� �ٲ۴�.
		if (!QuestTestServer())
		{
			pNode->nGameType = CCMATCH_GAMETYPE_DEATHMATCH_SOLO;
		}
	}

	// ����Ʈ ��忴�ٰ� �ٸ� ��尡 �Ǹ� '���ԺҰ�'�� ������� ����
	if (MGetGameTypeMgr()->IsQuestDerived( nLastGameType ) == true &&
		MGetGameTypeMgr()->IsQuestDerived( pNode->nGameType ) == false)
		pNode->bForcedEntryEnabled = true;

	if (!MGetGameTypeMgr()->IsTeamGame(pNode->nGameType))
	{
		pNode->bAutoTeamBalancing = true;
	}

	// �����̸� ����
	pStage->SetIsRelayMap(strcmp(CCMATCH_MAPNAME_RELAYMAP, pNode->szMapName) == 0);
	pStage->SetIsStartRelayMap(false);

	if(!pStage->IsRelayMap())
	{	// �����̸��� �ƴϸ� �⺻���� �ʱ�ȭ ���ش�.
		pNode->bIsRelayMap = pStage->IsRelayMap();
		pNode->bIsStartRelayMap = pStage->IsStartRelayMap();
		for (int i=0; i<MAX_RELAYMAP_LIST_COUNT; ++i)
			pNode->MapList[i].nMapID = -1;
		pNode->nRelayMapListCount = 0;
		pNode->nRelayMapType = RELAY_MAP_TURN;
		pNode->nRelayMapRepeatCount = RELAY_MAP_3REPEAT;
	}


	pSetting->UpdateStageSetting(pNode);
	pStage->ChangeRule(pNode->nGameType);


	CCCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	RouteToStage(uidStage, pCmd);


	// ���� ��尡 ����Ǿ������
	if (nLastGameType != pSetting->GetGameType())
	{
		char szNewMap[ MAPNAME_LENGTH ] = {0};

		if (MGetGameTypeMgr()->IsQuestDerived( nLastGameType ) == false &&
			MGetGameTypeMgr()->IsQuestDerived( pSetting->GetGameType() ) == true)
		{
//			OnStageMap(uidStage, GetQuest()->GetSurvivalMapInfo(MSURVIVAL_MAP(0))->szName);
//			OnStageMap(uidStage, pSetting->GetMapName());
			OnStageMap(uidStage, CCMATCH_DEFAULT_STAGESETTING_MAPNAME);

			CCMatchRuleBaseQuest* pQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule());
			pQuest->RefreshStageGameInfo();
		}
		else if ( (nLastGameType != CCMATCH_GAMETYPE_DUEL) && ( pSetting->GetGameType() == CCMATCH_GAMETYPE_DUEL))
		{
			strcpy( szNewMap, MGetMapDescMgr()->GetMapName( CCMATCH_MAP_HALL));
			OnStageMap(uidStage, szNewMap);
		}
		else if ( ((nLastGameType == CCMATCH_GAMETYPE_QUEST) || (nLastGameType == CCMATCH_GAMETYPE_SURVIVAL) || (nLastGameType == CCMATCH_GAMETYPE_DUEL)) &&
			      ((pSetting->GetGameType() != CCMATCH_GAMETYPE_QUEST) && (pSetting->GetGameType() != CCMATCH_GAMETYPE_SURVIVAL) && ( pSetting->GetGameType() != CCMATCH_GAMETYPE_DUEL)))
		{
			strcpy( szNewMap, MGetMapDescMgr()->GetMapName( CCMATCH_MAP_MANSION));
			OnStageMap(uidStage, szNewMap);
		}
	}
}

void CCMatchServer::OnRequestStageSetting(const CCUID& uidComm, const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	CCCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	pCmd->m_Receiver = uidComm;
	Post(pCmd);

	// �� ������ �����̸��̸� ó�����ش�.
	OnStageRelayMapListInfo(uidStage, uidComm);

	CCMatchObject* pChar = GetObject(uidComm);
	if (pChar && (CCMUGEVENTMASTER == pChar->GetAccountInfo()->m_nUGrade)) 	{
		// �̺�Ʈ �����Ϳ��� ó�� �游����� ����� �˷��ش�
		StageShowInfo(this, uidComm, uidStage, "/showinfo");
	}
}

void CCMatchServer::OnRequestPeerList(const CCUID& uidChar, const CCUID& uidStage)
{
	ResponsePeerList(uidChar, uidStage);
}

void CCMatchServer::OnRequestGameInfo(const CCUID& uidChar, const CCUID& uidStage)
{
	ResponseGameInfo(uidChar, uidStage);
}

void CCMatchServer::ResponseGameInfo(const CCUID& uidChar, const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage); if (pStage == NULL) return;
	CCMatchObject* pObj = GetObject(uidChar); if (pObj == NULL) return;
	if (pStage->GetRule() == NULL) return;

	CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_GAME_INFO, CCUID(0,0));
	pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));

	// ��������
	void* pGameInfoArray = CCMakeBlobArray(sizeof(CCTD_GameInfo), 1);
	CCTD_GameInfo* pGameItem = (CCTD_GameInfo*)CCGetBlobArrayElement(pGameInfoArray, 0);
	memset(pGameItem, 0, sizeof(CCTD_GameInfo));
	
	if (pStage->GetStageSetting()->IsTeamPlay())
	{
		pGameItem->nRedTeamScore = static_cast<char>(pStage->GetTeamScore(CCMT_RED));
		pGameItem->nBlueTeamScore = static_cast<char>(pStage->GetTeamScore(CCMT_BLUE));

		pGameItem->nRedTeamKills = static_cast<short>(pStage->GetTeamKills(CCMT_RED));
		pGameItem->nBlueTeamKills = static_cast<short>(pStage->GetTeamKills(CCMT_BLUE));
	}

	pNew->AddParameter(new CCCommandParameterBlob(pGameInfoArray, CCGetBlobArraySize(pGameInfoArray)));
	CCEraseBlobArray(pGameInfoArray);

	// ������
	void* pRuleInfoArray = NULL;
	if (pStage->GetRule())
		pRuleInfoArray = pStage->GetRule()->CreateRuleInfoBlob();
	if (pRuleInfoArray == NULL)
		pRuleInfoArray = CCMakeBlobArray(0, 0);
	pNew->AddParameter(new CCCommandParameterBlob(pRuleInfoArray, CCGetBlobArraySize(pRuleInfoArray)));
	CCEraseBlobArray(pRuleInfoArray);

	// Battle�� �� ����� List�� �����.
	int nPlayerCount = pStage->GetObjInBattleCount();

	void* pPlayerItemArray = CCMakeBlobArray(sizeof(CCTD_GameInfoPlayerItem), nPlayerCount);
	int nIndex=0;
	for (CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;

		CCTD_GameInfoPlayerItem* pPlayerItem = (CCTD_GameInfoPlayerItem*)CCGetBlobArrayElement(pPlayerItemArray, nIndex++);
		pPlayerItem->uidPlayer = pObj->GetUID();
		pPlayerItem->bAlive = pObj->CheckAlive();
		pPlayerItem->nKillCount = pObj->GetAllRoundKillCount();
		pPlayerItem->nDeathCount = pObj->GetAllRoundDeathCount();
	}
	pNew->AddParameter(new CCCommandParameterBlob(pPlayerItemArray, CCGetBlobArraySize(pPlayerItemArray)));
	CCEraseBlobArray(pPlayerItemArray);

	RouteToListener(pObj, pNew);
}

void CCMatchServer::OnMatchLoadingComplete(const CCUID& uidPlayer, int nPercent)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCCommand* pCmd = CreateCommand(MC_MATCH_LOADING_COMPLETE, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(uidPlayer));
	pCmd->AddParameter(new CCCmdParamInt(nPercent));
	RouteToStage(pObj->GetStageUID(), pCmd);	
}


void CCMatchServer::OnGameRoundState(const CCUID& uidStage, int nState, int nRound)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	pStage->RoundStateFromClient(uidStage, nState, nRound);
}


void CCMatchServer::OnDuelSetObserver(const CCUID& uidChar)
{
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	CCCommand* pCmd = CreateCommand(MC_MATCH_SET_OBSERVER, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(uidChar));
	RouteToBattle(pObj->GetStageUID(), pCmd);
}

void CCMatchServer::OnRequestSpawn(const CCUID& uidChar, const CCVector& pos, const CCVector& dir)
{
	CCMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	// Do Not Spawn when AdminHiding
	if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(CCTD_PlayerFlags_AdminHide)) return;


	// ������ �׾��� �ð��� ���� �������� ��û�� �ð� ���̿� 2�� �̻��� �ð��� �־����� �˻��Ѵ�.
	DWORD dwTime = timeGetTime() - pObj->GetLastSpawnTime();	
	if ( dwTime < RESPAWN_DELAYTIME_AFTER_DYING_MIN) return;
	pObj->SetLastSpawnTime(timeGetTime());

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;
	if ( (pStage->GetRule()->GetRoundState() != CCMATCH_ROUNDSTATE_PREPARE) && (!pObj->IsEnabledRespawnDeathTime(GetTickTime())) )
		 return;

	CCMatchRule* pRule = pStage->GetRule();					// �̷� ���� �ڵ�� ������ �ȵ����� -_-; ����Ÿ�� ���� ����ó��.
	CCMATCH_GAMETYPE gameType = pRule->GetGameType();		// �ٸ� ��� �ֳ���.
	if (gameType == CCMATCH_GAMETYPE_DUEL)
	{
		CCMatchRuleDuel* pDuel = (CCMatchRuleDuel*)pRule;		// RTTI �ȽἭ dynamic cast�� �н�.. ����ó���� ¥������ -,.-
		if (uidChar != pDuel->uidChampion && uidChar != pDuel->uidChallenger)
		{
			OnDuelSetObserver(uidChar);
			return;
		}
	}

	pObj->ResetCustomItemUseCount();
	pObj->SetAlive(true);

	CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_RESPONSE_SPAWN, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParaCCUID(uidChar));
	pCmd->AddParameter(new CCCmdParamShortVector(pos.x, pos.y, pos.z));
	pCmd->AddParameter(new CCCmdParamShortVector(DirElementToShort(dir.x), DirElementToShort(dir.y), DirElementToShort(dir.z)));
	RouteToBattle(pObj->GetStageUID(), pCmd);
}

void CCMatchServer::OnGameRequestTimeSync(const CCUID& uidComm, unsigned long nLocalTimeStamp)
{
	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	CCMatchTimeSyncInfo* pSync = pObj->GetSyncInfo();
	pSync->Update(GetGlobalClockCount());

	CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_RESPONSE_TIMESYNC, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamUInt(nLocalTimeStamp));
	pCmd->AddParameter(new CCCmdParamUInt(GetGlobalClockCount()));
	RouteToListener(pObj, pCmd);
}

void CCMatchServer::OnGameReportTimeSync(const CCUID& uidComm, unsigned long nLocalTimeStamp, unsigned int nDataChecksum)
{
	CCMatchObject* pObj = GetPlayerByComCCUID(uidComm);
	if (pObj == NULL) return;

	pObj->UpdateTickLastPacketRecved();	// Last Packet Timestamp

	if (pObj->GetEnterBattle() == false)
		return;

	//// SpeedHack Test ////
	CCMatchTimeSyncInfo* pSync = pObj->GetSyncInfo();
	int nSyncDiff = nLocalTimeStamp - pSync->GetLastSyncClock();
	float fError = static_cast<float>(nSyncDiff) / static_cast<float>(MATCH_CYCLE_CHECK_SPEEDHACK);
	if (fError > 2.f) {	
		pSync->AddFoulCount();
		if (pSync->GetFoulCount() >= 3) {	// 3���� ���ǵ��� ���� - 3���ƿ�

			#ifndef _DEBUG		// ������Ҷ��� ��������
				NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GAME_SPEEDHACK);
				StageLeave(pObj->GetUID());//, pObj->GetStageUID());
				Disconnect(pObj->GetUID());
			#endif
			cclog("SPEEDHACK : User='%s', SyncRatio=%f (TimeDiff=%d) \n", 
				pObj->GetName(), fError, nSyncDiff);
			pSync->ResetFoulCount();
		}
	} else {
		pSync->ResetFoulCount();
	}
	pSync->Update(GetGlobalClockCount());

	//// MemoryHack Test ////
	if (nDataChecksum > 0) {	// ������ Client MemoryChecksum �𸣹Ƿ� �ϴ� Ŭ���̾�Ʈ�� �Ű��ϴ��ǹ̷� ����Ѵ�.
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GAME_MEMORYHACK);
		StageLeave(pObj->GetUID());//, pObj->GetStageUID());
		Disconnect(pObj->GetUID());
		cclog("MEMORYHACK : User='%s', MemoryChecksum=%u \n", pObj->GetName(), nDataChecksum);
	}
}

void CCMatchServer::OnUpdateFinishedRound(const CCUID& uidStage, const CCUID& uidChar, 
						   void* pPeerInfo, void* pKillInfo)
{

}

void CCMatchServer::OnRequestForcedEntry(const CCUID& uidStage, const CCUID& uidChar)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	CCMatchObject* pObj = GetObject(uidChar);	
	if (pObj == NULL) return;

	pObj->SetForcedEntry(true);

	RouteResponseToListener(pObj, MC_MATCH_STAGE_RESPONSE_FORCED_ENTRY, MOK);
}

void CCMatchServer::OnRequestSuicide(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	pStage->ReserveSuicide( uidPlayer, MGetMatchServer()->GetGlobalClockCount() );

	// OnGameKill(uidPlayer, uidPlayer);

	//CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SUICIDE, CCUID(0,0));
	//int nResult = MOK;
	//pNew->AddParameter(new CCCommandParameterInt(nResult));
	//pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
	//RouteToBattle(pObj->GetStageUID(), pNew);
}

void CCMatchServer::OnRequestObtainWorldItem(const CCUID& uidPlayer, const int nIteCCUID)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	pStage->ObtainWorldItem(pObj, nIteCCUID);
}

void CCMatchServer::OnRequestSpawnWorldItem(const CCUID& uidPlayer, const int nItemID, const float x, const float y, const float z, float fDropDelayTime)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if( !pObj->IsHaveCustomItem() )
		return;

	/*
	�����ϰ� �ִ� ���� �� �� �����Ƿ� ������ ������ �ִ� ����� �� �ִ� ������ ������
	�� �ѵ� �ȿ��� ����� �� �ֵ��� �Ѵ�.
	*/
	if( pObj->IncreaseCustomItemUseCount() )
	{
		pStage->RequestSpawnWorldItem(pObj, nItemID, x, y, z, fDropDelayTime);
	}
}

void CCMatchServer::OnNotifyThrowTrapItem(const CCUID& uidPlayer, const int nItemID)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if (!pObj->IsEquipCustomItem(nItemID))
		return;

	pStage->OnNotifyThrowTrapItem(uidPlayer, nItemID);
}

void CCMatchServer::OnNotifyActivatedTrapItem(const CCUID& uidPlayer, const int nItemID, const CCVector3& pos)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	pStage->OnNotifyActivatedTrapItem(uidPlayer, nItemID, pos);
}

float CCMatchServer::GetDuelVictoryMultiflier(int nVictorty)
{
	return 1.0f;
}

float CCMatchServer::GetDuelPlayersMultiflier(int nPlayerCount)
{
	return 1.0f;
}

void CCMatchServer::CalcExpOnGameKill(CCMatchStage* pStage, CCMatchObject* pAttacker, CCMatchObject* pVictim, 
					   int* poutAttackerExp, int* poutVictimExp)
{
	bool bSuicide = false;		// �ڻ�
	if (pAttacker == pVictim) bSuicide = true;		

	CCMATCH_GAMETYPE nGameType = pStage->GetStageSetting()->GetGameType();
	float fGameExpRatio = MGetGameTypeMgr()->GetInfo(nGameType)->fGameExpRatio;

	// ����Ÿ���� Training�̸� �ٷ� 0����
	if (nGameType == CCMATCH_GAMETYPE_TRAINING)
	{
		*poutAttackerExp = 0;
		*poutVictimExp = 0;
		return;
	}
	// ����Ÿ���� ����Ŀ�� ���
	else if (nGameType == CCMATCH_GAMETYPE_BERSERKER)
	{
		CCMatchRuleBerserker* pRuleBerserker = (CCMatchRuleBerserker*)pStage->GetRule();

		if (pRuleBerserker->GetBerserker() == pAttacker->GetUID())
		{
			if (pAttacker != pVictim)
			{
				// ����Ŀ�� ����ġ�� 80%�� ȹ���Ѵ�.
				fGameExpRatio = fGameExpRatio * 0.8f;
			}
			else
			{
				// ����Ŀ�� �ڻ� �Ǵ� �ǰ� �پ� �״°�� �ս� ����ġ�� ������ �Ѵ�.
				fGameExpRatio = 0.0f;
			}
		}
	}
	else if (nGameType == CCMATCH_GAMETYPE_DUEL)
	{
		CCMatchRuleDuel* pRuleDuel = (CCMatchRuleDuel*)pStage->GetRule();
		if (pVictim->GetUID() == pRuleDuel->uidChallenger)
		{
			fGameExpRatio *= GetDuelVictoryMultiflier(pRuleDuel->GetVictory());
		}
		else
		{
			fGameExpRatio *= GetDuelVictoryMultiflier(pRuleDuel->GetVictory()) * GetDuelPlayersMultiflier(pStage->GetPlayers());

		}
//		if (pRuleDuel->GetVictory() <= 1)
//		{
//			fGameExpRatio = fGameExpRatio * GetDuelPlayersMultiflier(pStage->GetPlayers()) * GetDuelVictoryMultiflier()
//		}
	}

	// ��, ����Ÿ�Կ� ���� ����ġ ���� ����
	int nMapIndex = pStage->GetStageSetting()->GetMapIndex();
	if ((nMapIndex >=0) && (nMapIndex < CCMATCH_MAP_COUNT))
	{
		float fMapExpRatio = MGetMapDescMgr()->GetExpRatio(nMapIndex);
		fGameExpRatio = fGameExpRatio * fMapExpRatio;
	}

	int nAttackerLevel = pAttacker->GetCharInfo()->m_nLevel;
	int nVictimLevel = pVictim->GetCharInfo()->m_nLevel;

	// ����ġ ���
	int nAttackerExp = (int)(CCMatchFormula::GetGettingExp(nAttackerLevel, nVictimLevel) * fGameExpRatio);
	int nVictimExp = (int)(CCMatchFormula::CalcPanaltyEXP(nAttackerLevel, nVictimLevel) * fGameExpRatio);


	// Ŭ������ ���� ȹ�� ����ġ�� 1.5��, �սǰ���ġ ����
	if ((CCGetServerConfig()->GetServerMode() == CSM_CLAN) && (pStage->GetStageType() == CCST_LADDER))
	{
		nAttackerExp = (int)((float)nAttackerExp * 1.5f);
		nVictimExp = 0;
	}

	// ���ä��, �ʰ��ä���� ��쿡�� ��ġ�ٿ� ����(�ڻ�����)
	CCMatchChannel* pOwnerChannel = FindChannel(pStage->GetOwnerChannel());
	if ((pOwnerChannel) && (!bSuicide))
	{
		if ((pOwnerChannel->GetRuleType() == CCCHANNEL_RULE_MASTERY) || 
			(pOwnerChannel->GetRuleType() == CCCHANNEL_RULE_ELITE) ||
			(pOwnerChannel->GetRuleType() == CCCHANNEL_RULE_CHAMPION))
		{
			nVictimExp=0;
		}
	}

	// ��������� ���, �������� ��� ����ġ �ι�
	if ((pVictim->GetAccountInfo()->m_nUGrade == CCMUGADMIN) || 
		(pVictim->GetAccountInfo()->m_nUGrade == CCMUGDEVELOPER))
	{
		nAttackerExp = nAttackerExp * 2;
	}
	// ���λ���� ���, �������� ��� ��ġ�ٿ� ����
	if ((!bSuicide) &&
		((pAttacker->GetAccountInfo()->m_nUGrade == CCMUGADMIN) || 
		(pAttacker->GetAccountInfo()->m_nUGrade == CCMUGDEVELOPER)))
	{
		nVictimExp = 0;
	}

	// �ڻ��� ��� ����ġ �ս��� �ι�
	if (bSuicide) 
	{
		nVictimExp = (int)(CCMatchFormula::GetSuicidePanaltyEXP(nVictimLevel) * fGameExpRatio);
		nAttackerExp = 0;
	}

	// ��ų�ΰ�� ����ġ ����
	if ((pStage->GetStageSetting()->IsTeamPlay()) && (pAttacker->GetTeam() == pVictim->GetTeam()))
	{
		nAttackerExp = 0;
	}


	// ������ ��� ����ġ ���
	if (pStage->IsApplyTeamBonus())
	{
		int nTeamBonus = 0;
		if (pStage->GetRule() != NULL)
		{
			int nNewAttackerExp = nAttackerExp;
			pStage->GetRule()->CalcTeamBonus(pAttacker, pVictim, nAttackerExp, &nNewAttackerExp, &nTeamBonus);
			nAttackerExp = nNewAttackerExp;
		}

		// �� ����ġ ����
		pStage->AddTeamBonus(nTeamBonus, CCMatchTeam(pAttacker->GetTeam()));
	}

	// xp ���ʽ� ����(�ݸ��� PC��, ����ġ ����)
	int nAttackerExpBonus = 0;
	if (nAttackerExp != 0)
	{
		//const float ta = float(atoi("15")) / 100.0f;
		//cclog( "test float : %f\n", ta * 100.0f );

		//CCMatchItemBonusType nBonusType			= GetStageBonusType(pStage->GetStageSetting());
		//const double		dAttackerExp		= static_cast< double >( nAttackerExp );
		//const double		dXPBonusRatio		= static_cast< double >( CCMatchFormula::CalcXPBonusRatio(pAttacker, nBonusType) );
		//const double		dAttackerExpBouns	= dAttackerExp * dXPBonusRatio;
		//const double		dSumAttackerExp		= dAttackerExp + dAttackerExpBouns;
		//
		//
		//nAttackerExpBonus = static_cast< int >( dAttackerExpBouns + 0.00001); 

		CCMatchItemBonusType nBonusType = GetStageBonusType(pStage->GetStageSetting());
		const float fAttackerExpBonusRatio = CCMatchFormula::CalcXPBonusRatio(pAttacker, nBonusType);
		 //�ε��Ҽ��� ���������� ��꿡 ������ ���� �ʴ� �������� ������ ���ش�.
		// ���� �̺κп��� �ٽ� ������ �߻��Ѵٸ� ������ �ƴ� ���̽����� ���� �۾��� �� ��� �Ѵ�.
		 nAttackerExpBonus = (int)(nAttackerExp * (fAttackerExpBonusRatio + 0.00001f));
	}

	*poutAttackerExp = nAttackerExp + nAttackerExpBonus;

	*poutVictimExp = nVictimExp;
}


const int CCMatchServer::CalcBPonGameKill( CCMatchStage* pStage, CCMatchObject* pAttacker, const int nAttackerLevel, const int nVictimLevel )
{
	if( (0 == pStage) || (0 == pAttacker) ) 
		return -1;

	const int	nAddedBP		= static_cast< int >( CCMatchFormula::GetGettingBounty(nAttackerLevel, nVictimLevel) );
	const float fBPBonusRatio	= CCMatchFormula::CalcBPBounsRatio( pAttacker, GetStageBonusType(pStage->GetStageSetting()) );
	const int	nBPBonus		= static_cast< int >( nAddedBP * fBPBonusRatio );

	return nAddedBP + nBPBonus;
}




void CCMatchServer::ProcessPlayerXPBP(CCMatchStage* pStage, CCMatchObject* pPlayer, int nAddedXP, int nAddedBP)
{
	if (pStage == NULL) return;
	if (!IsEnabledObject(pPlayer)) return;

	/*
		����ġ ���
		ĳ���Ϳ� ����ġ ����
		���� ���
		DBĳ�� ������Ʈ
		������,�ٿ� �޼��� ����
	*/

	CCUID uidStage = pPlayer->GetStageUID();
	int nPlayerLevel = pPlayer->GetCharInfo()->m_nLevel;

	// ĳ���� XP ������Ʈ
	pPlayer->GetCharInfo()->IncXP(nAddedXP);

	// ���� ���
	int nNewPlayerLevel = -1;
	if ((pPlayer->GetCharInfo()->m_nLevel < MAX_LEVEL) &&
		(pPlayer->GetCharInfo()->m_nXP >= CCMatchFormula::GetNeedExp(nPlayerLevel)))
	{
		nNewPlayerLevel = CCMatchFormula::GetLevelFromExp(pPlayer->GetCharInfo()->m_nXP);
		if (nNewPlayerLevel != pPlayer->GetCharInfo()->m_nLevel) pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;
	}

	// �ٿ�Ƽ �߰����ش�
	pPlayer->GetCharInfo()->IncBP(nAddedBP);


	// DB ĳ�� ������Ʈ
	if (pPlayer->GetCharInfo()->GetDBCachingData()->IsRequestUpdate()) {
		UpdateCharDBCachingData(pPlayer);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
	}

	// ���� ������ �ٲ�� ���� �������Ѵ�.
	if ((nNewPlayerLevel >= 0) && (nNewPlayerLevel != nPlayerLevel))
	{
		// ������ �ٲ�� �ٷ� ĳ�� ������Ʈ�Ѵ�
		UpdateCharDBCachingData(pPlayer);

		pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;
		if (!m_MatchDBMgr.UpdateCharLevel(pPlayer->GetCharInfo()->m_nCID, 
										  nNewPlayerLevel, 
										  pPlayer->GetCharInfo()->m_nBP,
										  pPlayer->GetCharInfo()->m_nTotalKillCount, 
										  pPlayer->GetCharInfo()->m_nTotalDeathCount,
										  pPlayer->GetCharInfo()->m_nTotalPlayTimeSec,
										  true))
		{
			cclog("DB UpdateCharLevel Error : %s\n", pPlayer->GetCharInfo()->m_szName);
		}
	}

	// ������, ���� �ٿ� �޼��� ������
	if (nNewPlayerLevel > 0)
	{
		if (nNewPlayerLevel > nPlayerLevel)
		{
			CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_LEVEL_UP, CCUID(0,0));
			pCmd->AddParameter(new CCCommandParameterUID(pPlayer->GetUID()));
			pCmd->AddParameter(new CCCommandParameterInt(nNewPlayerLevel));
			RouteToBattle(uidStage, pCmd);	
		}
		else if (nNewPlayerLevel < nPlayerLevel)
		{
			CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_LEVEL_DOWN, CCUID(0,0));
			pCmd->AddParameter(new CCCommandParameterUID(pPlayer->GetUID()));
			pCmd->AddParameter(new CCCommandParameterInt(nNewPlayerLevel));
			RouteToBattle(uidStage, pCmd);	
		}
	}
}

// �� ���ʽ� ����
void CCMatchServer::ApplyObjectTeamBonus(CCMatchObject* pObject, int nAddedExp)
{
	if (!IsEnabledObject(pObject)) return;
	if (nAddedExp <= 0)
	{
		_ASSERT(0);
		return;
	}
	
	bool bIsLevelUp = false;

	// ���ʽ� ����
	if (nAddedExp != 0)
	{
		int nExpBonus = (int)(nAddedExp * CCMatchFormula::CalcXPBonusRatio(pObject, MIBT_TEAM));
		nAddedExp += nExpBonus;
	}




	// ĳ���� XP ������Ʈ
	pObject->GetCharInfo()->IncXP(nAddedExp);

	// ���� ���
	int nNewLevel = -1;
	int nCurrLevel = pObject->GetCharInfo()->m_nLevel;

	if (nNewLevel > nCurrLevel) bIsLevelUp = true;

	if ((pObject->GetCharInfo()->m_nLevel < MAX_LEVEL) &&
		(pObject->GetCharInfo()->m_nXP >= CCMatchFormula::GetNeedExp(nCurrLevel)))
	{
		nNewLevel = CCMatchFormula::GetLevelFromExp(pObject->GetCharInfo()->m_nXP);
		if (nNewLevel != nCurrLevel) pObject->GetCharInfo()->m_nLevel = nNewLevel;
	}

	// DB ĳ�� ������Ʈ
	if (pObject->GetCharInfo()->GetDBCachingData()->IsRequestUpdate())
	{
		UpdateCharDBCachingData(pObject);
	}

	// ���� ������ �ٲ�� �ٷ� �������Ѵ�.
	if ((nNewLevel >= 0) && (nNewLevel != nCurrLevel))
	{
		// ������ �ٲ�� �ٷ� ĳ�� ������Ʈ�Ѵ�
		UpdateCharDBCachingData(pObject);

		pObject->GetCharInfo()->m_nLevel = nNewLevel;
		nCurrLevel = nNewLevel;

		if (!m_MatchDBMgr.UpdateCharLevel(pObject->GetCharInfo()->m_nCID, 
			                              nNewLevel,
										  pObject->GetCharInfo()->m_nBP,
										  pObject->GetCharInfo()->m_nTotalKillCount,
										  pObject->GetCharInfo()->m_nTotalDeathCount,
										  pObject->GetCharInfo()->m_nTotalPlayTimeSec,
										  bIsLevelUp
										  ))
		{
			cclog("DB UpdateCharLevel Error : %s\n", pObject->GetCharInfo()->m_szName);
		}
	}


	CCUID uidStage = pObject->GetStageUID();

	unsigned long int nExpArg;
	unsigned long int nChrExp;
	int nPercent;

	nChrExp = pObject->GetCharInfo()->m_nXP;
	nPercent = CCMatchFormula::GetLevelPercent(nChrExp, nCurrLevel);
	// ���� 2����Ʈ�� ����ġ, ���� 2����Ʈ�� ����ġ�� �ۼ�Ʈ�̴�.
	nExpArg = MakeExpTransData(nAddedExp, nPercent);


	CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_TEAMBONUS, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(pObject->GetUID()));
	pCmd->AddParameter(new CCCommandParameterUInt(nExpArg));
	RouteToBattle(uidStage, pCmd);	


	// ������ �޼��� ������
	if ((nNewLevel >= 0) && (nNewLevel > nCurrLevel))
	{
		CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_LEVEL_UP, CCUID(0,0));
		pCmd->AddParameter(new CCCommandParameterUID(pObject->GetUID()));
		pCmd->AddParameter(new CCCommandParameterInt(nNewLevel));
		RouteToBattle(uidStage, pCmd);	
	}
}

// �÷��� �� ĳ���� ���� ������Ʈ
void CCMatchServer::ProcessCharPlayInfo(CCMatchObject* pPlayer)
{
	if (!IsEnabledObject(pPlayer)) return;

	/*
	���Ҷ����� ĳ���� ������ ������Ʈ ����
	����ġ ���
	ĳ���Ϳ� ����ġ ����
	���� ���
	������,�ٿ� �޼��� ����
	�ٿ�Ƽ �߰����ش�
	���ӽð�, ���� ����ð�, �÷��� �ð�
	*/

	CCUID uidStage = pPlayer->GetStageUID();
	int nPlayerLevel = pPlayer->GetCharInfo()->m_nLevel;

	// ���� ���
	int nNewPlayerLevel = -1;
	if ((pPlayer->GetCharInfo()->m_nLevel < MAX_LEVEL) &&
		(pPlayer->GetCharInfo()->m_nXP >= CCMatchFormula::GetNeedExp(nPlayerLevel)))
	{
		nNewPlayerLevel = CCMatchFormula::GetLevelFromExp(pPlayer->GetCharInfo()->m_nXP);
		if (nNewPlayerLevel != pPlayer->GetCharInfo()->m_nLevel) pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;
	}
	// ���� ������ �ٲ�� ���� �������Ѵ�.
	if ((nNewPlayerLevel >= 0) && (nNewPlayerLevel != nPlayerLevel))
		pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;

	// ���ӽð�, ���� ����ð�, �÷��� �ð�
	unsigned long int nNowTime = CCMatchServer::GetInstance()->GetGlobalClockCount();
	unsigned long int nBattlePlayingTimeSec = 0;
	if(pPlayer->GetCharInfo()->m_nBattleStartTime != 0)
	{
		nBattlePlayingTimeSec = CCGetTimeDistance(pPlayer->GetCharInfo()->m_nBattleStartTime, nNowTime) / 1000;	// ��Ʋ�� ������ �ð�
		
		/*
		// �ƹ� ó���� ���� �ʴµ�, �� �α״� ����°ǰ���? �ϴ� �ּ� ó���մϴ�. - carrot318
		if(nBattlePlayingTimeSec > 60*60)
		{// �̻������� ���� ���õŸ� �α׸� �����.
			CTime theTime = CTime::GetCurrentTime();
			CString szTime = theTime.Format( "[%c] " );

			// ���� ���
			char buf[64]={0,};
			CCMatchStage* pStage = FindStage(uidStage);

			if( pStage != NULL )
			{
				switch((int)pStage->GetStageSetting()->GetGameType())
				{
				case CCMATCH_GAMETYPE_DEATHMATCH_SOLO:	{sprintf(buf, "DEATHMATCH_SOLO");	} break;		///< ���� ������ġ
				case CCMATCH_GAMETYPE_DEATHMATCH_TEAM:	{sprintf(buf, "DEATHMATCH_TEAM");	} break;		///< �� ������ġ
				case CCMATCH_GAMETYPE_GLADIATOR_SOLO:	{sprintf(buf, "GLADIATOR_SOLO");	} break;		///< ���� �۷�������
				case CCMATCH_GAMETYPE_GLADIATOR_TEAM:	{sprintf(buf, "GLADIATOR_TEAM");	} break;		///< �� �۷�������
				case CCMATCH_GAMETYPE_ASSASSINATE:		{sprintf(buf, "ASSASSINATE");		} break;		///< ������
				case CCMATCH_GAMETYPE_TRAINING:			{sprintf(buf, "TRAINING");			} break;		///< ����

				case CCMATCH_GAMETYPE_SURVIVAL:			{sprintf(buf, "SURVIVAL");			} break;		///< �����̹�
				case CCMATCH_GAMETYPE_QUEST:				{sprintf(buf, "QUEST");				} break;		///< ����Ʈ

				case CCMATCH_GAMETYPE_BERSERKER:			{sprintf(buf, "BERSERKER");			} break;		
				case CCMATCH_GAMETYPE_DEATHMATCH_TEAM2:	{sprintf(buf, "DEATHMATCH_TEAM2");	} break;		
				case CCMATCH_GAMETYPE_DUEL:				{sprintf(buf, "DUEL");				} break;	
				default:								{sprintf(buf, "don't know");		} break;
				}
				cclog("%s BattlePlayT Error GameMode:%s, CID:%d, Name:%s, ServerCurrT:%u, BattleStartT:%u, PlayT:%d, PlayerConnectT:%u \n"
					, szTime, buf, pPlayer->GetCharInfo()->m_nCID, pPlayer->GetCharInfo()->m_szName, nNowTime, pPlayer->GetCharInfo()->m_nBattleStartTime, nBattlePlayingTimeSec, pPlayer->GetCharInfo()->m_nConnTime);
			}
		}
		*/
		//pPlayer->GetCharInfo()->m_nBattleStartTime = 0;
	}
	unsigned long int nLoginTotalTimeSec = CCGetTimeDistance(pPlayer->GetCharInfo()->m_nConnTime, nNowTime) / 1000;	// ������ ������ �ð�

	// �̻������� ����ġ�� ȹ�������� �α׸� �����ش�.
	// �ƹ� ó���� ���� �ʴµ�, �� �α״� ����°ǰ���? �ϴ� �ּ� ó���մϴ�. - carrot318
	/*
	long int nBattleEXPGained = pPlayer->GetCharInfo()->m_nXP - pPlayer->GetCharInfo()->m_nBattleStartXP;
	if(nBattleEXPGained < -150000 || 150000 < nBattleEXPGained)
	{
		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format( "[%c] " );
		cclog("%s BattleXPGained Error CID:%d, Name:%s, StartXP:%d, EXPGained:%d \n", szTime, pPlayer->GetCharInfo()->m_nCID, pPlayer->GetCharInfo()->m_szName, pPlayer->GetCharInfo()->m_nBattleStartXP, nBattleEXPGained);
	}
	*/

#ifdef LOCALE_NHNUSA
	if (!m_MatchDBMgr.UpdateCharPlayInfo(pPlayer->GetAccountInfo()->m_nAID
										, pPlayer->GetCharInfo()->m_nCID
										, pPlayer->GetCharInfo()->m_nXP
										, pPlayer->GetCharInfo()->m_nLevel
										, nBattlePlayingTimeSec										// ��Ʋ �ð�
										, nLoginTotalTimeSec										// �Ѱ����� ������ �ð�
										, pPlayer->GetCharInfo()->m_nTotalKillCount
										, pPlayer->GetCharInfo()->m_nTotalDeathCount
										, pPlayer->GetCharInfo()->m_nBP
										, false))
	{
		cclog("DB UpdateCharPlayInfo Error : %s\n", pPlayer->GetCharInfo()->m_szName);
	}
#endif

}

void CCMatchServer::PostGameDeadOnGameKill(CCUID& uidStage, CCMatchObject* pAttacker, CCMatchObject* pVictim,
									int nAddedAttackerExp, int nSubedVictimExp)
{
	unsigned long int nAttackerArg = 0;
	unsigned long int nVictimArg =0;

	int nRealAttackerLevel = pAttacker->GetCharInfo()->m_nLevel;
	int nRealVictimLevel = pVictim->GetCharInfo()->m_nLevel;

	unsigned long int nChrExp;
	int nPercent;

	nChrExp = pAttacker->GetCharInfo()->m_nXP;
	nPercent = CCMatchFormula::GetLevelPercent(nChrExp, nRealAttackerLevel);
	nAttackerArg = MakeExpTransData(nAddedAttackerExp, nPercent);

	nChrExp = pVictim->GetCharInfo()->m_nXP;
	nPercent = CCMatchFormula::GetLevelPercent(nChrExp, nRealVictimLevel);
	nVictimArg = MakeExpTransData(nSubedVictimExp, nPercent);

	CCCommand* pCmd = CreateCommand(MC_MATCH_GAME_DEAD, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(pAttacker->GetUID()));
	pCmd->AddParameter(new CCCommandParameterUInt(nAttackerArg));
	pCmd->AddParameter(new CCCommandParameterUID(pVictim->GetUID()));
	pCmd->AddParameter(new CCCommandParameterUInt(nVictimArg));
	RouteToBattle(uidStage, pCmd);	
}

void CCMatchServer::StageList(const CCUID& uidPlayer, int nStageStartIndex, bool bCacheUpdate)
{
	CCMatchObject* pChar = GetObject(uidPlayer);
	if (pChar == NULL) return;
	CCMatchChannel* pChannel = FindChannel(pChar->GetChannelUID());
	if (pChannel == NULL) return;

	// Ŭ�������ε� Ŭ��ä���� ��쿡�� �� ����Ʈ��� ����� Ŭ�� ����Ʈ�� ������.
	if ((CCGetServerConfig()->GetServerMode() == CSM_CLAN) && (pChannel->GetChannelType() == CCCHANNEL_TYPE_CLAN))
	{
		StandbyClanList(uidPlayer, nStageStartIndex, bCacheUpdate);
		return;
	}


	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LIST), CCUID(0,0), m_This);

	int nPrevStageCount = -1, nNextStageCount = -1;
	int nNextStageIndex = pChannel->GetMaxPlayers()-1;


	// 2008.09.16 
	int nRealStageStartIndex = nStageStartIndex;
	int nStageCount = 0;
	for(int i = 0; i < pChannel->GetMaxPlayers(); i++)
	{
		// ���� ��������� ó�����Ѵ�
		if (pChannel->IsEmptyStage(i)) continue;
		// ���� ������ ó��
		if(nStageCount < nStageStartIndex) // �տ� �ǿ� ó���� ��� < ���� �ǿ��� ������ �� Index
			nStageCount++;
		else
		{
			nRealStageStartIndex = i;
			break;
		}
	}

	int nRealStageCount = 0;
	for (int i = /*nStageStartIndex*/nRealStageStartIndex; i < pChannel->GetMaxPlayers(); i++)
	{
		if (pChannel->IsEmptyStage(i)) continue;

		CCMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;

		nRealStageCount++;
		if (nRealStageCount >= TRANS_STAGELIST_NODE_COUNT) 
		{
			nNextStageIndex = i;
			break;
		}
	}

	if (!bCacheUpdate)
	{
		nPrevStageCount = pChannel->GetPrevStageCount(nStageStartIndex);
		nNextStageCount = pChannel->GetNextStageCount(nNextStageIndex);
	}

	pNew->AddParameter(new CCCommandParameterChar((char)nPrevStageCount));
	pNew->AddParameter(new CCCommandParameterChar((char)nNextStageCount));


	void* pStageArray = CCMakeBlobArray(sizeof(CCTD_StageListNode), nRealStageCount);
	int nArrayIndex=0;

	for (int i = /*nStageStartIndex*/nRealStageStartIndex; i < pChannel->GetMaxPlayers(); i++)
	{
		if (pChannel->IsEmptyStage(i)) continue;
		CCMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;
		
		if( pStage->GetState() < STAGE_STATE_STANDBY || pStage->GetState() > STAGE_STATE_COUNT )
		{
			/* ũ������ ���� ����ڵ� �߰�. ���߿� pChannel->m_pStages �迭�� �ƴ� ���̳� �ٸ��ɷ� �����丵�ʿ�*/
			LOG(LOG_FILE, "there is unavailable stages in %s channel. No:%d \n", pChannel->GetName(), i);
			continue;
		}


		if (nArrayIndex >= nRealStageCount) break;

		CCTD_StageListNode* pNode = (CCTD_StageListNode*)CCGetBlobArrayElement(pStageArray, nArrayIndex++);
		pNode->uidStage = pStage->GetUID();
		strcpy(pNode->szStageName, pStage->GetName());
		pNode->nNo = (unsigned char)(pStage->GetIndex() + 1);	// ����ڿ��� �����ִ� �ε����� 1���� �����Ѵ�
		pNode->nPlayers = (char)pStage->GetPlayers();
		pNode->nMaxPlayers = pStage->GetStageSetting()->GetMaxPlayers();
		pNode->nState = pStage->GetState();
		pNode->nGameType = pStage->GetStageSetting()->GetGameType();
		
		// �����̸� �κ� �渮��Ʈ ��ʸ� �����̸����� �������ش�.
		if(pStage->IsRelayMap()) pNode->nMapIndex = CCMATCH_MAP_RELAYMAP;
		else		 			 pNode->nMapIndex = pStage->GetStageSetting()->GetMapIndex();
		
		pNode->nSettingFlag = 0;
		// ����
		if (pStage->GetStageSetting()->GetForcedEntry())
		{
			pNode->nSettingFlag |= MSTAGENODE_FLAG_FORCEDENTRY_ENABLED;
		}
		// ��й�
		if (pStage->IsPrivate())
		{
			pNode->nSettingFlag |= MSTAGENODE_FLAG_PRIVATE;
		}
		// ��������
		pNode->nLimitLevel = pStage->GetStageSetting()->GetLimitLevel();
		pNode->nMasterLevel = 0;

		if (pNode->nLimitLevel != 0)
		{
			pNode->nSettingFlag |= MSTAGENODE_FLAG_LIMITLEVEL;

			;
			CCMatchObject* pMaster = GetObject(pStage->GetMasterUID());
			if (pMaster)
			{
				if (pMaster->GetCharInfo())
				{
					pNode->nMasterLevel = pMaster->GetCharInfo()->m_nLevel;
				}
			}
		}
	}

	pNew->AddParameter(new CCCommandParameterBlob(pStageArray, CCGetBlobArraySize(pStageArray)));
	CCEraseBlobArray(pStageArray);

	RouteToListener(pChar, pNew);	
}


void CCMatchServer::OnStageRequestStageList(const CCUID& uidPlayer, const CCUID& uidChannel, const int nStageCursor)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;

	pObj->SetStageCursor(nStageCursor);
	StageList(pObj->GetUID(), nStageCursor, false);
}


void CCMatchServer::OnRequestQuickJoin(const CCUID& uidPlayer, void* pQuickJoinBlob)
{
	CCTD_QuickJoinParam* pNode = (CCTD_QuickJoinParam*)CCGetBlobArrayElement(pQuickJoinBlob, 0);
	ResponseQuickJoin(uidPlayer, pNode);
}

void CCMatchServer::ResponseQuickJoin(const CCUID& uidPlayer, CCTD_QuickJoinParam* pQuickJoinParam)
{
	if (pQuickJoinParam == NULL) return;

	CCMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return;
	CCMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pChannel == NULL) return;

	list<CCUID>	recommended_stage_list;
	CCUID uidRecommendedStage = CCUID(0,0);
	int nQuickJoinResult = MOK;


	for (int i = 0; i < pChannel->GetMaxStages(); i++)
	{
		if (pChannel->IsEmptyStage(i)) continue;
		CCMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;

		int ret = ValidateStageJoin(pObj->GetUID(), pStage->GetUID());
		if (ret == MOK)
		{
			if (pStage->IsPrivate()) continue;

			int nMapIndex = pStage->GetStageSetting()->GetMapIndex();
			int nGameType = pStage->GetStageSetting()->GetGameType();

			if (!CheckBitSet(pQuickJoinParam->nMapEnum, nMapIndex)) continue;
			if (!CheckBitSet(pQuickJoinParam->nModeEnum, nGameType)) continue;

			//if (((1 << nMapIndex) & (pQuickJoinParam->nMapEnum)) == 0) continue;
			//if (((1 << nGameType) & (pQuickJoinParam->nModeEnum)) == 0) continue;

			recommended_stage_list.push_back(pStage->GetUID());
		}
	}

	if (!recommended_stage_list.empty())
	{
		int nSize=(int)recommended_stage_list.size();
		int nIndex = rand() % nSize;

		int nCnt = 0;
		for (list<CCUID>::iterator itor = recommended_stage_list.begin(); itor != recommended_stage_list.end(); ++itor)
		{
			if (nIndex == nCnt)
			{
				uidRecommendedStage = (*itor);
				break;
			}
			nCnt++;
		}
	}
	else
	{
		nQuickJoinResult = MERR_CANNOT_NO_STAGE;
	}

	CCCommand* pCmd = CreateCommand(MC_MATCH_STAGE_RESPONSE_QUICKJOIN, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterInt(nQuickJoinResult));
	pCmd->AddParameter(new CCCommandParameterUID(uidRecommendedStage));
	RouteToListener(pObj, pCmd);	
}

static int __cdecl _int_sortfunc(const void* a, const void* b)
{
	return *((int*)a) - *((int*)b);
}


int CCMatchServer::GetLadderTeamIDFromDB(const int nTeamTableIndex, const int* pnMemberCIDArray, const int nMemberCount)
{
	if ((nMemberCount <= 0) || (nTeamTableIndex != nMemberCount))
	{
		_ASSERT(0);
		return 0;
	}

	// cid ������������ ���� - db�� ���õǾ� ���ִ�. 
	int* pnSortedCIDs = new int[nMemberCount];
	for (int i = 0; i < nMemberCount; i++)
	{
		pnSortedCIDs[i] = pnMemberCIDArray[i];
	}
	qsort(pnSortedCIDs, nMemberCount, sizeof(int), _int_sortfunc);

	int nTID = 0;
	if (pnSortedCIDs[0] != 0)
	{
		if (!m_MatchDBMgr.GetLadderTeamID(nTeamTableIndex, pnSortedCIDs, nMemberCount, &nTID))
		{
			nTID = 0;
		}
	}
	

	delete[] pnSortedCIDs;

	return nTID;
}

void CCMatchServer::SaveLadderTeamPointToDB(const int nTeamTableIndex, const int nWinnerTeamID, const int nLoserTeamID, const bool bIsDrawGame)
{
	// ����Ʈ ��� - �׼Ǹ��� ����
	int nWinnerPoint = 0, nLoserPoint = 0, nDrawPoint = 0;

	nLoserPoint = -1;
	switch (nTeamTableIndex)
	{
	case 2:	// 2��2
		{
			nWinnerPoint = 4;
			nDrawPoint = 1;
		}
		break;
	case 3:
		{
			nWinnerPoint = 6;
			nDrawPoint = 1;
		}
		break;
	case 4:
		{
			nWinnerPoint = 10;
			nDrawPoint = 2;
		}
		break;
	}

	if (!m_MatchDBMgr.LadderTeamWinTheGame(nTeamTableIndex, nWinnerTeamID, nLoserTeamID, bIsDrawGame,
		                                   nWinnerPoint, nLoserPoint, nDrawPoint))
	{
		cclog("DB Query(SaveLadderTeamPointToDB) Failed\n");
	}
}


void CCMatchServer::OnVoteCallVote(const CCUID& uidPlayer, const char* pszDiscuss, const char* pszArg)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	// ��ڰ� ������ǥ�ϸ� ������ ����
	if (IsAdminGrade(pObj)) {
		CCMatchStage* pStage = FindStage(pObj->GetStageUID());
		if (pStage)
			pStage->KickBanPlayer(pszArg, false);
		return;
	}

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	char szMsg[256];
	// ��ڰ� ���� �������̸� ��ǥ �Ұ���
	for (CCUIDRefCache::iterator itor = pStage->GetObjBegin(); itor != pStage->GetObjEnd(); itor++) {
		CCUID uidObj = (CCUID)(*itor).first;
		CCMatchObject* pPlayer = (CCMatchObject*)GetObject(uidObj);
		if ((pPlayer) && (IsAdminGrade(pPlayer)))
		{
			sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE);
			Announce(uidPlayer, szMsg);

			return;
		}
	}


	if( pObj->WasCallVote() )
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE);
		Announce(uidPlayer, szMsg);

		return;
	}

	// ��ǥ�� �ߴٴ°� ǥ���س���.
	pObj->SetVoteState( true );

	if (pStage->GetStageType() == CCST_LADDER)
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE_LADERGAME);
		Announce(uidPlayer, szMsg);

		return;
	}

	if (pStage->GetRule() && pStage->GetRule()->GetGameType() == CCMATCH_GAMETYPE_DUELTOURNAMENT)
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE);
		Announce(uidPlayer, szMsg);

		return;
	}
#ifdef _VOTESETTING
	// �� ������ ��ǥ����� �˻���.
	if( !pStage->GetStageSetting()->bVoteEnabled ) {
		VoteAbort( uidPlayer );
		return;
	}

	// �̹� ���ӿ��� ��ǥ�� �����ߴ��� �˻�.
	if( pStage->WasCallVote() ) {
		VoteAbort( uidPlayer );
		return;
	}
	else {
		pStage->SetVoteState( true );
	}
#endif

	if (pStage->GetVoteMgr()->GetDiscuss())
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_VOTE_ALREADY_START);
		Announce(uidPlayer, szMsg);

		return;
	}

	CCVoteDiscuss* pDiscuss = CCVoteDiscussBuilder::Build(uidPlayer, pStage->GetUID(), pszDiscuss, pszArg);
	if (pDiscuss == NULL) return;

	if (pStage->GetVoteMgr()->CallVote(pDiscuss)) {
		pDiscuss->Vote(uidPlayer, MVOTE_YES);	// ������ ������ ����

		CCCommand* pCmd = CreateCommand(MC_MATCH_NOTIFY_CALLVOTE, CCUID(0,0));
		pCmd->AddParameter(new CCCmdParamStr(pszDiscuss));
		pCmd->AddParameter(new CCCmdParamStr(pszArg));
		RouteToStage(pStage->GetUID(), pCmd);
		return;
	}
	else
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_VOTE_FAILED);
		Announce(uidPlayer, szMsg);

		return;
	}
}

void CCMatchServer::OnVoteYes(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	CCVoteDiscuss* pDiscuss = pStage->GetVoteMgr()->GetDiscuss();
    if (pDiscuss == NULL) return;

	pDiscuss->Vote(uidPlayer, MVOTE_YES);
}

void CCMatchServer::OnVoteNo(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	CCVoteDiscuss* pDiscuss = pStage->GetVoteMgr()->GetDiscuss();
    if (pDiscuss == NULL) return;

	pDiscuss->Vote(uidPlayer, MVOTE_NO);
}

void CCMatchServer::VoteAbort( const CCUID& uidPlayer )
{
#ifndef MERR_CANNOT_VOTE
#define MERR_CANNOT_VOTE 120000
#endif

	CCMatchObject* pObj = GetObject( uidPlayer );
	if( 0 == pObj )
		return;

	CCCommand* pCmd = CreateCommand( MC_MATCH_VOTE_RESPONSE, CCUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new CCCommandParameterInt(MERR_CANNOT_VOTE) );
	RouteToListener( pObj, pCmd );
}



void CCMatchServer::OnEventChangeMaster(const CCUID& uidAdmin)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj )
		return;

	if (!IsEnabledObject(pObj)) return;

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ������ ���´�.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	if (pStage->GetMasterUID() == uidAdmin)
		return;

	pStage->SetMasterUID(uidAdmin);
	StageMaster(pStage->GetUID());
}

void CCMatchServer::OnEventChangePassword(const CCUID& uidAdmin, const char* pszPassword)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj ) 
		return;

	if (!IsEnabledObject(pObj)) return;

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ������ ���´�.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	pStage->SetPassword(pszPassword);
	pStage->SetPrivate(true);
}

void CCMatchServer::OnEventRequestJjang(const CCUID& uidAdmin, const char* pszTargetName)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj )
		return;

	if (!IsEnabledObject(pObj)) return;

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ����
	if (!IsAdminGrade(pObj))
	{
		return;
	}

	CCMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;
	if (IsAdminGrade(pTargetObj)) return;		// ���� ������� ¯�Ұ�
	if (CCMUGSTAR == pTargetObj->GetAccountInfo()->m_nUGrade) return;	// �̹� ¯

	pTargetObj->GetAccountInfo()->m_nUGrade = CCMUGSTAR;

	if (m_MatchDBMgr.EventJjangUpdate(pTargetObj->GetAccountInfo()->m_nAID, true)) {
		CCMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pTargetObj);
		CCCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REPLACE, this);
		RouteToStage(pStage->GetUID(), pCmdCacheUpdate);

		CCCommand* pCmdUIUpdate = CreateCommand(MC_EVENT_UPDATE_JJANG, CCUID(0,0));
		pCmdUIUpdate->AddParameter(new CCCommandParameterUID(pTargetObj->GetUID()));
		pCmdUIUpdate->AddParameter(new CCCommandParameterBool(true));
		RouteToStage(pStage->GetUID(), pCmdUIUpdate);
	}
}

void CCMatchServer::OnEventRemoveJjang(const CCUID& uidAdmin, const char* pszTargetName)
{
	CCMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj )
		return;

	if (!IsEnabledObject(pObj)) return;

	CCMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ������ ���´�.
	if (!IsAdminGrade(pObj))
	{
		return;
	}
	
	CCMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;			// ���� ������� ¯�Ұ�

	pTargetObj->GetAccountInfo()->m_nUGrade = CCMUGFREE;

	if (m_MatchDBMgr.EventJjangUpdate(pTargetObj->GetAccountInfo()->m_nAID, false)) {
		CCMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pTargetObj);
		CCCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REPLACE, this);
		RouteToStage(pStage->GetUID(), pCmdCacheUpdate);

		CCCommand* pCmdUIUpdate = CreateCommand(MC_EVENT_UPDATE_JJANG, CCUID(0,0));
		pCmdUIUpdate->AddParameter(new CCCommandParameterUID(pTargetObj->GetUID()));
		pCmdUIUpdate->AddParameter(new CCCommandParameterBool(false));
		RouteToStage(pStage->GetUID(), pCmdUIUpdate);
	}
}


void CCMatchServer::OnStageGo(const CCUID& uidPlayer, unsigned int nRoomNo)
{
	CCMatchObject* pChar = GetObject(uidPlayer);
	if( 0 == pChar ) return;
	if (!IsEnabledObject(pChar)) return;
	if (pChar->GetPlace() != MMP_LOBBY) return;
	CCMatchChannel* pChannel = FindChannel(pChar->GetChannelUID());
	if (pChannel == NULL) return;

	CCMatchStage* pStage = pChannel->GetStage(nRoomNo-1);
	if (pStage) {
		CCCommand* pNew = CreateCommand(MC_MATCH_REQUEST_STAGE_JOIN, GetUID());
		pNew->SetSenderUID(uidPlayer);	// �÷��̾ ���� �޽����� ��ó�� ����
		pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
		pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
		Post(pNew);
	}
}



void CCMatchServer::OnDuelQueueInfo(const CCUID& uidStage, const CCTD_DuelQueueInfo& QueueInfo)
{
	CCCommand* pCmd = CreateCommand(MC_MATCH_DUEL_QUEUEINFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamBlob(&QueueInfo, sizeof(CCTD_DuelQueueInfo)));
	RouteToBattle(uidStage, pCmd);
}


void CCMatchServer::OnQuestSendPing(const CCUID& uidStage, unsigned long int t)
{
	CCCommand* pCmd = CreateCommand(MC_QUEST_PING, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUInt(t));
	RouteToBattle(uidStage, pCmd);
}

void CCMatchServer::SaveGameLog(const CCUID& uidStage)
{
	CCMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	int nMapID		= pStage->GetStageSetting()->GetMapIndex();
	int nGameType	= (int)pStage->GetStageSetting()->GetGameType();
	
	

	// test �ʵ��� �α� ������ �ʴ´�.
	if ( (MGetMapDescMgr()->MIsCorrectMap(nMapID)) && (MGetGameTypeMgr()->IsCorrectGameType(nGameType)) )
	{
		if (pStage->GetStageType() != CCST_LADDER)
		{
			CCMatchObject* pMaster = GetObject(pStage->GetMasterUID());

			CCAsyncDBJob_InsertGameLog* pJob = new CCAsyncDBJob_InsertGameLog(uidStage);
			pJob->Input(pMaster == NULL ? 0 : pMaster->GetCharInfo()->m_nCID,
				MGetMapDescMgr()->GetMapName(nMapID), 
				MGetGameTypeMgr()->GetInfo(CCMATCH_GAMETYPE(nGameType))->szGameTypeStr);
			PostAsyncJob(pJob);
		}
	}

}

void CCMatchServer::SaveGamePlayerLog(CCMatchObject* pObj, unsigned int nStageID)
{	
	if( pObj == NULL ) return;
	if( nStageID == 0 ) return;
	if( pObj->GetCharInfo() == NULL ) return;

	CCAsyncDBJob_InsertGamePlayerLog* pJob = new CCAsyncDBJob_InsertGamePlayerLog;
	pJob->Input(nStageID, pObj->GetCharInfo()->m_nCID,
		(GetGlobalClockCount() - pObj->GetCharInfo()->m_nBattleStartTime) / 1000,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nKillCount,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nDeathCount,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nXP,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nBP);
	PostAsyncJob(pJob);
}