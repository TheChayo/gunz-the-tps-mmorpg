#include "stdafx.h"
#include "CCQuestLevel.h"
#include "CCMatchTransDataType.h"
#include "CCMath.h"
#include "CCQuestFormula.h"
#include "CCQuestNPC.h"
#include "CCQuestConst.h"


CCQuestNPCQueue::CCQuestNPCQueue() : m_nCursor(0), m_bContainKeyNPC(false)
{

}

CCQuestNPCQueue::~CCQuestNPCQueue()
{

}

void CCQuestNPCQueue::Make(int nQLD, CCQuestNPCSetInfo* pNPCSetInfo, CCQUEST_NPC nKeyNPC)
{
	if (pNPCSetInfo == NULL) return;

	m_nCursor = 0;
	int nSize = nQLD;

	m_Queue.reserve(nSize);			// QLD�� ���� NPC ������ �ǹ��Ѵ�.
	m_Queue.resize(nSize, pNPCSetInfo->nBaseNPC);

	int nNPCSetCount = (int)pNPCSetInfo->vecNPCs.size();
	int cursor = 0;
	for (int i = 0; i < nNPCSetCount; i++)
	{
		CCNPCSetNPC npc = pNPCSetInfo->vecNPCs[i];

		//float fSpawnRate = (float)RandomNumber((int)(npc.fMinRate*100.0f), (int)(npc.fMaxRate*100.0f));
		float fSpawnRate = (float)(RandomNumber(npc.nMinRate, npc.nMaxRate) / 100.0f);
		int nSpawnCount = (int)floor(nSize * fSpawnRate);

		// ���� ������ 1���� �ȳ����� NPC�� ������ 1���� ���� �� �ִ� ������ ����Ѵ�.
		if (nSpawnCount <= 0)
		{
			if (RandomNumber(0.0f, 1.0f) < float(nSize / 100.0f))
			{
				nSpawnCount = 1;
			}
		}
		if ((npc.nMaxSpawnCount > 0) && (nSpawnCount > npc.nMaxSpawnCount))
			nSpawnCount = npc.nMaxSpawnCount;

		for (int j = 0; j < nSpawnCount; j++)
		{
			if (cursor < nSize)
			{
				m_Queue[cursor] = npc.nNPC;
				cursor++;
			}
		}
	}

	// shuffle
	for (int i = 0; i < nSize; i++)
	{
		int nTarIndex = RandomNumber(i, nSize-1);
		CCQUEST_NPC temp = m_Queue[nTarIndex];
		m_Queue[nTarIndex] = m_Queue[i];
		m_Queue[i] = temp;
	}

	// Ű NPC�� ������ ���� ó���� �ִ´�.
	if (nKeyNPC != NPC_NONE)
	{
		m_bContainKeyNPC = true;
		m_Queue[0] = nKeyNPC;
	}
	else
		m_bContainKeyNPC = false;
}

bool CCQuestNPCQueue::Pop(CCQUEST_NPC& outNPC)
{
	if (IsEmpty()) return false;

	outNPC = m_Queue[m_nCursor];
	m_nCursor++;

	return true;
}

bool CCQuestNPCQueue::GetFirst(CCQUEST_NPC& outNPC)
{
	if (IsEmpty()) return false;
	outNPC = m_Queue[m_nCursor];

	return true;
}

void CCQuestNPCQueue::Clear()
{
	m_Queue.clear();
	m_nCursor = 0;
}

bool CCQuestNPCQueue::IsEmpty()
{
	if ((m_Queue.empty()) || (m_nCursor >= GetCount())) return true;
	return false;
}

int CCQuestNPCQueue::GetCount()
{
	return (int)m_Queue.size();
}

