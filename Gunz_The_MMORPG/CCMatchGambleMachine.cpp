#include "stdafx.h"
#include "CCMath.h"
#include "CCMatchGambleMachine.h"
#include "CCMatchDBGambleItem.h"
#include "CCDebug.h"



CCMatchGambleMachine::CCMatchGambleMachine()
{
	m_dwLastUpdateTime = 0;
}


CCMatchGambleMachine::~CCMatchGambleMachine()
{
}


const CCMatchGambleRewardItem* CCMatchGambleMachine::Gamble( const DWORD dwGambleItemID ) const
{
	return GetGambleRewardItem( dwGambleItemID, RandomNumber(0, MAX_GAMBLE_RATE) );
}


// �׺� �������� ��� �ð��� �˻��ؼ� ������ �ϴ� �׺� �������� ���ڷ� �Ѿ�� ���Ϳ� ����ش�. //
void CCMatchGambleMachine::GetItemVectorByCheckedItemTime(vector<DWORD>& outItemIndexVec, const DWORD dwCurTime) const
{
	// �ȿ� �������� ��������� �ȵȴ�.
	_ASSERT( outItemIndexVec.empty() );

	const int nCurTimeMin = static_cast<int>(dwCurTime / 60000);

	const DWORD VecSize = (DWORD)m_GambleItemVec.size();
	for (DWORD i = 0; i < VecSize; i++)
	{
		const CCMatchGambleItem* pGambleItem = m_GambleItemVec[i];
		if (pGambleItem == NULL)
		{
			ASSERT( 0 );
			continue;
		}

		// DB�� ���常 �ǰ� ���� �ǸŸ� �������� �ʾ���.
		if( !pGambleItem->IsOpened() )
			continue;

		// Cashitem�� �������� �ʴ´�.
		if( pGambleItem->IsCash() )
			continue;

		if( !CheckGambleItemIsSelling(
			pGambleItem->GetStartTimeMin()
			, pGambleItem->GetEndTimeMin()
			, nCurTimeMin
			, pGambleItem->IsNoTimeLimitItem()) )
			continue;

		outItemIndexVec.push_back(i);
	}
}

const CCMatchGambleRewardItem* CCMatchGambleMachine::GetGambleRewardItem( const DWORD dwGambleItemID, const WORD wRate ) const
{
	map< DWORD, CCMatchGambleItem* >::const_iterator itFind = m_GambleItemMap.find( dwGambleItemID );
	if( m_GambleItemMap.end() == itFind )
		return NULL;

	return itFind->second->GetGambleRewardItemByRate( wRate );
}


const CCMatchGambleItem* CCMatchGambleMachine::GetGambleItemByIndex( const DWORD dwIndex ) const
{
	if( dwIndex < m_GambleItemVec.size() )
		return m_GambleItemVec[ dwIndex ];

	return NULL;
}


bool CCMatchGambleMachine::CreateGambleItemListWithGambleRewardList( vector<CCMatchGambleItem*>& vGambleItemList
																, vector<CCMatchGambleRewardItem*>& vGambleRewardItemList )
{
	Release();

	DWORD dwGIID = 0;

	vector<CCMatchGambleItem*>::iterator itGI, endGI;
	vector<CCMatchGambleRewardItem*>::iterator itRI, endRI;

	endGI = vGambleItemList.end();
	endRI = vGambleRewardItemList.end();

	cclog( "Start GambleItem Init.\n" );

	for( itGI = vGambleItemList.begin(); itGI != endGI; ++itGI )
	{
		dwGIID = (*itGI)->GetGambleItemID();

		cclog( "GIID : %u\n", dwGIID );

		for( itRI = vGambleRewardItemList.begin(); itRI != endRI; ++itRI )
		{
			if( (*itRI)->GetGambleItemID() == dwGIID )
			{
				if( !(*itGI)->AddGambleRewardItem((*itRI)) )
					return false;

				cclog( "  GRIID : M(%u), F(%u), RentHourPeriod(%u), ItemCnt(%u).\n"
					, (*itRI)->GetItemIDMale(), (*itRI)->GetItemIDFemale()
					, (*itRI)->GetRentHourPeriod(), (*itRI)->GetItemCnt() );
			}
		}

		_ASSERT( 1000 == (*itGI)->GetTotalRate() );

		m_GambleItemMap.insert( map<DWORD, CCMatchGambleItem*>::value_type(dwGIID, (*itGI)) );
		m_GambleItemVec.push_back( (*itGI) );
	}

	cclog( "End GambleItem Init.\n" );

	vGambleItemList.clear();
	vGambleRewardItemList.clear();

	return true;
}


bool CCMatchGambleMachine::CreateGambleItemList( vector<CCMatchGambleItem*>& vGambleItemList )
{
	Release();

	vector<CCMatchGambleItem*>::iterator itGI, endGI;
	endGI = vGambleItemList.end();

	for( itGI = vGambleItemList.begin(); itGI != endGI; ++itGI )
	{
		m_GambleItemMap.insert( map<DWORD, CCMatchGambleItem*>::value_type((*itGI)->GetGambleItemID(), (*itGI)) );
		m_GambleItemVec.push_back( (*itGI) );
	}

	vGambleItemList.clear();

	return true;
}


