#pragma once
struct SurvivalRanking
{
	char szCharacterName[MATCHOBJECT_NAME_LENGTH];
	DWORD dwRankPoint;
	DWORD dwRank;

	SurvivalRanking() : dwRank(0), dwRankPoint(0) { szCharacterName[0]=0; }
};

class CCSurvivalRankInfo
{
	// �ó������� ���� ��ŷ ����Ʈ
	SurvivalRanking m_arRanking[MAX_SURVIVAL_SCENARIO_COUNT][MAX_SURVIVAL_RANKING_LIST];

public:
	CCSurvivalRankInfo();
	~CCSurvivalRankInfo();

	const SurvivalRanking* GetRanking(DWORD scenarioIndex, DWORD rankIndex) const;
	bool SetRanking(DWORD scenarioIndex, DWORD rankArrayIndex, DWORD realRank, const char* szCharName, DWORD rankPoint);

	void ClearRanking();
	
	void FillDummyRankingListForDebug();
};
