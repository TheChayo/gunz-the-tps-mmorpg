#include "stdafx.h"
#include "CCMatchRuleDeathMatch.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"

// TEAM DEATH RULE ///////////////////////////////////////////////////////////////
CCMatchRuleTeamDeath::CCMatchRuleTeamDeath(CCMatchStage* pStage) : CCMatchRule(pStage)
{
}

void CCMatchRuleTeamDeath::OnBegin()
{
}

void CCMatchRuleTeamDeath::OnEnd()
{
}

bool CCMatchRuleTeamDeath::OnRun()
{
	bool ret = CCMatchRule::OnRun();


	return ret;
}

void CCMatchRuleTeamDeath::OnRoundBegin()
{
	CCMatchRule::OnRoundBegin();
}

void CCMatchRuleTeamDeath::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		switch(m_nRoundArg)
		{
			case MMATCH_ROUNDRESULT_BLUE_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(CCMT_RED);break;
			case MMATCH_ROUNDRESULT_RED_ALL_OUT: m_pStage->OnRoundEnd_FromTeamGame(CCMT_BLUE); break;
			case MMATCH_ROUNDRESULT_REDWON: m_pStage->OnRoundEnd_FromTeamGame(CCMT_RED); break;
			case MMATCH_ROUNDRESULT_BLUEWON: m_pStage->OnRoundEnd_FromTeamGame(CCMT_BLUE); break;
			case MMATCH_ROUNDRESULT_DRAW: break;
		}
	}

	CCMatchRule::OnRoundEnd();
}

bool CCMatchRuleTeamDeath::OnCheckEnableBattleCondition()
{
	// �������� ���� Free���°� �ȵȴ�.
	if (m_pStage->GetStageSetting()->IsTeamWinThePoint() == true)
	{
		return true;
	}

	int nRedTeam = 0, nBlueTeam = 0;
	int nPreRedTeam = 0, nPreBlueTeam = 0;
	int nStageObjects = 0;		// ���Ӿȿ� ���� ���������� �ִ� ���

	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
		{
			nStageObjects++;
			continue;
		}

		if (pObj->GetTeam() == CCMT_RED)
		{
			nRedTeam++;
		}
		else if (pObj->GetTeam() == CCMT_BLUE)
		{
			nBlueTeam++;
		}
	}

	if ( nRedTeam == 0 || nBlueTeam == 0)
	{
		return false;
	}

	return true;
}

// ���� �������̳� ��������� ������ 0���� ���� false ��ȯ , true,false ��� AliveCount ��ȯ
bool CCMatchRuleTeamDeath::GetAliveCount(int* pRedAliveCount, int* pBlueAliveCount)
{
	int nRedCount = 0, nBlueCount = 0;
	int nRedAliveCount = 0, nBlueAliveCount = 0;
	(*pRedAliveCount) = 0;
	(*pBlueAliveCount) = 0;

	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return false;

	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ

		if (pObj->GetTeam() == CCMT_RED)
		{
			nRedCount++;
			if (pObj->CheckAlive()==true)
			{
				nRedAliveCount++;
			}
		}
		else if (pObj->GetTeam() == CCMT_BLUE)
		{
			nBlueCount++;
			if (pObj->CheckAlive()==true)
			{
				nBlueAliveCount++;
			}
		}
	}

	(*pRedAliveCount) = nRedAliveCount;
	(*pBlueAliveCount) = nBlueAliveCount;

	if ((nRedAliveCount == 0) || (nBlueAliveCount == 0))
	{
		return false;
	}
	return true;
}

