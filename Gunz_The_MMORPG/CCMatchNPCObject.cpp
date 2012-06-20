#include "stdafx.h"
#include "CCMatchNPCObject.h"
#include "CCMatchStage.h"
#include "CCMatchObject.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCMath.h"
#include "CCMatchRuleQuest.h"
#include "CCQuestPlayer.h"

CCMatchNPCObject::CCMatchNPCObject(CCUID& uid, CCQUEST_NPC nType, unsigned long int nFlags)
					: m_UID(uid), m_nType(nType), m_uidController(CCUID(0,0)), m_Pos(0.0f,0.0f,0.0f), m_nFlags(nFlags)
{

}

void CCMatchNPCObject::AssignControl(CCUID& uidPlayer)
{
	m_uidController = uidPlayer;
}

void CCMatchNPCObject::ReleaseControl()
{
	m_uidController = CCUID(0,0);
}

void CCMatchNPCObject::SetDropItem(CCQuestDropItem* pDropItem)
{
	m_DropItem.Assign(pDropItem);

	// monster bible.
	//m_DropItem.nMonsetBibleIndex	= pDropItem->nMonsetBibleIndex;
}

////////////////////////////////////////////////////////////////////////////
CCMatchNPCManager::CCMatchNPCManager() : m_pStage(NULL), m_nLastSpawnTime(0), m_pPlayerManager(0), m_nBossCount(0), m_bBossDie(false), m_bKeyNPCDie(false)
{

}

CCMatchNPCManager::~CCMatchNPCManager()
{

}

bool CCMatchNPCManager::AssignControl(CCUID& uidNPC, CCUID& uidPlayer)
{
	CCMatchObject* pObject = CCMatchServer::GetInstance()->GetObject(uidPlayer);
	if (!pObject) return false;
	CCMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
	if (!pNPCObject) return false;

	// ControllerInfo ����
	SetNPCObjectToControllerInfo(uidPlayer, pNPCObject);

	// route cmd
	CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_ENTRUST_NPC_CONTROL, uidPlayer);
	pCmd->AddParameter(new CCCmdParamCCUID(uidPlayer));
	pCmd->AddParameter(new CCCmdParamCCUID(uidNPC));
	CCMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);

	#ifdef _DEBUG
	char text[256];
	sprintf(text, "AssignControl(%u:%u) - (%u:%u)\n", uidNPC.High, uidNPC.Low, uidPlayer.High, uidPlayer.Low);
	OutputDebugString(text);
	#endif


	return true;
}

bool CCMatchNPCManager::Spawn(CCUID& uidNPC, CCUID& uidController, unsigned char nSpawnPositionIndex)
{
	CCMatchObject* pObject = CCMatchServer::GetInstance()->GetObject(uidController);
	if ((pObject) && (m_pStage))
	{
		CCMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
		if (pNPCObject)
		{
			SetNPCObjectToControllerInfo(uidController, pNPCObject);

			// route cmd
			CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_QUEST_NPC_SPAWN, uidController);
			pCmd->AddParameter(new CCCmdParamCCUID(uidController));
			pCmd->AddParameter(new CCCmdParamCCUID(uidNPC));
			pCmd->AddParameter(new CCCmdParamUChar((unsigned char)pNPCObject->GetType()));
			pCmd->AddParameter(new CCCmdParamUChar((unsigned char)nSpawnPositionIndex));
			CCMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);


//			#ifdef _DEBUG
//			char text[256];
//			sprintf(text, "Spawn(%u:%u) - (%u:%u)\n", uidNPC.High, uidNPC.Low, uidController.High, uidController.Low);
//			OutputDebugString(text);
//			#endif

			return true;
		}
	}
	return false;
}

