#include "stdafx.h"
#include "CCServer.h"
#include "CCMatchSchedule.h"

CCMatchDayOfMonth::CCMatchDayOfMonth()
{
	Init();
}

CCMatchDayOfMonth::~CCMatchDayOfMonth()
{
}

void CCMatchDayOfMonth::Init()
{
	m_cDayOfMonth[ 0 ] = 31;
	
	struct tm* ptm = CCMatchGetLocalTime();
	if( 0 != (ptm->tm_year % 4) )
		m_cDayOfMonth[ 1 ] = 28;
	else
		m_cDayOfMonth[ 1 ] = 29;

	m_cDayOfMonth[ 2 ] = 31;
	m_cDayOfMonth[ 3 ] = 30;
	m_cDayOfMonth[ 4 ] = 31;
	m_cDayOfMonth[ 5 ] = 30;
	m_cDayOfMonth[ 6 ] = 31;
	m_cDayOfMonth[ 7 ] = 31;
	m_cDayOfMonth[ 8 ] = 30;
	m_cDayOfMonth[ 9 ] = 31;
	m_cDayOfMonth[ 10 ] = 30;
	m_cDayOfMonth[ 11 ] = 31;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchScheduleData::CCMatchScheduleData() : m_cYear( 0 ), m_cMonth( 0 ), m_cDay( 0 ), m_cHour( 0 ), m_cMin( 0 ), 
	m_pCmd( 0 ), m_nType( -1 ), m_bIsNeedDelete( false ), // ,nID( 0 ),
	m_cNextYear( 0 ), m_cNextMonth( 0 ), m_cNextDay( 0 ), m_cNextHour( 0 ), m_cNextMin( 0 ), m_nCount( 0 )
{
}

CCMatchScheduleData::~CCMatchScheduleData()
{
}

void CCMatchScheduleData::CorrectTime()
{
	const struct tm* tmLocalTime = CCMatchGetLocalTime();

	// 분단위로 설정된 시간에서 오버된 초가 보정시간보다 큰지 검사.
	if( tmLocalTime->tm_sec > m_nErrorTime ) return;

	// 늦은 시간을 강제로 증가시킴.
	++m_cMin;

	if( 59 < m_cMin ){
		m_cMin -= 60;
		++m_cHour;
	}

	if( 23 < m_cHour ){
		m_cHour -= 24;
		++m_cDay;
	}

	// 이번 달이 몇일까지 있는지를 알아와야 함... 
	if( GetMaxDay() < m_cDay ){
		m_cDay -= GetMaxDay();
		m_cYear += 1;
	}
	//
}

// return if same, then 0, less -1, greater 1.
int	CCMatchScheduleData::CompareCurrentTime()
{
	const struct tm* pLocalTime = CCMatchGetLocalTime();

	// 현제 로컬 시간과 스케쥴에 저장된 시간을 비교함.
		
	if( (pLocalTime->tm_year - 100) > GetYear() )
		return -1;
	else if( (pLocalTime->tm_year - 100) < GetYear() )
		return 1;

	if(	(pLocalTime->tm_mon + 1) > GetMonth() )
		return -1;
	else if( (pLocalTime->tm_mon + 1) < GetMonth() )
		return 1;

	if(	pLocalTime->tm_mday > GetDay() )
		return -1;
	else if( pLocalTime->tm_mday < GetDay() )
		return 1;

	if(	pLocalTime->tm_hour > GetHour() ) 
		return -1;
	else if( pLocalTime->tm_hour < GetHour() ) 
		return 1;

	if( pLocalTime->tm_min > GetMin()	) 
		return -1;
	else if( pLocalTime->tm_min < GetMin() ) 
		return 1;

	return 0;
}

bool CCMatchScheduleData::SetTimes( const unsigned char cYear,
								   const unsigned char cMonth, 
								   const unsigned char cDay, 
								   const unsigned char cHour, 
								   const unsigned char cMin )
{
	if( 04 > cYear )	return false;
	if( 12 < cMonth )	return false;
	if( 31 < cDay )		return false;
	if( 23 < cHour )	return false;
	if( 59 < cMin )		return false;

	SetYear( cYear );
	SetMonth( cMonth );
	SetDay( cDay );
	SetHour( cHour );
	SetMin( cMin );

	return true;
}

bool CCMatchScheduleData::SetNextTimes( const unsigned char cNextYear,
									   const unsigned char cNextMonth, 
									   const unsigned char cNextDay, 
									   const unsigned char cNextHour, 
									   const unsigned char cNextMin )
{
	if( 12 < cNextMonth )	return false;
	if( 31 < cNextDay )		return false;
	if( 23 < cNextHour )	return false;
	if( 59 < cNextMin )		return false;

	SetNextYear( cNextYear );
	SetNextMonth( cNextMonth );
	SetNextDay( cNextDay );
	SetNextHour( cNextHour );
	SetNextMin( cNextMin );

	return true;
}

void CCMatchScheduleData::ResetTime()
{
	m_cMin += m_cNextMin;
	if( 59 < m_cMin ){
        m_cMin -= 60;
		++m_cHour;
	}

	m_cHour += m_cNextHour;
	if( 23 < m_cHour ){
		m_cHour -= 24;
		++m_cDay;
	}

	m_cDay += m_cNextDay;
	if( GetMaxDay() < m_cDay ){
		m_cDay -= GetMaxDay();
		++m_cMonth;
	}

	m_cMonth += m_cNextMonth;
	if( 12 < m_cMonth ){
		m_cMonth -= 12;	
		++m_cYear;
	}
}

void CCMatchScheduleData::Release()
{
	if( 0 != m_pCmd ){
		delete m_pCmd;
		m_pCmd = 0;
	}
}

/////////////////////////////////////////////////////
// 시간을 다시 설정.
void CCMatchRepeatScheduleImpl::Reset( CCMatchScheduleData* pScheduleData )
{
	pScheduleData->ResetTime();
}
// 시간 오차 보정.
void CCMatchRepeatScheduleImpl::CorrectTime( CCMatchScheduleData* pScheduleData )
{
	pScheduleData->CorrectTime();
	// 보정을 해도 현제 시간보다 늦으면 문제가 있는 것이므로 스케쥴에서 제거함.
	if( 0 > pScheduleData->CompareCurrentTime() ){
		pScheduleData->SetDeleteState( true );
		return;
	}

	// 현제 시간으로 설정.
	const struct tm* tmLocalTime = CCMatchGetLocalTime();
	pScheduleData->SetTimes( tmLocalTime->tm_year - 100, 
							 tmLocalTime->tm_mon + 1, 
							 tmLocalTime->tm_mday, 
							 tmLocalTime->tm_hour, 
							 tmLocalTime->tm_min );
}

// 시간을 다시 설정.
void CCMatchCountScheduleImpl::Reset( CCMatchScheduleData* pScheduleData )
{
	// count타입의 스케쥬은 count값이 0이되면 제거 되어야 한다.
	pScheduleData->DecreaseCount();
	if( 0 == pScheduleData->GetCount() ) 
		pScheduleData->SetDeleteState( true );

	pScheduleData->ResetTime();
}
// 시간 오차 보정.
void CCMatchCountScheduleImpl::CorrectTime( CCMatchScheduleData* pScheduleData )
{
	pScheduleData->CorrectTime();
	// 보정을 해도 현제 시간보다 늦으면 카운트를 하나 감소시킴.
	if( 0 > pScheduleData->CompareCurrentTime() ){
		pScheduleData->DecreaseCount();
		if( 0 == pScheduleData->GetCount() )
			pScheduleData->SetDeleteState( true );
		return;
	}

	// 현제 시간으로 설정.
	const struct tm* tmLocalTime = CCMatchGetLocalTime();
	pScheduleData->SetTimes( tmLocalTime->tm_year - 100, 
							 tmLocalTime->tm_mon + 1, 
							 tmLocalTime->tm_mday, 
							 tmLocalTime->tm_hour, 
							 tmLocalTime->tm_min );
}

// 시간을 다시 설정.
void CCMatchOnceScheduleImpl::Reset( CCMatchScheduleData* pScheduleData )
{
	pScheduleData->SetDeleteState( true );
}
// 시간 오차 보정.
void CCMatchOnceScheduleImpl::CorrectTime( CCMatchScheduleData* pScheduleData )
{
	// 현제시간으로 다시 설정해줌. 
	const struct tm* tmLocalTime = CCMatchGetLocalTime();
	pScheduleData->SetTimes( tmLocalTime->tm_year - 100, 
							 tmLocalTime->tm_mon + 1, 
							 tmLocalTime->tm_mday, 
							 tmLocalTime->tm_hour, 
							 tmLocalTime->tm_min );
}

/////////////////////////////////////////////////////

CCMatchScheduleMgr::CCMatchScheduleMgr( MServer* pServer ) : m_pServer( pServer ), m_nIndex( 0 ),
	m_tmUpdateTerm( 0 )
{
}

CCMatchScheduleMgr::~CCMatchScheduleMgr()
{
}

// 마지막 업데이트 시점에서부터 경과시간 계산.
const time_t CCMatchScheduleMgr::CalculateElapseUpdateTime()
{
	time_t tmCurTime;
	time( &tmCurTime );
	return ( tmCurTime - m_tmLastUpdateTime );
}

bool CCMatchScheduleMgr::Init()
{
	if( !m_ScheduleImplPrototype.Init() ) return false;

	SetLastUpdateTime();
	
	return true;
}

bool CCMatchScheduleMgr::IsUpdate()
{
	if( CalculateElapseUpdateTime() > m_tmUpdateTerm ) return true;

	return false;
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 정상적인 데이터인지 검사하여 정상일경우만 true를 반환함.
bool CCMatchScheduleMgr::CheckData( CCMatchScheduleData* pScheduleData ) const
{
	if( 0 == pScheduleData )				return false;
	if( 0 == pScheduleData->GetCommand() )	return false;
	if( 0 > pScheduleData->GetType() )		return false;

	if( 04 > pScheduleData->GetYear() )				return false;
	if( 12 < pScheduleData->GetMonth() )			return false;
	if( GetMaxDay() < pScheduleData->GetDay() )		return false;
	if( 23 < pScheduleData->GetHour() )				return false;
	if( 59 < pScheduleData->GetMin() )				return false;

	if( 12 < pScheduleData->GetNextMonth() )	return false;
	if( 31 < pScheduleData->GetNextDay() )		return false;
	if( 23 < pScheduleData->GetNextHour() )		return false;
	if( 59 < pScheduleData->GetNextMin() )		return false;

	// pScheduleData->nID = m_nIndex;
	// ++m_nIndex;

	return true;
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 시간을 비교하여 같을경우만 true를 반환함.
bool CCMatchScheduleMgr::CompareTime( CCMatchScheduleData* pScheduleData )
{
	struct tm* pCurTime = CCMatchGetLocalTime();

	bool bIsEqual = true;

	const int nCompareResult = pScheduleData->CompareCurrentTime();

	// 로컬 시간과 비교.
	if( 0 < nCompareResult )
		return false;
	else if( 0 == nCompareResult )
		return true;
	else if( 0 > nCompareResult )
		bIsEqual = false;

	// 시간이 다를경우 오차보정을 함.
	if( !bIsEqual ){
		pScheduleData->GetImpl()->CorrectTime( pScheduleData );

		bIsEqual = true;

		// 보정된 시간을 가지고 다시 비교함.
		if( 0 != pScheduleData->CompareCurrentTime() )
			bIsEqual = false;
	}
	
	return bIsEqual;
}

void CCMatchScheduleMgr::SetLastUpdateTime()
{
	// set start time.
	time( &m_tmLastUpdateTime );
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 스케쥴 리스트를 업데이트 함.
void CCMatchScheduleMgr::Update()
{
	if( !IsUpdate() ) return;

	UpdateStaticSchedule();
	UpdateDynamicSchedule();

	SetLastUpdateTime();
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 변동이 거의 업는 정적 스케쥴 업데이트.
void CCMatchScheduleMgr::UpdateStaticSchedule()
{
	CCCommand* pCmd = 0;
	ScheduleVecIter it, end;

	end = m_vecStaticSchedule.end();
	for( it = m_vecStaticSchedule.begin(); it != end; ++it )
	{
		// 실행될 시간인가?
		if( !CompareTime((*it)) ) continue;

		pCmd = (*it)->GetCommand()->Clone();
		if( 0 != pCmd ){
			m_pServer->GetCommandManager()->Post( pCmd );
		}

		// 혹시 지울 것인가?
		// 정적 스케쥴이 IsNeedDelete()이 true면 날자만 업데이트 하지 않아 실행되는것을 막음.
		if( (*it)->IsNeedDelete() ) continue;

		(*it)->GetImpl()->Reset( (*it) );
	}
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 공지사항 스케쥴 업데이트.
void CCMatchScheduleMgr::UpdateDynamicSchedule()
{
	CCCommand* pCmd = 0;
	ScheduleLstIter it, end;

	end = m_lstDynamicSchedule.end();
	for( it = m_lstDynamicSchedule.begin(); it != end; )
	{
		if( !CompareTime((*it)) )
		{
			++it;
			continue;
		}

		pCmd = (*it)->GetCommand()->Clone();
		if( 0 != pCmd )
		{
			m_pServer->GetCommandManager()->Post( pCmd );
		}

		(*it)->GetImpl()->Reset( (*it) );

		// 이번이 지울 차례인가?
		if( (*it)->IsNeedDelete() )
		{
			delete *it;
			m_lstDynamicSchedule.erase( it++ );
			continue;
		}

		++it;
	}
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 새로운 공지 사항을 등록함.
bool CCMatchScheduleMgr::AddDynamicSchedule( CCMatchScheduleData* pNewSchedule )
{
	if( !CheckData(pNewSchedule) ) return false;

	if( 0 == pNewSchedule->GetImpl() )
		pNewSchedule->SetImpl( m_ScheduleImplPrototype.GetImpl(pNewSchedule->GetType()) );

	if( 0 == pNewSchedule->GetImpl() ) return false;

	m_lstDynamicSchedule.push_back( pNewSchedule );

	return true;
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 정적 스케쥴 등록.
bool CCMatchScheduleMgr::AddStaticSchedule( CCMatchScheduleData* pNewSchedule )
{
	if( !CheckData(pNewSchedule) ) return false;

	if( 0 == pNewSchedule->GetImpl() )
		pNewSchedule->SetImpl( m_ScheduleImplPrototype.GetImpl(pNewSchedule->GetType()) );

	if( 0 == pNewSchedule->GetImpl() ) return false;

	m_vecStaticSchedule.push_back( pNewSchedule );

	return true;
}

CCMatchScheduleData* CCMatchScheduleMgr::MakeRepeatScheduleData( const unsigned char cNextYear,	// 다음에 실행될 해의 상대값.	ex) 1년 후.
															   const unsigned char cNextMonth,	// 다음에 실행될 달의 상대값.	ex) 1달 후.
															   const unsigned char cNextDay,	// 다음에 실행될 일의 상대값.	ex) 1일 후.
															   const unsigned char cNextHour,	// 다음에 실행될 시간의 상대값.	ex) 1시간 후.
															   const unsigned char cNextMin,	// 다음에 실행될 분의 상대값.	ex) 1분 후.
															   CCCommand* pCmd					// 수행할 명령.
															   )
{
	if( 0 == pCmd ) return 0;

	CCMatchScheduleData* pData = new CCMatchScheduleData;
	if( 0 == pData ) return 0;

	struct tm* tmTime = CCMatchGetLocalTime();
	if( 0 == tmTime ) return 0;

	if( !pData->SetTimes(tmTime->tm_year + cNextYear - 100, 
						 tmTime->tm_mon + cNextMonth + 1, 
						 tmTime->tm_mday + cNextDay, 
						 tmTime->tm_hour + cNextHour, 
						 tmTime->tm_min + cNextMin) )
		return 0;

	if( !pData->SetNextTimes(cNextYear, cNextMonth, cNextDay, cNextHour, cNextMin) )
		return 0;

	pData->SetType( CCMatchScheduleData::REPEAT );
	pData->SetCommand( pCmd );
	pData->SetErrorTime( static_cast<int>(m_tmUpdateTerm) );

	pData->SetImpl( m_ScheduleImplPrototype.GetImpl(pData->GetType()) );
	if( 0 == pData->GetImpl() ) return 0;

	return pData;
}

CCMatchScheduleData* CCMatchScheduleMgr::MakeCountScheduleData( const unsigned char cNextYear,	// 다음에 실행될 해의 상대값.	ex) 1년 후.
															  const unsigned char cNextMonth,	// 다음에 실행될 달의 상대값.	ex) 1달 후.
															  const unsigned char cNextDay,		// 다음에 실행될 일의 상대값.	ex) 1일 후.
															  const unsigned char cNextHour,	// 다음에 실행될 시간의 상대값.	ex) 1시간 후.
															  const unsigned char cNextMin,		// 다음에 실행될 분의 상대값.	ex) 1분 후.
															  const unsigned int nCount,		// 실행될 횟수.
															  CCCommand* pCmd					// 수행할 명령.
															  )
{
	if( 0 == pCmd ) return 0;

	CCMatchScheduleData* pData = new CCMatchScheduleData;
	if( 0 == pData ) return 0;

	struct tm* tmTime = CCMatchGetLocalTime();
	if( 0 == tmTime ) return 0;

	pData->SetCount( nCount );

	if( !pData->SetTimes(tmTime->tm_year + cNextYear - 100, 
					     tmTime->tm_mon + cNextMonth + 1, 
						 tmTime->tm_mday + cNextDay, 
						 tmTime->tm_hour + cNextHour, 
						 tmTime->tm_min + cNextMin) )
		return 0;

	if( !pData->SetNextTimes(cNextYear, cNextMonth, cNextDay, cNextHour, cNextMin) ) 
		return 0;

	pData->SetType( CCMatchScheduleData::COUNT );
	pData->SetCommand( pCmd );
	pData->SetErrorTime( static_cast<int>(m_tmUpdateTerm) );

	pData->SetImpl( m_ScheduleImplPrototype.GetImpl(pData->GetType()) );
	if( 0 == pData->GetImpl() ) return 0;

	return pData;
}

CCMatchScheduleData* CCMatchScheduleMgr::MakeOnceScheduleData( const unsigned char cYear,		// 실행될 해.
															 const unsigned char cMonth,	// 실행될 달.
															 const unsigned char cDay,		// 실행될 날.
															 const unsigned char cHour,		// 실행될 시간.
															 const unsigned char cMin,		// 실행될 분.
															 CCCommand* pCmd					// 수행할 명령.	
															 )
{
	if( 0 == pCmd ) return 0;

	CCMatchScheduleData* pData = new CCMatchScheduleData;
	if( 0 == pData ) return 0;

	// cYear % 100 => 두에 두자리만을 사용함.
	if( !pData->SetTimes(cYear % 100, cMonth, cDay, cHour, cMin) ) return 0;

	cclog( "\n%u\n", m_tmUpdateTerm );

	pData->SetType( CCMatchScheduleData::ONCE );
	pData->SetCommand( pCmd );
	pData->SetErrorTime( static_cast<int>(m_tmUpdateTerm) );

	pData->SetImpl( m_ScheduleImplPrototype.GetImpl(pData->GetType()) );
	if( 0 == pData->GetImpl() ) return 0;

	return pData;
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 모든 리스트 제거.
void CCMatchScheduleMgr::Release()
{
	ReleaseStaticSchedule();
	ReleaseDynamicSchedule();
	m_ScheduleImplPrototype.Release();
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 정적 스케쥴 제거.
void CCMatchScheduleMgr::ReleaseStaticSchedule()
{
	for_each( m_vecStaticSchedule.begin(), 
			  m_vecStaticSchedule.end(), 
			  CCMatchScheduleReleaser() );
	m_vecStaticSchedule.clear();
}

// first	: 추교성. 2004. 11. 08
// last		: 추교성. 2004. 11. 08
// 공지사항 스케쥴 제거.
void CCMatchScheduleMgr::ReleaseDynamicSchedule()
{
	for_each( m_lstDynamicSchedule.begin(),
			  m_lstDynamicSchedule.end(),
			  CCMatchScheduleReleaser() );
	m_lstDynamicSchedule.clear();
}

// first	: 추교성. 2004. 11. 09.
// last		: 추교성. 2004. 11. 09.
// Impl을 prototype클래스에 등록함.
bool CCMatchScheduleMgr::CCMatchScheduleImplPrototype::Init()
{
	CCMatchScheduleImpl* pImpl;

	pImpl = new CCMatchRepeatScheduleImpl;
	if( 0 == pImpl ) return false;
	m_ScheduleImplVec.push_back( pImpl );

	pImpl = new CCMatchCountScheduleImpl;
	if( 0 == pImpl ) return false;
	m_ScheduleImplVec.push_back( pImpl );

	pImpl = new CCMatchOnceScheduleImpl;
	if( 0 == pImpl ) return false;
	m_ScheduleImplVec.push_back( pImpl );

	return true;
}

// first	: 추교성. 2004. 11. 09.
// last		: 추교성. 2005. 01. 27.
// 해당 스케쥴 타입에 맞는 수행자를 반환해줌.
CCMatchScheduleImpl* CCMatchScheduleMgr::CCMatchScheduleImplPrototype::GetImpl( const unsigned int nTypeID )
{
	if( nTypeID >= m_ScheduleImplVec.size() ) return 0;

	return m_ScheduleImplVec[ nTypeID ];
}

// first	: 추교성. 2004. 11. 09.
// last		: 추교성. 2004. 11. 09.
// 등록되어있는 수행자를 제거함.
void CCMatchScheduleMgr::CCMatchScheduleImplPrototype::Release()
{
	for_each( m_ScheduleImplVec.begin(), 
			  m_ScheduleImplVec.end(), 
			  CCMatchScheduleImplRelease() );
	m_ScheduleImplVec.clear();
}

//////////////////////////////////////////////////////////////////////
struct tm* CCMatchGetLocalTime()
{
	__time64_t long_time;

	_time64( &long_time );

	/* Obtain coordinated universal time: */
	return _localtime64( &long_time );
}

char GetMaxDay( const int iMonth )
{
	return CCMatchDayOfMonth::GetInst().GetMaxDay( iMonth );
}

char GetMaxDay()
{
	return GetMaxDay( CCMatchGetLocalTime()->tm_mon + 1 );
}


bool AddDynamicSchedule( CCMatchScheduleMgr* pScheduleMgr, const int nType, CCCommand* pCmd, const int nYear, const int nMonth, const int nDay, const int nHour, const int nMin, const int nCount )
{
	if( 0 == pScheduleMgr ) return false;

	unsigned char nConYear = nYear % 100;

	switch( nType )
	{
	case CCMatchScheduleData::REPEAT :
		{
			CCMatchScheduleData* pScheduleData = pScheduleMgr->MakeRepeatScheduleData( static_cast<unsigned char>(nYear), 
																					  static_cast<unsigned char>(nMonth),
																					  static_cast<unsigned char>(nDay), 
																					  static_cast<unsigned char>(nHour), 
																					  static_cast<unsigned char>(nMin),
																					  pCmd );
			if( 0 != pScheduleData )
			{
				if( !pScheduleMgr->AddDynamicSchedule(pScheduleData) )
					delete pScheduleData;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	case CCMatchScheduleData::COUNT :
		{
			CCMatchScheduleData* pScheduleData = pScheduleMgr->MakeCountScheduleData( static_cast<unsigned char>(nYear), 
																					 static_cast<unsigned char>(nMonth),
																					 static_cast<unsigned char>(nDay), 
																					 static_cast<unsigned char>(nHour), 
																					 static_cast<unsigned char>(nMin),
																					 static_cast<unsigned int>(nCount),
																					 pCmd );

			if( 0 != pScheduleData )
			{
				if( !pScheduleMgr->AddDynamicSchedule(pScheduleData) )
					delete pScheduleData;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	case CCMatchScheduleData::ONCE :
		{
			CCMatchScheduleData* pScheduleData = pScheduleMgr->MakeOnceScheduleData( static_cast<unsigned char>(nYear), 
																					static_cast<unsigned char>(nMonth),
																					static_cast<unsigned char>(nDay), 
																					static_cast<unsigned char>(nHour), 
																					static_cast<unsigned char>(nMin),
																					pCmd );

			if( 0 != pScheduleData )
			{
				if( !pScheduleMgr->AddDynamicSchedule(pScheduleData) )
					delete pScheduleData;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	default :
		{
			ASSERT( 0 );
			return false;
		}
		break;
	}
}