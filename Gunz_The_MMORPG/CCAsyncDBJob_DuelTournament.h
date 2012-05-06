//////////////////////////////////////////////////////////////////////////////////////////////
// Added By Hong KiJu
#pragma once

#ifndef _MASYNCDBJOB_DUELTOURNAMENT
#define _MASYNCDBJOB_DUELTOURNAMENT

#include "CCAsyncDBJob.h"


class MAsyncDBJob_GetDuelTournamentTimeStamp : public CCAsyncJob
{
protected:
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

public:
	MAsyncDBJob_GetDuelTournamentTimeStamp() : CCAsyncJob(CCAsyncJob_GET_DUELTOURNAMENT_TIMESTAMP, CCUID(0, 0)) {}
	~MAsyncDBJob_GetDuelTournamentTimeStamp(){}


	void Input() {}
	virtual void Run(void* pContext);
	char* GetTimeStamp(){ return m_szTimeStamp; }
};

class MAsyncDBJob_GetDuelTournamentCharInfo : public CCAsyncJob
{
protected:
	DWORD m_dwPlayerCID;

	CCUID m_uidPlayer;
	MMatchObjectDuelTournamentCharInfo m_pDTCharInfo;
public:
	MAsyncDBJob_GetDuelTournamentCharInfo() : CCAsyncJob(CCAsyncJob_GET_DUELTOURNAMENT_CHARINFO, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = CCUID(0, 0);
	}
	~MAsyncDBJob_GetDuelTournamentCharInfo(){}

	void Input(CCUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);

	CCUID GetPlayerUID()									{ return m_uidPlayer; }
	MMatchObjectDuelTournamentCharInfo* GetDTCharInfo()	{ return &m_pDTCharInfo; }	
};

class MAsyncDBJob_GetDuelTournamentPreviousCharInfo : public CCAsyncJob
{
protected:
	CCUID m_uidPlayer;
	DWORD m_dwPlayerCID;	

	int m_nPrevTP;
	int m_nPrevWins;
	int m_nPrevLoses;
	int m_nPrevRanking;
	int m_nPrevFinalWins;
public:
	MAsyncDBJob_GetDuelTournamentPreviousCharInfo() : CCAsyncJob(CCAsyncJob_GET_DUELTOURNAMENT_PREVIOUS_CHARINFO, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = CCUID(0, 0);

		m_nPrevTP = 0;
		m_nPrevWins = 0;
		m_nPrevLoses = 0;
		m_nPrevRanking = 0;
		m_nPrevFinalWins = 0;
	}
	~MAsyncDBJob_GetDuelTournamentPreviousCharInfo(){}


	void Input(CCUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);
	CCUID GetPlayerUID()					{ return m_uidPlayer; }

	int GetPrevTP()						{ return m_nPrevTP; }
	int GetPrevWins()					{ return m_nPrevWins; }
	int GetPrevLoses()					{ return m_nPrevLoses; }
	int GetPrevRanking()				{ return m_nPrevRanking; }
	int GetPrevFinalWins()				{ return m_nPrevFinalWins; }


};

class MAsyncDBJob_UpdateDuelTournamentCharInfo : public CCAsyncJob
{
protected:
	DWORD m_dwPlayerCID;
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];
	MMatchObjectDuelTournamentCharInfo *m_pDTCharInfo;