CCMatchNPCObject* CCMatchNPCManager::CreateNPCObject(CCQUEST_NPC nType, unsigned char nSpawnPositionIndex, bool bKeyNPC)
{
	CCQuestNPCInfo* pNPCInfo = CCMatchServer::GetInstance()->GetQuest()->GetNPCInfo(nType);
	if (pNPCInfo == NULL) return NULL;

	CCUID uidNPC = NewUID();
	unsigned long int nNPCFlags=0;

	CCMatchNPCObject* pNewNPC = new CCMatchNPCObject(uidNPC, nType, nNPCFlags);

	m_NPCObjectMap.insert(CCMatchNPCObjectMap::value_type(uidNPC, pNewNPC));

	// keyNPC��� ����صд�
	if (bKeyNPC)
		m_uidKeyNPC = uidNPC;

	// spawn�� NPC Count����
	CCQuestNPCSpawnType nSpawnType = MNST_MELEE;
	
	if (pNPCInfo)
	{
		nSpawnType = pNPCInfo->GetSpawnType();

		// ���� �����̸� BossCount ����
		if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
		{
			m_nBossCount++;
		}
	}
	m_nNPCCount[nSpawnType]++;
	


	// ��Ʈ�ѷ� �Ҵ�
	CCUID uidController = CCUID(0,0);
	if (!FindSuitableController(uidController, NULL))
	{
		// ������ ����� ������ ����
		CCQuestDropItem TempItem;
		DestroyNPCObject(uidNPC, TempItem);
		return NULL;
	}

	// ����
	if (!Spawn(uidNPC, uidController, nSpawnPositionIndex))
	{
		CCQuestDropItem TempItem;
		DestroyNPCObject(uidNPC, TempItem);
		return NULL;
	}

	return pNewNPC;
}

bool CCMatchNPCManager::DestroyNPCObject(CCUID& uidNPC, CCQuestDropItem& outItem)
{
	CCMatchNPCObjectMap::iterator itor = m_NPCObjectMap.find(uidNPC);
	if (itor != m_NPCObjectMap.end())
	{
		CCMatchNPCObject* pNPCObject = (*itor).second;

		outItem.Assign(pNPCObject->GetDropItem());
#ifdef _MONSTER_BIBLE
		// outItem.nMonsetBibleIndex	= pNPCObject->GetDropItem()->nMonsetBibleIndex;
#ifdef _DEBUG
		// cclog( "CCMatchNPCManager::DestroyNPCObject - Destroy npc's drop item monster bible index:%d\n", outItem.nMonsetBibleIndex );
#endif
#endif

		// Controller�� �־����� Controller������ �����ش�.
		CCUID uidController = pNPCObject->GetController();
		if (uidController != CCUID(0,0))
		{
			DelNPCObjectToControllerInfo(uidController, pNPCObject);
		}

		// keyNPC���ٸ� ���� ǥ��
		if (uidNPC == m_uidKeyNPC)
 			m_bKeyNPCDie = true;

		// spawn�� NPC Count����
		CCQuestNPCSpawnType nSpawnType = MNST_MELEE;
		CCQuestNPCInfo* pNPCInfo = CCMatchServer::GetInstance()->GetQuest()->GetNPCInfo(pNPCObject->GetType());
		if (pNPCInfo)
		{
			nSpawnType = pNPCInfo->GetSpawnType();

			// ���� �����̸� BossCount ����
			if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
			{
				m_nBossCount--;

				if ( m_nBossCount <= 0)
					m_bBossDie = true;
			}
		}
		m_nNPCCount[nSpawnType]--;



		delete pNPCObject;
		m_NPCObjectMap.erase(itor);
		return true;
	}

	return false;
}

CCUID CCMatchNPCManager::NewUID()
{
	// CCMatchObject uid�� ���� �׷� ���
	return CCMatchServer::GetInstance()->UseUID();
}

void CCMatchNPCManager::Create(CCMatchStage* pStage, CCQuestPlayerManager* pPlayerManager)
{
	m_pStage = pStage;
	m_pPlayerManager = pPlayerManager;
	m_bBossDie = false;
	m_nBossCount = 0;
	m_bKeyNPCDie = false;

	memset(m_nNPCCount, 0, sizeof(m_nNPCCount));
}

