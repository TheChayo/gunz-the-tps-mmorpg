#ifndef _ZQUEST_MAP_H
#define _ZQUEST_MAP_H

#include "CCBaseQuest.h"

class ZQuestMap
{
private:
	vector<int>					m_MapVector;
public:
	ZQuestMap();
	~ZQuestMap();
	void Init();
	void Final();
};




#endif