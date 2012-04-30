#ifndef _MMATCHRULE_BERSERKER_H
#define _MMATCHRULE_BERSERKER_H


#include "CCMatchRule.h"
#include "CCMatchRuleDeathMatch.h"


class CCMatchRuleBerserker : public CCMatchRuleSoloDeath {
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
	CCMatchRuleBerserker(CCMatchStage* pStage);
	virtual ~CCMatchRuleBerserker() { }
	virtual void* CreateRuleInfoBlob();
	virtual void OnEnterBattle(CCUID& uidChar);			///< ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnLeaveBattle(CCUID& uidChar);			///< ������ �������� ȣ��ȴ�.
	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_BERSERKER; }
	CCUID& GetBerserker() { return m_uidBerserker; }
};

#endif