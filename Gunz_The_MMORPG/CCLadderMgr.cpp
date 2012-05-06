#include "stdafx.h"
#include "CCMatchServer.h"

#include "CCLadderMgr.h"
#include "CCLadderPicker.h"
#include "CCMatchGlobal.h"
#include "CCThread.h"
#include "CCSharedCommandTable.h"
#include "CCMatchUtil.h"


MLadderGroupMap* MLadderMgr::GetWaitGroupContainer(CCLADDERTYPE nLadderType)
{
	if ((nLadderType < 0) || (nLadderType >= CCLADDERTYPE_MAX))
	{
		_ASSERT("UNKNOWN LADDERMAP");
		return NULL;
	}

	return &m_WaitingMaps[(int)nLadderType];
}

MLadderGroup* MLadderMgr::CreateLadderGroup()
{
	return new MLadderGroup(CCMatchServer::GetInstance()->GetTickTime());
}

MLadderGroup* MLadderMgr::FindLadderGroup(int nGroupID)
{
	MLadderGroup* pGroup = NULL;

	for (int i = 0; i < CCLADDERTYPE_MAX; i++)
	{
		if (pGroup=m_WaitingMaps[i].Find(nGroupID))
			return pGroup;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
bool MLadderMgr::Init()
{
	m_Stat.Init();

	return true;
}

void MLadderMgr::AddGroup(CCLADDERTYPE nLadderType, MLadderGroup* pGroup)
{
	pGroup->SetLadderType(nLadderType);

	MLadderGroupMap* pGroupMap = GetWaitGroupContainer(nLadderType);
	if (pGroupMap == NULL) return;

	pGroupMap->Add(pGroup);
	m_GroupList.push_back(pGroup);

	// Ladder ��� ã���� �˸�(for UI)
	for (list<CCUID>::iterator i=pGroup->GetPlayerListBegin(); i!= pGroup->GetPlayerListEnd(); i++)
	{
		CCUID uidPlayer = (*i);
		CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_LADDER_SEARCH_RIVAL, uidPlayer);
		
		CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject(uidPlayer);
		if (!IsEnabledObject(pObj))
		{
			delete pCmd;
			continue;
		}

		CCMatchServer::GetInstance()->RouteToListener(pObj, pCmd);
	}	
}

bool MLadderMgr::Challenge(MLadderGroup* pGroup)
{
	int nPlayerCount = (int)pGroup->GetPlayerCount();

	if (nPlayerCount > 0)
	{
		for (int i = 0; i < CCLADDERTYPE_MAX; i++)
		{
			if (nPlayerCount == GetNeedMemberCount(CCLADDERTYPE(i)))
			{
				AddGroup(CCLADDERTYPE(i), pGroup);
			}
		}

		//AddGroup(CCLADDERTYPE(nPlayerCount-1), pGroup);
		return true;
	}

	_ASSERT("UNKNOWN LADDERTYPE");

	return false;
}

void MLadderMgr::RemoveFromGroupList(MLadderGroup* pGroup)
{
	if (pGroup)
	{
		m_GroupList.remove(pGroup);
	}
}

void MLadderMgr::CancelChallenge(int nGroupID, const char* pszCancelName)
{
	MLadderGroup* pGroup = FindLadderGroup(nGroupID);
	if (pGroup == NULL) return;
	MLadderGroupMap* pGroupMap = GetWaitGroupContainer((CCLADDERTYPE)pGroup->GetLadderType());
	if (pGroupMap == NULL) return;

	for (list<CCUID>::iterator i=pGroup->GetPlayerListBegin(); i!= pGroup->GetPlayerListEnd(); i++)
	{
		CCUID uidMember = (*i);

		CCMatchObject* pMemberObject = CCMatchServer::GetInstance()->GetObject(uidMember);
		if (!IsEnabledObject(pMemberObject)) continue;
		pMemberObject->SetLadderChallenging(false);
		pMemberObject->SetLadderGroupID(0);

		CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_LADDER_CANCEL_CHALLENGE, uidMember);
		pCmd->AddParameter(new CCCmdParamStr(pszCancelName));

		CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject(uidMember);
		if (!IsEnabledObject(pObj))
		{
			delete pCmd;
			continue;
		}

		CCMatchServer::GetInstance()->RouteToListener(pObj, pCmd);
	}
	pGroupMap->Remove(pGroup->GetID());
	RemoveFromGroupList(pGroup);
	delete pGroup;
}

