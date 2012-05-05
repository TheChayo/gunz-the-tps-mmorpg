#pragma once

#include "CCBaseQuest.h"
#include "CCQuestScenario.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

struct CCTD_QuestGameInfo;
struct CCQuestNPCSetInfo;

class CCQuestNPCQueue
{
private:
	vector<CCQUEST_NPC>			m_Queue;			///< NPC�� vector���·� ����ִ�.
	int							m_nCursor;
	bool						m_bContainKeyNPC;	///< keyNPC�� ���� �ִ°�
public:
	CCQuestNPCQueue();										///< ������
	~CCQuestNPCQueue();										///< �Ҹ���

	/// QLD�� NPCSet�� �������� NPCť�� �����.
	/// @param nQLD				QLD
	/// @param pNPCSetInfo		NPC Set ����
	void Make(int nQLD, CCQuestNPCSetInfo* pNPCSetInfo, CCQUEST_NPC nKeyNPC=NPC_NONE);		
	void Clear();											///< ť �ʱ�ȭ
	bool Pop(CCQUEST_NPC& outNPC);							///< ť���� NPC�� �ϳ� ������.
	bool GetFirst(CCQUEST_NPC& outNPC);						///< ���� ó�� NPC���� �д´�.
	bool IsEmpty();											///< ť�� ������� üũ
	int GetCount();											///< ť�� ����ִ� NPC�� ��ȯ. Pop���� �̹� ���� NPC�� �����Ѵ�.
	bool IsContainKeyNPC() { return m_bContainKeyNPC; }		///< ť�� KeyNPC�� ����ִ°�
	bool IsKeyNPC(CCQUEST_NPC npc);							///< KeyNPC�� ť�� ����ִٸ� �־��� ���ڰ� keyNPC�� �´��� Ȯ��
};




/// ������ �ʴ� ����Ʈ ���巹�� ����
struct CCQuestLevelStaticInfo
{
	CCQuestScenarioInfo*				pScenario;			///< �ش� ������ �ó�����
	int								nDice;				///< ���� �ֻ�����
	set<CCQUEST_NPC>					NPCs;				///< ���� NPC ����
	vector<CCQuestLevelSectorNode>	SectorList;			///< Map Sector ����Ʈ
	int								nQL;				///< ����Ʈ ����
	float							fNPC_TC;			///< NPC ���̵� ���� ���(NPC Toughness Coefficient)
	int								nQLD;				///< ����Ʈ ���� ���̵�
	int								nLMT;				///< �ѹ��� �����Ǵ� ���� ���� ����

	CCQuestLevelStaticInfo()
	{
		pScenario=NULL;
		nDice = 0;
		nQL=0;
		fNPC_TC = 1.0f;
		nQLD = 0;
		nLMT = 1;
	}
};

/// ����Ʈ���� ���� ������
struct CCQuestLevelItem
{
	unsigned long int	nItemID;			///< ����Ʈ ������ ID
	int					nRentPeriodHour;	///< �Ϲ� �������� ��� ��� �Ⱓ
	bool				bObtained;			///< �÷��̾ �Ծ����� ����
	int					nMonsetBibleIndex;	// ���� ������ ���� ���� Ÿ���� ���� �ε���.

	CCQuestLevelItem() : nItemID(0), bObtained(false), nRentPeriodHour(0) {}
	bool IsQuestItem() { return IsQuestItemID(nItemID); }
};

/// ����Ʈ���� ���� �����۵�
class CCQuestLevelItemMap : public multimap<unsigned long int, CCQuestLevelItem*>
{
public:
	CCQuestLevelItemMap() {}
	~CCQuestLevelItemMap() { Clear(); }
	void Clear()
	{
		for (iterator itor = begin(); itor != end(); ++itor)
		{
			delete (*itor).second;
		}
		clear();
	}
};

/// ����Ʈ ���� �����ϸ鼭 ���ϴ� ����
struct CCQuestLevelDynamicInfo
{
	int						nCurrSectorIndex;								///< ���� �������� ���� �ε���
	CCQuestMapSectorInfo*	pCurrSector;									///< ���� �������� ����
	int						nQLD;											///< ����Ʈ ���� ���̵� ���(QLD)
	bool					bCurrBossSector;								///< ���� ���Ͱ� ���� �������� ����
	CCQuestLevelItemMap		ItemMap;										///< ����Ʈ���� ���� �����۵�
	int						nRepeated;										///< ���� �ó����� �ݺ� Ƚ�� (�����̹���)

