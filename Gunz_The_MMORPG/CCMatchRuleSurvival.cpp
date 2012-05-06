#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCMatchRuleSurvival.h"
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

CCMatchRuleSurvival::CCMatchRuleSurvival(CCMatchStage* pStage) : CCMatchRuleBaseQuest(pStage), m_pQuestLevel(NULL),
m_nCombatState(CCQUEST_COMBAT_NONE), m_nPrepareStartTime(0),
m_nCombatStartTime(0), m_nQuestCompleteTime(0), m_nPlayerCount( 0 )
{
	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
		m_SacrificeSlot[ i ].Release();

	m_StageGameInfo.nQL = 0;
	m_StageGameInfo.nPlayerQL = 0;
	m_StageGameInfo.nMapsetID = 1;
	m_StageGameInfo.nScenarioID = CCMatchServer::GetInstance()->GetQuest()->GetSurvivalScenarioCatalogue()->GetDefaultStandardScenarioID();
}

CCMatchRuleSurvival::~CCMatchRuleSurvival()
{
	ClearQuestLevel();
}

// Route ������ ���� /////////////////////////////////////////////////////////////////
void CCMatchRuleSurvival::RouteMapSectorStart()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_START, CCUID(0,0));
	char nSectorIndex = char(m_pQuestLevel->GetCurrSectorIndex());
	pCmd->AddParameter(new CCCommandParameterChar(nSectorIndex));
	pCmd->AddParameter(new CCCommandParameterUChar(unsigned char(m_pQuestLevel->GetDynamicInfo()->nRepeated)));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteCombatState()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_COMBAT_STATE, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterChar(char(m_nCombatState)));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteMovetoPortal(const CCUID& uidPlayer)
{
	if (m_pQuestLevel == NULL) return;

	int nCurrSectorIndex = m_pQuestLevel->GetCurrSectorIndex();

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_MOVETO_PORTAL, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterChar(char(nCurrSectorIndex)));
	pCmd->AddParameter(new CCCommandParameterUChar(unsigned char(m_pQuestLevel->GetDynamicInfo()->nRepeated)));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteReadyToNewSector(const CCUID& uidPlayer)
{
	if (m_pQuestLevel == NULL) return;

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_READYTO_NEWSECTOR, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidPlayer));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteObtainQuestItem(unsigned long int nQuestItemID)
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_OBTAIN_QUESTITEM, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamUInt(nQuestItemID));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteObtainZItem(unsigned long int nItemID)
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_OBTAIN_ZITEM, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamUInt(nItemID));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteGameInfo()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_GAME_INFO, CCUID(0,0));

	void* pBlobGameInfoArray = CCMakeBlobArray(sizeof(CCTD_QuestGameInfo), 1);
	CCTD_QuestGameInfo* pGameInfoNode = (CCTD_QuestGameInfo*)CCGetBlobArrayElement(pBlobGameInfoArray, 0);

	if (m_pQuestLevel)
	{
		m_pQuestLevel->Make_MTDQuestGameInfo(pGameInfoNode, CCMATCH_GAMETYPE_SURVIVAL);
	}

	pCmd->AddParameter(new CCCommandParameterBlob(pBlobGameInfoArray, CCGetBlobArraySize(pBlobGameInfoArray)));
	CCEraseBlobArray(pBlobGameInfoArray);

	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteCompleted()
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

void CCMatchRuleSurvival::RouteFailed()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_FAILED, CCUID(0,0));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteStageGameInfo()
{
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_STAGE_GAME_INFO, CCUID(0,0));
	pCmd->AddParameter(new CCCmdParamChar(char(m_StageGameInfo.nQL)));
	pCmd->AddParameter(new CCCmdParamChar(char(m_StageGameInfo.nMapsetID)));
	pCmd->AddParameter(new CCCmdParamUInt(m_StageGameInfo.nScenarioID));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleSurvival::RouteSectorBonus(const CCUID& uidPlayer, unsigned long int nEXPValue, unsigned long int nBP)
{
	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject(uidPlayer);	
	if (!IsEnabledObject(pPlayer)) return;

	CCCommand* pNewCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_BONUS, CCUID(0,0));
	pNewCmd->AddParameter(new CCCmdParamUID(uidPlayer));
	pNewCmd->AddParameter(new CCCmdParamUInt(nEXPValue));
	pNewCmd->AddParameter(new CCCmdParamUInt(nBP));
	CCMatchServer::GetInstance()->RouteToListener( pPlayer, pNewCmd );
}

// Route ������ �� ///////////////////////////////////////////////////////////////////

void CCMatchRuleSurvival::OnBegin()
{
	m_nQuestCompleteTime = 0;

	MakeQuestLevel();

	CCMatchRuleBaseQuest::OnBegin();		// ���⼭ ���������� ���� - ������ ����

	// ������ �Ϸ� �Ͽ����� �����Ҷ��� �ο����� ���� ������ ���ؼ� ���� ���� ���� �����Ѵ�.
	m_nPlayerCount = static_cast< int >( m_PlayerManager.size() );

	// ���ӽ����ϸ� ������ ��� ������ ��.
	// ��������� �α� ������ DestroyAllSlot()���� m_QuestGameLogInfoMgr�� ����.
	//DestroyAllSlot();

	// ���� �������� Log�� �ʿ��� ������ ������.
	CollectStartingQuestGameLogInfo();

	SetCombatState(CCQUEST_COMBAT_PREPARE);
}

void CCMatchRuleSurvival::OnEnd()
{
	ClearQuestLevel();

	CCMatchRuleBaseQuest::OnEnd();
}

bool CCMatchRuleSurvival::OnRun()
{
	bool ret = CCMatchRuleBaseQuest::OnRun();
	if (ret == false) return false;

	if (GetRoundState() == CCMATCH_ROUNDSTATE_PLAY)
	{
		CombatProcess();
	}

	return true;
}


// ������ �� ��������.
void CCMatchRuleSurvival::CombatProcess()
{
	switch (m_nCombatState)
	{
	case CCQUEST_COMBAT_PREPARE:			// ��ε� ���ͷ� �����⸦ ��ٸ��� �ñ�
		{
			if (CheckReadytoNewSector())		// ��� �� ���Ϳ� ���ö����� PREPARE
			{
				SetCombatState(CCQUEST_COMBAT_PLAY);				
			};
		}
		break;
	case CCQUEST_COMBAT_PLAY:			// ���� ���� �÷��� �ñ�
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
					// �����̹����� ��������ۻ��̹Ƿ� ������ ���� �ð��� �������� �ʾƵ���
					//if (CheckQuestCompleteDelayTime())
					{
						SetCombatState(CCQUEST_COMBAT_COMPLETED);
					}
				}
				break;
			case CPR_FAILED:
				{
					// ������� �������� �� ���� Ŭ�������� ������ �������θ� �˻��ؼ� ������ ��������
				}
				break;
			};
		}
		break;
	case CCQUEST_COMBAT_COMPLETED:			// ������ ������ ���� ��ũ�� �ǳʰ��� �ñ�
		{
			// ����Ʈ Ŭ��� �ƴϰ� ���� ���Ͱ� ���� ������ �ٷ� PREPARE���°� �ȴ�.
			if (!m_bQuestCompleted)
			{
				SetCombatState(CCQUEST_COMBAT_PREPARE);
			}
		}
		break;
	};
}


void CCMatchRuleSurvival::OnBeginCombatState(CCQuestCombatState nState)
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
			// ���� �ʿ��� �����ִ� npc���� ����
			ClearAllNPC();

			// �� �ó����� ù ���� ���۽� �ɷ�ġ�� ��ȭ��Ų NPC������ ����
			if (m_pQuestLevel->GetCurrSectorIndex() == 0)
			{
				ReinforceNPC();
				PostNPCInfo();
			}

			m_nCombatStartTime = CCMatchServer::GetInstance()->GetTickTime();
			// ��������� �ʱ�ȭ
			m_pStage->m_WorldItemManager.OnRoundBegin();
			m_pStage->m_ActiveTrapManager.Clear();
			m_pStage->ResetPlayersCustomItem();

			RouteMapSectorStart();

			// ��� ��Ȱ
			if (GetCurrentRoundIndex() != 0)
				RefreshPlayerStatus();

			// ���� ���� �̵����� �÷��� ��
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
				// ������ �߰��� ����.
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

