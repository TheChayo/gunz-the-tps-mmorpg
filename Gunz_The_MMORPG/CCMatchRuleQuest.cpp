#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCMatchRuleQuest.h"
#include "CCQuestLevel.h"
#include "CCQuestLevelGenerator.h"
#include "CCBlobArray.h"
#include "CCQuestFormula.h"
#include "CCCommandCommunicator.h"
#include "CCSharedCommandTable.h"
#include "CCMatchTransDataType.h"
#include "CCMatchConfig.h"
#include "CCMatchFormula.h"
#include "CCQuestItem.h"
#include "CCMATH.H"
#include "CCAsyncDBJob.h"
#include "CCQuestNPCSpawnTrigger.h"
#include "CCQuestItem.h"

CCMatchRuleQuest::CCMatchRuleQuest(CCMatchStage* pStage) : CCMatchRuleBaseQuest(pStage), m_pQuestLevel(NULL),
														m_nCombatState(CCQUEST_COMBAT_NONE), m_nPrepareStartTime(0),
														m_nCombatStartTime(0), m_nQuestCompleteTime(0), m_nPlayerCount( 0 )
{
	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
		m_SacrificeSlot[ i ].Release();

	m_StageGameInfo.nQL = 0;
	m_StageGameInfo.nPlayerQL = 0;
	m_StageGameInfo.nMapsetID = 1;
	m_StageGameInfo.nScenarioID = CCMatchServer::GetInstance()->GetQuest()->GetScenarioCatalogue()->GetDefaultStandardScenarioID();
}

CCMatchRuleQuest::~CCMatchRuleQuest()
{
	ClearQuestLevel();
}

// Route 씨리즈 시작 /////////////////////////////////////////////////////////////////
void CCMatchRuleQuest::RouteMapSectorStart()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_START, CCUID(0,0));
	char nSectorIndex = char(m_pQuestLevel->GetCurrSectorIndex());
	pCmd->AddParameter(new CCCommandParameterChar(nSectorIndex));
	pCmd->AddParameter(new CCCommandParameterUChar(0));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteCombatState()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_COMBAT_STATE, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterChar(char(m_nCombatState)));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteMovetoPortal(const CCUID& uidPlayer)
{
	if (m_pQuestLevel == NULL) return;

	int nCurrSectorIndex = m_pQuestLevel->GetCurrSectorIndex();

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_MOVETO_PORTAL, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterChar(char(nCurrSectorIndex)));
	pCmd->AddParameter(new CCCommandParameterUChar(0));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteReadyToNewSector(const CCUID& uidPlayer)
{
	if (m_pQuestLevel == NULL) return;

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_READYTO_NEWSECTOR, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteObtainQuestItem(unsigned long int nQuestItemID)
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_OBTAIN_QUESTITEM, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamUInt(nQuestItemID));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteObtainZItem(unsigned long int nItemID)
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_OBTAIN_ZITEM, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamUInt(nItemID));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteGameInfo()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_GAME_INFO, CCUID(0,0));

	void* pBlobGameInfoArray = CCMakeBlobArray(sizeof(CCTD_QuestGameInfo), 1);
	CCTD_QuestGameInfo* pGameInfoNode = (CCTD_QuestGameInfo*)CCGetBlobArrayElement(pBlobGameInfoArray, 0);

	if (m_pQuestLevel)
	{
		m_pQuestLevel->Make_MTDQuestGameInfo(pGameInfoNode, CCMATCH_GAMETYPE_QUEST);
	}

	pCmd->AddParameter(new CCCommandParameterBlob(pBlobGameInfoArray, CCGetBlobArraySize(pBlobGameInfoArray)));
	CCEraseBlobArray(pBlobGameInfoArray);

	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteCompleted()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_COMPLETED, CCUID(0,0));

	int nSize = (int)m_PlayerManager.size();
	void* pBlobRewardArray = CCMakeBlobArray(sizeof(CCTD_QuestReward), nSize);

	int idx = 0;
	for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;
		CCTD_QuestReward* pRewardNode = (CCTD_QuestReward*)CCGetBlobArrayElement(pBlobRewardArray, idx);
		idx++;

		pRewardNode->uidPlayer = (*itor).first;
		pRewardNode->nXP = pPlayerInfo->nXP;
		pRewardNode->nBP = pPlayerInfo->nBP;
	}

	pCmd->AddParameter(new CCCommandParameterBlob(pBlobRewardArray, CCGetBlobArraySize(pBlobRewardArray)));
	CCEraseBlobArray(pBlobRewardArray);

	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteFailed()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_FAILED, CCUID(0,0));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteStageGameInfo()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_STAGE_GAME_INFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamChar(char(m_StageGameInfo.nQL)));
	pCmd->AddParameter(new CCCmdParamChar(char(m_StageGameInfo.nMapsetID)));
	pCmd->AddParameter(new CCCmdParamUInt(m_StageGameInfo.nScenarioID));

	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleQuest::RouteSectorBonus(const CCUID& uidPlayer, unsigned long int nEXPValue, unsigned long int nBP)
{
	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject(uidPlayer);	
	if (!IsEnabledObject(pPlayer)) return;

	CCCommand* pNewCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_BONUS, CCUID(0,0));
	pNewCmd->AddParameter(new CCCmdParamCCUID(uidPlayer));
	pNewCmd->AddParameter(new CCCmdParamUInt(nEXPValue));
	pNewCmd->AddParameter(new CCCmdParamUInt(nBP));	// BP용이지만 퀘스트에서는 쓰지 않음
	CCMatchServer::GetInstance()->RouteToListener( pPlayer, pNewCmd );
}

// Route 씨리즈 끝 ///////////////////////////////////////////////////////////////////

void CCMatchRuleQuest::OnBegin()
{
	m_nQuestCompleteTime = 0;

	MakeQuestLevel();

	CCMatchRuleBaseQuest::OnBegin();		// 여기서 게임정보도 보냄 - 순서에 주의

	// 게임을 완료 하였을시 시작할때의 인원수에 따라서 보상을 위해서 현재 유저 수를 저장한다.
	m_nPlayerCount = static_cast< int >( m_PlayerManager.size() );

	// 게임시작하면 슬롯을 모두 비워줘야 함.
	// 희생아이템 로그 정보는 DestroyAllSlot()에서 m_QuestGameLogInfoMgr로 저장.
	DestroyAllSlot();

	// 게임 시작전에 Log에 필요한 정보를 수집함.
	CollectStartingQuestGameLogInfo();

	SetCombatState(CCQUEST_COMBAT_PREPARE);
}

void CCMatchRuleQuest::OnEnd()
{
	ClearQuestLevel();

	CCMatchRuleBaseQuest::OnEnd();
}

bool CCMatchRuleQuest::OnRun()
{
	bool ret = CCMatchRuleBaseQuest::OnRun();
	if (ret == false) return false;

	if (GetRoundState() == CCMATCH_ROUNDSTATE_PLAY)
	{
		CombatProcess();
	}

	return true;
}


