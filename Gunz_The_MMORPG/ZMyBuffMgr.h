#ifndef _ZMYBUFFMGR_H
#define _ZMYBUFFMGR_H

#include "ZMyBuff.h"

class ZMyBuffMgr
{
protected:
	ZMyShortBuffMap m_ShortBuffMap;

	ZMyBuffSummary	m_BuffSummary;

	bool InsertShortBuffInfo(MShortBuffInfo *pShortInfo);

public:
	ZMyBuffMgr();
	~ZMyBuffMgr();

//���������ӽ��ּ� 	void Set(CCTD_CharBuffInfo* pCharBuffInfo);
	void Clear();		
};

#endif