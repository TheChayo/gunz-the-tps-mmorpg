#include "stdafx.h"
#include "CCMatchRuleBaseQuest.h"
#include "CCMath.h"
#include <mmsystem.h>
#include "CCBlobArray.h"
#include "CCQuestFormula.h"
#include "CCQuestLevelGenerator.h"


const int NPC_ASSIGN_DELAY = 5000;
const int LATENCY_CHECK_DELAY = 5000;
const int PING_LOST_LIMIT = 2000;


//////////////////////////////////////////////////////////////////////////////////
CCMatchRuleBaseQuest::CCMatchRuleBaseQuest(CCMatchStage* pStage) 
		:	CCMatchRule(pStage), m_nLastNPCSpawnTime(0), m_nNPCSpawnCount(0),
			m_nSpawnTime(500), m_nFirstPlayerCount(1)
{
	
}

CCMatchRuleBaseQuest::~CCMatchRuleBaseQuest()
{
	
}

void CCMatchRuleBaseQuest::OnBegin()
{
	m_PlayerManager.Create(m_pStage);
	m_NPCManager.Create(m_pStage, &m_PlayerManager);
	m_nFirstPlayerCount = (int)m_pStage->GetObjCount();
	m_nNPCSpawnCount=0;

	// ���� ������ �� �������� ������.
	RouteGameInfo();

	m_bQuestCompleted = false;
}

void CCMatchRuleBaseQuest::OnEnd()
{
	m_NPCManager.Destroy();
	m_PlayerManager.Destroy();
}

bool CCMatchRuleBaseQuest::OnRun()
{
	if (GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		SendClientLatencyPing();
		ReAssignNPC();
	}

	return CCMatchRule::OnRun();
}

void CCMatchRuleBaseQuest::OnRoundBegin()
{
	CCMatchRule::OnRoundBegin();

	m_nLastNPCAssignCheckTime = CCMatchServer::GetInstance()->GetGlobalClockCount();
	m_nLastPingTime = CCMatchServer::GetInstance()->GetGlobalClockCount();
}

void CCMatchRuleBaseQuest::OnRoundEnd()
{
	CCMatchRule::OnRoundEnd();

}

bool CCMatchRuleBaseQuest::OnCheckRoundFinish()
{
	if (CheckPlayersAlive() == false)
	{
		/// �� ������ ������. => ���� ����� �뺸�ϴ� ������� �����Ǿ�� ��. - by �߱���.

		// �� �׾������� �׳� ������ ���� ����Ʈ�� ���е� ������ ������ �Ǿ�� �Ѵ�.
		// ����� �� �׾������� ������ �׳� ������ ���������� ����Ʈ�� ���������� ���� ������ ó���� �Ѵ�.

#ifdef _QUEST_ITEM
		// ����Ʈ ���и� ���⼭ �ٷ� ó������.
		OnFailed();
#endif

		return true;
	}

	// Quest Completed�Ǿ����� ���� �ð��� ����ȴ�. ������ �׳� �ٷ�..-_-��
	if (m_bQuestCompleted)
	{
		return true;
	}



	return false;
}

void CCMatchRuleBaseQuest::OnRoundTimeOut()
{
	CCMatchRule::OnRoundTimeOut();
}

// ��ȯ���� false�̸� ������ ������.
bool CCMatchRuleBaseQuest::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool CCMatchRuleBaseQuest::OnCheckEnableBattleCondition()
{
	return true;
}

void CCMatchRuleBaseQuest::OnLeaveBattle(CCUID& uidChar)
{
	if (GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
	}
	CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject(uidChar);
	if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) return;

	m_NPCManager.OnDelPlayer(uidChar);
	m_PlayerManager.DelPlayer(uidChar);
}

void CCMatchRuleBaseQuest::OnEnterBattle(CCUID& uidChar)
{

}

void CCMatchRuleBaseQuest::OnCommand(MCommand* pCommand)
{



}