// 지금은 좀 꼬여있음.
void CCMatchRuleQuest::CombatProcess()
{
	switch (m_nCombatState)
	{
	case CCQUEST_COMBAT_PREPARE:			// 모두들 섹터로 들어오기를 기다리는 시기
		{
			if (CheckReadytoNewSector())		// 모두 다 섹터에 들어올때까지 PREPARE
			{
				SetCombatState(CCQUEST_COMBAT_PLAY);				
			};
		}
		break;
	case CCQUEST_COMBAT_PLAY:			// 실제 게임 플레이 시기
		{
			COMBAT_PLAY_RESULT nResult = CheckCombatPlay();
			switch(nResult)
			{
			case CPR_PLAYING:
				{
					ProcessCombatPlay();
				}
				break;
			case CPR_COMPLETE:
				{
					if (CheckQuestCompleteDelayTime())
					{
						SetCombatState(CCQUEST_COMBAT_COMPLETED);
					}
				}
				break;
			case CPR_FAILED:
				{
					// 여기까지 오기전에 이 상위 클래스에서 유저의 생존여부를 검사해서 게임을 끝내버림... - by 추교성.
					// OnFail을 OnCheckRoundFinish에서 처리하는 방향으로 수정했음.
					// SetCombatState(CCQUEST_COMBAT_NONE);
					// m_bQuestCompleted = false;
					// OnFailed();
				}
				break;
			};
		}
		break;
	case CCQUEST_COMBAT_COMPLETED:			// 게임이 끝나고 다음 링크로 건너가는 시기
		{
			// 퀘스트 클리어가 아니고 다음 섹터가 남아 있으면 바로 PREPARE상태가 된다.
			if (!m_bQuestCompleted)
			{
                SetCombatState(CCQUEST_COMBAT_PREPARE);
			}
		}
		break;
	};
}


void CCMatchRuleQuest::OnBeginCombatState(CCQuestCombatState nState)
{
#ifdef _DEBUG
	cclog( "Quest state : %d.\n", nState );
#endif

	switch (nState)
	{
	case CCQUEST_COMBAT_PREPARE:
		{
			m_nPrepareStartTime = CCMatchServer::GetInstance()->GetTickTime();
		}
		break;
	case CCQUEST_COMBAT_PLAY:
		{
			m_nCombatStartTime = CCMatchServer::GetInstance()->GetTickTime();
			// 월드아이템 초기화
			m_pStage->m_WorldItemManager.OnRoundBegin();
			m_pStage->m_ActiveTrapManager.Clear();
			m_pStage->ResetPlayersCustomItem();

			RouteMapSectorStart();

			// 모두 부활
			if (m_pQuestLevel->GetCurrSectorIndex() != 0)
				RefreshPlayerStatus();

			// 다음 섹터 이동여부 플래그 끔
			for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
			{
				CCQuestPlayerInfo* pPlayerInfo = (*itor).second;
				pPlayerInfo->bMovedtoNewSector = false;
			}
		}
		break;
	case CCQUEST_COMBAT_COMPLETED:
		{
			if (CheckQuestCompleted())
			{
				OnCompleted();
			}
			else if( !CheckPlayersAlive() )
			{
				// 게임이 중간에 끝남.
				OnFailed();
			}
			else
			{
				OnSectorCompleted();
			}
		}
		break;
	};
}

void CCMatchRuleQuest::OnEndCombatState(CCQuestCombatState nState)
{
	switch (nState)
	{
	case CCQUEST_COMBAT_PREPARE:
		break;
	case CCQUEST_COMBAT_PLAY:
		break;
	case CCQUEST_COMBAT_COMPLETED:
		break;
	};
}

CCMatchRuleQuest::COMBAT_PLAY_RESULT CCMatchRuleQuest::CheckCombatPlay()
{
	// 보스방에서 보스가 죽으면 Complete
	if ( m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
	{
		// 마지막 섹터인지 검사한다.
//		if ( (m_pQuestLevel) && (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1)))
		{
			if ( m_NPCManager.IsBossDie())
				return CPR_COMPLETE;
		}
	}

	// 모든 맙을 다 죽였으면 Complete
	if ((m_pQuestLevel->GetNPCQueue()->IsEmpty()) && (m_NPCManager.GetNPCObjectCount() <= 0))
	{
		return CPR_COMPLETE;
	}

	// 모든 유저가 죽었으면 게임 실패로 설정함.
	if( !CheckPlayersAlive() )
	{
		return CPR_FAILED;
	}

	return CPR_PLAYING;
}

void CCMatchRuleQuest::OnCommand(CCCommand* pCommand)
{
	CCMatchRuleBaseQuest::OnCommand(pCommand);
}


///
// First : 
// Last  : 2005.04.27 추교성.
//
// 희생아이템을 슬롯에 올려놓으면, QL계산과 희생아이템 테이블에서 아이템에 해당하는 테이블이 있는지 검사하기 위해 호출됨.
//  아이템을 슬롯에 올려놓을때는 QL만을 계산을 함. 희생아이템 테이블 검색 결과는 사용되지 않음.
//  게임을 시작할시에는 희생아이템 테이블 검색 결과가 정상일때만 게임을 시작함.
///
bool CCMatchRuleQuest::MakeQuestLevel()
{
	// 이전의 퀘스트 레벨 정보는 제거함.
	if( 0 != m_pQuestLevel )
	{
		delete m_pQuestLevel;
		m_pQuestLevel = 0;
	}

	CCQuestLevelGenerator	LG( GetGameType() );

	LG.BuildPlayerQL(m_StageGameInfo.nPlayerQL);
	LG.BuildMapset(m_StageGameInfo.nMapsetID);
	
	for (int i = 0; i < MAX_SCENARIO_SACRI_ITEM; i++)
	{
		LG.BuildSacriQItem(m_SacrificeSlot[i].GetItemID());
	}

	m_pQuestLevel = LG.MakeLevel();


	// 첫섹터부터 보스방일 수 있으므로..
	InitJacoSpawnTrigger();

	return true;
}

void CCMatchRuleQuest::ClearQuestLevel()
{
	if (m_pQuestLevel)
	{
		delete m_pQuestLevel;
		m_pQuestLevel = NULL;
	}
}




void CCMatchRuleQuest::MoveToNextSector()
{
	// m_pQuestLevel도 다음맵으로 이동해준다.
	m_pQuestLevel->MoveToNextSector(GetGameType());

	for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;
		pPlayerInfo->bMovedtoNewSector = false;
	}

	InitJacoSpawnTrigger();	
}

void CCMatchRuleQuest::InitJacoSpawnTrigger()
{
	// 만약 다음 섹터가 보스섹터이면 JacoTrigger 발동
	if (m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
	{
		int nDice = m_pQuestLevel->GetStaticInfo()->nDice;
		CCQuestScenarioInfoMaps* pMap = &m_pQuestLevel->GetStaticInfo()->pScenario->Maps[nDice];

		SpawnTriggerInfo info;
		
		info.nSpawnNPCCount = pMap->nJacoCount;
		info.nSpawnTickTime = pMap->nJacoSpawnTickTime;
		info.nCurrMinNPCCount = pMap->nJacoMinNPCCount;
		info.nCurrMaxNPCCount = pMap->nJacoMaxNPCCount;

		m_JacoSpawnTrigger.Clear();
		m_JacoSpawnTrigger.BuildCondition(info);

		for (vector<CCQuestScenarioInfoMapJaco>::iterator itor = pMap->vecJacoArray.begin(); itor != pMap->vecJacoArray.end(); ++itor)
		{
			SpawnTriggerNPCInfoNode node;
			node.nNPCID = (*itor).nNPCID;
			node.fRate = (*itor).fRate;

			m_JacoSpawnTrigger.BuildNPCInfo(node);
		}
	}
}

void CCMatchRuleQuest::SetCombatState(CCQuestCombatState nState)
{
	if (m_nCombatState == nState) return;

	OnEndCombatState(m_nCombatState);
	m_nCombatState = nState;
	OnBeginCombatState(m_nCombatState);

	RouteCombatState();
}


bool CCMatchRuleQuest::CheckReadytoNewSector()
{
	// 일정 시간이 지나면 바로 다음 섹터로 이동한다.
	unsigned long nNowTime = CCMatchServer::GetInstance()->GetTickTime();
	if ((nNowTime - m_nPrepareStartTime) > PORTAL_MOVING_TIME)
	{
		return true;
	}

	for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;
		if ((pPlayerInfo->pObject->CheckAlive()) && (pPlayerInfo->bMovedtoNewSector == false)) return false;
	}
	
	return true;
}