bool CCQuestNPCQueue::IsKeyNPC(CCQUEST_NPC npc)
{
	if (!IsEmpty() && IsContainKeyNPC()) // Make()���� ť ù��° ĭ�� KeyNPC�� �־����Ƿ� �װͰ� ��
		return npc==m_Queue[0];

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
CCQuestLevel::CCQuestLevel()
{

}

CCQuestLevel::~CCQuestLevel()
{

}

const bool CCQuestLevel::Make_MTDQuestGameInfo(CCTD_QuestGameInfo* pout, CCMATCH_GAMETYPE eGameType)
{
	if( MAX_QUEST_NPC_INFO_COUNT <= static_cast<int>(m_StaticInfo.NPCs.size()) )
	{
		_ASSERT( 0 );
		return false;
	}

	if( MAX_QUEST_MAP_SECTOR_COUNT <= static_cast<int>(m_StaticInfo.SectorList.size()) )
	{
		_ASSERT( 0 );
		return false;
	}

	
	pout->nNPCInfoCount = (int)m_StaticInfo.NPCs.size();
	int idx = 0;
	for (set<CCQUEST_NPC>::iterator itor = m_StaticInfo.NPCs.begin(); itor != m_StaticInfo.NPCs.end(); ++itor)
	{
		pout->nNPCInfo[idx] = (*itor);
		idx++;
	}

	pout->nMapSectorCount = (int)m_StaticInfo.SectorList.size();
	for (int i = 0; i < pout->nMapSectorCount; i++)
	{
		pout->nMapSectorID[i]			= m_StaticInfo.SectorList[i].nSectorID;
		pout->nMapSectorLinkIndex[i]	= m_StaticInfo.SectorList[i].nNextLinkIndex;
	}

	pout->nNPCCount = (unsigned short)m_NPCQueue.GetCount();
	pout->fNPC_TC	= m_StaticInfo.fNPC_TC;
	pout->nQL		= m_StaticInfo.nQL;
	pout->eGameType = eGameType;
	pout->nRepeat	= m_StaticInfo.pScenario->nRepeat;

	return true;
}


void CCQuestLevel::Init(int nScenarioID, int nDice, CCMATCH_GAMETYPE eGameType)
{
	CCQuestScenarioInfo* pScenario = NULL;
	CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();

	if (CCGetGameTypeMgr()->IsQuestOnly(eGameType))
	{
		pScenario = CCMatchServer::GetInstance()->GetQuest()->GetScenarioInfo(nScenarioID);
	}
	else if (CCGetGameTypeMgr()->IsSurvivalOnly(eGameType))
	{
		pScenario = CCMatchServer::GetInstance()->GetQuest()->GetSurvivalScenarioInfo(nScenarioID);
	}
	else
	{
		ASSERT(0);
		return;
	}

	m_StaticInfo.pScenario = pScenario;

	m_StaticInfo.nDice = nDice - 1;		// 0���� 5����

	if (m_StaticInfo.pScenario)
	{
		InitSectors(eGameType);
		InitNPCs();
	}

	m_DynamicInfo.nCurrSectorIndex = 0;

	InitStaticInfo(eGameType);	// ���̵� ���, NPC ���̵� ���� ��� ���� ����
	
#ifdef _DEBUG_QUEST
	if( nScenarioID == 100)
		m_StaticInfo.nQLD = 1;
#endif
	
	InitCurrSector(eGameType);
}

bool CCQuestLevel::InitSectors(CCMATCH_GAMETYPE eGameType)
{
	if (m_StaticInfo.pScenario == NULL) 
	{
		_ASSERT(0);
		return false;
	}

	CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();

	int nSectorCount = m_StaticInfo.pScenario->GetSectorCount(m_StaticInfo.nDice);
	int nKeySector = m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].nKeySectorID;


	m_StaticInfo.SectorList.reserve(nSectorCount);
	m_StaticInfo.SectorList.resize(nSectorCount);

	int nSectorIndex = nSectorCount-1;

	int nSectorID = nKeySector;
	int nLinkIndex = 0;
	for (int i = 0; i < nSectorCount; i++)
	{
		CCQuestMapSectorInfo* pSector = NULL;

		if (CCGetGameTypeMgr()->IsQuestOnly(eGameType))
			pSector = pQuest->GetSectorInfo(nSectorID);
		else if (CCGetGameTypeMgr()->IsSurvivalOnly(eGameType))
			pSector = pQuest->GetSurvivalSectorInfo(nSectorID);
		else
			ASSERT(0);

		if (pSector == NULL) 
		{
			_ASSERT(0);
			return false;
		}

		// ���� ���� �Է�
		CCQuestLevelSectorNode node;
		node.nSectorID = nSectorID;
		node.nNextLinkIndex = nLinkIndex;
		m_StaticInfo.SectorList[nSectorIndex] = node;


		if (i != (nSectorCount-1)) 
		{
			// ���� ���ͳ�带 �������� ���� ���Ϳ� ��ũ�� �����Ѵ�.
			int nBacklinkCount = (int)pSector->VecBacklinks.size();
			if (nBacklinkCount > 0)
			{
				bool bSameNode = false;
				int nLoopCount = 0;
				do
				{
					nLoopCount++;

					int backlink_index = RandomNumber(0, (nBacklinkCount-1));
					nSectorID = pSector->VecBacklinks[backlink_index].nSectorID;
					nLinkIndex = pSector->VecBacklinks[backlink_index].nLinkIndex;

					// ���� ��尡 �ι� �ݺ��ؼ� �ɸ��� �ʵ��� �Ѵ�.
					if ((nBacklinkCount > 1) && ((nSectorIndex+1) < nSectorCount))
					{
						if (nSectorID == m_StaticInfo.SectorList[nSectorIndex+1].nSectorID)
						{
							bSameNode = true;
						}
					}
				}
				while ((bSameNode) && (nLoopCount < 2));	// ���� ���� ���� ��尡 �ɸ��� �ݺ�

			}
			else
			{
				// ����ũ�� �ϳ��� �־�� �Ѵ�.
				_ASSERT(0);
				return false;
			}

			nSectorIndex--;
		}
	}

	return true;
}

