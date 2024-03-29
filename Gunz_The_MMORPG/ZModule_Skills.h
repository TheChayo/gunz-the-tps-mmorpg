#ifndef _ZMODULE_SKILLS_H
#define _ZMODULE_SKILLS_H

#include "ZModule.h"
#include "ZModuleID.h"
#include "CCQuestNPC.h"
#include "ZSkill.h"

class ZModule_Skills : public ZModule {
	int			m_nSkillCount;
    ZSkill		m_Skills[MAX_SKILL];


public:
	DECLARE_ID(ZMID_SKILLS)
	
	ZModule_Skills();

	void Init(int nSkills, const int *pSkillIDs);

	bool Update(float fElapsed);
	void InitStatus();

	int GetSkillCount();
	ZSkill *GetSkill(int nSkill);			// 스킬을 얻어낸다.

	void PreExcute(int nSkill,CCUID uidTarget,rvector targetPosition);	// 모션이 시작할때
	void Excute(int nSkill,CCUID uidTarget,rvector targetPosition);		// 실행한다
	void LastExcute(int nSkill,CCUID uidTarget,rvector targetPosition);	// 모션이 끝날떄

};

#endif