public:
	MAsyncDBJob_UpdateDuelTournamentCharInfo() : CCAsyncJob(CCAsyncJob_UPDATE_DUELTOURNAMENT_CHARINFO, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_pDTCharInfo = NULL;
		ZeroMemory(m_szTimeStamp, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	~MAsyncDBJob_UpdateDuelTournamentCharInfo(){
		if( m_pDTCharInfo != NULL ) {
			delete m_pDTCharInfo;
		}
	}

	void Input(DWORD dwPlayerCID, char* szTimeStamp, MMatchObjectDuelTournamentCharInfo *pDTCharInfo) {
		m_dwPlayerCID = dwPlayerCID;
		m_pDTCharInfo = pDTCharInfo;

		strcpy(m_szTimeStamp, szTimeStamp);
		m_pDTCharInfo = new MMatchObjectDuelTournamentCharInfo(pDTCharInfo);
	}

	virtual void Run(void* pContext);
};


class MAsyncDBJob_UpdateDuelTournamentGameLog : public CCAsyncJob
{
protected:
	int m_nLogID;
	int m_nChampionCID;
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

public:
	MAsyncDBJob_UpdateDuelTournamentGameLog() : CCAsyncJob(CCAsyncJob_UPDATE_DUELTOURNAMENT_GAMELOG, CCUID(0, 0)) {
		memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	~MAsyncDBJob_UpdateDuelTournamentGameLog(){}


	void Input(char* szTimeStamp, int nLogID, int nChampionCID) {
		m_nLogID = nLogID;
		m_nChampionCID = nChampionCID;
		strcpy(m_szTimeStamp, szTimeStamp);
	}

	virtual void Run(void* pContext);
};


class MAsyncDBJob_InsertDuelTournamentGameLogDetail : public CCAsyncJob
{
protected:
	int m_nLogID;
	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

	MDUELTOURNAMENTROUNDSTATE m_nDTRoundState;
	int m_nPlayTime;
	
	int m_nWinnerCID;
	int m_nLoserCID;
	int m_nGainTP;
	int m_nLoseTP;

public:
	MAsyncDBJob_InsertDuelTournamentGameLogDetail() : CCAsyncJob(CCAsyncJob_INSERT_DUELTOURNAMENT_GAMELOGDETAIL, CCUID(0, 0)) {
		memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	~MAsyncDBJob_InsertDuelTournamentGameLogDetail(){}

	void Input(int nLogID, char* szTimeStamp, MDUELTOURNAMENTROUNDSTATE nDTRoundState, int nWinnerCID, int nLoserCID, int nGainTP, int nLoseTp, int nPlayTime) {
		m_nLogID = nLogID;
		strcpy(m_szTimeStamp, szTimeStamp);

		m_nDTRoundState = nDTRoundState;
		m_nPlayTime = nPlayTime;

		m_nWinnerCID = nWinnerCID;
		m_nLoserCID = nLoserCID;
		m_nGainTP = nGainTP;
		m_nLoseTP = nLoseTp;
	}

	virtual void Run(void* pContext);
};

class MAsyncDBJob_GetDuelTournamentSideRankingInfo : public CCAsyncJob
{
protected:
	DWORD m_dwPlayerCID;

	CCUID m_uidPlayer;
	list<DTRankingInfo*> m_SideRankingList;
public:
	MAsyncDBJob_GetDuelTournamentSideRankingInfo() : CCAsyncJob(CCAsyncJob_GET_DUELTOURNAMENT_SIDERANKING, CCUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = CCUID(0, 0);
	}

	~MAsyncDBJob_GetDuelTournamentSideRankingInfo()
	{
		RemoveListAll();
	}


	void Input(CCUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);

	CCUID GetPlayerUID()							{ return m_uidPlayer; }
	list<DTRankingInfo*>* GetSideRankingList()	{ return &m_SideRankingList; }	

	void RemoveListAll()
	{
		list<DTRankingInfo*>::iterator iter = m_SideRankingList.begin();
		for(; iter != m_SideRankingList.end(); ){
			DTRankingInfo *pInfo = (*iter);
			delete pInfo;

			iter = m_SideRankingList.erase(iter);			
		}
	}
};


class MAsyncDBJob_GetDuelTournamentGroupRankingInfo : public CCAsyncJob
{
protected:
	list<DTRankingInfo*> m_GroupRankingList;

public:
	MAsyncDBJob_GetDuelTournamentGroupRankingInfo() : CCAsyncJob(CCAsyncJob_GET_DUELTOURNAMENT_GROUPRANKING, CCUID(0, 0)) 
	{}

	~MAsyncDBJob_GetDuelTournamentGroupRankingInfo()
	{
		RemoveListAll();
	}


	void Input() {}

	virtual void Run(void* pContext);

	list<DTRankingInfo*>* GetGroupRankingList()	{ return &m_GroupRankingList; }	

	void RemoveListAll()
	{
		list<DTRankingInfo*>::iterator iter = m_GroupRankingList.begin();
		for(; iter != m_GroupRankingList.end(); ){
			DTRankingInfo *pInfo = (*iter);
			iter = m_GroupRankingList.erase(iter);

			delete pInfo;
		}
	}
};
#endif