void CCMatchRuleSurvival::OnEndCombatState(CCQuestCombatState nState)
{
	switch (nState)
	{
	case CCQUEST_COMBAT_PREPARE:
		break;
	case CCQUEST_COMBAT_PLAY:
		{
			// �̹� ������ 1�δ� ������ ����ؼ� ���� (�� �������� ��ü ������ ������ ��)
			// (���� �߰��� ���� ���� �� �����Ƿ� ���帶�� 1�δ� ������ ����ؼ� �����Ѵ�)
			//int pointPerPlayerOnThisRound = CalcPointForThisRound();
			//m_pointPerPlayer += pointPerPlayerOnThisRound;

#ifdef _DEBUG
			CCMatchObject* pPlayer;
			char sz[256];
			for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
			{
				CCQuestPlayerInfo* pPlayerInfo = (*itor).second;

				pPlayer = CCMatchServer::GetInstance()->GetObject((*itor).first);
				if( !IsEnabledObject(pPlayer) ) continue;

				sprintf(sz, "RoundClear : CharName[%s], HpApOfDeadNpc's[%d]/10 - PlayerDeath[%d]*100 = RankPoint[%d]\n",
					pPlayer->GetCharInfo()->m_szName, pPlayerInfo->nKilledNpcHpApAccum, pPlayerInfo->nDeathCount, pPlayerInfo->nKilledNpcHpApAccum/10-pPlayerInfo->nDeathCount*100);
				OutputDebugString(sz);
			}
#endif
		}
		break;
	case CCQUEST_COMBAT_COMPLETED:
		break;
	};
}

CCMatchRuleSurvival::COMBAT_PLAY_RESULT CCMatchRuleSurvival::CheckCombatPlay()
{
	// ���� ������ keyNPC�� ���̸� Complete
	if (m_NPCManager.IsKeyNPCDie())
		return CPR_COMPLETE;

	// �����̹������� ���Ǹ��� keyNPC�� �ְ����� �Ǽ��� keyNPC ������ ������ ��츦 �����
	// ��� ���� �� �׿��� complete �ǵ��� ����
	if ((m_pQuestLevel->GetNPCQueue()->IsEmpty()) && (m_NPCManager.GetNPCObjectCount() <= 0))
	{
		return CPR_COMPLETE;
	}

	// ��� ������ �׾����� ���� ���з� ������.
	if( !CheckPlayersAlive() )
	{
		return CPR_FAILED;
	}

	return CPR_PLAYING;
}

void CCMatchRuleSurvival::OnCommand(CCCommand* pCommand)
{
	CCMatchRuleBaseQuest::OnCommand(pCommand);
}


///
// First : 
// Last  : 2005.04.27 �߱���.
//
// ����������� ���Կ� �÷�������, QL���� ��������� ���̺��� �����ۿ� �ش��ϴ� ���̺��� �ִ��� �˻��ϱ� ���� ȣ���.
//  �������� ���Կ� �÷��������� QL���� ����� ��. ��������� ���̺� �˻� ����� ������ ����.
//  ������ �����ҽÿ��� ��������� ���̺� �˻� ����� �����϶��� ������ ������.
///
bool CCMatchRuleSurvival::MakeQuestLevel()
{
	// ������ ����Ʈ ���� ������ ������.
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


	// ù���ͺ��� �������� �� �����Ƿ�..
	InitJacoSpawnTrigger();

	return true;
}

void CCMatchRuleSurvival::ClearQuestLevel()
{
	if (m_pQuestLevel)
	{
		delete m_pQuestLevel;
		m_pQuestLevel = NULL;
	}
}




void CCMatchRuleSurvival::MoveToNextSector()
{
	// m_pQuestLevel�� ���������� �̵����ش�.
	m_pQuestLevel->MoveToNextSector(GetGameType());

	InitJacoSpawnTrigger();	
}

