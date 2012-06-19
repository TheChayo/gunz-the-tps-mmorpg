#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCMatchRule.h"
#include "CCMatchStage.h"
#include "CCMatchObject.h"
#include "CCSharedCommandTable.h"
#include "CCBlobArray.h"
#include "CCMatchConfig.h"
#include "CCMatchEventFactory.h"



void CCMatchRule::DebugTest()
{
#ifdef _DEBUG
	SetRoundState(CCMATCH_ROUNDSTATE_FINISH);
#endif
}

//// RULE //////////////////////////////////////////////////////////////////////
CCMatchRule::CCMatchRule(CCMatchStage* pStage)	: m_pStage(pStage), m_nRoundCount(0), m_nRoundArg(0),
				/*m_pGameTypeInfo(NULL),*/ m_nRoundState(CCMATCH_ROUNDSTATE_FREE)
{ 
	SetRoundStateTimer(CCMatchServer::GetInstance()->GetGlobalClockCount());
}

void CCMatchRule::SetRoundState(CCMATCH_ROUNDSTATE nState)
{ 
	if (m_nRoundState == nState) return;

/*#ifdef _DEBUG

	{
		char szTemp[256]="";
		switch (nState)
		{
		case CCMATCH_ROUNDSTATE_PREPARE:		sprintf(szTemp, "SetRoundState: PREPARE\n");	break;
		case CCMATCH_ROUNDSTATE_COUNTDOWN:	sprintf(szTemp, "SetRoundState: COUNTDOWN\n");	break;
		case CCMATCH_ROUNDSTATE_PLAY:		sprintf(szTemp, "SetRoundState: PLAY(%d)\n", m_nRoundCount); break;
		case CCMATCH_ROUNDSTATE_FINISH:		sprintf(szTemp, "SetRoundState: FINISH\n");		break;
		case CCMATCH_ROUNDSTATE_EXIT:		sprintf(szTemp, "SetRoundState: EXIT\n");		break;
		case CCMATCH_ROUNDSTATE_FREE:		sprintf(szTemp, "SetRoundState: FREE\n");		break;
		case CCMATCH_ROUNDSTATE_FAILED:		sprintf(szTemp, "SetRoundState: FAILED\n");		break;
		}
		CCMatchServer::GetInstance()->LOG(CCMatchServer::LOG_PROG, "%s", szTemp);
	}
#endif*/

	if (nState == CCMATCH_ROUNDSTATE_FINISH)	OnRoundEnd();
	else if (nState == CCMATCH_ROUNDSTATE_PLAY) OnRoundBegin();

	m_nRoundState = nState;
	SetRoundStateTimer(CCMatchServer::GetInstance()->GetGlobalClockCount());

	// ���尡 ���� ���۵Ǹ� ���� �ʱ�ȭ
	if (nState == CCMATCH_ROUNDSTATE_COUNTDOWN)
	{
		InitRound();
	}

	// �÷��̾�鿡�� ���� ���� �ٲ���ٰ� �˷���
	CCMatchServer::GetInstance()->ResponseRoundState(GetStage()->GetUID());

}

void CCMatchRule::InitRound()
{
	if (m_pStage == NULL) 
	{
		_ASSERT(0);
		return;
	}

	// ���� ���ʽ� �ʱ�ȭ
	m_pStage->OnInitRound();

	// ��������� �ʱ�ȭ
	m_pStage->m_WorldItemManager.OnRoundBegin();
	m_pStage->m_ActiveTrapManager.Clear();

	// �ð����� �ܷ� ���� �ʱ�ȭ
	SetLastTimeLimitAnnounce(INT_MAX);
}

void CCMatchRule::OnBegin()
{
}
void CCMatchRule::OnEnd()
{
}
void CCMatchRule::OnRoundBegin()
{
//	OutputDebugString("OnRoundBegin \n");
}
void CCMatchRule::OnRoundEnd()
{
//	OutputDebugString("OnRoundEnd \n");
}

