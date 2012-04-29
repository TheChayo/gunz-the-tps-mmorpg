#include "stdafx.h"
#include "MMatchRuleBerserker.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"
#include "CCMatchServer.h"

//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleBerserker ///////////////////////////////////////////////////////////
MMatchRuleBerserker::MMatchRuleBerserker(CCMatchStage* pStage) : MMatchRuleSoloDeath(pStage), m_uidBerserker(0,0)
{

}

bool MMatchRuleBerserker::OnCheckRoundFinish()
{
	return MMatchRuleSoloDeath::OnCheckRoundFinish();
}

void MMatchRuleBerserker::OnRoundBegin()
{
	m_uidBerserker = CCUID(0,0);
}

void* MMatchRuleBerserker::CreateRuleInfoBlob()
{
	void* pRuleInfoArray = MMakeBlobArray(sizeof(MTD_RuleInfo_Berserker), 1);
	MTD_RuleInfo_Berserker* pRuleItem = (MTD_RuleInfo_Berserker*)MGetBlobArrayElement(pRuleInfoArray, 0);
	memset(pRuleItem, 0, sizeof(MTD_RuleInfo_Berserker));
	
	pRuleItem->nRuleType = MMATCH_GAMETYPE_BERSERKER;
	pRuleItem->uidBerserker = m_uidBerserker;

	return pRuleInfoArray;
}

void MMatchRuleBerserker::RouteAssignBerserker()
{	MCommand* pNew = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_ASSIGN_BERSERKER, CCUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(m_uidBerserker));
	CCMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}


CCUID MMatchRuleBerserker::RecommendBerserker()
{
	CCMatchStage* pStage = GetStage();
	if (pStage == NULL) return CCUID(0,0);

	int nCount = 0;
	for(CCUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		CCMatchObject* pObj = (CCMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;	// ��Ʋ�����ϰ� �ִ� �÷��̾ üũ
		if (pObj->CheckAlive())
		{
			return pObj->GetUID();
		}
	}
	return CCUID(0,0);

}


void MMatchRuleBerserker::OnEnterBattle(CCUID& uidChar)
{
}

void MMatchRuleBerserker::OnLeaveBattle(CCUID& uidChar)
{
	if (uidChar == m_uidBerserker)
	{
		m_uidBerserker = CCUID(0,0);
		RouteAssignBerserker();
	}
}

void MMatchRuleBerserker::OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim)
{
	// ����ڰ� ����Ŀ�̰ų� ���� ����Ŀ�� �Ѹ� ������
	if ((m_uidBerserker == uidVictim) || (m_uidBerserker == CCUID(0,0)))
	{
		bool bAttackerCanBeBerserker = false;

		 // �����ڰ� �ڽ��� �ƴ� ���
		if (uidAttacker != uidVictim)
		{
			CCMatchObject* pAttacker = CCMatchServer::GetInstance()->GetObject(uidAttacker);

			// �����ڰ� �׾������� ����Ŀ�� �� �� ����(���꼦)
			if ((pAttacker) && (pAttacker->CheckAlive()))
			{
				bAttackerCanBeBerserker = true;
			}
		}
		// �����ڰ� �ڽ��� ��� ����Ŀ�� �ƹ��� ���� �ʴ´�.
		else if ((uidAttacker == CCUID(0,0)) || (uidAttacker == uidVictim))
		{
			bAttackerCanBeBerserker = false;
		}

		if (bAttackerCanBeBerserker) m_uidBerserker = uidAttacker;
		else m_uidBerserker = CCUID(0,0);

		RouteAssignBerserker();
	}
}