void CCMatchRuleSurvival::InitJacoSpawnTrigger()
{
	// ���� ���� ���Ͱ� ���������̸� JacoTrigger �ߵ�
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

void CCMatchRuleSurvival::SetCombatState(CCQuestCombatState nState)
{
	if (m_nCombatState == nState) return;

	OnEndCombatState(m_nCombatState);
	m_nCombatState = nState;
	OnBeginCombatState(m_nCombatState);

	RouteCombatState();
}


bool CCMatchRuleSurvival::CheckReadytoNewSector()
{
	// ���� �ð��� ������ �ٷ� ���� ���ͷ� �̵��Ѵ�.
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

void CCMatchRuleSurvival::RewardSectorXpBp()
{
	// ���� Ŭ���� ����ġ
	CCQuestScenarioInfo* pScenario = m_pQuestLevel->GetStaticInfo()->pScenario;
	if (pScenario)
	{
		const std::vector<int>& vecSectorXp = m_pQuestLevel->GetStaticInfo()->pScenario->Maps[m_pQuestLevel->GetStaticInfo()->nDice].vecSectorXpArray;
		const std::vector<int>& vecSectorBp = m_pQuestLevel->GetStaticInfo()->pScenario->Maps[m_pQuestLevel->GetStaticInfo()->nDice].vecSectorBpArray;

		int currSectorIndex = m_pQuestLevel->GetCurrSectorIndex();
		if(currSectorIndex < (int)vecSectorXp.size() && currSectorIndex < (int)vecSectorBp.size())
		{
			float fSectorXP = (float)vecSectorXp[currSectorIndex];
			float fSectorBP = (float)vecSectorBp[currSectorIndex];

			// �ó������� �ݺ��� ������ ȹ�淮�� 2%�� ���� (����)
			int nRepeated = m_pQuestLevel->GetDynamicInfo()->nRepeated;
			for (int i=0; i<nRepeated; ++i)
			{
				fSectorXP *= 0.98f;
				fSectorBP *= 0.98f;
			}

			int nSectorXP = (int)fSectorXP;
			int nSectorBP = (int)fSectorBP;

			if ((nSectorXP > 0) || (nSectorBP > 0))
			{
				for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
				{
					int nAddedSectorXP = nSectorXP;
					int nAddedSectorBP = nSectorBP;

					CCMatchObject* pPlayer = (*itor).second->pObject;
					if ((!IsEnabledObject(pPlayer)) || (!pPlayer->CheckAlive())) continue;

					// ����ġ, �ٿ�Ƽ ���ʽ� ���
					const float fXPBonusRatio = CCMatchFormula::CalcXPBonusRatio(pPlayer, MIBT_QUEST);
					const float fBPBonusRatio = CCMatchFormula::CalcBPBounsRatio(pPlayer, MIBT_QUEST);
					nAddedSectorXP += (int)(nAddedSectorXP * fXPBonusRatio);
					nAddedSectorBP += (int)(nAddedSectorBP * fBPBonusRatio);

					// ���� ����
					MGetMatchServer()->ProcessPlayerXPBP(m_pStage, pPlayer, nAddedSectorXP, nAddedSectorBP);

					// �����
					int nExpPercent = CCMatchFormula::GetLevelPercent(pPlayer->GetCharInfo()->m_nXP, 
						pPlayer->GetCharInfo()->m_nLevel);
					unsigned long int nExpValue = MakeExpTransData(nAddedSectorXP, nExpPercent);
					RouteSectorBonus(pPlayer->GetUID(), nExpValue, nSectorBP);
				}
			}

			m_SurvivalGameLogInfoMgr.AccumulateXP(nSectorXP);
			m_SurvivalGameLogInfoMgr.AccumulateBP(nSectorBP);
		}
		else
			ASSERT(0);
	}
}

// ���� Ŭ����
void CCMatchRuleSurvival::OnSectorCompleted()
{
	RewardSectorXpBp();
	
	// ���� ��� ��Ȱ��Ų��.
	//	RefreshPlayerStatus();

	MoveToNextSector();
}

// ����Ʈ ������
void CCMatchRuleSurvival::OnCompleted()
{
	RewardSectorXpBp();

	SendGameResult();
	PostPlayerPrivateRanking();
	PostRankingList();

	CCMatchRuleBaseQuest::OnCompleted();

#ifdef _QUEST_ITEM
	// ���⼭ DB�� QuestGameLog����.
	PostInsertQuestGameLogAsyncJob();	
	SetCombatState(CCQUEST_COMBAT_NONE);
#endif

}

// ����Ʈ ���н�
void CCMatchRuleSurvival::OnFailed()
{
	SetCombatState(CCQUEST_COMBAT_NONE);
	m_bQuestCompleted = false;

	SendGameResult();
	PostPlayerPrivateRanking();
	PostRankingList();

	CCMatchRuleBaseQuest::OnFailed();

	PostInsertQuestGameLogAsyncJob();
}

// ����Ʈ�� ��� �������� üũ
bool CCMatchRuleSurvival::CheckQuestCompleted()
{
	if (m_pQuestLevel)
	{
		// �ʹ� ���� �������� üũ
		unsigned long int nStartTime = GetStage()->GetStartTime();
		unsigned long int nNowTime = CCMatchServer::GetInstance()->GetTickTime();

		// �ּ��� �� ���ͺ� ���� ���� ������ * ���ͼ���ŭ�� �ð��� �귯�� ������ ���� �� �ִٰ� ������.
		unsigned long int nCheckTime = QUEST_COMBAT_PLAY_START_DELAY * m_pQuestLevel->GetMapSectorCount();

		if (MGetTimeDistance(nStartTime, nNowTime) < nCheckTime) return false;

		// �����̹��̴� �ó����� ���� �� * �ݺ�Ƚ���� ä���� ������ �ִ�
		if (m_pQuestLevel->GetStaticInfo()->pScenario->nRepeat == (m_pQuestLevel->GetDynamicInfo()->nRepeated+1) &&
		    m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1))
			return true;
	}

	return false;
}

// ������ ���ʹ� �������� ���� �� �ֵ��� ������ �ð��� �д�.
bool CCMatchRuleSurvival::CheckQuestCompleteDelayTime()
{
	if ((m_pQuestLevel) && (m_pQuestLevel->GetMapSectorCount() == (m_pQuestLevel->GetCurrSectorIndex()+1)))
	{
		unsigned long int nNowTime = CCMatchServer::GetInstance()->GetTickTime();
		if (m_nQuestCompleteTime == 0)
			m_nQuestCompleteTime = nNowTime;
		if (MGetTimeDistance(m_nQuestCompleteTime, nNowTime) > QUEST_COMPLETE_DELAY)
			return true;

		return false;
	}

	return true;
}

void CCMatchRuleSurvival::ProcessCombatPlay()
{
	ProcessNPCSpawn();

}

void CCMatchRuleSurvival::MakeNPCnSpawn(CCQUEST_NPC nNPCID, bool bAddQuestDropItem, bool bKeyNPC)
{
	CCQuestNPCSpawnType nSpawnType = MNST_MELEE;
	CCQuestNPCInfo* pNPCInfo = CCMatchServer::GetInstance()->GetQuest()->GetNPCInfo(nNPCID);
	if (pNPCInfo)
	{
		nSpawnType = pNPCInfo->GetSpawnType();
		int nPosIndex = m_pQuestLevel->GetRecommendedSpawnPosition(nSpawnType, CCMatchServer::GetInstance()->GetTickTime());

		CCMatchNPCObject* pNPCObject = SpawnNPC(nNPCID, nPosIndex, bKeyNPC);

		if (pNPCObject)
		{
			// drop item ����
			CCQuestDropItem item;
			int nDropTableID = pNPCInfo->nDropTableID;
			int nQL = m_pQuestLevel->GetStaticInfo()->nQL;
			CCMatchServer::GetInstance()->GetQuest()->GetDropTable()->Roll(item, nDropTableID, nQL);

			// AddQuestDropItem=false�̸� ��������۸� ����Ѵ�.
			if ((bAddQuestDropItem==true) || (item.nDropItemType == QDIT_WORLDITEM))
			{
				pNPCObject->SetDropItem(&item);

				// ������� �������� level�� �־���´�.
				if ((item.nDropItemType == QDIT_QUESTITEM) || (item.nDropItemType == QDIT_ZITEM))
				{
					m_pQuestLevel->OnItemCreated((unsigned long int)(item.nID), item.nRentPeriodHour);
				}
			}
		}
	}
}

int CCMatchRuleSurvival::GetRankInfo(int nKilledNpcHpApAccum, int nDeathCount)
{
	// ��ŷ ����Ʈ ���� => { (óġ�� NPC���� �� HP + AP) / 10 } - (��� Ƚ�� * 100)
	int nRankInfo = (int)((nKilledNpcHpApAccum/10) - (nDeathCount*100));
	if(nRankInfo < 0)
		nRankInfo = 0;
	return nRankInfo;
}

void CCMatchRuleSurvival::ProcessNPCSpawn()
{
	if (CheckNPCSpawnEnable())
	{
		CCQUEST_NPC npc;
		if (m_pQuestLevel->GetNPCQueue()->Pop(npc))
		{
			bool bKeyNPC = m_pQuestLevel->GetNPCQueue()->IsKeyNPC(npc);

			MakeNPCnSpawn(npc, false, bKeyNPC);	// �����̹������� ��������۸� ���
		}
	}
	// �����̹��� ���ڰ� ����
	//else
	//{
	//	// �������� ��� Queue�� �ִ� NPC���� ��� ������������ Jaco���� ������Ų��.
	//	if (m_pQuestLevel->GetDynamicInfo()->bCurrBossSector)
	//	{
	//		// ������ ����ְ� �⺻������ ���� NPC�� �� ���´����� ������ ����
	//		if ((m_NPCManager.GetBossCount() > 0) /* && (m_pQuestLevel->GetNPCQueue()->IsEmpty()) */ )
	//		{
	//			int nAliveNPCCount = m_NPCManager.GetNPCObjectCount();


	//			if (m_JacoSpawnTrigger.CheckSpawnEnable(nAliveNPCCount))
	//			{
	//				int nCount = (int)m_JacoSpawnTrigger.GetQueue().size();
	//				for (int i = 0; i < nCount; i++)
	//				{
	//					CCQUEST_NPC npc = m_JacoSpawnTrigger.GetQueue()[i];
	//					MakeNPCnSpawn(npc, false);
	//				}
	//			}
	//		}
	//	}
	//}
}


bool CCMatchRuleSurvival::CheckNPCSpawnEnable()
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

void CCMatchRuleSurvival::OnRequestTestSectorClear()
{
	ClearAllNPC();

	SetCombatState(CCQUEST_COMBAT_COMPLETED);
}

void CCMatchRuleSurvival::OnRequestTestFinish()
{
	ClearAllNPC();

	m_pQuestLevel->GetDynamicInfo()->nCurrSectorIndex = m_pQuestLevel->GetMapSectorCount()-1;

	SetCombatState(CCQUEST_COMBAT_COMPLETED);
}


void CCMatchRuleSurvival::OnRequestMovetoPortal(const CCUID& uidPlayer)
{
	//	CCQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidPlayer);

	RouteMovetoPortal(uidPlayer);
}




void CCMatchRuleSurvival::OnReadyToNewSector(const CCUID& uidPlayer)
{
	CCQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidPlayer);
	if (pPlayerInfo)
	{
		pPlayerInfo->bMovedtoNewSector = true;
	}

	RouteReadyToNewSector(uidPlayer);

	// �� �÷��̾ ��Ʈ���ϴ� NPC�� ���� ��Ż��ź �ٸ� �÷��̾�� �ѱ��
	m_NPCManager.RemovePlayerControl(uidPlayer);
}

