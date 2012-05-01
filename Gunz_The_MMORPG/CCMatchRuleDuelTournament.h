#pragma once

#include "CCMatchRule.h"
#include "CCDuelTournamentFormula.h"
#include <list>

#define DUELTOURNAMENT_PLAYER_UNEARNEDPOINT			0
#define DUELTOURNAMENT_PLAYER_LEAVE_PENALTYPOINT	10

using namespace std;

struct CCDuelTournamentGameInfo
{
	CCDUELTOURNAMENTTYPE		nType;
	int						nGameNumber;
	char					szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];
};

struct CCDuelTournamentMatchRecord
{
	CCUID uidWinner, uidLoser;
	int nCID1, nCID2;
	int nTP1, nTP2;
	int nGainTP, nLoseTP;
	int nRoundCount;
	int nPlayer1WinCount, nPlayer2WinCount;
	bool bIsPlayer1Win, bIsPlayer2Win;

	bool bMatchFinish;

	DWORD nStartTime;
	DWORD nFinishTime;
};

struct CCDuelTournamentRoundRecord
{
	CCUID uidWinnerPlayer;
	CCUID uidLoserPlayer;

	int fPlayer1DamagedPoint, fPlayer1HealthPoint, fPlayer1AmorPoint;
	int fPlayer2DamagedPoint, fPlayer2HealthPoint, fPlayer2AmorPoint;

	bool bPlayer1Win;
	bool bPlayer2Win;

	bool bIsTimeOut;

	DWORD nFinishTime;
};

class CCDuelTournamentPlayerInfo
{	
public:
	CCUID uidPlayer;
	int  nCID, nChangeTP;
	bool bLoser, bOutGame, bInMatch;
};

class CCDuelTournamentPlayerMap : public map <CCUID, CCDuelTournamentPlayerInfo*>
{
public:
	~CCDuelTournamentPlayerMap() {
		for(CCDuelTournamentPlayerMap::iterator i=begin() ; i != end(); ++i){delete (*i).second;}
		clear();
	}

	CCDuelTournamentPlayerInfo* GetPlayerInfo(CCUID uidPlayer){
		if( find(uidPlayer) != end() ) return find(uidPlayer)->second;
		return NULL;
	}

	void InsertPlayerInfo(CCUID uidPlayer, CCDuelTournamentPlayerInfo *pInfo) {
		insert(pair<CCUID, CCDuelTournamentPlayerInfo*>(uidPlayer, pInfo));
	}

	bool EnterMatch(CCUID uidPlayer){ 
		CCDuelTournamentPlayerInfo* pInfo = GetPlayerInfo(uidPlayer);
		if( pInfo != NULL ){ pInfo->bInMatch = true; return true;}
		return false;
	}
	
	void LeaveMatch(CCUID uidPlayer){
		CCDuelTournamentPlayerInfo* pInfo = GetPlayerInfo(uidPlayer);
		if( pInfo != NULL ) pInfo->bInMatch = false;
	}

	void SetLoser(CCUID uidPlayer){
		CCDuelTournamentPlayerInfo* pInfo = GetPlayerInfo(uidPlayer);
		if( pInfo != NULL ) pInfo->bLoser = true;
	}

	bool IsLoser(CCUID uidPlayer){
		CCDuelTournamentPlayerInfo* pInfo = GetPlayerInfo(uidPlayer);
		if( pInfo != NULL ) return pInfo->bLoser;
		return false;
	}

	void SetOutStage(CCUID uidPlayer){
		CCDuelTournamentPlayerInfo* pInfo = GetPlayerInfo(uidPlayer);
		if( pInfo != NULL ) pInfo->bOutGame = true;
	}

	bool IsOutUser(CCUID uidPlayer) {
		CCDuelTournamentPlayerInfo* pInfo = GetPlayerInfo(uidPlayer);
		if( pInfo == NULL ) return true;
		return pInfo->bOutGame;
	}
};

class CCMatchRuleDuelTournament : public CCMatchRule {
private:
	CCDuelTournamentGameInfo					m_GameInfo;
	CCMatchDuelTournamentMatch				m_CurrentMatchInfo;	
	map<int, CCMatchDuelTournamentMatch*>*	m_DuelTournamentMatchMap;

	CCDuelTournamentPlayerMap m_DTPlayerMap;

	CCDuelTournamentMatchRecord m_MatchRecord;
	CCDuelTournamentRoundRecord m_RoundRecord;
	
	CCDuelTournamentFormula m_DuelTournamentCalculator;

	bool					m_bIsRoundFinish;				///< ���� �����°�
	bool					m_bIsRoundTimeOut;
	bool					m_bIsCorrectFinish;	
	
	enum DTLogLevel { DTLOG_DEBUG = 1, DTLOG_RELEASE = 2,};

protected:	
	virtual void OnBegin();											///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();											///< ��ü ���� ����� ȣ��
	
	virtual void OnRoundBegin();									///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();										///< ���� ���� �� ȣ��
	bool OnRoundEnd_PlayerOut();
	void OnRoundEnd_TimeOut();

	virtual bool OnRun();