// 섹터 클리어
void CCMatchRuleQuest::OnSectorCompleted()
{
	// 섹터 보너스
	CCQuestScenarioInfo* pScenario = m_pQuestLevel->GetStaticInfo()->pScenario;
	if (pScenario)
	{
		int nSectorXP = pScenario->nSectorXP;
		int nSectorBP = pScenario->nSectorBP;

		// 섹터 보너스값 계산
		if (nSectorXP < 0)
		{
			int nSectorCount = (int)m_pQuestLevel->GetStaticInfo()->SectorList.size();
			nSectorXP = CCQuestFormula::CalcSectorXP(pScenario->nXPReward, nSectorCount);
		}
		// 섹터 보너스값 계산
		if (nSectorBP < 0)
		{
			int nSectorCount = (int)m_pQuestLevel->GetStaticInfo()->SectorList.size();
			nSectorBP = CCQuestFormula::CalcSectorXP(pScenario->nBPReward, nSectorCount);
		}

		if ((nSectorXP > 0) || (nSectorBP > 0))
		{
			for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
			{
				int nAddedSectorXP = nSectorXP;
				int nAddedSectorBP = nSectorBP;

				CCMatchObject* pPlayer = (*itor).second->pObject;
				if ((!IsEnabledObject(pPlayer)) || (!pPlayer->CheckAlive())) continue;

				// 경험치, 바운티 보너스 계산
				const float fXPBonusRatio = CCMatchFormula::CalcXPBonusRatio(pPlayer, MIBT_QUEST);
				const float fBPBonusRatio = CCMatchFormula::CalcBPBounsRatio(pPlayer, MIBT_QUEST);
				nAddedSectorXP += (int)(nAddedSectorXP * fXPBonusRatio);
				nAddedSectorBP += (int)(nAddedSectorBP * fBPBonusRatio);

				// 실제 적용
				CCGetMatchServer()->ProcessPlayerXPBP(m_pStage, pPlayer, nAddedSectorXP, nAddedSectorBP);

				// 라우팅
				int nExpPercent = CCMatchFormula::GetLevelPercent(pPlayer->GetCharInfo()->m_nXP, 
																pPlayer->GetCharInfo()->m_nLevel);
				unsigned long int nExpValue = MakeExpTransData(nAddedSectorXP, nExpPercent);
				RouteSectorBonus(pPlayer->GetUID(), nExpValue, nAddedSectorBP);
			}
		}
	}

	// 죽은 사람 부활시킨다.
//	RefreshPlayerStatus();

	MoveToNextSector();
}

// 퀘스트 성공시
void CCMatchRuleQuest::OnCompleted()
{
	CCMatchRuleBaseQuest::OnCompleted();

#ifdef _QUEST_ITEM
	// 여기서 DB로 QuestGameLog생성.
	PostInsertQuestGameLogAsyncJob();	
	SetCombatState(CCQUEST_COMBAT_NONE);
#endif
	
}

// 퀘스트 실패시
void CCMatchRuleQuest::OnFailed()
{
	SetCombatState(CCQUEST_COMBAT_NONE);
	m_bQuestCompleted = false;

	CCMatchRuleBaseQuest::OnFailed();

	PostInsertQuestGameLogAsyncJob();
}

// 퀘스트가 모두 끝났는지 체크
bool CCMatchRuleQuest::CheckQuestCompleted()
{
	if (m_pQuestLevel)
	{
		// 너무 빨리 끝났는지 체크
		unsigned long int nStartTime = GetStage()->GetStartTime();
		unsigned long int nNowTime = CCMatchServer::GetInstance()->GetTickTime();

		// 최소한 각 섹터별 게임 시작 딜레이 * 섹터수만큼은 시간이 흘러야 게임이 끝날 수 있다고 가정함.
		unsigned long int nCheckTime = QUEST_COMBAT_PLAY_START_DELAY * m_pQuestLevel->GetMapSectorCount();

		if (CCGetTimeDistance(nStartTime, nNowTime) < nCheckTime) return false;


		if (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1))
		{
			return true;
		}
	}

	return false;
}

// 마지막 섹터는 아이템을 먹을 수 있도록 딜레이 시간을 둔다.
bool CCMatchRuleQuest::CheckQuestCompleteDelayTime()
{
	if ((m_pQuestLevel) && (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1)))
	{
		unsigned long int nNowTime = CCMatchServer::GetInstance()->GetTickTime();
		if (m_nQuestCompleteTime == 0)
			m_nQuestCompleteTime = nNowTime;
		if (CCGetTimeDistance(m_nQuestCompleteTime, nNowTime) > QUEST_COMPLETE_DELAY)
			return true;

		return false;
	}

	return true;
}

void CCMatchRuleQuest::ProcessCombatPlay()
{
	ProcessNPCSpawn();

}

void CCMatchRuleQuest::MakeNPCnSpawn(CCQUEST_NPC nNPCID, bool bAddQuestDropItem)
{
	CCQuestNPCSpawnType nSpawnType = MNST_MELEE;
	CCQuestNPCInfo* pNPCInfo = CCMatchServer::GetInstance()->GetQuest()->GetNPCInfo(nNPCID);
	if (pNPCInfo)
	{
		nSpawnType = pNPCInfo->GetSpawnType();
		int nPosIndex = m_pQuestLevel->GetRecommendedSpawnPosition(nSpawnType, CCMatchServer::GetInstance()->GetTickTime());

		CCMatchNPCObject* pNPCObject = SpawnNPC(nNPCID, nPosIndex);

		if (pNPCObject)
		{
			// drop item 결정
			CCQuestDropItem item;
			int nDropTableID = pNPCInfo->nDropTableID;
			int nQL = m_pQuestLevel->GetStaticInfo()->nQL;
			CCMatchServer::GetInstance()->GetQuest()->GetDropTable()->Roll(item, nDropTableID, nQL);

			// AddQuestDropItem=false이면 월드아이템만 드롭한다.
			if ((bAddQuestDropItem==true) || (item.nDropItemType == QDIT_WORLDITEM))
			{
				pNPCObject->SetDropItem(&item);

				// 만들어진 아이템은 level에 넣어놓는다.
				if ((item.nDropItemType == QDIT_QUESTITEM) || (item.nDropItemType == QDIT_ZITEM))
				{
					m_pQuestLevel->OnItemCreated((unsigned long int)(item.nID), item.nRentPeriodHour);
				}
			}
		}
	}
}

void CCMatchRuleQuest::ProcessNPCSpawn()
{
	if (CheckNPCSpawnEnable())
	{
		CCQUEST_NPC npc;
		if (m_pQuestLevel->GetNPCQueue()->Pop(npc))
		{
			MakeNPCnSpawn(npc, true);
		}
	}
	else
	{
		// 보스방일 경우 Queue에 있는 NPC들을 모두 스폰시켰으면 Jaco들을 스폰시킨다.
		if (m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
		{
			// 보스가 살아있고 기본적으로 나올 NPC가 다 나온다음에 졸병들 스폰
			if ((m_NPCManager.GetBossCount() > 0) /* && (m_pQuestLevel->GetNPCQueue()->IsEmpty()) */ )
			{
				int nAliveNPCCount = m_NPCManager.GetNPCObjectCount();
				

				if (m_JacoSpawnTrigger.CheckSpawnEnable(nAliveNPCCount))
				{
					int nCount = (int)m_JacoSpawnTrigger.GetQueue().size();
					for (int i = 0; i < nCount; i++)
					{
						CCQUEST_NPC npc = m_JacoSpawnTrigger.GetQueue()[i];
						MakeNPCnSpawn(npc, false);
					}
				}
			}
		}
	}
}


bool CCMatchRuleQuest::CheckNPCSpawnEnable()
{
	if (m_pQuestLevel->GetNPCQueue()->IsEmpty()) return false;

	if (m_NPCManager.GetNPCObjectCount() >= m_pQuestLevel->GetStaticInfo()->nLMT) return false;
	unsigned long int nNowTime = CCMatchServer::GetInstance()->GetTickTime();

	if ((nNowTime - m_nCombatStartTime) < QUEST_COMBAT_PLAY_START_DELAY)
	{
		return false;
	}


	return true;

}

void CCMatchRuleQuest::OnRequestTestSectorClear()
{
	ClearAllNPC();

	SetCombatState(CCQUEST_COMBAT_COMPLETED);
}

void CCMatchRuleQuest::OnRequestTestFinish()
{
	ClearAllNPC();

	m_pQuestLevel->GetDynamicInfo()->nCurrSectorIndex = m_pQuestLevel->GetMapSectorCount()-1;

	SetCombatState(CCQUEST_COMBAT_COMPLETED);
}


void CCMatchRuleQuest::OnRequestMovetoPortal(const CCUID& uidPlayer)
{
//	CCQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidPlayer);

	RouteMovetoPortal(uidPlayer);
}




void CCMatchRuleQuest::OnReadyToNewSector(const CCUID& uidPlayer)
{
	CCQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidPlayer);
	if (pPlayerInfo)
	{
        pPlayerInfo->bMovedtoNewSector = true;
	}

	RouteReadyToNewSector(uidPlayer);
}

