#pragma once
#include "CCMatchGlobal.h"

#define LADDER_STATISTICS_LEVEL_UNIT			5		// 레벨은 5레벨을 한단위로 통계 계산
#define LADDER_STATISTICS_CLANPOINT_UNIT		20		// 클랜포인트는 20을 한단위로 통계 계산
#define LADDER_STATISTICS_CONTPOINT_UNIT		50		// 클랜기여도는 50을 한단위로 통계 계산


#define MAX_LADDER_STATISTICS_LEVEL				20		// (99 / 5)
#define MAX_LADDER_STATISTICS_CLANPOINT			200		// (4000 / 20)
#define MAX_LADDER_STATISTICS_CONTPOINT			200		// (10000 / 50)


class CCLadderStatistics
{
private:
	struct _RECORD
	{
		unsigned long int nCount;
		unsigned long int nWinCount;
	};

	_RECORD				m_LevelVictoriesRates[MAX_LADDER_STATISTICS_LEVEL];
	_RECORD				m_ClanPointVictoriesRates[MAX_LADDER_STATISTICS_CLANPOINT];
	_RECORD				m_ContPointVictoriesRates[MAX_LADDER_STATISTICS_CONTPOINT];

	unsigned long		m_nLastTick;
	void Load();
	void Save();
	unsigned long GetLastTick()	{ return m_nLastTick; }
	void SetLastTick(unsigned long nTick)	{ m_nLastTick = nTick; }

	void _InsertLevelRecord(int nLevelDiff, bool bMoreLevelWin);
	void _InsertClanPointRecord(int nClanPointDiff, bool bMorePointWin);
	void _InsertContPointRecord(int nContPointDiff, bool bMorePointWin);
public:
	CCLadderStatistics();
	virtual ~CCLadderStatistics();
	void Init();
	void Tick(unsigned long nTick);


	float GetLevelVictoriesRate(int nLevelDiff);
	float GetClanPointVictoriesRate(int nClanPointDiff);
	float GetContPointVictoriesRate(int nContPointDiff);


	void InsertLevelRecord(int nRedTeamCharLevel, int nBlueTeamCharLevel, CCMatchTeam nWinnerTeam);
	void InsertClanPointRecord(int nRedTeamClanPoint, int nBlueTeamClanPoint, CCMatchTeam nWinnerTeam);
	void InsertContPointRecord(int nRedTeamContPoint, int nBlueTeamContPoint, CCMatchTeam nWinnerTeam);

	void PrintDebug();
};