void CCMatchRule::OnRoundTimeOut()
{
//	OutputDebugString("OnRoundTimeOut \n");
}

bool CCMatchRule::OnCheckBattleTimeOut(unsigned int tmTimeSpend)	// true �����ϸ� Ÿ�Ӿƿ�
{
	int nLimitTime = GetStage()->GetStageSetting()->GetLimitTime() * 60 * 1000;
	if (nLimitTime <= STAGESETTING_LIMITTIME_UNLIMITED) return false;

//	int nLimitTime = 1 * 60 * 1000;
	int nTimeRemain = nLimitTime - tmTimeSpend;

	if ((unsigned int)nLimitTime < tmTimeSpend)
		return true;
	if ((nLimitTime == 0) && (tmTimeSpend > 24*60*60*1000))
		return true;

	char szMsg[256];

	if ((GetLastTimeLimitAnnounce() > 10) && (nTimeRemain < 10*1000))  {
		CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_ANNOUNCE, CCUID(0,0));
		pCmd->AddParameter(new CCCmdParamUInt(0));
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_TIME_10REMAINING);
		pCmd->AddParameter(new CCCmdParamStr( szMsg));

		CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
		SetLastTimeLimitAnnounce(10);
		return false;
	} else if ((GetLastTimeLimitAnnounce() > 30) && (nTimeRemain < 30*1000)) {
		CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_ANNOUNCE, CCUID(0,0));
		pCmd->AddParameter(new CCCmdParamUInt(0));
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_TIME_30REMAINING);
		pCmd->AddParameter(new CCCmdParamStr( szMsg));

		CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
		SetLastTimeLimitAnnounce(30);
		return false;
	} else if ((GetLastTimeLimitAnnounce() > 60) && (nTimeRemain < 60*1000)) {
		CCCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_ANNOUNCE, CCUID(0,0));
		pCmd->AddParameter(new CCCmdParamUInt(0));
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_TIME_60REMAINING);
		pCmd->AddParameter(new CCCmdParamStr( szMsg));

		CCMatchServer::GetInstance()->RouteToStage(GetStage()->GetUID(), pCmd);
		SetLastTimeLimitAnnounce(60);
		return false;
	}

	return false;
}

bool CCMatchRule::OnRun()
{
	DWORD nClock = CCMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
	case CCMATCH_ROUNDSTATE_PREPARE:
		{
			if (GetStage()->CheckBattleEntry() == true) 
			{
				if (OnCheckEnableBattleCondition())
				{
					// ���� ����
					SetRoundState(CCMATCH_ROUNDSTATE_COUNTDOWN);
				}
				else
				{
					SetRoundState(CCMATCH_ROUNDSTATE_FREE);
				}
				return true;
			} 
			else
			{
				// Drop Timedout Player
				if (nClock - GetRoundStateTimer() > 30*1000)
				{
					// ���� ������ �ʾҴ�.
				} 

				
				return true;
			}
		}
		break;
	case CCMATCH_ROUNDSTATE_COUNTDOWN:
		{
			if (nClock - GetRoundStateTimer() > 2*1000) {
				SetRoundState(CCMATCH_ROUNDSTATE_PLAY);
				return true;
			} else {
				// Countdown proceeding now, Do Nothing..
				return true;
			}

		}
		break;
	case CCMATCH_ROUNDSTATE_PLAY:
		{
			// �������� ���� ��Ȳ�̸� Free���·� ��ȯ
			if (!OnCheckEnableBattleCondition())
			{
				SetRoundState(CCMATCH_ROUNDSTATE_FREE);
				
				// �� SetRoundState() �ȿ��� RoundStateTimer�� ���ŵȴ�
				// ���� nClock�� ��������� �ϸ�, �׷��� ������ �Ʒ��� nClock - GetRoundStateTimer()���� ������ ���� �� �ְ� �Ǿ
				// unsigned ĳ�������� ��û ū ���� �ǰ�, OnCheckBattleTimeOut()�� �׻� true�̰� �ȴ�.
				nClock = CCMatchServer::GetInstance()->GetGlobalClockCount();
			}

			if (OnCheckRoundFinish())
			{
				// �������� ����Ʈ ����.
				// OnRoundTimeOut();
				SetRoundState( CCMATCH_ROUNDSTATE_FINISH );
			} 
			else if (OnCheckBattleTimeOut(nClock - GetRoundStateTimer())) 
			{
				// Make Draw Game...

				OnRoundTimeOut();
				SetRoundState(CCMATCH_ROUNDSTATE_FINISH);
			} 
			else 
			{
				// They still playing the game..
			}

			CheckOnGameEvent();
			
			return true;
		}
		break;
	case CCMATCH_ROUNDSTATE_FINISH:
		{
			if (nClock - GetRoundStateTimer() > 3*1000) {	// 3��
				if (RoundCount() == true)
				{
					SetRoundState(CCMATCH_ROUNDSTATE_PREPARE);
				}
				else
					SetRoundState(CCMATCH_ROUNDSTATE_EXIT);

				return true;
			} else {
				// Spend time, Do nothing
				return true;
			}
		}
		break;
	case CCMATCH_ROUNDSTATE_EXIT:
		{
			// End game
			return false;
		}
		break;
	case CCMATCH_ROUNDSTATE_FREE:
		{
			if (OnCheckEnableBattleCondition())
			{
				// ���� ����
				SetRoundState(CCMATCH_ROUNDSTATE_PREPARE);
			}

			return true;
		}
		break;
	default:
		{
			_ASSERT(0);
		}
	}

	return false;	// false return make RoundExit
}