bool CCMatchRuleQuest::OnCheckRoundFinish()
{
	return CCMatchRuleBaseQuest::OnCheckRoundFinish();
}

void CCMatchRuleQuest::DistributeReward()
{
	if (!m_pQuestLevel) return;

	// 현재 서버가 퀘스트 서버일 경우에만 가능하게 함.
	if( CSM_TEST != CCGetServerConfig()->GetServerMode() )  return;

	CCQuestScenarioInfo* pScenario = m_pQuestLevel->GetStaticInfo()->pScenario;
	if (!pScenario) return;

	CCMatchObject*					pPlayer;



	const int nRewardXP		= pScenario->nXPReward;
	const int nRewardBP		= pScenario->nBPReward;
	const int nScenarioQL	= pScenario->nQL;

	MakeRewardList();		// 아이템 배분

	for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;

		// 경험치, 바운티 배분
		DistributeXPnBP( pPlayerInfo, nRewardXP, nRewardBP, nScenarioQL );

		pPlayer = CCMatchServer::GetInstance()->GetObject((*itor).first);
		if( !IsEnabledObject(pPlayer) ) continue;

		// 퀘스트 아이템 배분
		void* pSimpleQuestItemBlob = NULL;
		if (!DistributeQItem( pPlayerInfo, &pSimpleQuestItemBlob )) continue;

		// 일반 아이템 배분
		void* pSimpleZItemBlob = NULL;
		if (!DistributeZItem( pPlayerInfo, &pSimpleZItemBlob )) continue;

		// DB동기화 여부 검사.
		pPlayer->GetCharInfo()->GetDBQuestCachingData().IncreasePlayCount();

		// 커맨드 생성.
		RouteRewardCommandToStage( pPlayer, (*itor).second->nXP, (*itor).second->nBP, pSimpleQuestItemBlob, pSimpleZItemBlob );

		CCEraseBlobArray( pSimpleQuestItemBlob );

		CCGetMatchServer()->ResponseCharacterItemList( pPlayer->GetUID() );
	}

}


void CCMatchRuleQuest::InsertNoParamQItemToPlayer( CCMatchObject* pPlayer, CCQuestItem* pQItem )
{
	if( !IsEnabledObject(pPlayer) || (0 == pQItem) ) return;

	CCQuestItemMap::iterator itMyQItem = pPlayer->GetCharInfo()->m_QuestItemList.find( pQItem->GetItemID() );

	if( pPlayer->GetCharInfo()->m_QuestItemList.end() != itMyQItem )
	{
		// 기존에 가지고 있던 퀘스트 아이템. 수량만 증가 시켜주면 됨.
		const int nOver = itMyQItem->second->Increase( pQItem->GetCount() );
		if( 0 < nOver )
			pQItem->Decrease( nOver );
	}
	else
	{
		// 처음 획득한 퀘스트 아이템. 새로 추가시켜 줘야 함.
		if( !pPlayer->GetCharInfo()->m_QuestItemList.CreateQuestItem(pQItem->GetItemID(), pQItem->GetCount(), pQItem->IsKnown()) )
			cclog( "CCMatchRuleQuest::DistributeReward - %d번호 아이템의 Create( ... )함수 호출 실패.\n", pQItem->GetItemID() );
	}
}


void CCMatchRuleQuest::MakeRewardList()
{
	int								nPos;
	int								nPlayerCount;
	int								nLimitRandNum;
	CCQuestItem*						pRewardQItem;
	CCQuestLevelItemMap::iterator	itObtainQItem, endObtainQItem;
	CCQuestLevelItem*				pObtainQItem;

	nPlayerCount	= static_cast< int >( m_PlayerManager.size() );
	endObtainQItem	= m_pQuestLevel->GetDynamicInfo()->ItemMap.end();
	nLimitRandNum	= m_nPlayerCount - 1;

	vector<CCQuestPlayerInfo*>	a_vecPlayerInfos;
	for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;

		// 혹시 예전 게임의 리워드 아이템이 남아있을지 모르니 초기화.
		pPlayerInfo->RewardQuestItemMap.Clear();
		pPlayerInfo->RewardZItemList.clear();

		a_vecPlayerInfos.push_back(pPlayerInfo);
	}

	for( itObtainQItem = m_pQuestLevel->GetDynamicInfo()->ItemMap.begin(); itObtainQItem != endObtainQItem; ++itObtainQItem )
	{
		pObtainQItem = itObtainQItem->second;

		// 획득하지 못했으면 무시.
		if (!pObtainQItem->bObtained) continue;	

		if (pObtainQItem->IsQuestItem())
		{
			// 퀘스트 아이템 -----------------------------------------------------

			// 시작할때의 인원을가지고 roll을 함.
			nPos = RandomNumber( 0, nLimitRandNum );

			// 현재 남아있는 인원보다 클경우 그냥 버림.
			if (( nPos < nPlayerCount ) && (nPos < (int)a_vecPlayerInfos.size()))
			{
				// 퀘스트 아이템일 경우 처리
				CCQuestItemMap* pRewardQuestItemMap = &a_vecPlayerInfos[ nPos ]->RewardQuestItemMap;

				pRewardQItem = pRewardQuestItemMap->Find( pObtainQItem->nItemID );
				if( 0!= pRewardQItem )
					pRewardQItem->Increase(); // 이전에 획득한 아이템.
				else
				{
					// 처음 획득.
					if( !pRewardQuestItemMap->CreateQuestItem(pObtainQItem->nItemID, 1) )
					{
						cclog( "CCMatchRuleQuest::MakeRewardList - ItemID:%d 처음 획득한 아이템 생성 실패.\n", pObtainQItem->nItemID );
						continue;
					}
				}
			}
		}
		else
		{
			// 일반 아이템일 경우 처리 -------------------------------------------

			
			
			RewardZItemInfo iteminfo;
			iteminfo.nItemID = pObtainQItem->nItemID;
			iteminfo.nRentPeriodHour = pObtainQItem->nRentPeriodHour;

			int nLoopCounter = 0;
			const int MAX_LOOP_COUNT = 5;

			// 최대 5번까지 랜덤으로 아이템의 성별이 같은 사람을 찾는다.
			while (nLoopCounter < MAX_LOOP_COUNT)
			{
				nLoopCounter++;

				// 시작할때의 인원을가지고 roll을 함.
				nPos = RandomNumber( 0, nLimitRandNum );

				// 현재 남아있는 인원보다 클경우 그냥 버림.
				if (( nPos < nPlayerCount ) && (nPos < (int)a_vecPlayerInfos.size()))
				{
					CCQuestPlayerInfo* pPlayerInfo = a_vecPlayerInfos[ nPos ];
					CCQuestRewardZItemList* pRewardZItemList = &pPlayerInfo->RewardZItemList;

					// 성별이 같아야만 가질 수 있다.
					if (IsEnabledObject(pPlayerInfo->pObject))
					{
						if (IsEquipableItem(iteminfo.nItemID, MAX_LEVEL, pPlayerInfo->pObject->GetCharInfo()->m_nSex))
						{
							pRewardZItemList->push_back(iteminfo);
							break;
						}
					}
				}
			}
		}

	}
}


