#ifndef _CCQUESTNPCSPAWNTRIGGER_H
#define _CCQUESTNPCSPAWNTRIGGER_H

#include "CCBaseQuest.h"


struct SpawnTriggerNPCInfoNode
{
	CCQUEST_NPC	nNPCID;
	float		fRate;
};

struct SpawnTriggerInfo
{
	int				nSpawnNPCCount;			// 1ȸ ������ ������ NPC��
	unsigned int	nSpawnTickTime;			// ���� ƽ Ÿ��

	// ����
	int				nCurrMinNPCCount;		// �̰������϶� �����Ѵ�.
	int				nCurrMaxNPCCount;		// �̰��̻��϶� �����Ѵ�.
};

class CCQuestNPCSpawnTrigger
{
private:
	vector<SpawnTriggerNPCInfoNode>		m_NPCInfo;
	SpawnTriggerInfo					m_Info;
	unsigned int						m_nLastTime;
	float								m_fRateSum;
	vector<CCQUEST_NPC>					m_NPCQueue;
	void MakeSpawnNPCs();
	CCQUEST_NPC GetRandomNPC();
public:
	CCQuestNPCSpawnTrigger();
	~CCQuestNPCSpawnTrigger();
	void BuildNPCInfo(SpawnTriggerNPCInfoNode& NPCInfo);
	void BuildCondition(SpawnTriggerInfo& Info);

	bool CheckSpawnEnable(int nCurrNPCCount);
	void Clear();
	vector<CCQUEST_NPC>& GetQueue() { return m_NPCQueue; }
};



#endif