bool CCMatchRule::Run()
{
	const bool bResult = OnRun();
	// CheckOnGameEvent();
	RunOnGameEvent();
	return bResult;
}

void CCMatchRule::Begin()
{
	m_nRoundCount = 0;
	SetRoundState(CCMATCH_ROUNDSTATE_PREPARE);
	SetRoundStateTimer( CCMatchServer::GetInstance()->GetGlobalClockCount() );
    
	InitOnBeginEventManager();
	InitOnGameEventManager();
	InitOnEndEventManager();
	
	OnBegin();

	CheckOnBeginEvent();
	RunOnBeginEvent();
}

void CCMatchRule::End()
{
	OnEnd();

	CheckOnEndEvent();
	RunOnEndEvent();
}

void CCMatchRule::CalcTeamBonus(CCMatchObject* pAttacker, CCMatchObject* pVictim,
								int nSrcExp, int* poutAttackerExp, int* poutTeamExp)
{
	*poutAttackerExp = nSrcExp;
	*poutTeamExp = 0;
}


void CCMatchRule::InitOnBeginEventManager()
{
	if( GetGameType() != GetOnBeginEventManager().GetLastSetGameType() )
	{
		EventPtrVec EvnPtrVec;
		if( !CCMatchEventFactoryManager::GetInstance().GetEventList(GetGameType(), ET_BEGIN, EvnPtrVec) )
		{
			ASSERT( 0 && "�̺�Ʈ ����Ʈ ���� ����.\n" );
			cclog( "CCMatchRule::InitOnBeginEventManager - ����Ʈ ���� ����.\n" );
			CCMatchEventManager::ClearEventPtrVec( EvnPtrVec );
			return;
		}
		GetOnBeginEventManager().ChangeEventList( EvnPtrVec );
	}
}


void CCMatchRule::InitOnGameEventManager()
{
	if( GetGameType() != GetOnGameEventManager().GetLastSetGameType() )
	{
		EventPtrVec EvnPtrVec;
		if( !CCMatchEventFactoryManager::GetInstance().GetEventList(GetGameType(), ET_ONGAME, EvnPtrVec) )
		{
			ASSERT( 0 && "�̺�Ʈ ����Ʈ ���� ����.\n" );
			cclog( "CCMatchRule::InitOnGameEventManager - ����Ʈ ���� ����.\n" );
			CCMatchEventManager::ClearEventPtrVec( EvnPtrVec );
			return;
		}
		GetOnGameEventManager().ChangeEventList( EvnPtrVec );
	}
}


