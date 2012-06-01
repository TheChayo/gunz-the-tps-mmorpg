#include "stdafx.h"
#include "CCMatchRuleDuel.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"
#include "CCMatchServer.h"

#include <algorithm>
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// CCMatchRuleDuel	   ///////////////////////////////////////////////////////////
CCMatchRuleDuel::CCMatchRuleDuel(CCMatchStage* pStage) : CCMatchRule(pStage)
{

}

void CCMatchRuleDuel::OnBegin()
{
	uidChampion = CCUID(0, 0);
	uidChallenger = CCUID(0, 0);

	CCMatchStage* pStage = GetStage();

	WaitQueue.clear();	// ��� ť�� ����

	if (pStage != NULL)
	{
		for(CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) 
			WaitQueue.push_back((*itor).first);			// �÷��̾�� �׳� ���� ��� ť�� �ִ´�.

//		SpawnPlayers();
	}

	nVictory = 0;

	return;
}

void CCMatchRuleDuel::OnEnd()
{
}


void CCMatchRuleDuel::OnRoundBegin()
{
	isRoundEnd = false;
	isTimeover = true;

	SpawnPlayers();
	SendQueueInfo(true);
	// �ֱ׷��� ���� ������ �ؾ� �� ���� �����Ȱ� ���� -_- �׿�������. �������� �����ȵ����� ���̻��� ó���� �ʿ������.
	// �̰� �� �Ŀ� �����Ǵ°Ÿ� ���ҵ�;
	for (list<CCUID>::iterator i = WaitQueue.begin(); i!=WaitQueue.end();  ++i)
		CCMatchServer::GetInstance()->OnDuelSetObserver(*i);							

}


void CCMatchRuleDuel::OnRoundEnd()
{
	if (isTimeover)
	{	
		WaitQueue.push_back(uidChampion);
		WaitQueue.push_back(uidChallenger);
		uidChampion = uidChallenger = CCUID(0, 0);
		nVictory = 0;
	}
	else
	{
		if (isChangeChampion || uidChampion == CCUID(0, 0))				// è�ǿ��� �ٲ��� �ϸ� �ϴ� è�ǿ°� �����ڸ� ����
		{
			CCUID uidTemp;
			uidTemp = uidChampion;
			uidChampion = uidChallenger;
			uidChallenger = uidTemp;
		}

		if (uidChallenger != CCUID(0, 0))
		{
			WaitQueue.push_back(uidChallenger);	// �����ڴ� ť�� �� �ڷ� �о�ְ�
			uidChallenger = CCUID(0, 0);			// �������� id�� ��ȿȭ
		}
	}

//	SpawnPlayers();
	LogInfo();
}

bool CCMatchRuleDuel::RoundCount() 
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	CCMatchStage* pStage = GetStage();
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

		if (pObj->GetAllRoundKillCount() >= (unsigned int)pStage->GetStageSetting()->GetRoundMax())
		{
			return false;
		}
	}

	return true;
}

bool CCMatchRuleDuel::OnCheckRoundFinish()
{
	if (!isRoundEnd)
		return false;
	else
	{
		isRoundEnd = false;
		isTimeover = false;
		return true;	
	}
}

void CCMatchRuleDuel::OnRoundTimeOut()
{
	SetRoundArg(CCMATCH_ROUNDRESULT_DRAW);
}


void CCMatchRuleDuel::OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim)
{
	isRoundEnd = true;

	/// ũ������ ���� ��� �ڵ� �߰�(��¿��� 0x00000000 �̰����� ������ ���⶧���� �ǽɰ��� �κ��� �ɷ��ش�.)
	if( CCMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel()) == NULL ) 
	{
		cclog( "error: can't find OwnerChannel [ CCMatchRuleDuel::OnGameKill() ]\n" );
		//LOG(LOG_FILE, "error: can't find OwnerChannel [ CCMatchRuleDuel::OnGameKill() ]\n" );
		return;
	}
	if( m_pStage == NULL)
	{
		cclog( "error: can't find m_pStage [ CCMatchRuleDuel::OnGameKill() ]\n" );
		//LOG(LOG_FILE, "error: can't find m_pStage [ CCMatchRuleDuel::OnGameKill() ]\n" );
		return;
	}

	CCUID chanID = CCMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel())->GetUID();

	if (uidVictim == uidChallenger)		// è�ǿ��� ������� è�ǿ� ����
	{
		isChangeChampion = false;
		nVictory++;

		if (m_pStage == NULL) return;
		if (m_pStage->IsPrivate()) return;		// ��й��̸� ��� �н�

		if (nVictory % 10 != 0) return;			// ���¼��� 10�� ����϶���

		CCMatchObject* pChamp;
		pChamp = m_pStage->GetObj(uidChampion);
		if (pChamp == NULL) return;


		CCMatchServer::GetInstance()->BroadCastDuelRenewVictories(
			chanID,
			pChamp->GetName(), 
			CCMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel())->GetName(), 
			m_pStage->GetIndex()+1,
			nVictory
			);
	}
	else
	{
		isChangeChampion = true;

		int nowVictory = nVictory;

		nVictory = 1;

		if (nowVictory < 10) return;				// 10���� �̻��� ������������
		if (m_pStage == NULL) return;
		if (m_pStage->IsPrivate()) return;		// ��й��̸� ��� �н�
	
		CCMatchObject* pChamp, *pChallenger;
		pChamp = m_pStage->GetObj(uidChampion);
		if (pChamp == NULL) return;
		pChallenger = m_pStage->GetObj(uidChallenger);
		if (pChallenger == NULL) return;

		if (strcmp(m_pStage->GetPassword(), "") != 0) return;

		CCMatchServer::GetInstance()->BroadCastDuelInterruptVictories(
			chanID,
			pChamp->GetName(),
			pChallenger->GetName(),
			nowVictory
			);
	}


	LogInfo();

}

