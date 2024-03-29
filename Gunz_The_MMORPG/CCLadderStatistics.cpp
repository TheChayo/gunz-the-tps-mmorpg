#include "stdafx.h"
#include "CCLadderStatistics.h"
#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include <math.h>

#define FILENAME_LADDER_STATISTICS			"ladder_stat.dat"

CCLadderStatistics::CCLadderStatistics()
{
	
}

CCLadderStatistics::~CCLadderStatistics()
{

}

void CCLadderStatistics::Init()
{
	memset(m_LevelVictoriesRates,		0, sizeof(_RECORD) * MAX_LADDER_STATISTICS_LEVEL);
	memset(m_ClanPointVictoriesRates,	0, sizeof(_RECORD) * MAX_LADDER_STATISTICS_CLANPOINT);
	memset(m_ContPointVictoriesRates,	0, sizeof(_RECORD) * MAX_LADDER_STATISTICS_CONTPOINT);

	m_nLastTick = 0;

	Load();
}

void CCLadderStatistics::Load()
{
	FILE* fp = fopen(FILENAME_LADDER_STATISTICS, "rb");
	if (fp == NULL) return;
	size_t numread;

	numread = fread(m_LevelVictoriesRates, sizeof(_RECORD), MAX_LADDER_STATISTICS_LEVEL, fp);
	if (numread == 0)
	{
		fclose(fp);
		return;
	}

	fread(m_ClanPointVictoriesRates, sizeof(_RECORD), MAX_LADDER_STATISTICS_CLANPOINT, fp);
	fread(m_ContPointVictoriesRates, sizeof(_RECORD), MAX_LADDER_STATISTICS_CONTPOINT, fp);

	fclose(fp);
}

void CCLadderStatistics::Save()
{
	FILE* fp = fopen(FILENAME_LADDER_STATISTICS, "wb");
	if (fp == NULL) return;


	fwrite(m_LevelVictoriesRates,		sizeof(_RECORD), MAX_LADDER_STATISTICS_LEVEL, fp);
	fwrite(m_ClanPointVictoriesRates,	sizeof(_RECORD), MAX_LADDER_STATISTICS_CLANPOINT, fp);
	fwrite(m_ContPointVictoriesRates,	sizeof(_RECORD), MAX_LADDER_STATISTICS_CONTPOINT, fp);

	fclose(fp);
}


#define MTIME_LADDER_STAT_SAVE_TICK		3600000			// (1000 * 60 * 60)	1�ð�


void CCLadderStatistics::Tick(unsigned long nTick)
{
	if (nTick - GetLastTick() < MTIME_LADDER_STAT_SAVE_TICK)
		return;
	else
		SetLastTick(nTick);


	Save();
}

float CCLadderStatistics::GetLevelVictoriesRate(int nLevelDiff)
{
	int nIndex = nLevelDiff / LADDER_STATISTICS_LEVEL_UNIT;

	if (nIndex < 0) nIndex = 0;
	if (nIndex >= MAX_LADDER_STATISTICS_LEVEL) nIndex = MAX_LADDER_STATISTICS_LEVEL-1;

	if (m_LevelVictoriesRates[nIndex].nCount == 0) return 0.0f;

	return (float(m_LevelVictoriesRates[nIndex].nWinCount) / float(m_LevelVictoriesRates[nIndex].nCount));
}

float CCLadderStatistics::GetClanPointVictoriesRate(int nClanPointDiff)
{
	int nIndex = nClanPointDiff / LADDER_STATISTICS_CLANPOINT_UNIT;

	if (nIndex < 0) nIndex = 0;
	if (nIndex >= MAX_LADDER_STATISTICS_CLANPOINT) nIndex = MAX_LADDER_STATISTICS_CLANPOINT-1;

	if (m_ClanPointVictoriesRates[nIndex].nCount == 0) return 0.0f;

	return (float(m_ClanPointVictoriesRates[nIndex].nWinCount) / float(m_ClanPointVictoriesRates[nIndex].nCount));
}

float CCLadderStatistics::GetContPointVictoriesRate(int nContPointDiff)
{
	int nIndex = nContPointDiff / LADDER_STATISTICS_CONTPOINT_UNIT;

	if (nIndex < 0) nIndex = 0;
	if (nIndex >= MAX_LADDER_STATISTICS_CONTPOINT) nIndex = MAX_LADDER_STATISTICS_CONTPOINT-1;

	if (m_ContPointVictoriesRates[nIndex].nCount == 0) return 0.0f;

	return (float(m_ContPointVictoriesRates[nIndex].nWinCount) / float(m_ContPointVictoriesRates[nIndex].nCount));
}

void CCLadderStatistics::_InsertLevelRecord(int nLevelDiff, bool bMoreLevelWin)
{
	int nID = nLevelDiff / LADDER_STATISTICS_LEVEL_UNIT;
	if (nID >= MAX_LADDER_STATISTICS_LEVEL) nID = MAX_LADDER_STATISTICS_LEVEL-1;
	if (m_LevelVictoriesRates[nID].nCount >= INT_MAX) return;

	if (bMoreLevelWin) m_LevelVictoriesRates[nID].nWinCount++;
	m_LevelVictoriesRates[nID].nCount++;
}

