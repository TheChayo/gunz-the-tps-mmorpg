#include "CCBaseQuest.h"
#include "CCSurvivalRankInfo.h"

/// ������ ����Ʈ �ְ� ���� Ŭ����
class CCMatchQuest : public CCBaseQuest
{
protected:
	virtual bool OnCreate();				///< �ʱ�ȭ
	virtual void OnDestroy();				///< ����
	

	CCQuestNPCSetCatalogue		m_NPCSetCatalogue;						///< NPC Set ����
	CCQuestScenarioCatalogue		m_ScenarioCatalogue;					///< ����Ʈ �ó����� ����
	CCQuestScenarioCatalogue		m_SurvivalScenarioCatalogue;			///< �����̹� �ó����� ����
	CCSurvivalRankInfo			m_SurvivalRankInfo;						///< �����̹� ������ ��ŷ ���

public:
	CCMatchQuest();														///< ������
	virtual ~CCMatchQuest();												///< �Ҹ���

	inline CCQuestNPCSetInfo* GetNPCSetInfo(int nID);					///< NPC Set ���� ��ȯ
	inline CCQuestNPCSetInfo* GetNPCSetInfo(const char* szName);			///< NPC Set ���� ��ȯ
	inline CCQuestScenarioCatalogue* GetScenarioCatalogue();				///< ����Ʈ �ó����� ���� ��ȯ
	inline CCQuestScenarioInfo*		GetScenarioInfo(int nScenarioID);	///< ����Ʈ �ó����� ���� ��ȯ
	inline CCQuestScenarioCatalogue* GetSurvivalScenarioCatalogue();				///< �����̹� �ó����� ���� ��ȯ
	inline CCQuestScenarioInfo*		GetSurvivalScenarioInfo(int nScenarioID);	///< �����̹� �ó����� ���� ��ȯ
	inline CCSurvivalRankInfo*		GetSurvivalRankInfo();						///< �����̹� ��ŷ ��� ��ȯ

};




inline CCQuestNPCSetInfo* CCMatchQuest::GetNPCSetInfo(int nID)
{
	return m_NPCSetCatalogue.GetInfo(nID);
}

inline CCQuestNPCSetInfo* CCMatchQuest::GetNPCSetInfo(const char* szName)
{
	return m_NPCSetCatalogue.GetInfo(szName);
}

inline CCQuestScenarioCatalogue* CCMatchQuest::GetScenarioCatalogue()
{
	return &m_ScenarioCatalogue;
}

inline CCQuestScenarioInfo* CCMatchQuest::GetScenarioInfo(int nScenarioID)
{
	return m_ScenarioCatalogue.GetInfo(nScenarioID);
}

inline CCQuestScenarioCatalogue* CCMatchQuest::GetSurvivalScenarioCatalogue()
{
	return &m_SurvivalScenarioCatalogue;
}

inline CCQuestScenarioInfo* CCMatchQuest::GetSurvivalScenarioInfo(int nScenarioID)
{
	return m_SurvivalScenarioCatalogue.GetInfo(nScenarioID);
}

inline CCSurvivalRankInfo* CCMatchQuest::GetSurvivalRankInfo()
{
	return &m_SurvivalRankInfo;
}
