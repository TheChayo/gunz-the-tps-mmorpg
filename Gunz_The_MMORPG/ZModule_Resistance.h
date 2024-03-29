#ifndef ZMODULE_RESISTANCE_H
#define ZMODULE_RESISTANCE_H

#include "ZModule.h"

// Added R350a
#include "ZModuleID.h"

class ZModule_Resistance : public ZModule
{
	int	m_nFR;	// �� ����ġ
	int m_nCR;
	int m_nPR;
	int m_nLR;

	void OnAdd();

public:
	DECLARE_ID(ZMID_RESISTANCE)
	ZModule_Resistance();
	virtual ~ZModule_Resistance();

	virtual void InitStatus();

	int GetFR() { return m_nFR; }
	int GetCR() { return m_nCR; }
	int GetPR() { return m_nPR; }
	int GetLR() { return m_nLR; }
};


#endif