bool CCMatchRuleSurvival::OnCheckRoundFinish()
{
	return CCMatchRuleBaseQuest::OnCheckRoundFinish();
}

int CCMatchRuleSurvival::GetCurrentRoundIndex()
{
	if (!m_pQuestLevel) return 0;

	int nSectorIndex = m_pQuestLevel->GetCurrSectorIndex();
	int nRepeated = m_pQuestLevel->GetDynamicInfo()->nRepeated;
	int nSectorCount = (int)m_pQuestLevel->GetStaticInfo()->SectorList.size();
	return (nSectorIndex+1) + (nSectorCount * nRepeated);
}

void CCMatchRuleSurvival::SendGameResult()
{
	if (!m_pQuestLevel) return;

	CCQuestScenarioInfo* pScenario = m_pQuestLevel->GetStaticInfo()->pScenario;
	if (!pScenario) return;

	int nReachedRound = GetCurrentRoundIndex();
	
	CCMatchObject* pPlayer;

	// ���� ������ ����Ʈ ������ ��쿡�� �����ϰ� ��.
	if( CSM_TEST != MGetServerConfig()->GetServerMode() )  return;

	for (CCQuestPlayerManager::iterator itor = m_PlayerManager.begin(); itor != m_PlayerManager.end(); ++itor)
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;

		pPlayer = CCMatchServer::GetInstance()->GetObject((*itor).first);
		if( !IsEnabledObject(pPlayer) ) continue;

		// DB����ȭ ���� �˻�. ->�����̹��� ����Ʈ�������� �����Ƿ� ����, �׷��� ���� ���� ������Ʈ�� �� �ȿ��� �̷����Ƿ� ���� ������ ��Ȱ�ϸ� ���� �ʿ�
		//pPlayer->GetCharInfo()->GetDBQuestCachingData().IncreasePlayCount();

		// Ŀ�ǵ� ����
		RouteResultCommandToStage( pPlayer, nReachedRound, GetRankInfo(pPlayerInfo->nKilledNpcHpApAccum, pPlayerInfo->nDeathCount));

		MGetMatchServer()->ResponseCharacterItemList( pPlayer->GetUID() );
	}
}


void CCMatchRuleSurvival::InsertNoParamQItemToPlayer( CCMatchObject* pPlayer, CCQuestItem* pQItem )
{
	if( !IsEnabledObject(pPlayer) || (0 == pQItem) ) return;

	CCQuestItemMap::iterator itMyQItem = pPlayer->GetCharInfo()->m_QuestItemList.find( pQItem->GetItemID() );

	if( pPlayer->GetCharInfo()->m_QuestItemList.end() != itMyQItem )
	{
		// ������ ������ �ִ� ����Ʈ ������. ������ ���� �����ָ� ��.
		const int nOver = itMyQItem->second->Increase( pQItem->GetCount() );
		if( 0 < nOver )
			pQItem->Decrease( nOver );
	}
	else
	{
		// ó�� ȹ���� ����Ʈ ������. ���� �߰����� ��� ��.
		if( !pPlayer->GetCharInfo()->m_QuestItemList.CreateQuestItem(pQItem->GetItemID(), pQItem->GetCount(), pQItem->IsKnown()) )
			cclog( "CCMatchRuleSurvival::DistributeReward - %d��ȣ �������� Create( ... )�Լ� ȣ�� ����.\n", pQItem->GetItemID() );
	}
}