int MLadderMgr::MakeMatch(CCLADDERTYPE nLadderType)
{
	MLadderGroupMap* pWaitGroupMap = GetWaitGroupContainer(nLadderType);
	if (pWaitGroupMap == NULL) return 0;

	MTime time;
	MLadderPicker	ladderPicker;

	for (MLadderGroupMap::iterator i=pWaitGroupMap->begin();
		 i!=pWaitGroupMap->end(); i++) 
	{
		MLadderGroup* pGroup = (*i).second;
		pGroup->UpdateTick();

/*
#ifdef _DEBUG
		const unsigned int MIN_ADDTICKET_TICK = 3000;
		if (CCGetTimeDistance(CCMatchServer::GetInstance()->GetTickTime(), pGroup->GetRegTime()) < MIN_ADDTICKET_TICK) continue;
#endif
*/

		//ladderPicker.AddTicket( pGroup, time.MakeNumber(0,pGroup->GetScore()) );
		
		int nClanPoint = DEFAULT_CLAN_POINT;
		CCMatchClan* pClan = CCMatchServer::GetInstance()->GetClanMap()->GetClan(pGroup->GetCLID());
		if (pClan)
		{
			nClanPoint = pClan->GetClanInfoEx()->nPoint;
		}

		const int MAX_RANDOM_ARG = 100;

		ladderPicker.AddTicket(pGroup, nClanPoint, pGroup->GetTickCount(), time.MakeNumber(0, MAX_RANDOM_ARG));
	}

	ladderPicker.Shuffle();

	int nLaunchCount = 0;
	while(true) {
		int nGroupA = 0;
		int nGroupB = 0;
		if (ladderPicker.PickMatch(&nGroupA, &nGroupB) == false)
			break;
		LaunchLadder(nLadderType, nGroupA, nGroupB);
		nLaunchCount++;
	}
	return nLaunchCount;
}

void MLadderMgr::CleaningGarbages()
{
	for (int i = 0; i < CCLADDERTYPE_MAX; i++)
	{
		CCLADDERTYPE nLadderType = CCLADDERTYPE(i);

		MLadderGroupMap* pWaitGroupMap = GetWaitGroupContainer(nLadderType);
		if (pWaitGroupMap == NULL) continue;

		list<int>		CancelGroupIDList;

		for (MLadderGroupMap::iterator itorGroup = pWaitGroupMap->begin(); itorGroup!=pWaitGroupMap->end(); itorGroup++) 
		{
			MLadderGroup* pGroup = (*itorGroup).second;

			// �Ѹ��̶� ������ �������� Cancel�Ѵ�.
			bool bExistCannotPlayer = false;
			for (list<CCUID>::iterator itorPlayerUID=pGroup->GetPlayerListBegin(); itorPlayerUID!= pGroup->GetPlayerListEnd(); 
				itorPlayerUID++)
			{
				CCUID uidMember = (*itorPlayerUID);

				CCMatchObject* pMemberObject = CCMatchServer::GetInstance()->GetObject(uidMember);
				if (!IsEnabledObject(pMemberObject))
				{
					bExistCannotPlayer = true;
					break;
				}
			}

			if (bExistCannotPlayer)
			{
				CancelGroupIDList.push_back(pGroup->GetID());
			}
		}

		// ����� ������� �÷��̾��� ������ �������� Cancel�ؾ� �Ǵ� �׷��� Cancel���ش�.
		for (list<int>::iterator itorGroupID = CancelGroupIDList.begin(); itorGroupID != CancelGroupIDList.end(); 
			++itorGroupID)
		{
			CancelChallenge((*itorGroupID), "");
		}
	}
}

