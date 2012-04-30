#pragma once

// �̰����� ����Ʈ���� Ŭ���̾�Ʈ�� ������ �������� ����ϴ� ���� �ֵ��� �Ѵ�.
//////////////////////////////////////////////////////////////////////////////

#include "CCQuestNPC.h"
#include "CCQuestMap.h"
#include "CCQuestScenario.h"
//#include "MSurvivalMap.h"
#include "CCQuestDropTable.h"

#include <map>
using namespace std;


struct CCQuestNPCInfo;

/// ����Ʈ ���巹������ ����ϴ� ���� ���
struct CCQuestLevelSectorNode
{
	int		nSectorID;
	int		nNextLinkIndex;

	// ���⿡ �߰����� ����
};

////////////////////////////////////////////////////////////////////////////////////////////
/// ������ Ŭ���̾�Ʈ ����Ʈ �ְ� ����Ŭ������ ���� �θ� Ŭ����
class CCBaseQuest
{
private:
	bool m_bCreated;
protected:
	
	CCQuestMapCatalogue			m_MapCatalogue;								///< ����Ʈ �� ����
	CCQuestMapCatalogue			m_SurvivalMapCatalogue;						///< �����̹� �� ����
	CCQuestNPCCatalogue			m_NPCCatalogue;								///< NPC ����
	CCQuestDropTable				m_DropTable;								///< ��� ���̺� ����
	virtual bool OnCreate();												///< Create()ȣ��� �Ҹ��� �Լ�
	virtual void OnDestroy();												///< Destroy()ȣ��� �Ҹ��� �Լ�

	void ProcessNPCDropTableMatching();		// npc.xml�� Droptable�� ��Ī��Ų��.
public:
	CCBaseQuest();															///< ������
	virtual ~MBaseQuest();													///< �Ҹ���
	inline CCQuestMapSectorInfo*		GetSectorInfo(int nSectorID);			///< ���� ���� ��ȯ - ����Ʈ
	inline CCQuestMapSectorInfo*		GetSurvivalSectorInfo(int nSectorID);	///< ���� ���� ��ȯ - �����̹�
	inline CCQuestNPCInfo*			GetNPCInfo(MQUEST_NPC nNPC);			///< NPC ���� ��ȯ
	inline CCQuestNPCInfo*			GetNPCPageInfo( int nPage);				///< NPC ���� ��ȯ
	//inline MSurvivalMapInfo*		GetSurvivalMapInfo(MSURVIVAL_MAP nMap);	///< �����̹� ���� �� ���� ��ȯ
	inline CCQuestDropTable*			GetDropTable();							///< ����Ʈ ������ ��� ���̺� ���� ��ȯ
	bool Create();															///< �ʱ�ȭ
	int GetNumOfPage( void)			{ return (int)m_NPCCatalogue.size(); }
	void Destroy();															///< ����

	CCQuestNPCInfo* GetNPCIndexInfo( int nMonsterBibleIndex )
	{
		return m_NPCCatalogue.GetIndexInfo( nMonsterBibleIndex );
	}

	inline CCQuestMapCatalogue* GetMapCatalogue();
	inline CCQuestMapCatalogue* GetSurvivalMapCatalogue();
	inline CCQuestNPCCatalogue* GetNPCCatalogue();
};





// inline functions //////////////////////////////////////////////////////////////////////////
inline CCQuestMapSectorInfo* CCBaseQuest::GetSectorInfo(int nSectorID)
{
	return m_MapCatalogue.GetSectorInfo(nSectorID);
}

inline CCQuestMapSectorInfo* CCBaseQuest::GetSurvivalSectorInfo(int nSectorID)
{
	return m_SurvivalMapCatalogue.GetSectorInfo(nSectorID);
}

inline CCQuestNPCInfo* CCBaseQuest::GetNPCInfo(CCQUEST_NPC nNPC)
{
	return m_NPCCatalogue.GetInfo(nNPC);
}

inline CCQuestNPCInfo* CCBaseQuest::GetNPCPageInfo( int nPage)
{
	return m_NPCCatalogue.GetPageInfo(nPage);
}

//inline MSurvivalMapInfo* MBaseQuest::GetSurvivalMapInfo(MSURVIVAL_MAP nMap)
//{
//	return m_SurvivalMapCatalogue.GetInfo(nMap);
//}

inline CCQuestDropTable* CCBaseQuest::GetDropTable()
{
	return &m_DropTable;
}

inline CCQuestMapCatalogue* CCBaseQuest::GetMapCatalogue()
{
	return &m_MapCatalogue;
}

inline CCQuestMapCatalogue* CCBaseQuest::GetSurvivalMapCatalogue()
{
	return &m_SurvivalMapCatalogue;
}

inline CCQuestNPCCatalogue* CCBaseQuest::GetNPCCatalogue()
{
	return &m_NPCCatalogue;
}