	CCQuestLevelDynamicInfo()
	{
		nCurrSectorIndex = 0;
		pCurrSector = NULL;
		nQLD = 0;
		bCurrBossSector = false;
		nRepeated = 0;
	}
};


/// ����Ʈ ���� ���� - ����Ʈ�� ��� ������ ���� �� �ִ�.
class CCQuestLevel
{
private:
	/// NPC ���� �ð��� �����ϱ� ���� ����ü
	struct CCQuestLevelSpawnInfo
	{
		int					nIndex;								///< ���� �ε���
		unsigned long int	nRecentSpawnTime[MAX_SPAWN_COUNT];	///< �ֱٿ� ������ �ð�
	};

	CCQuestLevelStaticInfo		m_StaticInfo;					///< ����Ʈ �����ϸ� ������ �ʴ� ����
	CCQuestLevelDynamicInfo		m_DynamicInfo;					///< ����Ʈ �����ϸ鼭 ���ϴ� ����
	CCQuestNPCQueue				m_NPCQueue;						///< ������ NPC ������Ʈ ť

	CCQuestLevelSpawnInfo		m_SpawnInfos[MNST_END];			///< NPC �������� ����

	bool InitSectors(CCMATCH_GAMETYPE eGameType);
	bool InitNPCs();											
	void InitStaticInfo(CCMATCH_GAMETYPE eGameType);
	void InitCurrSector(CCMATCH_GAMETYPE eGameType);

public:
	CCQuestLevel();						///< ������
	~CCQuestLevel();						///< �Ҹ���

	/// �ó����� ID�� ������� ���巹�� �ʱ�ȭ
	/// @param nScenarioID			�ó����� ID
	/// @param nDice				�ֻ��� ����
	void Init(int nScenarioID, int nDice, CCMATCH_GAMETYPE eGameType);
	/// ������ ���巹���� �������� Ŭ���̾�Ʈ�� ������ ���۵���Ÿ�� �����.
	const bool Make_CCTDQuestGameInfo(CCTD_QuestGameInfo* pout, CCMATCH_GAMETYPE eGameType);
	/// �ʼ��� �� ��ȯ
	/// @return		���� ��
	int GetMapSectorCount();
	/// ���� ���� �ε��� ��ȯ
	/// @return		���� ���� �ε���
	int GetCurrSectorIndex();
	/// ���� ���ͷ� �̵��Ѵ�.
	/// @return		����/���� ����
	bool MoveToNextSector(CCMATCH_GAMETYPE eGameType);	
	/// ���� �¾ NPC�� ��ġ�� ��õ�Ѵ�.
	/// @param nSpawnType		NPC�� ����Ÿ��
	/// @param nNowTime			���� �ð�
	int GetRecommendedSpawnPosition(CCQuestNPCSpawnType nSpawnType, unsigned long int nNowTime);
	/// ���� NPC�� ������������ Ȯ���Ѵ�.
	/// @param nSpawnType		NPC�� ����Ÿ��
	/// @param nNowTime			���� �ð�
	bool IsEnableSpawnNow(CCQuestNPCSpawnType nSpawnType, unsigned long int nNowTime);
	/// ���� ���Ϳ��� ���� ���� ������ ���Ѵ�.
	/// @param nSpawnType		NPC�� ����Ÿ��
	int GetSpawnPositionCount(CCQuestNPCSpawnType nSpawnType);

	/// ����Ʈ �������� ������ ��� ȣ��ȴ�.
	/// @nItemID				����Ʈ ������ ID
	/// @nRentPeriodHour		�Ϲ� �������� ��� ��� �Ⱓ
	void OnItemCreated(unsigned long int	nItemID, int nRentPeriodHour);
	/// �÷��̾ ����Ʈ ������ �Ծ��� ��� ȣ��ȴ�.
	/// @nItemID				����Ʈ ������ ID
	bool OnItemObtained( CCMatchObject* pPlayer, unsigned long int	nItemID);		

	CCQuestNPCQueue* GetNPCQueue()				{ return &m_NPCQueue; }
	CCQuestLevelStaticInfo* GetStaticInfo()		{ return &m_StaticInfo; }
	CCQuestLevelDynamicInfo* GetDynamicInfo()	{ return &m_DynamicInfo; }
};