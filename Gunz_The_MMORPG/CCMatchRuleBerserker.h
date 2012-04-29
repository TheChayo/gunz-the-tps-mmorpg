#ifndef _MMATCHRULE_BERSERKER_H
#define _MMATCHRULE_BERSERKER_H


#include "MMatchRule.h"
#include "MMatchRuleDeathMatch.h"


class MMatchRuleBerserker : public MMatchRuleSoloDeath {
protected:
	// ������� ---------------------
	CCUID		m_uidBerserker;				// ���� ����Ŀ�� �÷��̾�

	// �Լ� -------------------------
	bool CheckKillCount(CCMatchObject* pOutObject);
	virtual void OnRoundBegin();
	virtual bool OnCheckRoundFinish();
	void RouteAssignBerserker();
	CCUID RecommendBerserker();
public:
	MMatchRuleBerserker(CCMatchStage* pStage);
	virtual ~MMatchRuleBerserker() { }
	virtual void* CreateRuleInfoBlob();
	virtual void OnEnterBattle(CCUID& uidChar);			///< ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnLeaveBattle(CCUID& uidChar);			///< ������ �������� ȣ��ȴ�.
	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_BERSERKER; }
	CCUID& GetBerserker() { return m_uidBerserker; }
};

#endif