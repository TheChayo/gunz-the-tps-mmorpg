#ifndef _ZMYBUFFMGR_H
#define _ZMYBUFFMGR_H

#include "ZMyBuff.h"

class ZMyBuffMgr
{
protected:
	ZMyShortBuffMap m_ShortBuffMap;

	ZMyBuffSummary	m_BuffSummary;

	bool InsertShortBuffInfo(CCShortBuffInfo *pShortInfo);

public:
	ZMyBuffMgr();
	~ZMyBuffMgr();

//버프정보임시주석 	void Set(CCTD_CharBuffInfo* pCharBuffInfo);
	void Clear();		
};

#endif