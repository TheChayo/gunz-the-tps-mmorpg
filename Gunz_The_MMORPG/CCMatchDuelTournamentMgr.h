#pragma once
#include "CCDuelTournamentMatchMaker.h"
#include "CCDuelTournamentTimeChecker.h"

#define MAX_DT_GROUP_RANKING_COUNT 100

class CCDuelTournamentMatchLauncher;

//////////////////////////////////////////////////////////////////////
// Duel Tournament�� ���� Ŭ�����̴�.
// ���� 1. ��� ��ʸ�Ʈ Match-Up�� �����ִ� MatchMaker ���
// ���� 2. ��� ��ʸ�Ʈ�� Group Ranking�� �������ִ� ���(������ �ð����� DB���� �޾ƿ��� ���)
//////////////////////////////////////////////////////////////////////

class CCMatchDuelTournamentMgr
{
	CCMatchObjectContainer m_matchObjectContainer;
	CCDuelTournamentTimeChecker m_TimeChecker;

	DWORD m_lastMatchedTick;		// ���������� ��ġ ó���� �ð�
	DWORD m_nLastTimeCheckedTick;	// ���������� TimeChecker�� ������ �ð�
	bool m_bIsServiceTime;			// ���� �ð��� ���� �α׸� ����� ����

protected:	
	DTRankingInfo m_GroupRankingBlob[MAX_DT_GROUP_RANKING_COUNT];
	CCDuelTournamentMatchMaker m_DTMatchMakers[CCDUELTOURNAMENTTYPE_MAX];
	CCDuelTournamentMatchLauncher* m_pDTMatchLauncher;

	void ClearGroupRanking();
	void LaunchMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup& vecUidPlayer);
public:
	CCMatchDuelTournamentMgr();
	~CCMatchDuelTournamentMgr();

	void Init();
	void Destory();

	bool AddPlayer(CCDUELTOURNAMENTTYPE nType, CCUID &uidPlayer);
	bool RemovePlayer(CCDUELTOURNAMENTTYPE nType, CCUID &uidPlayer);
	
	void Tick(unsigned long nTick);

	void AddGroupRanking(list<DTRankingInfo*>* pRankingList);

	void  SetTimeStamp(const char* szTimeStamp)		{ m_TimeChecker.SetTimeStamp(szTimeStamp); }
	char* GetTimeStamp()							{ return m_TimeChecker.GetTimeStamp(); }
	bool  IsSameTimeStamp(const char* szTimeStamp)  { return m_TimeChecker.IsSameTimeStamp(szTimeStamp); }

	bool GetTimeStampChanged()				{ return m_TimeChecker.GetTimeStampChanged(); }
	void SetTimeStampChanged(bool bValue)	{ m_TimeChecker.SetTimeStampChanged(bValue); }
};


// CCMatchDuelTournamentMgr�� �� Ŭ������ �̿��ؼ� MatchMaker�� �̾Ƴ� �׷��� ������ ����Ų��
class CCDuelTournamentMatchLauncher
{
	DWORD m_dwLimitUserWaitTime;
	DWORD m_dwAcceptableTpGap;

public:
	CCDuelTournamentMatchLauncher() : m_dwLimitUserWaitTime(10000), m_dwAcceptableTpGap(10) {}

	void SetLimitUserWaitTime(DWORD n) { m_dwLimitUserWaitTime = n; }
	void SetAcceptableTpGap(DWORD n) { m_dwAcceptableTpGap = n; }

	void LaunchAvailableMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentMatchMaker& matchMaker, DWORD nCurTick);

protected:
	void LaunchMatchGroups(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup& vecUidPlayer, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);

	// ���� CCMatchServer���� ��� �����Ű�� �κ��� �����ϴ� �Լ� (�����׽�Ʈ ����)
	virtual void LaunchMatch(CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup* pPickedGroup, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor);
};