bool CCQuestLevel::InitNPCs()
{
	if (m_StaticInfo.pScenario == NULL) 
	{
		_ASSERT(0);
		return false;
	}

	int nDice = m_StaticInfo.nDice;
	CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();
	int nArraySize = (int)m_StaticInfo.pScenario->Maps[nDice].vecNPCSetArray.size();
	for (int i = 0; i < nArraySize; i++)
	{
		int nNPCSetID = m_StaticInfo.pScenario->Maps[nDice].vecNPCSetArray[i];
		CCQuestNPCSetInfo* pNPCSetInfo = pQuest->GetNPCSetInfo(nNPCSetID);
		if (pNPCSetInfo == NULL) 
		{
			_ASSERT(0);
			return false;
		}
		
		// base npc�� ���� �ִ´�.
		m_StaticInfo.NPCs.insert(set<CCQUEST_NPC>::value_type(pNPCSetInfo->nBaseNPC));

		int nNPCSize = (int)pNPCSetInfo->vecNPCs.size();
		for (int j = 0; j < nNPCSize; j++)
		{
			CCQUEST_NPC npc = (CCQUEST_NPC)pNPCSetInfo->vecNPCs[j].nNPC;
			m_StaticInfo.NPCs.insert(set<CCQUEST_NPC>::value_type(npc));
		}
	}

	
	return true;
}


int CCQuestLevel::GetMapSectorCount()
{
	return (int)m_StaticInfo.SectorList.size();
}


bool CCQuestLevel::MoveToNextSector(CCMATCH_GAMETYPE eGameType)
{
	if (CCGetGameTypeMgr()->IsQuestOnly(eGameType))
	{
		if ((m_DynamicInfo.nCurrSectorIndex+1) >= GetMapSectorCount()) return false;
	
		CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();

		m_DynamicInfo.nCurrSectorIndex++;
	}
	else if (CCGetGameTypeMgr()->IsSurvivalOnly(eGameType))
	{
		m_DynamicInfo.nCurrSectorIndex++;

		if (m_DynamicInfo.nCurrSectorIndex >= GetMapSectorCount())
		{
			m_DynamicInfo.nCurrSectorIndex = 0;
			m_DynamicInfo.nRepeated++;
		}
	}
	else
	{
		_ASSERT(0);
		return false;
	}

	InitCurrSector(eGameType);
	return true;
}

