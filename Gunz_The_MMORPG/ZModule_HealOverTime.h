#ifndef _ZMODULE_HEALOVERTIME_H
#define _ZMODULE_HEALOVERTIME_H

#include "ZModule.h"
#include "ZModuleID.h"

// �ð��� �帧�� ���� HP�� AP�� ȸ���Ǵ� ȿ���� ���� ���

class ZModule_HealOverTime : public ZModule {
	CCProtectValue<float>	m_fBeginTime;		// �� ������ �ð�
	CCProtectValue<float>	m_fNextHealTime;	// ������ �� ���� �ð�

	CCProtectValue<float>	m_fHeal;			// 1ȸ ȸ����
	CCProtectValue<int>		m_numHealDesire;	// �� ��ǥ ȸ��Ƚ��
	CCProtectValue<int>		m_numHealDone;		// ���� ȸ���� Ƚ��

	CCProtectValue<CCMatchDamageType> m_type;
	CCMatchItemEffectId		m_nEffectId;
	int						m_nItemId;			// ȿ���� ����Ų ������ID

	CCProtectValue<bool>		m_bOnHeal;	// �� �ް� �ִ� ���ΰ�

public:
	DECLARE_ID(ZMID_HEALOVERTIME)
	ZModule_HealOverTime();

	virtual bool Update(float fElapsed);
	virtual void InitStatus();

	void BeginHeal(CCMatchDamageType type, int nHealAmount, int numHeal, CCMatchItemEffectId effectId, int nItemId);
	bool IsOnHeal() { return m_bOnHeal.Ref(); }
	float GetHealBeginTime() { return m_fBeginTime.Ref(); }

	bool GetHealOverTimeBuffInfo(CCTD_BuffInfo& out);

	void ShiftFugitiveValues();
};

#endif