bool CCMatchRuleTeamDeath::OnCheckRoundFinish()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;

	// ������ 0���� ���� ������ false��ȯ
	if (GetAliveCount(&nRedAliveCount, &nBlueAliveCount) == false)
	{
		int nRedTeam = 0, nBlueTeam = 0;
		int nStageObjects = 0;		// ���Ӿȿ� ���� ���������� �ִ� ���

		CCMatchStage* pStage = GetStage();

		for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
		{
			CCMatchObject* pObj = (CCMatchObject*)(*i).second;
			if ((pObj->GetEnterBattle() == false) && (!pObj->IsLaunchedGame()))
			{
				nStageObjects++;
				continue;
			}

			if (pObj->GetTeam() == CCMT_RED)		nRedTeam++;
			else if (pObj->GetTeam() == CCMT_BLUE)	nBlueTeam++;
		}

		if( nBlueTeam ==0 && (pStage->GetTeamScore(CCMT_BLUE) > pStage->GetTeamScore(CCMT_RED)) )
			SetRoundArg(MMATCH_ROUNDRESULT_BLUE_ALL_OUT);
		else if( nRedTeam ==0 && (pStage->GetTeamScore(CCMT_RED) > pStage->GetTeamScore(CCMT_BLUE)) )
			SetRoundArg(MMATCH_ROUNDRESULT_RED_ALL_OUT);
		else if ( (nRedAliveCount == 0) && (nBlueAliveCount == 0) )
			SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
		else if (nRedAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		else if (nBlueAliveCount == 0)
			SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	}

	if (nRedAliveCount==0 || nBlueAliveCount==0) return true;
	else return false;
}

void CCMatchRuleTeamDeath::OnRoundTimeOut()
{
	int nRedAliveCount = 0;
	int nBlueAliveCount = 0;
	GetAliveCount(&nRedAliveCount, &nBlueAliveCount);

	if (nRedAliveCount > nBlueAliveCount)
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
	else if (nBlueAliveCount > nRedAliveCount)
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	else SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// ��ȯ���� false�̸� ������ ������.
bool CCMatchRuleTeamDeath::RoundCount() 
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	if (m_pStage->GetStageSetting()->IsTeamWinThePoint() == false)
	{
		// �������� �ƴ� ���
		if (m_nRoundCount < nTotalRound) return true;

	}
	else
	{
		// �������� ��� 

		// ������ 0���� ���� �־ ������ ������.
		int nRedTeamCount=0, nBlueTeamCount=0;
		m_pStage->GetTeamMemberCount(&nRedTeamCount, &nBlueTeamCount, NULL, true);

		if ((nRedTeamCount == 0) || (nBlueTeamCount == 0))
		{
			return false;
		}

		int nRedScore = m_pStage->GetTeamScore(CCMT_RED);
		int nBlueScore = m_pStage->GetTeamScore(CCMT_BLUE);
		
		// �������ӿ��� ���� 4���� ���� �¸�
		const int LADDER_WINNING_ROUNT_COUNT = 4;


		// ������ ��� 4���� �ƴϸ� true��ȯ
		if ((nRedScore < LADDER_WINNING_ROUNT_COUNT) && (nBlueScore < LADDER_WINNING_ROUNT_COUNT))
		{
			return true;
		}
	}

	return false;
}

void CCMatchRuleTeamDeath::CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	if (m_pStage == NULL)
	{
		*poutAttackerExp = nSrcExp;
		*poutTeamExp = 0;
		return;
	}

	*poutTeamExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamBonusExpRatio);
	*poutAttackerExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamMyExpRatio);
}

//////////////////////////////////////////////////////////////////////////////////
// CCMatchRuleSoloDeath ///////////////////////////////////////////////////////////
CCMatchRuleSoloDeath::CCMatchRuleSoloDeath(CCMatchStage* pStage) : CCMatchRule(pStage)
{

}

void CCMatchRuleSoloDeath::OnBegin()
{

}
void CCMatchRuleSoloDeath::OnEnd()
{
}

bool CCMatchRuleSoloDeath::RoundCount()
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

bool CCMatchRuleSoloDeath::CheckKillCount(CCMatchObject* pOutObject)
{
	CCMatchStage* pStage = GetStage();
	for (CCUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		CCMatchObject* pObj = (CCMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

		if (pObj->GetKillCount() >= (unsigned int)pStage->GetStageSetting()->GetRoundMax())
		{
			pOutObject = pObj;
			return true;
		}
	}
	return false;
}

bool CCMatchRuleSoloDeath::OnCheckRoundFinish()
{
	CCMatchObject* pObject = NULL;

	if (CheckKillCount(pObject))
	{
		return true;
	}
	return false;
}

void CCMatchRuleSoloDeath::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}