///< 경험치와 바운티 배분 옮김. -by 추교성.
void CCMatchRuleQuest::DistributeXPnBP( CCQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL )
{
	float fXPRate, fBPRate;

	CCQuestFormula::CalcRewardRate(fXPRate, 
								  fBPRate,
								  nScenarioQL, 
								  pPlayerInfo->nQL,
								  pPlayerInfo->nDeathCount, 
								  pPlayerInfo->nUsedPageSacriItemCount, 
								  pPlayerInfo->nUsedExtraSacriItemCount);

	pPlayerInfo->nXP = int(nRewardXP * fXPRate);
	pPlayerInfo->nBP = int(nRewardBP * fBPRate);


	// 실제로 경험치, 바운티 지급
	if (IsEnabledObject(pPlayerInfo->pObject))
	{
		// 경험치 보너스 계산
		const float fXPBonusRatio = CCMatchFormula::CalcXPBonusRatio(pPlayerInfo->pObject, MIBT_QUEST);
		const float fBPBonusRatio = CCMatchFormula::CalcBPBounsRatio(pPlayerInfo->pObject, MIBT_QUEST);

		int nExpBonus = (int)(pPlayerInfo->nXP * fXPBonusRatio);
		pPlayerInfo->nXP += nExpBonus;

		int nBPBonus = (int)(pPlayerInfo->nBP * fBPBonusRatio);
		pPlayerInfo->nBP += nBPBonus;

		CCMatchServer::GetInstance()->ProcessPlayerXPBP(m_pStage, pPlayerInfo->pObject, pPlayerInfo->nXP, pPlayerInfo->nBP);
	}
}

// 퀘스트 아이템 배분
bool CCMatchRuleQuest::DistributeQItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob)
{
	CCMatchObject* pPlayer = pPlayerInfo->pObject;
	if (!IsEnabledObject(pPlayer)) return false;

	CCQuestItemMap* pObtainQuestItemMap = &pPlayerInfo->RewardQuestItemMap;

	// Client로 전송할수 있는 형태로 Quest item정보를 저장할 Blob생성.
	void* pSimpleQuestItemBlob = CCMakeBlobArray( sizeof(CCTD_QuestItemNode), static_cast<int>(pObtainQuestItemMap->size()) );
	if( 0 == pSimpleQuestItemBlob )
	{
		cclog( "CCMatchRuleQuest::DistributeReward - Quest item 정보를 보낼 Blob생성에 실패.\n" );
		return false;
	}

	// 로그를 위해서 해당 유저가 받을 아이템의 정보를 저장해 놓음.
	if( !m_QuestGameLogInfoMgr.AddRewardQuestItemInfo(pPlayer->GetUID(), pObtainQuestItemMap) )
	{
		cclog( "m_QuestGameLogInfoMgr -해당 유저의 로그객체를 찾는데 실패." );
	}

	int nBlobIndex = 0;
	for(CCQuestItemMap::iterator itQItem = pObtainQuestItemMap->begin(); itQItem != pObtainQuestItemMap->end(); ++itQItem )
	{
		CCQuestItem* pQItem = itQItem->second;
		CCQuestItemDesc* pQItemDesc = pQItem->GetDesc();
		if( 0 == pQItemDesc )
		{
			cclog( "CCMatchRuleQuest::DistributeReward - %d 아이템의 디스크립션 셋팅이 되어있지 않음.\n", pQItem->GetItemID() );
			continue;
		}

		// 유니크 아이템인지 검사를 함.
		pPlayer->GetCharInfo()->m_DBQuestCachingData.CheckUniqueItem( pQItem );
		// 보상받은 횟수를 검사를 함.
		pPlayer->GetCharInfo()->m_DBQuestCachingData.IncreaseRewardCount();

		if( CCQIT_MONBIBLE == pQItemDesc->m_nType )
		{
			// 몬스터 도감 처리.
			if( !pPlayer->GetCharInfo()->m_QMonsterBible.IsKnownMonster(pQItemDesc->m_nParam) )
				pPlayer->GetCharInfo()->m_QMonsterBible.WriteMonsterInfo( pQItemDesc->m_nParam );
		}
		else if( 0 != pQItemDesc->m_nParam )
		{
			// Param값이 설정되어 있는 아이템은 따로 처리를 해줘야 함.				
		}
		else
		{
			// DB에 저장이 되는 퀘스트 아이템만 유저한테 저장함.
			InsertNoParamQItemToPlayer( pPlayer, pQItem );
		}

		CCTD_QuestItemNode* pQuestItemNode;
		pQuestItemNode = reinterpret_cast< CCTD_QuestItemNode* >( CCGetBlobArrayElement(pSimpleQuestItemBlob, nBlobIndex++) );
		Make_MTDQuestItemNode( pQuestItemNode, pQItem->GetItemID(), pQItem->GetCount() );
	}

	*ppoutSimpleQuestItemBlob = pSimpleQuestItemBlob;
	return true;
}

bool CCMatchRuleQuest::DistributeZItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob)
{
	CCMatchObject* pPlayer = pPlayerInfo->pObject;
	if (!IsEnabledObject(pPlayer)) return false;

	// 아이템 갯수가 MAX_QUEST_REWARD_ITEM_COUNT(500개)보다 많으면 일반아이템은 추가해주지 않는다.
	if(!CCGetMatchServer()->CheckUserCanDistributeRewardItem(pPlayer))
	{
		*ppoutQuestRewardZItemBlob = CCMakeBlobArray( sizeof(CCTD_QuestZItemNode), 0 );
		return true;	// 단, 다른것들(XP, BP, 퀘스트희생 아이템 등)은 업데이트 해준다.
	}

	CCQuestRewardZItemList* pObtainZItemList = &pPlayerInfo->RewardZItemList;

	// Client로 전송할수 있는 형태로 Quest item정보를 저장할 Blob생성.
	void* pSimpleZItemBlob = CCMakeBlobArray( sizeof(CCTD_QuestZItemNode), (int)(pObtainZItemList->size()) );
	if( 0 == pSimpleZItemBlob )
	{
		cclog( "CCMatchRuleQuest::DistributeZItem - Ztem 정보를 보낼 Blob생성에 실패.\n" );
		return false;
	}

	// 캐시 아이템 획득 로그를 남기기 위함.
	if( !m_QuestGameLogInfoMgr.AddRewardZItemInfo(pPlayer->GetUID(), pObtainZItemList) )
	{
		cclog( "m_QuestGameLogInfoMgr -해당 유저의 로그객체를 찾는데 실패." );
	}

	int nBlobIndex = 0;
	for(CCQuestRewardZItemList::iterator itor = pObtainZItemList->begin(); itor != pObtainZItemList->end(); ++itor )
	{
		RewardZItemInfo iteminfo = (*itor);
		CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(iteminfo.nItemID);

		if (pItemDesc == NULL) continue;
		if (!IsEquipableItem(iteminfo.nItemID, MAX_LEVEL, pPlayer->GetCharInfo()->m_nSex)) 	continue;

		// 실제로 아이템 등록
		// TodoH(하) - 퀘스트 아이템 보상에 대한 부분. 일단 무조건 1개만 지급!
		CCMatchServer::GetInstance()->DistributeZItem(pPlayer->GetUID(), iteminfo.nItemID, true, iteminfo.nRentPeriodHour, 1);

		// 블롭생성
		CCTD_QuestZItemNode* pZItemNode  = (CCTD_QuestZItemNode*)(CCGetBlobArrayElement(pSimpleZItemBlob, nBlobIndex++));
		pZItemNode->m_nItemID			= iteminfo.nItemID;
		pZItemNode->m_nRentPeriodHour	= iteminfo.nRentPeriodHour;
	}

	*ppoutQuestRewardZItemBlob = pSimpleZItemBlob;

	return true;
}

