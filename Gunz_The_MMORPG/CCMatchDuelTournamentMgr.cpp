#include "stdafx.h"
#include "CCMatchGlobal.h"
#include "CCMatchDuelTournamentMgr.h"
#include "CCMatchConfig.h"

CCMatchDuelTournamentMgr::CCMatchDuelTournamentMgr()
: m_lastMatchedTick(0)
, m_pDTMatchLauncher(new CCDuelTournamentMatchLauncher)
, m_nLastTimeCheckedTick(0)
, m_bIsServiceTime(false)
{
	for (int i=0; i<CCDUELTOURNAMENTTYPE_MAX; ++i)
		m_DTMatchMakers[i].SetMatchObjectContainer(&m_matchObjectContainer);
}

CCMatchDuelTournamentMgr::~CCMatchDuelTournamentMgr()
{
	delete m_pDTMatchLauncher;
}

void CCMatchDuelTournamentMgr::Init()
{
	ClearGroupRanking();

	const DWORD DT_MATCH_WAIT_TIME_LIMIT = MGetServerConfig()->GetDuelTournamentMatchMakingWaitLimit();
	const DWORD DT_MATCH_ACCEPTABLE_TP_GAP = MGetServerConfig()->GetDuelTournamentMatchMakingAcceptableTpGap();

	m_pDTMatchLauncher->SetLimitUserWaitTime(DT_MATCH_WAIT_TIME_LIMIT);
	m_pDTMatchLauncher->SetAcceptableTpGap(DT_MATCH_ACCEPTABLE_TP_GAP);
}

void CCMatchDuelTournamentMgr::Destory()
{
}

void CCMatchDuelTournamentMgr::AddGroupRanking(list<DTRankingInfo*>* pRankingList)
{
	ClearGroupRanking();

	list<DTRankingInfo*>::iterator iter = pRankingList->begin();
	for(; iter != pRankingList->end(); ++iter) {
		DTRankingInfo* pInfo = (*iter);
		memcpy(&m_GroupRankingBlob[pInfo->m_nRanking-1], pInfo, sizeof(DTRankingInfo));
	}
}

void CCMatchDuelTournamentMgr::ClearGroupRanking()
{
	ZeroMemory(m_GroupRankingBlob, sizeof(DTRankingInfo) * MAX_DT_GROUP_RANKING_COUNT);
}

bool CCMatchDuelTournamentMgr::AddPlayer(CCDUELTOURNAMENTTYPE nType, CCUID &uidPlayer)
{
	if (0 <= nType && nType < CCDUELTOURNAMENTTYPE_MAX)
	{
		CCMatchObjectDuelTournamentCharInfo* pDTCharInfo = m_matchObjectContainer.GetDuelTournamentCharInfo(uidPlayer);
		if (pDTCharInfo) {
			m_DTMatchMakers[nType].AddPlayer(uidPlayer, pDTCharInfo->GetTP(), CCMatchServer::GetInstance()->GetGlobalClockCount());
			return true;
		}
	}
	return false;
}

bool CCMatchDuelTournamentMgr::RemovePlayer(CCDUELTOURNAMENTTYPE nType, CCUID &uidPlayer)
{
	if (0 <= nType && nType < CCDUELTOURNAMENTTYPE_MAX)
	{
		return m_DTMatchMakers[nType].RemovePlayer(uidPlayer);	
	}
	return false;
}

