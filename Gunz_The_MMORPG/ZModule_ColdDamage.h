#ifndef _ZMODULE_COLDDAMAGE_H
#define _ZMODULE_COLDDAMAGE_H

#include "ZModule.h"
#include "ZModuleID.h"

class ZModule_ColdDamage : public ZModule {
	float	m_fBeginTime;	// 데미지 시작한 시간
	float	m_fNextDamageTime;	// 다음번 데미지를 받을 시간
	float	m_fDuration;	// 지속시간
//	float	m_fMoveSpeed;	// 이동제한 비율
	float	m_fNextEffectTime;

	bool	m_bOnDamage;	// 데미지 받고 있는 중인가

public:
	DECLARE_ID(ZMID_COLDDAMAGE)
	ZModule_ColdDamage();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();
	bool IsOnDamage() { return m_bOnDamage; }
	float GetDamageBeginTime() { return m_fBeginTime; }

	void BeginDamage(float fMoveSpeed,float fDuration);	// 데미지를 주기 시작한다
};

#endif