void CCMatchRuleBaseQuest::OnRequestNPCDead(CCUID& uidSender, CCUID& uidKiller, CCUID& uidNPC, MVector& pos)
{
	if (m_NPCManager.IsControllersNPC(uidSender, uidNPC))
	{
		CCQuestDropItem DropItem;
		if (m_NPCManager.DestroyNPCObject(uidNPC, DropItem))
		{
			MCommand* pNew = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_NPC_DEAD, uidSender);
			pNew->AddParameter(new MCmdParamUID(uidKiller));
			pNew->AddParameter(new MCmdParamUID(uidNPC));
			CCMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);

			CheckRewards(uidKiller, &DropItem, pos);
		}
	}
	else
	{
		if(m_NPCManager.GetNPCObject(uidNPC)==NULL) {
			// finish�� ���Ŀ��� npc���� Ŭ����� �����̴�
			//_ASSERT(0);
		}else {
			// �������ִ�.
			if (GetGameType() == MMATCH_GAMETYPE_QUEST)
				_ASSERT(0);

			// �����̹������� ���� ���Ƶ� ������ ������ ���尡 �����Ƿ�, npc ��Ʈ�ѷ��� �÷��̾ ��Ż�� Ÿ��
			// �Ѿ���ȴٸ� �̷� ��찡 ������� ������ ������ ������ �ʴ´�
		}
	}
}

void CCMatchRuleBaseQuest::OnRequestPlayerDead(const CCUID& uidVictim)
{

}

void CCMatchRuleBaseQuest::CheckRewards(CCUID& uidPlayer, CCQuestDropItem* pDropItem, MVector& pos)
{
	CCMatchObject* pPlayer = CCMatchServer::GetInstance()->GetObject(uidPlayer);
	if (!pPlayer) return;

	switch (pDropItem->nDropItemType)
	{
	case QDIT_WORLDITEM:
		{
			int nSpawnItemID = pDropItem->nID;
			int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
			for (int i = 0; i < WORLDITEM_EXTRAVALUE_NUM; i++) nWorldItemExtraValues[i] = 0;

			m_pStage->SpawnServerSideWorldItem(pPlayer, nSpawnItemID, pos.x, pos.y, pos.z, 
											   QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues );
		};
		break;
	case QDIT_QUESTITEM:
		{
			int nSpawnItemID		= QUEST_WORLDITEM_ITEMBOX_ID;

			int nQuestItemID		= pDropItem->nID;
			int nRentPeriodHour		= 0;

			int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
			nWorldItemExtraValues[0] = nQuestItemID;
			nWorldItemExtraValues[1] = nRentPeriodHour;

			m_pStage->SpawnServerSideWorldItem(pPlayer, nSpawnItemID, pos.x, pos.y, pos.z, 
											   QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues );
		}
		break;
	case QDIT_ZITEM:
		{
			int nSpawnItemID		= QUEST_WORLDITEM_ITEMBOX_ID;
			int nQuestItemID		= pDropItem->nID;
			int nRentPeriodHour		= pDropItem->nRentPeriodHour;

			int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
			nWorldItemExtraValues[0] = nQuestItemID;
			nWorldItemExtraValues[1] = nRentPeriodHour;

			m_pStage->SpawnServerSideWorldItem(pPlayer, nSpawnItemID, pos.x, pos.y, pos.z, 
											   QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues );
		}
		break;
	};
}

void CCMatchRuleBaseQuest::RefreshPlayerStatus()
{
	for (CCUIDRefCache::iterator i=m_pStage->GetObjBegin(); i!=m_pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		// ��� ��Ȱ
		pObj->SetAlive(true);
	}

	MCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_REFRESH_PLAYER_STATUS, CCUID(0,0));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}

void CCMatchRuleBaseQuest::ClearAllNPC()
{
	m_NPCManager.ClearNPC();
	m_nNPCSpawnCount = 0;

	MCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_NPC_ALL_CLEAR, CCUID(0,0));
	CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
}


bool CCMatchRuleBaseQuest::CheckPlayersAlive()
{
	int nAliveCount = 0;
	CCMatchObject* pObj;
	for (CCUIDRefCache::iterator i=m_pStage->GetObjBegin(); i!=m_pStage->GetObjEnd(); i++) 
	{
		pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		if (pObj->CheckAlive()==true)
		{
			++nAliveCount;
		}
	}

	// ��� �׾����� ����
	if (nAliveCount == 0) return false;

	return true;
}



CCMatchNPCObject* CCMatchRuleBaseQuest::SpawnNPC(MQUEST_NPC nNPC, int nPosIndex, bool bKeyNPC)
{
	CCMatchNPCObject* pNPCObject = m_NPCManager.CreateNPCObject(nNPC, nPosIndex, bKeyNPC);
	if (pNPCObject)
	{
		m_nNPCSpawnCount++;
	}

	return pNPCObject;
}


void CCMatchRuleBaseQuest::OnRequestTestNPCSpawn(int nNPCType, int nNPCCount)
{
	for (int i = 0; i < nNPCCount; i++)
	{
		int nPosIndex = RandomNumber(0, 26);
        SpawnNPC(MQUEST_NPC(nNPCType), nPosIndex);
	}
}