void CCMatchRuleQuest::RouteRewardCommandToStage( CCMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob)
{
	if( !IsEnabledObject(pPlayer) || (0 == pSimpleQuestItemBlob) )
		return;

	CCCommand* pNewCmd = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_USER_REWARD_QUEST, CCUID(0, 0) );
	if( 0 == pNewCmd )
		return;

	pNewCmd->AddParameter( new CCCmdParamInt(nRewardXP) );
	pNewCmd->AddParameter( new CCCmdParamInt(nRewardBP) );
	pNewCmd->AddParameter( new CCCommandParameterBlob(pSimpleQuestItemBlob, CCGetBlobArraySize(pSimpleQuestItemBlob)) );
	pNewCmd->AddParameter( new CCCommandParameterBlob(pSimpleZItemBlob, CCGetBlobArraySize(pSimpleZItemBlob)) );

	CCMatchServer::GetInstance()->RouteToListener( pPlayer, pNewCmd );
}



void CCMatchRuleQuest::OnRequestPlayerDead(const CCUID& uidVictim)
{
	CCQuestPlayerManager::iterator itor = m_PlayerManager.find(uidVictim);
	if (itor != m_PlayerManager.end())
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;
		pPlayerInfo->nDeathCount++;
	}
}


void CCMatchRuleQuest::OnObtainWorldItem(CCMatchObject* pObj, int nItemID, int* pnExtraValues)
{
	if( 0 == pObj )
		return;
	
	if (m_nCombatState != CCQUEST_COMBAT_PLAY) 
	{
#ifdef _DEBUG
		cclog( "obtain quest item fail. not combat play.\n" );
#endif
		return;
	}

	int nQuestItemID = pnExtraValues[0];
	int nRentPeriodHour = pnExtraValues[1];

	if (m_pQuestLevel->OnItemObtained(pObj, (unsigned long int)nQuestItemID))
	{
		// true값이면 실제로 먹은것임.

		if (IsQuestItemID(nQuestItemID))
            RouteObtainQuestItem(unsigned long int(nQuestItemID));
		else 
			RouteObtainZItem(unsigned long int(nQuestItemID));
	}
}


void CCMatchRuleQuest::OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		OnResponseDropSacrificeItemOnSlot( uidSender, nSlotIndex, nItemID );
	}
}


void CCMatchRuleQuest::OnResponseDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( (MAX_SACRIFICE_SLOT_COUNT > nSlotIndex) && (0 <= nSlotIndex) ) 
	{
		// 중복 검사.
		// if( IsSacrificeItemDuplicated(uidSender, nSlotIndex, nItemID) )
		//	return;
		
		CCQuestItemDesc* pQItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID );
		if( 0 == pQItemDesc )
		{
			// ItemID가 비 정상적이거나 ItemID에 해당하는 Description이 없음.
			// 여하튼 error...

			cclog( "CCMatchRuleBaseQuest::SetSacrificeItemOnSlot - ItemID가 비 정상적이거나 %d에 해당하는 Description이 없음.\n", nItemID );
			ASSERT( 0 );
			return;
		}

		// 아이템의 타입이 희생아이템인 경우만 실행.
		if( pQItemDesc->m_bSecrifice )
		{
			CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
			if( !IsEnabledObject(pPlayer) )
			{
				cclog( "CCMatchRuleBaseQuest::SetSacrificeItemOnSlot - 비정상 유저.\n" );
				return;
			}

			CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
			if( 0 == pStage )
				return;

			// 아무나 슬롯에 접근할수 있음.

			CCQuestItem* pQuestItem = pPlayer->GetCharInfo()->m_QuestItemList.Find( nItemID );
			if( 0 == pQuestItem )
				return;
			
			// 수량이 충분한지 검사.
			int nMySacriQItemCount = CalcuOwnerQItemCount( uidSender, nItemID );
			if( -1 == nMySacriQItemCount )
				return;
			if( nMySacriQItemCount >= pQuestItem->GetCount() )
			{
				// 수량이 부족해서 올리지 못했다고 통보함.
				CCCommand* pCmdMore = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM, CCUID(0, 0) );
				if( 0 == pCmdMore )
					return;

				pCmdMore->AddParameter( new CCCmdParamInt(NEED_MORE_QUEST_ITEM) );
				pCmdMore->AddParameter( new CCCmdParamCCUID(uidSender) );
				pCmdMore->AddParameter( new CCCmdParamInt(nSlotIndex) );
				pCmdMore->AddParameter( new CCCmdParamInt(nItemID) );

				CCMatchServer::GetInstance()->RouteToListener( pPlayer, pCmdMore );
				return;
			}

			CCCommand* pCmdOk = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM, CCUID(0, 0) );
			if( 0 == pCmdOk )
			{
				return;
			}

			pCmdOk->AddParameter( new CCCmdParamInt(MOK) );
			pCmdOk->AddParameter( new CCCmdParamCCUID(uidSender) );
			pCmdOk->AddParameter( new CCCmdParamInt(nSlotIndex) );
			pCmdOk->AddParameter( new CCCmdParamInt(nItemID) );
			
			CCMatchServer::GetInstance()->RouteToStage( pStage->GetUID(), pCmdOk );
			
			// 일반적인 처리.
			m_SacrificeSlot[ nSlotIndex ].SetAll( uidSender, nItemID );

			// 슬롯의 정보가 업데이트되면 업데이트된 정보를 다시 보내줌.
			RefreshStageGameInfo();
		}
		else
		{
			// 희새아이템이 아님.
			ASSERT( 0 );
			return;
		}// if( pQItemDesc->m_bSecrifice )
	}
	else
	{
		// 슬롯의 인덱스가 비 정상적임.
		cclog( "CCMatchRuleBaseQuest::OnResponseDropSacrificeItemOnSlot - %d번 슬롯 인덱스는 유효하지 않는 인덱스임.\n", nSlotIndex );
		ASSERT( 0 );
		return;
	}
}


void CCMatchRuleQuest::OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		OnResponseCallBackSacrificeItem( uidSender, nSlotIndex, nItemID );
	}
}


void CCMatchRuleQuest::OnResponseCallBackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	// 아무나 접근할수 있음.
	if( (MAX_SACRIFICE_SLOT_COUNT <= nSlotIndex) && (0 > nSlotIndex) ) 
		return;


	if( (0 == nItemID) || (0 == m_SacrificeSlot[nSlotIndex].GetItemID()) )
		return;

	if( nItemID != m_SacrificeSlot[nSlotIndex].GetItemID() )
		return;

	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
	{
		cclog( "CCMatchRuleBaseQuest::OnResponseCallBackSacrificeItem - 비정상적인 유저.\n" );
		return;
	}

	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
	if( 0 == pStage )
		return;

	CCCommand* pCmdOk = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_CALLBACK_SACRIFICE_ITEM, CCUID(0, 0) );
	if( 0 == pCmdOk )
	{
		return;
	}

	pCmdOk->AddParameter( new CCCmdParamInt(MOK) );
	pCmdOk->AddParameter( new CCCmdParamCCUID(uidSender) );									// 아이템 회수를 요청한 아이디.
	pCmdOk->AddParameter( new CCCmdParamInt(nSlotIndex) );
	pCmdOk->AddParameter( new CCCmdParamInt(nItemID) );

	CCMatchServer::GetInstance()->RouteToStage( pPlayer->GetStageUID(), pCmdOk );
	
	m_SacrificeSlot[ nSlotIndex ].Release();	

	// 슬롯의 정보가 업데이트되면 QL을 다시 보내줌.
	RefreshStageGameInfo();
}


