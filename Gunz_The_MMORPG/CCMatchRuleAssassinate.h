#ifndef _CCMATCHRULE_ASSASSINATE_H
#define _CCMATCHRULE_ASSASSINATE_H


#include "CCMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class CCMatchRuleAssassinate : public CCMatchRuleTeamDeath {
private:
	CCUID		m_uidRedCommander;
	CCUID		m_uidBlueCommander;
	bool		m_bIsAdminCommander;
private:
	const CCUID ChooseCommander(int nTeam);
protected:
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual bool OnCheckRoundFinish();
public:
	CCMatchRuleAssassinate(CCMatchStage* pStage);
	virtual ~CCMatchRuleAssassinate()				{}
	virtual void* CreateRuleInfoBlob();
	virtual void CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp);
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_ASSASSINATE; }
	void ChooseAdminAsCommander();
};



#endif