void CCLadderStatistics::_InsertClanPointRecord(int nClanPointDiff, bool bMorePointWin)
{
	int nID = nClanPointDiff / LADDER_STATISTICS_CLANPOINT_UNIT;
	if (nID >= MAX_LADDER_STATISTICS_CLANPOINT) nID = MAX_LADDER_STATISTICS_CLANPOINT-1;
	if (m_ClanPointVictoriesRates[nID].nCount >= INT_MAX) return;

	if (bMorePointWin) m_ClanPointVictoriesRates[nID].nWinCount++;
	m_ClanPointVictoriesRates[nID].nCount++;
}

void CCLadderStatistics::_InsertContPointRecord(int nContPointDiff, bool bMorePointWin)
{
	int nID = nContPointDiff / LADDER_STATISTICS_CONTPOINT_UNIT;
	if (nID >= MAX_LADDER_STATISTICS_CONTPOINT) nID = MAX_LADDER_STATISTICS_CONTPOINT-1;
	if (m_ContPointVictoriesRates[nID].nCount >= INT_MAX) return;

	if (bMorePointWin) m_ContPointVictoriesRates[nID].nWinCount++;
	m_ContPointVictoriesRates[nID].nCount++;
}


void CCLadderStatistics::InsertLevelRecord(int nRedTeamCharLevel, int nBlueTeamCharLevel, CCMatchTeam nWinnerTeam)
{
	int nLevelDiff = abs(nRedTeamCharLevel - nBlueTeamCharLevel);
	bool bMoreLevelWin = false;

	if ((nWinnerTeam == CCMT_RED) && (nRedTeamCharLevel >= nBlueTeamCharLevel)) bMoreLevelWin = true;
	else if ((nWinnerTeam == CCMT_BLUE) && (nBlueTeamCharLevel >= nRedTeamCharLevel)) bMoreLevelWin = true;
	else bMoreLevelWin = false;

	_InsertLevelRecord(nLevelDiff, bMoreLevelWin);

}

void CCLadderStatistics::InsertClanPointRecord(int nRedTeamClanPoint, int nBlueTeamClanPoint, CCMatchTeam nWinnerTeam)
{
	int nClanPointDiff = abs(nRedTeamClanPoint - nBlueTeamClanPoint);
	bool bMorePointWin = false;

	if ((nWinnerTeam == CCMT_RED) && (nRedTeamClanPoint >= nBlueTeamClanPoint)) bMorePointWin = true;
	else if ((nWinnerTeam == CCMT_BLUE) && (nBlueTeamClanPoint >= nRedTeamClanPoint)) bMorePointWin = true;
	else bMorePointWin = false;

	_InsertClanPointRecord(nClanPointDiff, bMorePointWin);
}

void CCLadderStatistics::InsertContPointRecord(int nRedTeamContPoint, int nBlueTeamContPoint, CCMatchTeam nWinnerTeam)
{
	int nContPointDiff = abs(nRedTeamContPoint - nBlueTeamContPoint);
	bool bMorePointWin = false;

	if ((nWinnerTeam == CCMT_RED) && (nRedTeamContPoint >= nBlueTeamContPoint)) bMorePointWin = true;
	else if ((nWinnerTeam == CCMT_BLUE) && (nBlueTeamContPoint >= nRedTeamContPoint)) bMorePointWin = true;
	else bMorePointWin = false;

	_InsertContPointRecord(nContPointDiff, bMorePointWin);
}


#include "stdlib.h"
#include "CCDebug.h"
void CCLadderStatistics::PrintDebug()
{
	for (int i = 0; i < MAX_LADDER_STATISTICS_LEVEL*LADDER_STATISTICS_LEVEL_UNIT; i++)
	{
		int nID = i / LADDER_STATISTICS_LEVEL_UNIT;
		float fRate = GetLevelVictoriesRate(i);

		cclog("LevelDiff %2d rate = %.5f, %d Counts, %d Wins \n", i, fRate, m_LevelVictoriesRates[nID].nCount,
			m_LevelVictoriesRates[nID].nWinCount);
	}

	cclog("=====================\n\n");
	for (int i = 0; i < MAX_LADDER_STATISTICS_CLANPOINT*LADDER_STATISTICS_CLANPOINT_UNIT; i++)
	{
		int nID = i / LADDER_STATISTICS_CLANPOINT_UNIT;
		float fRate = GetClanPointVictoriesRate(i);

		cclog("ClanPointDiff %2d rate = %.5f, %d Counts, %d Wins \n", i, fRate, m_ClanPointVictoriesRates[nID].nCount,
			m_ClanPointVictoriesRates[nID].nWinCount);
	}

	cclog("=====================\n\n");
	for (int i = 0; i < MAX_LADDER_STATISTICS_CONTPOINT*LADDER_STATISTICS_CONTPOINT_UNIT; i++)
	{
		int nID = i / LADDER_STATISTICS_CONTPOINT_UNIT;
		float fRate = GetContPointVictoriesRate(i);

		cclog("ContPointDiff %2d rate = %.5f, %d Counts, %d Wins \n", i, fRate, m_ContPointVictoriesRates[nID].nCount,
			m_ContPointVictoriesRates[nID].nWinCount);
	}

}