void CCMatchNPCManager::Destroy()
{
	Clear();
}

CCMatchNPCObject* CCMatchNPCManager::GetNPCObject(CCUID& uidNPC)
{
	CCMatchNPCObjectMap::iterator itor = m_NPCObjectMap.find(uidNPC);
	if (itor != m_NPCObjectMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void CCMatchNPCManager::Clear()
{
	ClearNPC();
}

void CCMatchNPCManager::OnDelPlayer(const CCUID& uidPlayer)
{
	// Create ȣ�� ����.. ����� ���� �ִ�. - By ȫ���� 2009.08.17
	if( m_pPlayerManager == NULL ) return;

	CCQuestPlayerInfo* pDelPlayerInfo = m_pPlayerManager->GetPlayerInfo(uidPlayer);

	if (pDelPlayerInfo)
	{
		pDelPlayerInfo->bEnableNPCControl = false;

//		for (CCMatchNPCObjectMap::iterator itorNPC = pDelPlayerInfo->NPCObjects.begin(); 
//			itorNPC != pDelPlayerInfo->NPCObjects.end(); ++itorNPC)

		// ���� ���� ���̻ڴ�. ���߿� �̻ڰ� ���ľ��ҵ� - bird
		while (!pDelPlayerInfo->NPCObjects.empty())
		{
			CCMatchNPCObjectMap::iterator itorNPC = pDelPlayerInfo->NPCObjects.begin();

			CCMatchNPCObject* pNPCObject = (*itorNPC).second;
			CCUID uidNPC = pNPCObject->GetUID();		// ���⼭ ������ - bird

			CCUID uidController = CCUID(0,0);
			if (FindSuitableController(uidController, pDelPlayerInfo))
			{
				// ���� �÷��̾�� �Ҵ�� NPC�� �ٸ� �÷��̾�� �Ű��ش�.
				AssignControl(uidNPC, uidController);
			}
			else
			{
				// ������ ����� ������ NPC ����
				CCQuestDropItem TempItem;
				DestroyNPCObject(uidNPC, TempItem);	// ���⼭ NPCObjects.erase ��
			}
		}
	}
}


void CCMatchNPCManager::ClearNPC()
{
	for (CCMatchNPCObjectMap::iterator itor = m_NPCObjectMap.begin(); itor != m_NPCObjectMap.end(); ++itor)
	{
		CCMatchNPCObject* pNPCObject = (*itor).second;

		// Controller�� �־����� Controller������ �����ش�.
		CCUID uidController = pNPCObject->GetController();
		if (uidController != CCUID(0,0))
		{
			DelNPCObjectToControllerInfo(uidController, pNPCObject);
		}

		delete pNPCObject;
	}

	ClearKeyNPCState();

	m_NPCObjectMap.clear();
	memset(m_nNPCCount, 0, sizeof(m_nNPCCount));
}

void CCMatchNPCManager::ClearKeyNPCState()
{
	m_uidKeyNPC.SetZero();
	m_bKeyNPCDie = false;
}


bool CCMatchNPCManager::FindSuitableController(CCUID& out, CCQuestPlayerInfo* pSender)
{
	int nScore = 999999;
	CCUID uidChar = CCUID(0,0);
	bool bExist = false;

	// Create ȣ�� ����.. ����� ���� �ִ�. - By ȫ���� 2009.08.17
	// �� ����� ����� �����غ���! - �ϴ��� �̷��� ��ġ��
	if( m_pPlayerManager == NULL ) return false;

	for (CCQuestPlayerManager::iterator itor = m_pPlayerManager->begin(); itor != m_pPlayerManager->end(); ++itor)
	{
		CCQuestPlayerInfo* pControllerInfo = (*itor).second;

		if ((pSender != NULL) && (pControllerInfo == pSender)) continue;
		if (pControllerInfo->bEnableNPCControl == false) continue;
		if (pControllerInfo->bMovedtoNewSector == true) continue;

		int nControllerScore = pControllerInfo->GetNPCControlScore();
		if (nControllerScore < nScore)
		{
			bExist = true;
			nScore = nControllerScore;
			uidChar = (*itor).first;
		}
	}

	if (bExist)
	{
		out = uidChar;
		return true;
	}
	
	return false;
}



void CCMatchNPCManager::SetNPCObjectToControllerInfo(CCUID& uidChar, CCMatchNPCObject* pNPCObject)
{
	CCUID uidLaster = pNPCObject->GetController();
	if (uidLaster != CCUID(0,0))
	{
		DelNPCObjectToControllerInfo(uidLaster, pNPCObject);
	}

	// Create ȣ�� ����.. ����� ���� �ִ�. - By ȫ���� 2009.08.17
	// �� ����� ����� �����غ���! - �ϴ��� �̷��� ��ġ��
	if( m_pPlayerManager == NULL ) return;

	CCQuestPlayerInfo* pControllerInfo = m_pPlayerManager->GetPlayerInfo(uidChar);
	if (pControllerInfo)
	{
		pNPCObject->AssignControl(uidChar);
		pControllerInfo->NPCObjects.insert(CCMatchNPCObjectMap::value_type(pNPCObject->GetUID(), pNPCObject));
	}
}

void CCMatchNPCManager::DelNPCObjectToControllerInfo(CCUID& uidChar, CCMatchNPCObject* pNPCObject)
{
	pNPCObject->ReleaseControl();

	// Create ȣ�� ����.. ����� ���� �ִ�. - By ȫ���� 2009.08.17
	// �� ����� ����� �����غ���! - �ϴ��� �̷��� ��ġ��
	if( m_pPlayerManager == NULL ) return;

	CCQuestPlayerInfo* pControllerInfo = m_pPlayerManager->GetPlayerInfo(uidChar);
	if (pControllerInfo)
	{
		CCMatchNPCObjectMap::iterator itor = pControllerInfo->NPCObjects.find(pNPCObject->GetUID());
		if (itor != pControllerInfo->NPCObjects.end())
		{
			pControllerInfo->NPCObjects.erase(itor);
		}
	}
}

bool CCMatchNPCManager::IsControllersNPC(CCUID& uidChar, CCUID& uidNPC)
{
	CCMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
	if (pNPCObject)
	{
		if (pNPCObject->GetController() == uidChar) return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// �� �÷��̾��� ��Ʈ���� �ٸ�������� �ű��.

void CCMatchNPCManager::RemovePlayerControl(const CCUID& uidPlayer)
{
	// Create ȣ�� ����.. ����� ���� �ִ�. - By ȫ���� 2009.08.17
	// �� ����� ����� �����غ���! - �ϴ��� �̷��� ��ġ��
	if( m_pPlayerManager == NULL ) return;

	CCQuestPlayerInfo* pDelPlayerInfo = m_pPlayerManager->GetPlayerInfo(uidPlayer);

	if (pDelPlayerInfo)
	{
		pDelPlayerInfo->bEnableNPCControl = false;

		while (!pDelPlayerInfo->NPCObjects.empty())
		{
			CCMatchNPCObjectMap::iterator itorNPC = pDelPlayerInfo->NPCObjects.begin();

			CCMatchNPCObject* pNPCObject = (*itorNPC).second;
			CCUID uidNPC = pNPCObject->GetUID();		// ���⼭ ������ - bird

			CCUID uidController = CCUID(0,0);
			if (FindSuitableController(uidController, pDelPlayerInfo))
			{
				// ���� �÷��̾�� �Ҵ�� NPC�� �ٸ� �÷��̾�� �Ű��ش�.
				AssignControl(uidNPC, uidController);
			}
			else
			{
				// ������ ����� ������ ���Ҵ� ����
				break;
			}
		}

		pDelPlayerInfo->bEnableNPCControl = true;
	}
}