void MLadderMgr::LaunchLadder(CCLADDERTYPE nLadderType, int nGroupA, int nGroupB)
{
#ifdef _DEBUG
	//char szLog[128];
	//sprintf(szLog, "Team(%d) vs Group(%d) LADDER LAUNCHED \n", nGroupA, nGroupB);
	//OutputDebugString(szLog);
#endif 

	MLadderGroupMap* pGroupMap = GetWaitGroupContainer(nLadderType);
	if (pGroupMap == NULL) return;

	MLadderGroup* pGroupA = pGroupMap->Find(nGroupA);
	MLadderGroup* pGroupB = pGroupMap->Find(nGroupB);

	// ���� ���� Ŭ���̰ų� ���� �׷��̸� ��ġ�� �ȵȴ�.
	if ((pGroupA != NULL) && (pGroupB != NULL) && (pGroupA->IsSameGroup(pGroupB))) return;

	pGroupMap->Remove(nGroupA);
	pGroupMap->Remove(nGroupB);

	RemoveFromGroupList(pGroupA);
	RemoveFromGroupList(pGroupB);

	if ((pGroupA == NULL) || (pGroupB == NULL)) {
#ifdef _DEBUG
		OutputDebugString("LADDER �ҹ� \n");
#endif
		return;
	}

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	pServer->LadderGameLaunch(pGroupA, pGroupB);
}

#define MTIME_LADDER_DEFAULT_TICKINTERVAL		10000			// �⺻ƽ 10��

unsigned long int MLadderMgr::GetTickInterval()
{
	unsigned long int nDefaultTickInterval = MTIME_LADDER_DEFAULT_TICKINTERVAL;

	// ������ ���� ƽ�ð��� �������� �Ѵ�.
	int nObjSize = (int)CCMatchServer::GetInstance()->GetObjects()->size();
	
	if (nObjSize < 50)
	{
		nDefaultTickInterval = 5000;		// 50��̸��� 5��
	}
	else if ((nObjSize>=50) && (nObjSize < 150))
	{
		nDefaultTickInterval = 7000;		// 150�� �̸��� 7��
	}
	else if ((nObjSize>=150) && (nObjSize < 300))
	{
		nDefaultTickInterval = 9000;		// 300�� �̸��� 9��
	}
	return nDefaultTickInterval;
}


void MLadderMgr::Tick(unsigned long nTick)
{
	if (nTick - GetLastTick() < GetTickInterval())
		return;
	else
		SetLastTick(nTick);

	CleaningGarbages();

	for (int i = 0; i < CCLADDERTYPE_MAX; i++)
	{
		MakeMatch(CCLADDERTYPE(i));
	}

	m_Stat.Tick(nTick);
}

int MLadderMgr::GetNeedMemberCount(CCLADDERTYPE nLadderType)
{
	if ((nLadderType >= 0) && (nLadderType < CCLADDERTYPE_MAX))
	{
		return g_nNeedLadderMemberCount[(int)nLadderType];
	}

	return -1;
}

int MLadderMgr::GetTotalGroupCount()
{
	int ret = 0;
	for (int i = 0; i < CCLADDERTYPE_MAX; i++)
	{
		ret += (int)m_WaitingMaps[i].size();
	}
	return ret;
}

unsigned long MLadderMgr::GetChecksum(int nFirstIndex, int nGroupCount)
{
	unsigned long int nGroupListChecksum = 0;

	list<MLadderGroup*>::iterator itorGroup = m_GroupList.begin();
	for (int i = 0; i < nFirstIndex; i++, itorGroup++)
	{
		if (itorGroup == m_GroupList.end()) return 0;
	}
	
	for (int i = 0; i < nGroupCount; i++)
	{
		if (itorGroup == m_GroupList.end()) return nGroupListChecksum;

		MLadderGroup* pGroup = (*itorGroup);
		nGroupListChecksum += pGroup->GetChecksum();
		itorGroup++;
	}

	return nGroupListChecksum;

}

void MLadderMgr::DebugTest()
{
#ifdef _DEBUG
	OutputDebugString("Ahehheh \n");

	CCMatchServer* pServer = CCMatchServer::GetInstance();

	CCMatchObject* pPlayer1 = pServer->GetPlayerByName("��¸���");
	if (pPlayer1 == NULL) return;
	CCMatchObject* pPlayer2 = pServer->GetPlayerByName("�������2");
	if (pPlayer2 == NULL) return;


	// Ensure All Player Not in LadderGroup
	MLadderGroup* pGroupA = CreateLadderGroup();
	pGroupA->AddPlayer(pPlayer1);
	MLadderGroup* pGroupB = CreateLadderGroup();
	pGroupB->AddPlayer(pPlayer2);

//	if (ValidateLadderGroup(pGroup))
	Challenge(pGroupA);
	Challenge(pGroupB);

#endif
}