void CCMatchRuleSurvival::MakeRewardList()
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

		// Ȥ�� ���� ������ ������ �������� ���������� �𸣴� �ʱ�ȭ.
		pPlayerInfo->RewardQuestItemMap.Clear();
		pPlayerInfo->RewardZItemList.clear();

		a_vecPlayerInfos.push_back(pPlayerInfo);
	}

	for( itObtainQItem = m_pQuestLevel->GetDynamicInfo()->ItemMap.begin(); itObtainQItem != endObtainQItem; ++itObtainQItem )
	{
		pObtainQItem = itObtainQItem->second;

		// ȹ������ �������� ����.
		if (!pObtainQItem->bObtained) continue;	

		if (pObtainQItem->IsQuestItem())
		{
			// ����Ʈ ������ -----------------------------------------------------

			// �����Ҷ��� �ο��������� roll�� ��.
			nPos = RandomNumber( 0, nLimitRandNum );

			// ���� �����ִ� �ο����� Ŭ��� �׳� ����.
			if (( nPos < nPlayerCount ) && (nPos < (int)a_vecPlayerInfos.size()))
			{
				// ����Ʈ �������� ��� ó��
				CCQuestItemMap* pRewardQuestItemMap = &a_vecPlayerInfos[ nPos ]->RewardQuestItemMap;

				pRewardQItem = pRewardQuestItemMap->Find( pObtainQItem->nItemID );
				if( 0!= pRewardQItem )
					pRewardQItem->Increase(); // ������ ȹ���� ������.
				else
				{
					// ó�� ȹ��.
					if( !pRewardQuestItemMap->CreateQuestItem(pObtainQItem->nItemID, 1) )
					{
						cclog( "CCMatchRuleSurvival::MakeRewardList - ItemID:%d ó�� ȹ���� ������ ���� ����.\n", pObtainQItem->nItemID );
						continue;
					}
				}
			}
		}
		else
		{
			// �Ϲ� �������� ��� ó�� -------------------------------------------

			RewardZItemInfo iteminfo;
			iteminfo.nItemID		 = pObtainQItem->nItemID;
			iteminfo.nRentPeriodHour = pObtainQItem->nRentPeriodHour;

			int nLoopCounter = 0;
			const int MAX_LOOP_COUNT = 5;

			// �ִ� 5������ �������� �������� ������ ���� ����� ã�´�.
			while (nLoopCounter < MAX_LOOP_COUNT)
			{
				nLoopCounter++;

				// �����Ҷ��� �ο��������� roll�� ��.
				nPos = RandomNumber( 0, nLimitRandNum );

				// ���� �����ִ� �ο����� Ŭ��� �׳� ����.
				if (( nPos < nPlayerCount ) && (nPos < (int)a_vecPlayerInfos.size()))
				{
					CCQuestPlayerInfo* pPlayerInfo = a_vecPlayerInfos[ nPos ];
					CCQuestRewardZItemList* pRewardZItemList = &pPlayerInfo->RewardZItemList;

					// ������ ���ƾ߸� ���� �� �ִ�.
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


///< ����ġ�� �ٿ�Ƽ ��� �ű�. -by �߱���.
/*void CCMatchRuleSurvival::DistributeXPnBP( CCQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL )
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


	// ������ ����ġ, �ٿ�Ƽ ����
	if (IsEnabledObject(pPlayerInfo->pObject))
	{
		// ����ġ ���ʽ� ���
		const float fXPBonusRatio = CCMatchFormula::CalcXPBonusRatio(pPlayerInfo->pObject, MIBT_QUEST);
		const float fBPBonusRatio = CCMatchFormula::CalcBPBounsRatio(pPlayerInfo->pObject, MIBT_QUEST);

		int nExpBonus = (int)(pPlayerInfo->nXP * fXPBonusRatio);
		pPlayerInfo->nXP += nExpBonus;

		int nBPBonus = (int)(pPlayerInfo->nBP * fBPBonusRatio);
		pPlayerInfo->nBP += nBPBonus;

		CCMatchServer::GetInstance()->ProcessPlayerXPBP(m_pStage, pPlayerInfo->pObject, pPlayerInfo->nXP, pPlayerInfo->nBP);
	}
}*/

/*// ����Ʈ ������ ���
bool CCMatchRuleSurvival::DistributeQItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob)
{
	CCMatchObject* pPlayer = pPlayerInfo->pObject;
	if (!IsEnabledObject(pPlayer)) return false;

	CCQuestItemMap* pObtainQuestItemMap = &pPlayerInfo->RewardQuestItemMap;

	// Client�� �����Ҽ� �ִ� ���·� Quest item������ ������ Blob����.
	void* pSimpleQuestItemBlob = CCMakeBlobArray( sizeof(CCTD_QuestItemNode), static_cast<int>(pObtainQuestItemMap->size()) );
	if( 0 == pSimpleQuestItemBlob )
	{
		cclog( "CCMatchRuleSurvival::DistributeReward - Quest item ������ ���� Blob������ ����.\n" );
		return false;
	}

	// �α׸� ���ؼ� �ش� ������ ���� �������� ������ ������ ����.
	if( !m_QuestGameLogInfoMgr.AddRewardQuestItemInfo(pPlayer->GetUID(), pObtainQuestItemMap) )
	{
		cclog( "m_QuestGameLogInfoMgr -�ش� ������ �αװ�ü�� ã�µ� ����." );
	}

	int nBlobIndex = 0;
	for(CCQuestItemMap::iterator itQItem = pObtainQuestItemMap->begin(); itQItem != pObtainQuestItemMap->end(); ++itQItem )
	{
		CCQuestItem* pQItem = itQItem->second;
		CCQuestItemDesc* pQItemDesc = pQItem->GetDesc();
		if( 0 == pQItemDesc )
		{
			cclog( "CCMatchRuleSurvival::DistributeReward - %d �������� ��ũ���� ������ �Ǿ����� ����.\n", pQItem->GetItemID() );
			continue;
		}

		// ����ũ ���������� �˻縦 ��.
		pPlayer->GetCharInfo()->m_DBQuestCachingData.CheckUniqueItem( pQItem );
		// ������� Ƚ���� �˻縦 ��.
		pPlayer->GetCharInfo()->m_DBQuestCachingData.IncreaseRewardCount();

		if( MMQIT_MONBIBLE == pQItemDesc->m_nType )
		{
			// ���� ���� ó��.
			if( !pPlayer->GetCharInfo()->m_QMonsterBible.IsKnownMonster(pQItemDesc->m_nParam) )
				pPlayer->GetCharInfo()->m_QMonsterBible.WriteMonsterInfo( pQItemDesc->m_nParam );
		}
		else if( 0 != pQItemDesc->m_nParam )
		{
			// Param���� �����Ǿ� �ִ� �������� ���� ó���� ����� ��.				
		}
		else
		{
			// DB�� ������ �Ǵ� ����Ʈ �����۸� �������� ������.
			InsertNoParamQItemToPlayer( pPlayer, pQItem );
		}

		CCTD_QuestItemNode* pQuestItemNode;
		pQuestItemNode = reinterpret_cast< CCTD_QuestItemNode* >( CCGetBlobArrayElement(pSimpleQuestItemBlob, nBlobIndex++) );
		Make_MTDQuestItemNode( pQuestItemNode, pQItem->GetItemID(), pQItem->GetCount() );
	}

	*ppoutSimpleQuestItemBlob = pSimpleQuestItemBlob;
	return true;
}*/

/*bool CCMatchRuleSurvival::DistributeZItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob)
{
	CCMatchObject* pPlayer = pPlayerInfo->pObject;
	if (!IsEnabledObject(pPlayer)) return false;

	CCQuestRewardZItemList* pObtainZItemList = &pPlayerInfo->RewardZItemList;

	// Client�� �����Ҽ� �ִ� ���·� Quest item������ ������ Blob����.
	void* pSimpleZItemBlob = CCMakeBlobArray( sizeof(CCTD_QuestZItemNode), (int)(pObtainZItemList->size()) );
	if( 0 == pSimpleZItemBlob )
	{
		cclog( "CCMatchRuleSurvival::DistributeZItem - Ztem ������ ���� Blob������ ����.\n" );
		return false;
	}

	// ĳ�� ������ ȹ�� �α׸� ����� ����.
	if( !m_QuestGameLogInfoMgr.AddRewardZItemInfo(pPlayer->GetUID(), pObtainZItemList) )
	{
		cclog( "m_QuestGameLogInfoMgr -�ش� ������ �αװ�ü�� ã�µ� ����." );
	}

	int nBlobIndex = 0;
	for(CCQuestRewardZItemList::iterator itor = pObtainZItemList->begin(); itor != pObtainZItemList->end(); ++itor )
	{
		RewardZItemInfo iteminfo = (*itor);
		CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(iteminfo.nItemID);
		if (pItemDesc == NULL) continue;

		if (!IsEquipableItem(iteminfo.nItemID, MAX_LEVEL, pPlayer->GetCharInfo()->m_nSex)) 
			continue;

		// ������ ������ ���
		CCMatchServer::GetInstance()->InsertCharItem(pPlayer->GetUID(), iteminfo.nItemID, true, iteminfo.nRentPeriodHour);

		// ��ӻ���
		CCTD_QuestZItemNode* pZItemNode = (CCTD_QuestZItemNode*)(CCGetBlobArrayElement(pSimpleZItemBlob, nBlobIndex++));
		pZItemNode->m_nItemID = iteminfo.nItemID;
		pZItemNode->m_nRentPeriodHour = iteminfo.nRentPeriodHour;
	}

	*ppoutQuestRewardZItemBlob = pSimpleZItemBlob;

	return true;
}*/
/*
void CCMatchRuleSurvival::RouteRewardCommandToStage( CCMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob)
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
*/
void CCMatchRuleSurvival::RouteResultCommandToStage( CCMatchObject* pPlayer, int nReachedRound, int nPoint)
{
	if( !IsEnabledObject(pPlayer) )
		return;

	CCCommand* pNewCmd = CCMatchServer::GetInstance()->CreateCommand( MC_QUEST_SURVIVAL_RESULT, CCUID(0, 0) );
	if( 0 == pNewCmd )
		return;

	pNewCmd->AddParameter( new CCCmdParamInt(nReachedRound) );
	pNewCmd->AddParameter( new CCCmdParamInt(nPoint) );

	CCMatchServer::GetInstance()->RouteToListener( pPlayer, pNewCmd );
}



void CCMatchRuleSurvival::OnRequestPlayerDead(const CCUID& uidVictim)
{
	CCQuestPlayerManager::iterator itor = m_PlayerManager.find(uidVictim);
	if (itor != m_PlayerManager.end())
	{
		CCQuestPlayerInfo* pPlayerInfo = (*itor).second;
		pPlayerInfo->nDeathCount++;
	}
}


void CCMatchRuleSurvival::OnObtainWorldItem(CCMatchObject* pObj, int nItemID, int* pnExtraValues)
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
		// true���̸� ������ ��������.

		if (IsQuestItemID(nQuestItemID))
			RouteObtainQuestItem(unsigned long int(nQuestItemID));
		else 
			RouteObtainZItem(unsigned long int(nQuestItemID));
	}
}


void CCMatchRuleSurvival::OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		OnResponseDropSacrificeItemOnSlot( uidSender, nSlotIndex, nItemID );
	}
}