	virtual void OnLeaveBattle(CCUID& uidChar);									///< ������ �������� ȣ��ȴ�.
	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);	///< ų������ �������� ų���� è�Ǿ��� ų���� üũ	

	virtual bool RoundCount(){
		if( m_MatchRecord.bMatchFinish == true && m_CurrentMatchInfo.nNextMatchNumber == 0 ) return false;
		return true;
	}

	virtual bool OnCheckEnableBattleCondition(){
		CCMatchObject* pObj;

		if (m_pStage == NULL) {
			SetRoundFinish(true);
			return false;
		}

		if (m_CurrentMatchInfo.uidPlayer1 == CCUID(0, 0) || m_CurrentMatchInfo.uidPlayer2 == CCUID(0, 0)) {
			SetRoundFinish(true);
		} else if( (pObj = m_pStage->GetObj(m_CurrentMatchInfo.uidPlayer1)) == NULL ) {
			OnLeaveBattle(m_CurrentMatchInfo.uidPlayer1);
		} else if( (pObj = m_pStage->GetObj(m_CurrentMatchInfo.uidPlayer2)) == NULL ) {
			OnLeaveBattle(m_CurrentMatchInfo.uidPlayer2);
		}

		return true;
	}

	virtual bool OnCheckRoundFinish() { return IsRoundFinish();}
	virtual void OnCommand(CCCommand* pCommand);

	void OnPreCountDown();

	void SetDTRoundState(CCMATCH_ROUNDSTATE nState);

	void InitMatchRecord();
	void InitRoundRecord();

	void SpawnPlayers();										///< �÷��̾���� ������Ų��.	
	void PrepareDuelTournamentMatchRecord();	
	
	void OnMatchFinish(bool bIsPlayer1Win);
	void OnMatchNotFinish();

	void MakeNextMatch();
	void RecordGameResult();

	void UpdateDuelTournamentPlayerInfo(CCUID uidPlayer, CCDUELTOURNAMENTROUNDSTATE nState, bool bIsWinner, int nChangeTP, bool bIsLeaveUser = false);
	void InsertDuelTournamentGameLogDeatil(CCDUELTOURNAMENTROUNDSTATE nDTRoundState, int nWinnerCID, int nLoserCID, int nGainTP, int nLoseTP, int nPlayTime);

	void SendDuelTournamentGameInfo(bool bIsRoundEnd = false);
	void SendDuelTournamentNextGamePlayerInfo();
	void SendDuelTournamentRoundResultInfo(bool bIsMatchFinish, bool bIsRoundDraw);
	void SendDuelTournamentMatchResultInfo();
	
	bool IsRoundFinish()							{ return m_bIsRoundFinish; }
	void SetRoundFinish(bool bValue)				{ m_bIsRoundFinish = bValue; }

	bool IsRoundTimeOut()							{ return m_bIsRoundTimeOut; }
	void SetRoundTimeOut(bool bValue)				{ m_bIsRoundTimeOut = bValue; }

	bool IsCorrectFinish()							{ return m_bIsCorrectFinish; }
	void SetCorrectFinish(bool bValue)				{ m_bIsCorrectFinish = bValue; }
	
	void DTLog(DTLogLevel nLevel, const char *pFormat,...){
		if( nLevel == DTLOG_DEBUG ) {
#ifdef _DUELTOURNAMENT_LOG_ENABLE_	
			va_list args;
			static char temp[1024];

			va_start(args, pFormat);
			vsprintf(temp, pFormat, args);

			CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, "DT_LOG(%d) - %s", m_GameInfo.nGameNumber, temp);
#endif
		} else if( nLevel == DTLOG_RELEASE ) {
			va_list args;
			static char temp[1024];

			va_start(args, pFormat);
			vsprintf(temp, pFormat, args);

			CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, "DT_LOG(%d) - %s", m_GameInfo.nGameNumber, temp);
		}
	}

	void LogInfo() {
#ifdef _DUELTOURNAMENT_LOG_ENABLE_
		if (m_pStage == NULL) return;	

		CCMatchObject* pObj1, *pObj2;
		pObj1 = m_pStage->GetObj(m_CurrentMatchInfo.uidPlayer1);
		pObj2 = m_pStage->GetObj(m_CurrentMatchInfo.uidPlayer2);

		int nIndex = 0;
		char szTemp[1024] = {0, };	
		for (CCDuelTournamentPlayerMap::iterator i = m_DTPlayerMap.begin(); i!= m_DTPlayerMap.end();  i++)
		{
			CCDuelTournamentPlayerInfo* pInfo = (*i).second;
			if( pInfo->bOutGame == false && pInfo->bInMatch == false ) {
				CCMatchObject* pObj = m_pStage->GetObj((*i).first);

				if( pObj != NULL ){
					nIndex += sprintf(&szTemp[nIndex], "Player(%d%d) - %s, ", pObj->GetUID().High, pObj->GetUID().Low, pObj->GetName());
				}
			}			
		}

		DTLog(DTLOG_DEBUG, "����Logging DuelTournament Info������������������������������������������������");
		DTLog(DTLOG_DEBUG, "��MatchNumber : %d, RoundCount : %d", m_CurrentMatchInfo.nMatchNumber, m_MatchRecord.nRoundCount);

		if (pObj1 != NULL) DTLog(DTLOG_DEBUG, "��Player1 name : %s, WinCount = %d", pObj1->GetName(), m_MatchRecord.nPlayer1WinCount);
		if (pObj2 != NULL) DTLog(DTLOG_DEBUG, "��Player2 name : %s, WinCount = %d", pObj2->GetName(), m_MatchRecord.nPlayer2WinCount);

		DTLog(DTLOG_DEBUG, "��Wait Queue Info : %s", szTemp);
		DTLog(DTLOG_DEBUG, "��������������������������������������������������������������������������������");
#endif
	}

	
public:
	CCMatchRuleDuelTournament(CCMatchStage* pStage);
	virtual ~CCMatchRuleDuelTournament(){}

	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_DUELTOURNAMENT; }
};