void CCMatchGambleMachine::Release()
{
	vector< CCMatchGambleItem* >::iterator it, end;
	end = m_GambleItemVec.end();
	for( it = m_GambleItemVec.begin(); it != end; ++it )
	{
		(*it)->Release();
		delete (*it);
	}

	m_GambleItemMap.clear();
	m_GambleItemVec.clear();
}


const CCMatchGambleItem*	CCMatchGambleMachine::GetGambleItemByGambleItemID( const DWORD dwGambleItemID ) const
{
	map< DWORD, CCMatchGambleItem* >::const_iterator itFind = m_GambleItemMap.find( dwGambleItemID );
	if( m_GambleItemMap.end() == itFind )
		return NULL;

	return itFind->second;
}


const CCMatchGambleItem* CCMatchGambleMachine::GetGambleItemByName( const string& strGambleItemName ) const
{
	vector< CCMatchGambleItem* >::const_iterator it, end;
	end = m_GambleItemVec.end();
	for( it = m_GambleItemVec.begin(); it != end; ++it )
	{
		if( strGambleItemName == (*it)->GetName() )
			return (*it);
	}

	return NULL;
}


void CCMatchGambleMachine::GetOpenedGambleItemList( vector<DWORD>& outGItemList ) const
{
	const DWORD dwSize = GetGambleItesSize();
	for( DWORD i = 0; i < dwSize; ++i )
	{
		if( m_GambleItemVec[i]->IsOpened() )
			outGItemList.push_back( i );
	}
}


const bool CCMatchGambleMachine::IsItTimeoverEventGambleItem( const DWORD dwGambleItemID, const DWORD dwCurTime ) const
{
	const CCMatchGambleItem* pGItem = GetGambleItemByGambleItemID( dwGambleItemID );
	if( NULL == pGItem )
		return true;

	if( !pGItem->IsOpened() )
		return true;

	return !CheckGambleItemIsSelling( 
		pGItem->GetStartTimeMin()
		, pGItem->GetEndTimeMin() + MAX_BUYGAMBLEITEM_ELAPSEDTIME_MIN
		, static_cast<int>(dwCurTime / 60000)
		, pGItem->IsNoTimeLimitItem() );
}


const bool CCMatchGambleMachine::CheckGambleItemIsSelling( const int nStartTimeMin
														 , const int nEndTimeMin
														 , const int nCurTimeMin
														 , const bool bIsNoTimeLimit  ) const
{
	// �� �Լ��� �׺� ������ ��å�� ���߾� ������� �Լ� �Դϴ�.
	// �� �׺� �����ۿ��� ������ �ؾ� �մϴ�.
	// �� ���� ��Ȳ�� ���ؼ��� ���� ����! - by SungE 2007-06-22

	// ���� �Ǹű��� �ð��� ������.
	if( nCurTimeMin < nStartTimeMin )
		return false;

	// ������ ���������� �˻���.
	if( bIsNoTimeLimit )
		return true;
	else if( nEndTimeMin < nCurTimeMin )
		return false;

	// ������� �������� �Ǹ� �Ⱓ�� �ִ� �������̴�.
	return true;
}



void CCMatchGambleMachine::WriteGambleItemInfoToLog() const
{
	vector< CCMatchGambleItem* >::const_iterator end			= m_GambleItemVec.end();
	vector< CCMatchGambleItem* >::const_iterator	it			= m_GambleItemVec.begin();
	const DWORD									dwCurTime	= MGetMatchServer()->GetGlobalClockCount();

	cclog( "\n=== Dump GambleItem Info. ===\n" );
	cclog( "Gamble item list.\n" );
	for( ; end != it; ++it )
	{
		CCMatchGambleItem* pGItem = (*it);

		cclog( "Gamble item. ID(%d), Name(%s), IsTimeover(%d).\n"
			, pGItem->GetGambleItemID()
			, pGItem->GetName().c_str()
			, IsItTimeoverEventGambleItem(pGItem->GetGambleItemID(), dwCurTime) );
	}

	cclog( "\nSend shop gamble item list.\n" );
	vector< DWORD > vShopGItemList;
	GetItemVectorByCheckedItemTime( vShopGItemList, dwCurTime );
	cclog( "Shop gamble item count : %d\n", int(vShopGItemList.size()) );
	vector< DWORD >::const_iterator endShop = vShopGItemList.end();
	vector< DWORD >::const_iterator	itShop	= vShopGItemList.begin();
	for( ; endShop != itShop; ++itShop )
	{
		const CCMatchGambleItem* pGItem = GetGambleItemByIndex( (*itShop) );
		if( NULL == pGItem )
		{
			_ASSERT( NULL != pGItem );
			cclog( "Invalid GItemIndex : %d\n", (*itShop) );
			continue;
		}

		cclog( "Shop gamble item. ID(%d), Name(%s).\n"
			, pGItem->GetGambleItemID()
			, pGItem->GetName().c_str() );
	}
	cclog( "=== Completed Dump GambleItem Info. ===\n\n" );
}