void CCMatchRuleSurvival::OnResponseDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( (MAX_SACRIFICE_SLOT_COUNT > nSlotIndex) && (0 <= nSlotIndex) ) 
	{
		// �ߺ� �˻�.
		// if( IsSacrificeItemDuplicated(uidSender, nSlotIndex, nItemID) )
		//	return;

		CCQuestItemDesc* pQItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID );
		if( 0 == pQItemDesc )
		{
			// ItemID�� �� �������̰ų� ItemID�� �ش��ϴ� Description�� ����.
			// ����ư error...

			cclog( "CCMatchRuleBaseQuest::SetSacrificeItemOnSlot - ItemID�� �� �������̰ų� %d�� �ش��ϴ� Description�� ����.\n", nItemID );
			ASSERT( 0 );
			return;
		}

		// �������� Ÿ���� ����������� ��츸 ����.
		if( pQItemDesc->m_bSecrifice )
		{
			CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
			if( !IsEnabledObject(pPlayer) )
			{
				cclog( "CCMatchRuleBaseQuest::SetSacrificeItemOnSlot - ������ ����.\n" );
				return;
			}

			CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( pPlayer->GetStageUID() );
			if( 0 == pStage )
				return;

			// �ƹ��� ���Կ� �����Ҽ� ����.

			CCQuestItem* pQuestItem = pPlayer->GetCharInfo()->m_QuestItemList.Find( nItemID );
			if( 0 == pQuestItem )
				return;

			// ������ ������� �˻�.
			int nMySacriQItemCount = CalcuOwnerQItemCount( uidSender, nItemID );
			if( -1 == nMySacriQItemCount )
				return;
			if( nMySacriQItemCount >= pQuestItem->GetCount() )
			{
				// ������ �����ؼ� �ø��� ���ߴٰ� �뺸��.
				CCCommand* pCmdMore = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM, CCUID(0, 0) );
				if( 0 == pCmdMore )
					return;

				pCmdMore->AddParameter( new CCCmdParamInt(NEED_MORE_QUEST_ITEM) );
				pCmdMore->AddParameter( new CCCmdParamUID(uidSender) );
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
			pCmdOk->AddParameter( new CCCmdParamUID(uidSender) );
			pCmdOk->AddParameter( new CCCmdParamInt(nSlotIndex) );
			pCmdOk->AddParameter( new CCCmdParamInt(nItemID) );

			CCMatchServer::GetInstance()->RouteToStage( pStage->GetUID(), pCmdOk );

			// �Ϲ����� ó��.
			m_SacrificeSlot[ nSlotIndex ].SetAll( uidSender, nItemID );

			// ������ ������ ������Ʈ�Ǹ� ������Ʈ�� ������ �ٽ� ������.
			RefreshStageGameInfo();
		}
		else
		{
			// ����������� �ƴ�.
			ASSERT( 0 );
			return;
		}// if( pQItemDesc->m_bSecrifice )
	}
	else
	{
		// ������ �ε����� �� ��������.
		cclog( "CCMatchRuleBaseQuest::OnResponseDropSacrificeItemOnSlot - %d�� ���� �ε����� ��ȿ���� �ʴ� �ε�����.\n", nSlotIndex );
		ASSERT( 0 );
		return;
	}
}


void CCMatchRuleSurvival::OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		OnResponseCallBackSacrificeItem( uidSender, nSlotIndex, nItemID );
	}
}


void CCMatchRuleSurvival::OnResponseCallBackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	// �ƹ��� �����Ҽ� ����.
	if( (MAX_SACRIFICE_SLOT_COUNT <= nSlotIndex) && (0 > nSlotIndex) ) 
		return;


	if( (0 == nItemID) || (0 == m_SacrificeSlot[nSlotIndex].GetItemID()) )
		return;

	if( nItemID != m_SacrificeSlot[nSlotIndex].GetItemID() )
		return;

	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
	{
		cclog( "CCMatchRuleBaseQuest::OnResponseCallBackSacrificeItem - ���������� ����.\n" );
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
	pCmdOk->AddParameter( new CCCmdParamUID(uidSender) );									// ������ ȸ���� ��û�� ���̵�.
	pCmdOk->AddParameter( new CCCmdParamInt(nSlotIndex) );
	pCmdOk->AddParameter( new CCCmdParamInt(nItemID) );

	CCMatchServer::GetInstance()->RouteToStage( pPlayer->GetStageUID(), pCmdOk );

	m_SacrificeSlot[ nSlotIndex ].Release();	

	// ������ ������ ������Ʈ�Ǹ� QL�� �ٽ� ������.
	RefreshStageGameInfo();
}


bool CCMatchRuleSurvival::IsSacrificeItemDuplicated( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
	if( (uidSender == m_SacrificeSlot[nSlotIndex].GetOwnerUID()) && (nItemID == m_SacrificeSlot[nSlotIndex].GetItemID()) )
	{
		// ���� �������� �÷��������� �߱⿡ �׳� ������ ����.

		return true;
	}

	return false;
}


/*
* ���������� ���������� ó���ؾ� �� ���� ������� ���⿡ ������.
*/
void CCMatchRuleSurvival::PreProcessLeaveStage( const CCUID& uidLeaverUID )
{
	CCMatchRuleBaseQuest::PreProcessLeaveStage( uidLeaverUID );

	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidLeaverUID );
	if( !IsEnabledObject(pPlayer) )
		return;

	/*if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		// ���������� �������� ������ ������ ��� �������� ���Կ� �÷� ���Ҵ��� �˻縦 ��.
		// ���� �÷����� �������� �ִٸ� �ڵ����� ȸ���� ��. - �������϶��� ����
		if (GetStage()->GetState() == STAGE_STATE_STANDBY) 
		{
			// ������ ��������� ����.
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

				// ����� ���� ������ ������.
				OnResponseSacrificeSlotInfoToStage( pStage->GetUID() );
			}
		}
	}*/
}

/*
void CCMatchRuleSurvival::DestroyAllSlot()
{
	// ���⼭ ���Կ� �÷����ִ� �������� �Ҹ��Ŵ.

	CCMatchObject*	pOwner;
	CCQuestItem*		pQItem;
	CCUID			uidOwner;
	unsigned long	nItemID;

	for( int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; ++i )
	{
		if( CCUID(0, 0) == m_SacrificeSlot[i].GetOwnerUID() )
			continue;

		uidOwner = m_SacrificeSlot[ i ].GetOwnerUID();

		// �������� ������ ���������� �˻�.
		pOwner = CCMatchServer::GetInstance()->GetObject( uidOwner );
		if( !IsEnabledObject(pOwner) )
		{
			continue;
		}

		nItemID = m_SacrificeSlot[ i ].GetItemID();

		// �������� �������� ���������� �˻�.
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
*/

///
// First	: �߱���.
// Last		: �߱���.
//
// QL������ ��û�� ó����. �⺻������ ��û���� ���������� �뺸��.
///

void CCMatchRuleSurvival::OnRequestQL( const CCUID& uidSender )
{
	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject( uidSender );
		if( 0 == pPlayer )
		{
			cclog( "CCMatchRuleSurvival::OnRequestQL - ������ ����.\n" );
			return;
		}

		OnResponseQL_ToStage( pPlayer->GetStageUID() );
	}
}


///
// First : �߱���.
// Last  : �߱���.
//
// ��û���� ���������� QL������ �뺸.
///
void CCMatchRuleSurvival::OnResponseQL_ToStage( const CCUID& uidStage )
{
	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( uidStage );
	if( 0 == pStage )
	{
		cclog( "CCMatchRuleSurvival::OnRequestQL - �������� �˻� ����.\n" );
		return;
	}

	RefreshStageGameInfo();
}

///
// First : �߱���.
// Last  : �߱���.
//
// ���� ������ ������ ��û. �⺻������ ���������� �˸�.
///
void CCMatchRuleSurvival::OnRequestSacrificeSlotInfo( const CCUID& uidSender )
{
	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
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
// First : �߱���.
// Last  : �߱���.
//
// ���� ������ ������ ��û�ڿ� �˸�.
///
void CCMatchRuleSurvival::OnResponseSacrificeSlotInfoToListener( const CCUID& uidSender )
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

	pCmd->AddParameter( new CCCmdParamUID(m_SacrificeSlot[0].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[0].GetItemID()) );
	pCmd->AddParameter( new CCCmdParamUID(m_SacrificeSlot[1].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[1].GetItemID()) );

	CCMatchServer::GetInstance()->RouteToListener( pPlayer, pCmd );
}