void CCMatchRule::InitOnEndEventManager()
{
	if( GetGameType() != GetOnGameEventManager().GetLastSetGameType() )
	{
		EventPtrVec EvnPtrVec;
		if( !CCMatchEventFactoryManager::GetInstance().GetEventList(GetGameType(), ET_END, EvnPtrVec) )
		{
			ASSERT( 0 && "�̺�Ʈ ����Ʈ ���� ����.\n" );
			cclog( "CCMatchRule::InitOnEndEventManager - ����Ʈ ���� ����.\n" );
			CCMatchEventManager::ClearEventPtrVec( EvnPtrVec );
			return;
		}
		GetOnEndEventManager().ChangeEventList( EvnPtrVec );
	}
	
}


void CCMatchRule::CheckOnBeginEvent()
{
	if( !GetOnBeginEventManager().Empty() )
	{
		CCMatchObject* pObj;
		const DWORD dwClock = CCMatchServer::GetInstance()->GetGlobalClockCount();
		GetOnBeginEventManager().StartNewEvent();
		for (CCUIDRefCache::iterator i=GetStage()->GetObjBegin(); i!=GetStage()->GetObjEnd(); i++) 
		{
			pObj = (CCMatchObject*)(*i).second;
			if( MMP_STAGE == pObj->GetPlace() )
				GetOnBeginEventManager().CheckEventObj( pObj, dwClock );
		}

		GetOnBeginEventManager().SetLastCheckTime( dwClock );
	}
}


void CCMatchRule::CheckOnGameEvent()
{
	if( CCMATCH_ROUNDSTATE_PLAY == GetRoundState() ) {
		// ���ӻ� �ƹ��� ������ exit
		int nInGamePlayer = 0;

		CCMatchObject*	pObj;
		const DWORD		dwClock = CCMatchServer::GetInstance()->GetGlobalClockCount();

		for (CCUIDRefCache::iterator i=GetStage()->GetObjBegin(); i!=GetStage()->GetObjEnd(); i++) 
		{
			pObj = (CCMatchObject*)(*i).second;
			if( (pObj->GetEnterBattle() == true) || pObj->IsLaunchedGame() )
			{
				GetOnGameEventManager().CheckEventObj( pObj, dwClock );
				++nInGamePlayer;
			}
		}

		GetOnGameEventManager().SetLastCheckTime( dwClock );

		if (nInGamePlayer == 0) SetRoundState( CCMATCH_ROUNDSTATE_EXIT );
	}	
}


void CCMatchRule::CheckOnEndEvent()
{
	if( !GetOnEndEventManager().Empty() )
	{
		CCMatchObject*	pObj;
		const DWORD		dwClock = CCMatchServer::GetInstance()->GetGlobalClockCount();

		GetOnEndEventManager().StartNewEvent();
		for (CCUIDRefCache::iterator i=GetStage()->GetObjBegin(); i!=GetStage()->GetObjEnd(); i++) 
		{
			pObj = (CCMatchObject*)(*i).second;
			if( MMP_BATTLE == pObj->GetPlace() )
				GetOnEndEventManager().CheckEventObj( pObj, dwClock );
		}

		GetOnEndEventManager().SetLastCheckTime( dwClock );
	}
}


void CCMatchRule::RunOnBeginEvent()
{
	GetOnBeginEventManager().Run();	
}


void CCMatchRule::RunOnGameEvent()
{
	GetOnGameEventManager().Run();
	GetOnGameEventManager().StartNewEvent();
}


void CCMatchRule::RunOnEndEvent()
{
	GetOnEndEventManager().Run();
}
//////////////////////////////////////////////////////////////////////////////////