// ���� ��������ġ - �߰� by ����
//////////////////////////////////////////////////////////////////////////
CCMatchRuleTeamDeath2::CCMatchRuleTeamDeath2(CCMatchStage* pStage) : CCMatchRule(pStage)
{
}

void CCMatchRuleTeamDeath2::OnBegin()
{
	m_pStage->InitTeamKills();
}

void CCMatchRuleTeamDeath2::OnEnd()
{
}

bool CCMatchRuleTeamDeath2::OnRun()
{
	bool ret = CCMatchRule::OnRun();


	return ret;
}

void CCMatchRuleTeamDeath2::OnRoundBegin()
{
	CCMatchRule::OnRoundBegin();
}

void CCMatchRuleTeamDeath2::OnRoundEnd()
{
	if (m_pStage != NULL)
	{
		if (m_nRoundArg == MMATCH_ROUNDRESULT_REDWON) 
		{
			m_pStage->OnRoundEnd_FromTeamGame(CCMT_RED);
		} 
		else if (m_nRoundArg == MMATCH_ROUNDRESULT_BLUEWON) 
		{
			m_pStage->OnRoundEnd_FromTeamGame(CCMT_BLUE);
		} 
		else if (m_nRoundArg == MMATCH_ROUNDRESULT_DRAW) 
		{ 
			// Do Nothing
		}
	}

	CCMatchRule::OnRoundEnd();
}

// ���� �������̳� ��������� ������ 0���� ���� false ��ȯ , true,false ��� AliveCount ��ȯ
void CCMatchRuleTeamDeath2::GetTeamScore(int* pRedTeamScore, int* pBlueTeamScore)
{
	(*pRedTeamScore) = 0;
	(*pBlueTeamScore) = 0;

	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return;

	(*pRedTeamScore) = pStage->GetTeamKills(CCMT_RED);
	(*pBlueTeamScore) = pStage->GetTeamKills(CCMT_BLUE);

	return;
}

bool CCMatchRuleTeamDeath2::OnCheckRoundFinish()
{
	int nRedScore, nBlueScore;
	GetTeamScore(&nRedScore, &nBlueScore);

	CCMatchStage* pStage = GetStage();

	if (nRedScore >= pStage->GetStageSetting()->GetRoundMax())
	{
		SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
		return true;
	}
	else if (nBlueScore >= pStage->GetStageSetting()->GetRoundMax())
	{
		SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
		return true;
	}

	return false;
}

void CCMatchRuleTeamDeath2::OnRoundTimeOut()
{
	if (!OnCheckRoundFinish())
		SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}

// ��ȯ���� false�̸� ������ ������.
bool CCMatchRuleTeamDeath2::RoundCount() 
{
	if (++m_nRoundCount < 1) return true;
	return false;
}

void CCMatchRuleTeamDeath2::CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
										int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	if (m_pStage == NULL)
	{
		*poutAttackerExp = nSrcExp;
		*poutTeamExp = 0;
		return;
	}

	*poutTeamExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamBonusExpRatio);
	*poutAttackerExp = (int)(nSrcExp * m_pStage->GetStageSetting()->GetCurrGameTypeInfo()->fTeamMyExpRatio);
}




void CCMatchRuleTeamDeath2::OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim)
{
	CCMatchObject* pAttacker = CCMatchServer::GetInstance()->GetObject(uidAttacker);
	CCMatchObject* pVictim = CCMatchServer::GetInstance()->GetObject(uidVictim);

	if (m_pStage != NULL)
	{
//		if (pAttacker->GetTeam() != pVictim->GetTeam())
//		{
//			m_pStage->AddTeamKills(pAttacker->GetTeam());
//		}

		m_pStage->AddTeamKills(pVictim->GetTeam() == CCMT_BLUE ? CCMT_RED : CCMT_BLUE);		// ������� �ݴ����� ų�� �ø�
	}
}