bool CCMatchRuleQuest::IsSacrificeItemDuplicated( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( (uidSender == m_SacrificeSlot[nSlotIndex].GetOwnerUID()) && (nItemID == m_SacrificeSlot[nSlotIndex].GetItemID()) )
	{
		// 같은 아이템을 올려놓으려고 했기에 그냥 무시해 버림.

		return true;
	}

	return false;
}


/*
 * 스테이지를 나가기전에 처리해야 할 일이 있을경우 여기에 정리함.
 */
void CCMatchRuleQuest::PreProcessLeaveStage( const CCUID& uidLeaverUID )
{
	CCMatchRuleBaseQuest::PreProcessLeaveStage( uidLeaverUID );

	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidLeaverUID );
	if( !IsEnabledObject(pPlayer) )
		return;
	
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		// 스테이지를 나가려는 유저가 이전에 흐생 아이템을 스롯에 올려 놓았는지 검사를 함.
		// 만약 올려놓은 아이템이 있다면 자동으로 회수를 함. - 대기상태일때만 적용
		if (GetStage()->GetState() == STAGE_STATE_STANDBY) 
		{
			// 슬롯이 비어있으면 무시.
			if( (!m_SacrificeSlot[0].IsEmpty()) || (!m_SacrificeSlot[1].IsEmpty()) )
			{	
				for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
				{
					if( uidLeaverUID == m_SacrificeSlot[i].GetOwnerUID() )
						m_SacrificeSlot[ i ].Release();
				}

				CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
				if( 0 == pStage )
					return;

				// 변경된 슬롯 정보를 보내줌.
				OnResponseSacrificeSlotInfoToStage( pStage->GetUID() );
			}
		}
	}
}


void CCMatchRuleQuest::DestroyAllSlot()
{
	// 여기서 슬롯에 올려져있는 아이템을 소멸시킴.

	CCMatchObject*	pOwner;
	CCQuestItem*		pQItem;
	CCUID			uidOwner;
	unsigned long	nItemID;

	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
	{
		if( CCUID(0, 0) == m_SacrificeSlot[i].GetOwnerUID() )
			continue;

		uidOwner = m_SacrificeSlot[ i ].GetOwnerUID();
		
		// 정상적인 아이템 소유자인지 검사.
		pOwner = CCMatchServer::GetInstance()->GetObject( uidOwner );
		if( !IsEnabledObject(pOwner) )
		{
			continue;
		}

		nItemID = m_SacrificeSlot[ i ].GetItemID();

		// 소유자의 정상적인 아이템인지 검사.
		pQItem = pOwner->GetCharInfo()->m_QuestItemList.Find( nItemID );
		if( 0 == pQItem )
		{
			continue;
		}

		m_SacrificeSlot[ i ].Release();

		pQItem->Decrease();

		pOwner->GetCharInfo()->GetDBQuestCachingData().IncreasePlayCount();
		CCMatchServer::GetInstance()->OnRequestCharQuestItemList( uidOwner );
	}
}


///
// First	: 추교성.
// Last		: 추교성.
//
// QL정보의 요청을 처리함. 기본적으로 요청자의 스테이지에 통보함.
///

void CCMatchRuleQuest::OnRequestQL( const CCUID& uidSender )
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
		if( 0 == pPlayer )
		{
			cclog( "CCMatchRuleQuest::OnRequestQL - 비정상 유저.\n" );
			return;
		}

		OnResponseQL_ToStage( pPlayer->GetStageUID() );
	}
}


///
// First : 추교성.
// Last  : 추교성.
//
// 요청자의 스테이지에 QL정보를 통보.
///
void CCMatchRuleQuest::OnResponseQL_ToStage( const CCUID& uidStage )
{
	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( uidStage );
	if( 0 == pStage )
	{
		cclog( "CCMatchRuleQuest::OnRequestQL - 스테이지 검사 실패.\n" );
		return;
	}

	RefreshStageGameInfo();
}

///
// First : 추교성.
// Last  : 추교성.
//
// 현재 스롯의 정보를 요청. 기본적으로 스테이지에 알림.
///
void CCMatchRuleQuest::OnRequestSacrificeSlotInfo( const CCUID& uidSender )
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
		if( 0 == pPlayer )
			return;

		CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
		if( 0 == pStage )
			return;

		OnResponseSacrificeSlotInfoToStage( pStage->GetUID() );
	}
}


///
// First : 추교성.
// Last  : 추교성.
//
// 현재 스롯의 정보를 요청자에 알림.
///
void CCMatchRuleQuest::OnResponseSacrificeSlotInfoToListener( const CCUID& uidSender )
{
	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
	{
		return;
	}

	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
	if( 0 == pStage )
		return;

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_SLOT_INFO, CCUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new CCCmdParamCCUID(m_SacrificeSlot[0].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[0].GetItemID()) );
	pCmd->AddParameter( new CCCmdParamCCUID(m_SacrificeSlot[1].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[1].GetItemID()) );

	CCMatchServer::GetInstance()->RouteToListener( pPlayer, pCmd );
}


///
// First : 추교성.
// Last  : 추교성.
//
// 현재 스롯의 정보를 스테이지에 알림.
///
void CCMatchRuleQuest::OnResponseSacrificeSlotInfoToStage( const CCUID& uidStage )
{
	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( uidStage );
	if( 0 == pStage )
		return;

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_SLOT_INFO, CCUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new CCCmdParamCCUID(m_SacrificeSlot[0].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[0].GetItemID()) );
	pCmd->AddParameter( new CCCmdParamCCUID(m_SacrificeSlot[1].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[1].GetItemID()) );

	CCMatchServer::GetInstance()->RouteToStage( uidStage, pCmd );
}


void CCMatchRuleQuest::PostInsertQuestGameLogAsyncJob()
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		CollectEndQuestGameLogInfo();
		m_QuestGameLogInfoMgr.PostInsertQuestGameLog();
	}
}




int CCMatchRuleQuest::CalcuOwnerQItemCount( const CCUID& uidPlayer, const unsigned long nItemID )
{
	if(  0 == CCMatchServer::GetInstance()->GetObject(uidPlayer) )
		return -1;

	int nCount = 0;
	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
	{
		if( (uidPlayer == m_SacrificeSlot[i].GetOwnerUID()) &&
			(nItemID == m_SacrificeSlot[i].GetItemID()) )
		{
			++nCount;
		}
	}

	return nCount;
}