void CCQuestLevel::InitCurrSector(CCMATCH_GAMETYPE eGameType)
{
	// npc queue ����
	CCMatchQuest* pQuest = CCMatchServer::GetInstance()->GetQuest();
	
	int nNPCSetID = 0;

	if( m_DynamicInfo.nCurrSectorIndex < (int)m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].vecNPCSetArray.size() )
	{
		nNPCSetID = m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].vecNPCSetArray[m_DynamicInfo.nCurrSectorIndex];
	}
	else
	{
		_ASSERT( 0 && "NPC set�� ũ�⿡ ������ ����. ���ҽ� �˻簡 �ʿ���." );
		cclog( "NPC set�� ũ�⿡ ������ ����. ���ҽ� �˻簡 �ʿ���.\n" );
		return;
	}

	CCQuestNPCSetInfo* pNPCSetInfo = pQuest->GetNPCSetInfo(nNPCSetID);

	m_NPCQueue.Clear();

	m_DynamicInfo.bCurrBossSector = false;

	if (CCGetGameTypeMgr()->IsQuestOnly(eGameType))
	{
		// ���� Ű NPC�� �ְ�, ������ �����̸� Ű NPC ����
		if ((m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].nKeyNPCID != 0) &&
			(m_DynamicInfo.nCurrSectorIndex == GetMapSectorCount() - 1))
		{
			m_NPCQueue.Make(m_StaticInfo.nQLD, pNPCSetInfo, CCQUEST_NPC(m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].nKeyNPCID));
			if (m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].bKeyNPCIsBoss)
			{
				m_DynamicInfo.bCurrBossSector = true;
			}
		}
		else
		{
			m_NPCQueue.Make(m_StaticInfo.nQLD, pNPCSetInfo);
		}
	}
	else if (CCGetGameTypeMgr()->IsSurvivalOnly(eGameType))
	{
		const int nCurrSector = m_DynamicInfo.nCurrSectorIndex;
		const std::vector<int>& vecKeyNpc = m_StaticInfo.pScenario->Maps[m_StaticInfo.nDice].vecKeyNPCArray;

		// �� ������ ŰNPC�� ����
		int keyNpcID = NPC_NONE;
		if (nCurrSector < (int)vecKeyNpc.size())
		{
			keyNpcID = vecKeyNpc[nCurrSector];
		}

		if (keyNpcID != NPC_NONE)
		{
			m_NPCQueue.Make(m_StaticInfo.nQLD, pNPCSetInfo, CCQUEST_NPC(keyNpcID));
			m_DynamicInfo.bCurrBossSector = false;	// �����̹��� ������ ����(���� ������ �������� ����Ʈ������ ����ó�� �ٷ��� ����)
		}
		else
		{
			m_NPCQueue.Make(m_StaticInfo.nQLD, pNPCSetInfo);
		}
	}
	else
		_ASSERT(0);

	// spawn index ����
	memset(m_SpawnInfos, 0, sizeof(m_SpawnInfos));

	int nSectorID = m_StaticInfo.SectorList[m_DynamicInfo.nCurrSectorIndex].nSectorID;

	if (CCGetGameTypeMgr()->IsQuestOnly(eGameType))
		m_DynamicInfo.pCurrSector = pQuest->GetSectorInfo(nSectorID);
	else if (CCGetGameTypeMgr()->IsSurvivalOnly(eGameType))
		m_DynamicInfo.pCurrSector = pQuest->GetSurvivalSectorInfo(nSectorID);
	else
		ASSERT(0);
}

void CCQuestLevel::InitStaticInfo(CCMATCH_GAMETYPE eGameType)
{
	if (m_StaticInfo.pScenario)
	{
		m_StaticInfo.nQL = m_StaticInfo.pScenario->nQL;

		if (CCGetGameTypeMgr()->IsQuestOnly(eGameType))
		{
			m_StaticInfo.nQLD = (int)(CCQuestFormula::CalcQLD(m_StaticInfo.nQL) * m_StaticInfo.pScenario->fDC);
			m_StaticInfo.nLMT = (int)(CCQuestFormula::CalcLMT(m_StaticInfo.nQL) * m_StaticInfo.pScenario->fDC);
		}
		else if (CCGetGameTypeMgr()->IsSurvivalOnly(eGameType))
		{
			// �����̹� ����� ��� xml�� ���ǵ� ���� ���
			m_StaticInfo.nQLD = m_StaticInfo.pScenario->nMaxSpawn;				// �ִ� ���� ��
			m_StaticInfo.nLMT = m_StaticInfo.pScenario->nMaxSpawnSameTime;		// �ִ� ���� ���� ��
		}
		else 
			ASSERT(0);

		m_StaticInfo.fNPC_TC = CCQuestFormula::CalcTC(m_StaticInfo.nQL);

	}
}