///
// First : �߱���.
// Last  : �߱���.
//
// ���� ������ ������ ���������� �˸�.
///
void CCMatchRuleSurvival::OnResponseSacrificeSlotInfoToStage( const CCUID& uidStage )
{
	CCMatchStage* pStage = CCMatchServer::GetInstance()->FindStage( uidStage );
	if( 0 == pStage )
		return;

	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_RESPONSE_SLOT_INFO, CCUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new CCCmdParamUID(m_SacrificeSlot[0].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[0].GetItemID()) );
	pCmd->AddParameter( new CCCmdParamUID(m_SacrificeSlot[1].GetOwnerUID()) );
	pCmd->AddParameter( new CCCmdParamInt(m_SacrificeSlot[1].GetItemID()) );

	CCMatchServer::GetInstance()->RouteToStage( uidStage, pCmd );
}


void CCMatchRuleSurvival::PostInsertQuestGameLogAsyncJob()
{
	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		CollectEndQuestGameLogInfo();
		m_SurvivalGameLogInfoMgr.PostInsertSurvivalGameLog();
	}
}




int CCMatchRuleSurvival::CalcuOwnerQItemCount( const CCUID& uidPlayer, const unsigned long nItemID )
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

const bool CCMatchRuleSurvival::PostNPCInfo()
{
	CCMatchQuest*		pQuest			= CCMatchServer::GetInstance()->GetQuest();
	CCQuestScenarioInfo* pScenarioInfo	= pQuest->GetSurvivalScenarioInfo( m_StageGameInfo.nScenarioID );

	if( NULL == pScenarioInfo )
	{
		return false;
	}

	void* pBlobNPC = CCMakeBlobArray(sizeof(CCTD_NPCINFO), int(m_vecNPCInThisScenario.size()) );
	if( NULL == pBlobNPC )
	{
		return false;
	}

	vector< CCQUEST_NPC >::iterator	itNL;
	vector< CCQUEST_NPC >::iterator	endNL;
	CCQuestNPCInfo*					pQuestNPCInfo		= NULL;
	int								nNPCIndex			= 0;
	CCTD_NPCINFO*					pCCTD_QuestNPCInfo	= NULL;
	ItorReinforedNPCStat			itStat;

	endNL = m_vecNPCInThisScenario.end();
	for( itNL = m_vecNPCInThisScenario.begin(); endNL != itNL; ++ itNL )
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

		if (m_pQuestLevel)
		{
			// �⺻ NPC���� ���� �ó����� �ݺ��� ���� ��ȭ�� �ɷ�ġ�� �����
			itStat = m_mapReinforcedNPCStat.find((*itNL));
			if (itStat != m_mapReinforcedNPCStat.end())
			{
				pCCTD_QuestNPCInfo->m_nMaxAP = (int)itStat->second.fMaxAP;
				pCCTD_QuestNPCInfo->m_nMaxHP = (int)itStat->second.fMaxHP; 
			}
			else
				_ASSERT(0);
		}
	}

	CCCommand* pCmdNPCList = MGetMatchServer()->CreateCommand( MC_QUEST_NPCLIST, CCUID(0, 0) );
	if( NULL == pCmdNPCList )
	{
		CCEraseBlobArray( pBlobNPC );
		return false;
	}

	pCmdNPCList->AddParameter( new CCCommandParameterBlob(pBlobNPC, CCGetBlobArraySize(pBlobNPC)) );
	pCmdNPCList->AddParameter( new CCCommandParameterInt(GetGameType()) );

	MGetMatchServer()->RouteToStage( m_pStage->GetUID(), pCmdNPCList );

	CCEraseBlobArray( pBlobNPC );

	return true;
}

bool CCMatchRuleSurvival::PostRankingList()
{
	// ���� ������ ����Ʈ ������ ��쿡�� �����ϰ� ��.
	if( CSM_TEST != MGetServerConfig()->GetServerMode() )  return false;

	void* pBlobRanking = CCMakeBlobArray(sizeof(CCTD_SurvivalRanking), MAX_SURVIVAL_RANKING_LIST );
	if( NULL == pBlobRanking )
		return false;

	//CCMatchServer::GetInstance()->GetQuest()->GetSurvivalRankInfo()->FillDummyRankingListForDebug();	//todos del

	const CCSurvivalRankInfo* pRankInfo = CCMatchServer::GetInstance()->GetQuest()->GetSurvivalRankInfo();
	const SurvivalRanking* pRank;
	CCTD_SurvivalRanking* pCCTD_Rank;

	for (int i = 0; i < MAX_SURVIVAL_RANKING_LIST; ++i)
	{
		pCCTD_Rank= reinterpret_cast< CCTD_SurvivalRanking* >( CCGetBlobArrayElement(pBlobRanking, i) );
		if( NULL == pCCTD_Rank ) {
			_ASSERT( 0 );
			CCEraseBlobArray( pBlobRanking );
			return false;
		}

		pRank = pRankInfo->GetRanking( m_StageGameInfo.nMapsetID - 1, i );
		if (pRank) {
			pCCTD_Rank->m_dwRank = pRank->dwRank;
			pCCTD_Rank->m_dwPoint = pRank->dwRankPoint;
			strcpy(pCCTD_Rank->m_szCharName, pRank->szCharacterName);
		} else {
			pCCTD_Rank->m_dwRank = 0;
			pCCTD_Rank->m_dwPoint = 0;
			strcpy(pCCTD_Rank->m_szCharName, "");
		}
	}

	CCCommand* pCmdRankingList = MGetMatchServer()->CreateCommand( MC_SURVIVAL_RANKINGLIST, CCUID(0, 0) );
	if( NULL == pCmdRankingList )
	{
		CCEraseBlobArray( pBlobRanking );
		return false;
	}

	pCmdRankingList->AddParameter( new CCCommandParameterBlob(pBlobRanking, CCGetBlobArraySize(pBlobRanking)) );

	MGetMatchServer()->RouteToStage( m_pStage->GetUID(), pCmdRankingList );

	CCEraseBlobArray( pBlobRanking );

	return true;
}

// ������ �����ϱ����� �غ� �۾��� ������.
// �غ� �۾��� ���а� �����ô� ������ �������� ���ϰ� �ؾ� ��.
///
bool CCMatchRuleSurvival::PrepareStart()
{
	if( CSM_TEST == MGetServerConfig()->GetServerMode() && true == MGetServerConfig()->IsEnabledSurvivalMode()) 
	{
		MakeStageGameInfo();

		if ((m_StageGameInfo.nScenarioID > 0) || (m_StageGameInfo.nMapsetID > 0))
		{
			CollectNPCListInThisScenario();		// �� �ó��������� ���� NPC ���� ����� �ۼ�

			if( PostNPCInfo() )
			{
				return true;
			}
		}
	}

	return false;
}