void CCMatchRuleDuel::OnEnterBattle(CCUID& uidChar)
{
	if ((uidChar != uidChampion) && (uidChar != uidChallenger) && (find(WaitQueue.begin(), WaitQueue.end(), uidChar) == WaitQueue.end()))
	{
		WaitQueue.push_back(uidChar);
		SpawnPlayers();
	}
	SendQueueInfo();
	LogInfo();
}

void CCMatchRuleDuel::OnLeaveBattle(CCUID& uidChar)
{
	if (uidChar == uidChampion)
	{
		isChangeChampion = true;
		isRoundEnd = true;
		uidChampion = CCUID(0, 0);
		nVictory = 0;
	}
	else if (uidChar == uidChallenger)
	{
		isChangeChampion = false;
		isRoundEnd = true;
		uidChallenger = CCUID(0, 0);
	}
	else
	{
		WaitQueue.remove(uidChar);
		SendQueueInfo();
		LogInfo();
	}
}

void CCMatchRuleDuel::SpawnPlayers()
{
	if (uidChampion == CCUID(0, 0))
	{
		if (!WaitQueue.empty())
		{
			uidChampion = WaitQueue.front();
			WaitQueue.pop_front();
		}
	}
	if (uidChallenger == CCUID(0, 0))
	{
		if (!WaitQueue.empty())
		{
			uidChallenger = WaitQueue.front();
			WaitQueue.pop_front();
		}
	}
}

bool CCMatchRuleDuel::OnCheckEnableBattleCondition()
{
	if (uidChampion == CCUID(0, 0) || uidChallenger == CCUID(0, 0))
	{
		if (WaitQueue.empty())
			return false;
		else
			isRoundEnd = true;
	}

	return true;
}

void CCMatchRuleDuel::LogInfo()
{
#ifdef _DEBUG
	if (m_pStage == NULL) return;
	CCMatchObject* pObj;
	char buf[250];
	sprintf(buf, "Logging Que--------------------\n");
	OutputDebugString(buf);

	pObj = m_pStage->GetObj(uidChampion);
	if (pObj != NULL)
	{
		sprintf(buf, "Champion name : %s \n", pObj->GetName());
		OutputDebugString(buf);
	}

	pObj = m_pStage->GetObj(uidChallenger);
	if (pObj != NULL)
	{
		sprintf(buf, "Challenger name : %s \n", pObj->GetName());
		OutputDebugString(buf);
	}

	int x = 0;
	for (list<CCUID>::iterator i = WaitQueue.begin(); i!=WaitQueue.end();  ++i)
	{
		pObj = m_pStage->GetObj(*i);
		if (pObj != NULL)
		{
			sprintf(buf, "Wait Queue #%d : %s \n", x, pObj->GetName());
			OutputDebugString(buf);		
			x++;
		}
	}
#endif
}

void CCMatchRuleDuel::SendQueueInfo(bool isRoundEnd)
{
	if (m_pStage == NULL) return;
	CCTD_DuelQueueInfo QInfo;
	QInfo.m_uidChampion = uidChampion;
	QInfo.m_uidChallenger = uidChallenger;
	QInfo.m_nQueueLength = static_cast<char>(WaitQueue.size());
	QInfo.m_nVictory = nVictory;
	QInfo.m_bIsRoundEnd = isRoundEnd;

	int i=0;
	list<CCUID>::const_iterator itEnd = WaitQueue.end();
	for (list<CCUID>::iterator iter = WaitQueue.begin(); iter != itEnd; ++iter, ++i)
	{
		if( 14 > i )
		{
			QInfo.m_WaitQueue[i] = *iter;
		}
		else
		{
			cclog( "duel queue info index error. size : %u\n", WaitQueue.size() );
			break;
		}
	}

	CCMatchServer::GetInstance()->OnDuelQueueInfo(m_pStage->GetUID(), QInfo);
}