void CCMatchRuleBaseQuest::OnRequestTestClearNPC()
{
	ClearAllNPC();
}


void CCMatchRuleBaseQuest::OnCompleted()
{
	m_bQuestCompleted = true;
	DistributeReward();
	RouteCompleted();
}


void CCMatchRuleBaseQuest::OnFailed()
{
	RouteFailed();
}

void CCMatchRuleBaseQuest::PreProcessLeaveStage( const CCUID& uidLeaverUID )
{
}


void CCMatchRuleBaseQuest::CheckMonsterBible( const CCUID& uidUser, const int nMonsterBibleIndex )
{
	if( 0 > nMonsterBibleIndex )
		return;

	CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject( uidUser );
	if( !IsEnabledObject(pObj) )
		return;

	if( 0 == CCMatchServer::GetInstance()->FindStage(pObj->GetStageUID()) )
		return;

	if( (0 > nMonsterBibleIndex) || (MAX_DB_MONSTERBIBLE_SIZE <= nMonsterBibleIndex) )
		return;

	// ���⼭ ó��.

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if( 0 == pCharInfo )
		return;
	
	if( pCharInfo->m_QMonsterBible.IsKnownMonster(nMonsterBibleIndex) )
		return;

	pCharInfo->m_QMonsterBible.WriteMonsterInfo( nMonsterBibleIndex );

	PostNewMonsterInfo( pObj->GetUID(), nMonsterBibleIndex );


#ifdef _DEBUG
	// ó�� ȹ���� ���� ����.
	cclog( "CCMatchRuleBaseQuest::CheckMonsterBible - New obtain monster info:%d\n", nMonsterBibleIndex );

	// ���� ������ �ִ� ���� ����.
	CCQuestMonsterBible qmb = pCharInfo->m_QMonsterBible;
	for( int i = 0; i < MAX_DB_MONSTERBIBLE_SIZE; ++i )
	{
		if( 0 == qmb[ i ] )
			continue;

		CCQuestNPCInfo* pNPCInfo = CCMatchServer::GetInstance()->GetQuest()->GetNPCIndexInfo( i );
		ASSERT( 0 != pNPCInfo );
		cclog( "CCMatchRuleBaseQuest::CheckMonsterBible - Monster name : %s, Bible index : %d\n", pNPCInfo->szName, i );
	}	
#endif
}


void CCMatchRuleBaseQuest::PostNewMonsterInfo( const CCUID& uidUser, const char nMonIndex )
{
	if( 0 == CCMatchServer::GetInstance()->GetObject(uidUser) )
		return;

	if( 0 > nMonIndex )
		return;

	// ���⼭ �ٷ� ó�� ������ ���� ������ ������.
	MCommand* pMonInfoCmd = CCMatchServer::GetInstance()->CreateCommand( MC_MATCH_NEW_MONSTER_INFO, uidUser );
	if( 0 == pMonInfoCmd )
	{
		cclog( "CCMatchRuleBaseQuest::CheckMonsterBible - ���� ������ ���� ������ �˷��ִ� Ŀ�ǵ� ���� ����.\n" );
		return;
	}
	pMonInfoCmd->AddParameter( new MCmdParamChar(nMonIndex) );

	if( !CCMatchServer::GetInstance()->Post(pMonInfoCmd) )
		cclog( "CCMatchRuleBaseQuest::CheckMonsterBible - ���� ������ ���� ������ �˷��ִ� Ŀ�ǵ� POST����.\n" );
}


//////////////////////////////////////////////////////////////////////////

void CCMatchRuleBaseQuest::ReAssignNPC()
{
	unsigned long int nowTime = CCMatchServer::GetInstance()->GetGlobalClockCount();

	if (nowTime - m_nLastNPCAssignCheckTime > NPC_ASSIGN_DELAY)
	{
		CCMatchStage* pStage = GetStage();
		if (pStage == NULL) return;

		m_nLastNPCAssignCheckTime = nowTime;

		for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
		{
			CCMatchObject* pObj = (CCMatchObject*)(*i).second;
			if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ

			unsigned long int lat = pObj->GetQuestLatency();

			if (lat > PING_LOST_LIMIT)
			{
				m_NPCManager.RemovePlayerControl(pObj->GetUID());
			}
//			cclog("Ping : %s (%d)\n", pObj->GetName(), lat);
		}
	}
}