const bool CCMatchRuleQuest::PostNPCInfo()
{
	CCMatchQuest*		pQuest			= CCMatchServer::GetInstance()->GetQuest();
	CCQuestScenarioInfo* pScenarioInfo	= pQuest->GetScenarioInfo( m_StageGameInfo.nScenarioID );
	if( NULL == pScenarioInfo )
	{
		return false;
	}
	
	vector< CCQUEST_NPC > NPCList;
	
	for( size_t i = 0; i < SCENARIO_STANDARD_DICE_SIDES; ++i )
	{
		MakeJacoNPCList( NPCList, pScenarioInfo->Maps[i] );
		MakeNomalNPCList( NPCList, pScenarioInfo->Maps[i], pQuest );
	}

	void* pBlobNPC = CCMakeBlobArray(sizeof(CCTD_NPCINFO), int(NPCList.size()) );
	if( NULL == pBlobNPC )
	{
		return false;
	}

	vector< CCQUEST_NPC >::iterator	itNL;
	vector< CCQUEST_NPC >::iterator	endNL;
	CCQuestNPCInfo*					pQuestNPCInfo		= NULL;
	int								nNPCIndex			= 0;
	CCTD_NPCINFO*					pCCTD_QuestNPCInfo	= NULL;

	endNL = NPCList.end();
	for( itNL = NPCList.begin(); endNL != itNL; ++ itNL )
	{
		pQuestNPCInfo = pQuest->GetNPCInfo( (*itNL) );	
		if( NULL == pQuestNPCInfo )
		{
			CCEraseBlobArray( pBlobNPC );
			return false;
		}
			
		pCCTD_QuestNPCInfo = reinterpret_cast< CCTD_NPCINFO* >( CCGetBlobArrayElement(pBlobNPC, nNPCIndex++) );
		if( NULL == pCCTD_QuestNPCInfo )
		{
			_ASSERT( 0 );
			CCEraseBlobArray( pBlobNPC );
			return false;
		}

		CopyCCTD_NPCINFO( pCCTD_QuestNPCInfo, pQuestNPCInfo );
	}

	CCCommand* pCmdNPCList = CCGetMatchServer()->CreateCommand( MC_QUEST_NPCLIST, CCUID(0, 0) );
	if( NULL == pCmdNPCList )
	{
		CCEraseBlobArray( pBlobNPC );
		return false;
	}

	pCmdNPCList->AddParameter( new CCCommandParameterBlob(pBlobNPC, CCGetBlobArraySize(pBlobNPC)) );
	pCmdNPCList->AddParameter( new CCCommandParameterInt(GetGameType()) );
	
	CCGetMatchServer()->RouteToStage( m_pStage->GetUID(), pCmdNPCList );

	CCEraseBlobArray( pBlobNPC );

	return true;
}


///
// First : 2005.04.18 추교성.
// Last  : 2005.04.18 추교성.
//
// 게임을 시작하기전에 준배향 하는 작업을 수행함.
// 준비 작업중 실패가 있을시는 게임을 시작하지 못하게 해야 함.
///
bool CCMatchRuleQuest::PrepareStart()
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		MakeStageGameInfo();

		if ((m_StageGameInfo.nQL >= 0) || (m_StageGameInfo.nQL <= MAX_QL)) 
		{
#ifdef _DEBUG
#else
			if(m_StageGameInfo.nScenarioID == 100)
				m_StageGameInfo.nScenarioID = 0;
#endif
			if ((m_StageGameInfo.nScenarioID > 0) || (m_StageGameInfo.nMapsetID > 0))
			{
				if( PostNPCInfo() )
				{
					return true;
				}
			}
		}
	}

	if( NULL != CCMatchServer::GetInstance()->GetObject(m_pStage->GetMasterUID()) )
	{
		CCCommand* pCmdNotReady = CCGetMatchServer()->CreateCommand( MC_GAME_START_FAIL, m_pStage->GetMasterUID() );
		pCmdNotReady->AddParameter( new CCCmdParamInt(QUEST_START_FAILED_BY_SACRIFICE_SLOT) );
		pCmdNotReady->AddParameter( new CCCmdParamCCUID(CCUID(0, 0)) );
		CCGetMatchServer()->Post( pCmdNotReady );
	}

	return false;
}

void CCMatchRuleQuest::MakeStageGameInfo()
{	
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		if( (GetStage()->GetState() != STAGE_STATE_STANDBY) && (STAGE_STATE_COUNTDOWN != GetStage()->GetState()) )
		{
			return;
		}

		// 슬롯에 Level에 맞는 정상적인 아이템이 올려져 있는지 검사가 필요함.
		// 비정상 아이템이 올려져 있을경우 아이템 회수 요청을 해줘야 함.
		int nOutResultQL = -1;

		int nMinPlayerLevel = 1;
		CCMatchStage* pStage = GetStage();
		if (pStage != NULL)
		{
			nMinPlayerLevel = pStage->GetMinPlayerLevel();

			// 방장이 운영자이면 최소레벨은 운영자 레벨로 임의지정한다.
			CCMatchObject* pMaster = CCMatchServer::GetInstance()->GetObject(pStage->GetMasterUID());
			if (IsAdminGrade(pMaster))
			{
				nMinPlayerLevel = pMaster->GetCharInfo()->m_nLevel;
			}
		}

		int nPlayerQL = CCQuestFormula::CalcQL( nMinPlayerLevel );
//		m_StageGameInfo.nPlayerQL = nPlayerQL;

		unsigned int SQItems[MAX_SCENARIO_SACRI_ITEM];
		for (int i = 0; i < MAX_SCENARIO_SACRI_ITEM; i++)
		{
			SQItems[i] = (unsigned int)m_SacrificeSlot[i].GetItemID();
		}

		// 하드코딩.. 또또... -_-;
		m_StageGameInfo.nMapsetID = 1;
		if ( !stricmp( pStage->GetMapName(), "mansion"))
			m_StageGameInfo.nMapsetID = 1;
		else if ( !stricmp( pStage->GetMapName(), "prison"))
			m_StageGameInfo.nMapsetID = 2;
		else if ( !stricmp( pStage->GetMapName(), "dungeon"))
			m_StageGameInfo.nMapsetID = 3;


		CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();
		unsigned int nScenarioID = pQuest->GetScenarioCatalogue()->MakeScenarioID(m_StageGameInfo.nMapsetID,
																				  nPlayerQL, SQItems);

		m_StageGameInfo.nScenarioID = nScenarioID;
		CCQuestScenarioInfo* pScenario = pQuest->GetScenarioCatalogue()->GetInfo(nScenarioID);
		if (pScenario)
		{
			m_StageGameInfo.nQL = pScenario->nQL;
			m_StageGameInfo.nPlayerQL = nPlayerQL;
		}
		else
		{
			if ( nPlayerQL > 1)
			{
				m_StageGameInfo.nQL = 1;
				m_StageGameInfo.nPlayerQL = 1;
			}
			else
			{
				m_StageGameInfo.nQL = 0;
				m_StageGameInfo.nPlayerQL = 0;
			}
		}
	}
}

void CCMatchRuleQuest::RefreshStageGameInfo()
{
	MakeStageGameInfo();
	RouteStageGameInfo();
}

void CCMatchRuleQuest::OnChangeCondition()
{
	RefreshStageGameInfo();
}

void CCMatchRuleQuest::CollectStartingQuestGameLogInfo()
{
	// 수집하기전에 이전의 정보를 반드시 지워야 함.
	m_QuestGameLogInfoMgr.Clear();

	if( QuestTestServer() ) 
	{
		// Master CID
		CCMatchObject* pMaster = CCMatchServer::GetInstance()->GetObject( GetStage()->GetMasterUID() );
		if( IsEnabledObject(pMaster) )
			m_QuestGameLogInfoMgr.SetMasterCID( pMaster->GetCharInfo()->m_nCID );

		m_QuestGameLogInfoMgr.SetScenarioID( m_pQuestLevel->GetStaticInfo()->pScenario->nID );

		// Stage name 저장.
		m_QuestGameLogInfoMgr.SetStageName( GetStage()->GetName() );

		// 시작할때의 유저 정보를 저장함.
		for(CCQuestPlayerManager::iterator it = m_PlayerManager.begin() ; 
				it != m_PlayerManager.end(); ++it )
		{
			m_QuestGameLogInfoMgr.AddQuestPlayer( it->second->pObject->GetUID(), it->second->pObject );
		}

		m_QuestGameLogInfoMgr.SetStartTime( timeGetTime() );
	}
}


void CCMatchRuleQuest::CollectEndQuestGameLogInfo()
{
	m_QuestGameLogInfoMgr.SetEndTime( timeGetTime() );
}