int CCQuestLevel::GetCurrSectorIndex()
{
	return m_DynamicInfo.nCurrSectorIndex;
}

int CCQuestLevel::GetSpawnPositionCount(CCQuestNPCSpawnType nSpawnType)
{
	if (m_DynamicInfo.pCurrSector)
	{
		return m_DynamicInfo.pCurrSector->nSpawnPointCount[nSpawnType];
	}

	return 0;
}

int CCQuestLevel::GetRecommendedSpawnPosition(CCQuestNPCSpawnType nSpawnType, unsigned long int nTickTime)
{
	if (m_DynamicInfo.pCurrSector)
	{
		// �����̹���带 ���� ���� ó��- �������������� ���� ���� �׳� �и������� ������Ŵ //////
		if (nSpawnType == MNST_BOSS)
		{
			if (m_DynamicInfo.pCurrSector->nSpawnPointCount[MNST_BOSS] == 0)
				nSpawnType = MNST_MELEE;
		}
		//////////////////////////////////////////////////////////////////////////////////////////

		int nRecommendIndex = m_SpawnInfos[nSpawnType].nIndex;

		// ������������ ��õ�޾����� �����ð��� �����Ѵ�.
		if (nRecommendIndex < MAX_SPAWN_COUNT)
		{
			m_SpawnInfos[nSpawnType].nRecentSpawnTime[nRecommendIndex] = nTickTime;
		}

		m_SpawnInfos[nSpawnType].nIndex++;

		int nSpawnMax = m_DynamicInfo.pCurrSector->nSpawnPointCount[nSpawnType];
		if (m_SpawnInfos[nSpawnType].nIndex >= nSpawnMax) m_SpawnInfos[nSpawnType].nIndex = 0;

		return nRecommendIndex;
	}

	return 0;
}

bool CCQuestLevel::IsEnableSpawnNow(CCQuestNPCSpawnType nSpawnType, unsigned long int nNowTime)
{
	if (m_DynamicInfo.pCurrSector)
	{
		int idx = m_SpawnInfos[nSpawnType].nIndex;
		if ((nNowTime - m_SpawnInfos[nSpawnType].nRecentSpawnTime[idx]) > SAME_SPAWN_DELAY_TIME) return true;
	}

	return false;
}

void CCQuestLevel::OnItemCreated(unsigned long int nItemID, int nRentPeriodHour)
{
	CCQuestLevelItem* pNewItem = new CCQuestLevelItem();
	pNewItem->nItemID = nItemID;
	pNewItem->nRentPeriodHour = nRentPeriodHour;
	pNewItem->bObtained = false;

	m_DynamicInfo.ItemMap.insert(make_pair(nItemID, pNewItem));
}

bool CCQuestLevel::OnItemObtained( CCMatchObject* pPlayer, unsigned long int nItemID )
{
	if( 0 == pPlayer ) return false;

	for (CCQuestLevelItemMap::iterator itor = m_DynamicInfo.ItemMap.lower_bound(nItemID);
		itor != m_DynamicInfo.ItemMap.upper_bound(nItemID); ++itor)
	{
		CCQuestLevelItem* pQuestItem = (*itor).second;
		if (!pQuestItem->bObtained)
		{
			pQuestItem->bObtained = true;
			// pPlayer->GetCharInfo()->m_QMonsterBible.SetPage( 
			return true;
		}
	}
	
	// ���� false�̸� �÷��̾ ġ���� �ϴ� ����..-_-;
	return false;
}