void CCMatchRuleBaseQuest::SendClientLatencyPing()
{
	unsigned long int nowTime = CCMatchServer::GetInstance()->GetGlobalClockCount();

	if (nowTime - m_nLastPingTime > LATENCY_CHECK_DELAY)
	{
		CCMatchStage* pStage = GetStage();
		if (pStage == NULL) 
			return;

		m_nLastPingTime = nowTime;

		for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
		{
			CCMatchObject* pObj = (CCMatchObject*)(*i).second;
			if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ

			if (pObj->m_bQuestRecvPong)
			{
				pObj->SetPingTime(nowTime);
				pObj->m_bQuestRecvPong = false;
			}
		}

		CCMatchServer::GetInstance()->OnQuestSendPing(pStage->GetUID(), nowTime);
	}
}

void InsertNPCIDonUnique( vector<MQUEST_NPC>& outNPCList, MQUEST_NPC nNPCID )
{
	if( NPC_NONE == nNPCID )
	{
		return;
	}

	if( outNPCList.end() == find(outNPCList.begin(), outNPCList.end(), nNPCID) )
	{
		outNPCList.push_back( nNPCID );
	}		
}

void MakeJacoNPCList( vector<MQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps )
{
	// �ش� �ó������� Key npc�� �켱 ����Ѵ�. �������� ���ȴ�.
	InsertNPCIDonUnique( outNPCList, MQUEST_NPC(ScenarioInfoMaps.nKeyNPCID) );

	const size_t nJacoArrayCount = ScenarioInfoMaps.vecJacoArray.size();

	for( size_t i = 0; i < nJacoArrayCount; ++i )
	{
		InsertNPCIDonUnique( outNPCList, ScenarioInfoMaps.vecJacoArray[i].nNPCID );
	}
}

void MakeSurvivalKeyNPCList( vector<MQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps )
{
	// �ش� �����̹� �ó������� ���Ǻ� Key npc�� ����Ѵ�.
	const size_t nKeyNPCCount = ScenarioInfoMaps.vecKeyNPCArray.size();

	for( size_t i = 0; i < nKeyNPCCount; ++i )
	{
		InsertNPCIDonUnique( outNPCList, MQUEST_NPC(ScenarioInfoMaps.vecKeyNPCArray[i]) );
	}
}


void MakeNomalNPCList( vector<MQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps, CCMatchQuest* pQuest ) 
{
	CCQuestNPCSetInfo*	pNPCSetInfo	= NULL;
	int					nNPCSetID	= 0;
	const size_t		nNPCSetArrayCount = ScenarioInfoMaps.vecNPCSetArray.size();
	size_t				nNPCArrayCount = 0;
	size_t				j;

	for( size_t i = 0; i < nNPCSetArrayCount; ++i )
	{
		nNPCSetID = MQUEST_NPC( ScenarioInfoMaps.vecNPCSetArray[i] );

		pNPCSetInfo = pQuest->GetNPCSetInfo( nNPCSetID );
		if( NULL == pNPCSetInfo )
		{
			_ASSERT( 0 );
			return;
		}

		InsertNPCIDonUnique( outNPCList, pNPCSetInfo->nBaseNPC ); 

		nNPCArrayCount = pNPCSetInfo->vecNPCs.size();

		for( j = 0; j < nNPCArrayCount; ++j )
		{
			InsertNPCIDonUnique( outNPCList, pNPCSetInfo->vecNPCs[j].nNPC );
		}
	}
}


void CopyMTD_NPCINFO( MTD_NPCINFO* pDest, const CCQuestNPCInfo* pSource )
{
	pDest->m_nNPCTID			= pSource->nID;
	pDest->m_nMaxHP				= pSource->nMaxHP;
	pDest->m_nMaxAP				= pSource->nMaxAP;
	pDest->m_nInt				= pSource->nIntelligence;
	pDest->m_nAgility			= pSource->nAgility;
	pDest->m_fAngle				= pSource->fViewAngle;
	pDest->m_fDyingTime			= pSource->fDyingTime;

	pDest->m_fCollisonRadius	= pSource->fCollRadius;
	pDest->m_fCollisonHight		= pSource->fCollHeight;

	pDest->m_nAttackType		= pSource->nNPCAttackTypes;
	pDest->m_fAttackRange		= pSource->fAttackRange;
	pDest->m_nWeaponItemID		= pSource->nWeaponItemID;
	pDest->m_fDefaultSpeed		= pSource->fSpeed;
}