void CCMatchRuleSurvival::MakeStageGameInfo()
{	
	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
	{
		if( (GetStage()->GetState() != STAGE_STATE_STANDBY) && (STAGE_STATE_COUNTDOWN != GetStage()->GetState()) )
		{
			return;
		}

		// ���Կ� Level�� �´� �������� �������� �÷��� �ִ��� �˻簡 �ʿ���.
		// ������ �������� �÷��� ������� ������ ȸ�� ��û�� ����� ��.
		int nOutResultQL = -1;

		int nMinPlayerLevel = 1;
		CCMatchStage* pStage = GetStage();
		if (pStage != NULL)
		{
			nMinPlayerLevel = pStage->GetMinPlayerLevel();

			// ������ ����̸� �ּҷ����� ��� ������ ���������Ѵ�.
			CCMatchObject* pMaster = CCMatchServer::GetInstance()->GetObject(pStage->GetMasterUID());
			if (IsAdminGrade(pMaster))
			{
				nMinPlayerLevel = pMaster->GetCharInfo()->m_nLevel;
			}
		}

		//int nPlayerQL = CCQuestFormula::CalcQL( nMinPlayerLevel );
		int nPlayerQL = 0;	// �����̹������� �÷��̾� ������ ������� ������ QL=0�� �ó������� �۵��ǵ��� �Ѵ�
		//		m_StageGameInfo.nPlayerQL = nPlayerQL;

		unsigned int SQItems[MAX_SCENARIO_SACRI_ITEM];
		for (int i = 0; i < MAX_SCENARIO_SACRI_ITEM; i++)
		{
			SQItems[i] = (unsigned int)m_SacrificeSlot[i].GetItemID();
		}

		// �ϵ��ڵ�.. �Ƕ�... -_-;
		m_StageGameInfo.nMapsetID = 1;
		if ( !stricmp( pStage->GetMapName(), "mansion"))
			m_StageGameInfo.nMapsetID = 1;
		else if ( !stricmp( pStage->GetMapName(), "prison"))
			m_StageGameInfo.nMapsetID = 2;
		else if ( !stricmp( pStage->GetMapName(), "dungeon"))
			m_StageGameInfo.nMapsetID = 3;


		CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();
		unsigned int nScenarioID = pQuest->GetSurvivalScenarioCatalogue()->MakeScenarioID(m_StageGameInfo.nMapsetID,
			nPlayerQL, SQItems);

		m_StageGameInfo.nScenarioID = nScenarioID;
		CCQuestScenarioInfo* pScenario = pQuest->GetSurvivalScenarioCatalogue()->GetInfo(nScenarioID);
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

void CCMatchRuleSurvival::RefreshStageGameInfo()
{
	MakeStageGameInfo();
	RouteStageGameInfo();
}

void CCMatchRuleSurvival::OnChangeCondition()
{
	RefreshStageGameInfo();
}

void CCMatchRuleSurvival::CollectStartingQuestGameLogInfo()
{
	// �����ϱ����� ������ ������ �ݵ�� ������ ��.
	m_SurvivalGameLogInfoMgr.Clear();

	if( QuestTestServer() ) 
	{
		_ASSERT(m_PlayerManager.size() <= 4);

		for(CCQuestPlayerManager::iterator it = m_PlayerManager.begin(); it != m_PlayerManager.end(); ++it )
		{
			CCQuestPlayerInfo* pPlayerInfo = (*it).second;
			CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject((*it).first);
			if (IsEnabledObject(pPlayer))
			{
				m_SurvivalGameLogInfoMgr.AddPlayer( pPlayer->GetCharInfo()->m_nCID );
			}
		}

		CCMatchObject* pMaster = CCMatchServer::GetInstance()->GetObject( GetStage()->GetMasterUID() );
		if( IsEnabledObject(pMaster) )
			m_SurvivalGameLogInfoMgr.SetMasterCID( pMaster->GetCharInfo()->m_nCID );

		m_SurvivalGameLogInfoMgr.SetScenarioID( m_StageGameInfo.nMapsetID); //m_pQuestLevel->GetStaticInfo()->pScenario->nID );
		// �����̹��� �ʼ¸��� �ó������� 1�����̹Ƿ� ���� �ó�����ID�� �������� �ʴ´�

		m_SurvivalGameLogInfoMgr.SetStageName( GetStage()->GetName() );
		m_SurvivalGameLogInfoMgr.SetStartTime( timeGetTime() );
	}
}


void CCMatchRuleSurvival::CollectEndQuestGameLogInfo()
{
	m_SurvivalGameLogInfoMgr.SetReachedRound( GetCurrentRoundIndex() );
	m_SurvivalGameLogInfoMgr.SetEndTime( timeGetTime() );

	if( QuestTestServer() ) 
	{
		_ASSERT(m_PlayerManager.size() <= 4);

		for(CCQuestPlayerManager::iterator it = m_PlayerManager.begin(); it != m_PlayerManager.end(); ++it )
		{
			CCQuestPlayerInfo* pPlayerInfo = (*it).second;
			CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject((*it).first);
			if (IsEnabledObject(pPlayer))
			{ // ������ �÷��̾� ��ŷ ���� ������Ʈ ���ش�.
				m_SurvivalGameLogInfoMgr.SetPlayerRankPoint(pPlayer->GetCharInfo()->m_nCID, GetRankInfo(pPlayerInfo->nKilledNpcHpApAccum, pPlayerInfo->nDeathCount));
			}
		}
	}
}

bool CCMatchRuleSurvival::CollectNPCListInThisScenario()
{
	m_vecNPCInThisScenario.clear();

	CCMatchQuest*		pQuest			= CCMatchServer::GetInstance()->GetQuest();
	CCQuestScenarioInfo* pScenarioInfo	= pQuest->GetSurvivalScenarioInfo( m_StageGameInfo.nScenarioID );

	if( pScenarioInfo == NULL )	return false;

	for( size_t i = 0; i < SCENARIO_STANDARD_DICE_SIDES; ++i )
	{
		MakeSurvivalKeyNPCList( m_vecNPCInThisScenario, pScenarioInfo->Maps[i] );
		MakeNomalNPCList( m_vecNPCInThisScenario, pScenarioInfo->Maps[i], pQuest );
	}

	return true;
}

void CCMatchRuleSurvival::ReinforceNPC()
{
	if (!m_pQuestLevel) {_ASSERT(0); return;}

	int nRepeated = m_pQuestLevel->GetDynamicInfo()->nRepeated;
	if (nRepeated == 0)
	{
		m_mapReinforcedNPCStat.clear();

		CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();
		CCQuestNPCInfo* pNpcInfo;
		CCQUEST_NPC npcID;
		for (unsigned int i=0; i<m_vecNPCInThisScenario.size(); ++i)
		{
			npcID = m_vecNPCInThisScenario[i];
			pNpcInfo = pQuest->GetNPCInfo(npcID);
			if (!pNpcInfo)
				{_ASSERT(0);continue;}

			CCQuestLevelReinforcedNPCStat& npcStat = m_mapReinforcedNPCStat[npcID];
			npcStat.fMaxAP = (float)pNpcInfo->nMaxAP;
			npcStat.fMaxHP = (float)pNpcInfo->nMaxHP;
		}
	}
	else
	{
		const float reinforcementRate = 1.15f;	// HP AP ��ȸ 15% ���� (����)
		ItorReinforedNPCStat it;
		for (it=m_mapReinforcedNPCStat.begin(); it!=m_mapReinforcedNPCStat.end(); ++it)
		{
			it->second.fMaxHP *= reinforcementRate;
			it->second.fMaxAP *= reinforcementRate;
		}
	}
}

void CCMatchRuleSurvival::OnRequestNPCDead( CCUID& uidSender, CCUID& uidKiller, CCUID& uidNPC, CCVector& pos )
{
	// ��������� ���� ���� NPC�� HP/AP�� ����
	CCMatchNPCObject* pNPC = m_NPCManager.GetNPCObject(uidNPC);
	if (pNPC)
	{
		ItorReinforedNPCStat it = m_mapReinforcedNPCStat.find( pNPC->GetType() );
		if (m_mapReinforcedNPCStat.end() != it)
		{
			const CCQuestLevelReinforcedNPCStat& npcStat = it->second;
			CCQuestPlayerInfo* pPlayerInfo = m_PlayerManager.GetPlayerInfo(uidKiller);
			if(pPlayerInfo)
			{
				pPlayerInfo->nKilledNpcHpApAccum += (unsigned int)npcStat.fMaxAP;
				pPlayerInfo->nKilledNpcHpApAccum += (unsigned int)npcStat.fMaxHP;
			}
		}
		else
			ASSERT(0);
	}
	
	CCMatchRuleBaseQuest::OnRequestNPCDead(uidSender, uidKiller, uidNPC, pos);
}

void CCMatchRuleSurvival::PostPlayerPrivateRanking()
{
	for(CCQuestPlayerManager::iterator it = m_PlayerManager.begin(); it != m_PlayerManager.end(); ++it )
	{
		CCMatchObject* pPlayer = it->second->pObject;
		if (IsEnabledObject(pPlayer))
		{
			CCMatchServer::GetInstance()->OnRequestSurvivalModePrivateRanking( 
				GetStage()->GetUID(), pPlayer->GetUID(), m_StageGameInfo.nMapsetID, pPlayer->GetCharInfo()->m_nCID );
		}
	}
}