void CCMatchDuelTournamentMgr::Tick(unsigned long nCurTick)
{
	const DWORD DT_MATCH_TICK_INTERVAL = MGetServerConfig()->GetDuelTournamentMatchMakingInterval();

	if (nCurTick - m_lastMatchedTick > DT_MATCH_TICK_INTERVAL)
	{
		m_lastMatchedTick = nCurTick;

		SYSTEMTIME stCurTime;
		GetLocalTime( &stCurTime );
		int nStartTime = MGetServerConfig()->GetDuelTournamentServiceStartTime();
		int nEndTime = MGetServerConfig()->GetDuelTournamentServiceEndTime();

		bool bServiceTime = false;
		if (nStartTime > nEndTime)	// �� �ð��� ������ �Ѿ�� ���
		{
			if (nStartTime <= stCurTime.wHour ||	// ���۽ð�~���� ������ �����̰ų�
				stCurTime.wHour <= nEndTime)		// ����~���ð� ������ ������ ��� ���� ����
				bServiceTime = true;
		}
		else // �׳� �����ؼ� �׳� ������ ���
		{
			if ( nStartTime <= stCurTime.wHour && stCurTime.wHour <= nEndTime )
				bServiceTime = true;
		}

		if ( bServiceTime)
		{ // �÷��� ���ɽð� ex)0~23�̸� 24�ð� ��� �����Ѵ�.
			for (int nType=0; nType<CCDUELTOURNAMENTTYPE_MAX; ++nType)
			{
				m_pDTMatchLauncher->LaunchAvailableMatch(CCDUELTOURNAMENTTYPE(nType), m_DTMatchMakers[nType], nCurTick);
			}
		}
		else
		{
			for (int nType=0; nType<CCDUELTOURNAMENTTYPE_MAX; ++nType)
			{
				m_DTMatchMakers[nType].ServiceTimeClose();
			}
		}

		if(m_bIsServiceTime != bServiceTime)
		{
			m_bIsServiceTime = bServiceTime;
			if(m_bIsServiceTime)
				cclog("Dueltournament Service Open(%d-%d-%d Hour%d, Min%d) \n", stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay, stCurTime.wHour, stCurTime.wMinute);
			else
				cclog("Dueltournament Service Close(%d-%d-%d Hour%d, Min%d) \n", stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay, stCurTime.wHour, stCurTime.wMinute);
		}

	}

	if( nCurTick - m_nLastTimeCheckedTick > 1000 ) {
		m_nLastTimeCheckedTick = nCurTick;
		m_TimeChecker.Tick(nCurTick);
	}	
}
//=======================================================================================================================


void CCDuelTournamentMatchLauncher::LaunchAvailableMatch( CCDUELTOURNAMENTTYPE nType, CCDuelTournamentMatchMaker& matchMaker, DWORD nCurTick )
{
	CCDuelTournamentPickedGroup vecUidPlayer;

	int nPlayerCount = GetDTPlayerCount(CCDUELTOURNAMENTTYPE(nType));

	matchMaker.CleanDisabledUid();

	// ���� ��ٸ� ������ �켱������ ��ġ���ش�
	int numPlayer = matchMaker.GetNumPlayer();
	int nSafetyNet = 0;
	while (nSafetyNet++ < numPlayer) // <= while(1)�̾ �������� ������ ��� ���ѷ����� ������ �͸��� ���ϰ�����
	{
		const CCDuelTournamentMatchMaker::DTUser* pDTUser = matchMaker.FindLongWaitPlayer(m_dwLimitUserWaitTime, nCurTick);
		if (NULL == pDTUser)
			break;

		if (false == matchMaker.PickGroupForPlayerAndRemove(vecUidPlayer,nPlayerCount, *pDTUser))
			break;

		LaunchMatchGroups( CCDUELTOURNAMENTTYPE(nType), vecUidPlayer, CCDUELTOURNAMENTMATCHMAKINGFACTOR_OVERWAIT);
	}

	// TP�� ����� �������� ��ġ���ش�
	matchMaker.PickMatchableGroupsAndRemove(vecUidPlayer, nPlayerCount, m_dwAcceptableTpGap);
	if (!vecUidPlayer.empty())
		LaunchMatchGroups( CCDUELTOURNAMENTTYPE(nType), vecUidPlayer, CCDUELTOURNAMENTMATCHMAKINGFACTOR_TPGAP);
}

void CCDuelTournamentMatchLauncher::LaunchMatchGroups( CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup& vecUidPlayer, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor )
{
	CCDuelTournamentPickedGroup SmallPickedGroup;

	int nIndex = 0;
	int nPlayerCount = GetDTPlayerCount(nType);
	if (nPlayerCount == 0) {
		_ASSERT(0);
		return;	// 0���� ������ ����
	}

	for( CCDuelTournamentPickedGroup::iterator iter = vecUidPlayer.begin(); iter != vecUidPlayer.end(); ++iter)
	{
		nIndex++;

		SmallPickedGroup.push_back(*iter);

		if( nIndex % nPlayerCount == 0 ) {
			LaunchMatch(nType, &SmallPickedGroup, matchFactor);
			SmallPickedGroup.clear();
		}
	}
}

void CCDuelTournamentMatchLauncher::LaunchMatch( CCDUELTOURNAMENTTYPE nType, CCDuelTournamentPickedGroup* pPickedGroup, CCDUELTOURNAMENTMATCHMAKINGFACTOR matchFactor )
{
	pPickedGroup->Shuffle();
	CCMatchServer* pServer = CCMatchServer::GetInstance();
	pServer->LaunchDuelTournamentMatch(nType, pPickedGroup, matchFactor);
}