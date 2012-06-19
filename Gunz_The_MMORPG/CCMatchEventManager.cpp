#include "stdafx.h"
#include "CCMatchEventManager.h"

CCMatchEventManager::CCMatchEventManager()
{
}


CCMatchEventManager::~CCMatchEventManager()
{
	Clear();
}


// ������ ����Ʈ�� �ϳ��� �߰�.
void CCMatchEventManager::AddEvent( CCMatchEvent* pEvent )
{
	if( 0 != pEvent )
	{
		ASSERT( m_EventMap.end() == m_EventMap.find(pEvent->GetEventListID()) );

		m_EventVec.push_back( pEvent );
		m_EventMap.insert( pair<DWORD, CCMatchEvent*>(pEvent->GetEventListID(), pEvent) );
	}
}

// ������ �ִ� �̺�Ʈ ����Ʈ�� �߰��� �Ѵ�.
void CCMatchEventManager::AddEventList( EventPtrVec& EvnPtrVec )
{
	EventPtrVec::iterator it, end;
	end = EvnPtrVec.end();
	for( it = EvnPtrVec.begin(); it != end; ++it )
	{
		ASSERT( m_EventMap.end() == m_EventMap.find((*it)->GetEventListID()) );

		m_EventVec.push_back( (*it) );
		m_EventMap.insert( pair<DWORD, CCMatchEvent*>((*it)->GetEventListID(), (*it)) );
	}
}

// ������ �ִ� �̺�Ʈ ����Ʈ�� �������� ��ü�Ѵ�. ������ ������ ������.
void CCMatchEventManager::ChangeEventList( EventPtrVec& EvnPtrVec )
{
	Clear();
	m_EventVec.swap( EvnPtrVec );

	vector< CCMatchEvent* >::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		CCMatchEvent* pEvent = (*it);
		ASSERT( m_EventMap.end() == m_EventMap.find(pEvent->GetEventListID()) );
		m_EventMap.insert( pair<DWORD, CCMatchEvent*>(pEvent->GetEventListID(), pEvent) );
	}
}

// �ѹ��� �ϳ��� ���ŵ�. ������ ������ �����ô� ó���Ÿ� ������.
// ������ ���� ����� �����ô� GetEventCount�� ī��Ʈ�� Ȯ���Ѵ���,
//  �ʿ��� ��ŭ ȣ���ؾ� �ȴ�.
void CCMatchEventManager::DeleteEvent( const DWORD dwEventID )
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		if( (*it)->GetEventID() == dwEventID )
		{
			m_EventMap.erase( (*it)->GetEventID() );
			delete (*it);
			m_EventVec.erase( it );
			return;
		}
	}
}


void CCMatchEventManager::CheckEventObj( CCMatchObject* pObj, const DWORD dwCurTime  )
{
	if( 0 != pObj )
	{
		EventPtrVec::iterator it, end;
		end = m_EventVec.end();
		for( it = m_EventVec.begin(); it != end; ++it )
			(*it)->CheckEventObj( pObj, dwCurTime );
	}
}


void CCMatchEventManager::Run()
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		(*it)->Run();
}


void CCMatchEventManager::Clear()
{
	if( m_EventMap.empty() ) 
		return;

	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		delete (*it);
	m_EventVec.clear();
	m_EventMap.clear();
}


size_t CCMatchEventManager::GetEventCount( const DWORD dwEventID )
{
	size_t cnt = 0;
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		if( dwEventID == (*it)->GetEventID() )
			++cnt;
	}

	return cnt;
}


void CCMatchEventManager::StartNewEvent()
{
	size_t cnt = 0;
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		(*it)->StartNewEvent();
}


void CCMatchEventManager::CustomCheckEventObj( const DWORD dwEventID, CCMatchObject* pObj, void* pContext )
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
	{
		if( dwEventID == (*it)->GetEventID() )
			(*it)->CustomCheckEventObj( pObj, pContext );
	}
}


void CCMatchEventManager::CustomCheckEventObj( const DWORD dwEventListID, const DWORD dwEventID, CCMatchObject* pObj, void* pContext )
{
	EventPtrMap::iterator it = m_EventMap.find( dwEventListID );
	ASSERT( (m_EventMap.end() != it) && (dwEventID == it->second->GetEventID()) );
	if( (m_EventMap.end() != it) && (dwEventID == it->second->GetEventID()) )
		it->second->CustomCheckEventObj( pObj, pContext );
}

void CCMatchEventManager::SetLastCheckTime( const DWORD dwCurTime )
{
	EventPtrVec::iterator it, end;
	end = m_EventVec.end();
	for( it = m_EventVec.begin(); it != end; ++it )
		(*it)->SetLastCheckTime( dwCurTime );
}