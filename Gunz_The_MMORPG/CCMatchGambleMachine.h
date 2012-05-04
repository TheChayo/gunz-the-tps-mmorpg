#pragma once
#include <map>
#include <vector>


using std::map;
using std::vector;


// #define MAX_GAMBLE_RATE (1000 - 1) // 1000%�� �ִ����� ����� �Ҷ��� 0���� ������ �ϱ⶧���� 0 ~ 999������ �ȴ�.
static const DWORD MAX_GAMBLE_RATE = (1000 - 1); // 1000%�� �ִ����� ����� �Ҷ��� 0���� ������ �ϱ⶧���� 0 ~ 999������ �ȴ�.


class CCMatchGambleItem;
class CCMatchGambleRewardItem;


class CCMatchGambleMachine
{
private :
	map< DWORD, CCMatchGambleItem* >	m_GambleItemMap;
	vector< CCMatchGambleItem* >		m_GambleItemVec;
	DWORD							m_dwLastUpdateTime;

private :
	const CCMatchGambleRewardItem*	GetGambleRewardItem( const DWORD dwGambleItemID, const WORD wRate ) const;
									// �׺� ������ ��å������ ����� �ؾ� �Ѵ�.
	const bool						CheckGambleItemIsSelling( const int nStartTimeMin
															, const int nEndTimeMin
															, const int nCurTimeMin
															, const bool bIsNoTimeLimit  ) const;
	
public :
	CCMatchGambleMachine();
	~CCMatchGambleMachine(); 

									// Create�Լ��� ȣ��Ǹ� ������ ����Ʈ�� �ʱ�ȭ �ϰ� �ٽ� �����Ѵ�.
									// ����Ʈ ������ �������� ���ؼ� �ϳ��� �߰� �ϴ� �Լ��� ����.
	bool							CreateGambleItemListWithGambleRewardList( vector<CCMatchGambleItem*>& vGambleItemList
																		, vector<CCMatchGambleRewardItem*>& vGambleRewardItemList );
	bool							CreateGambleItemList( vector<CCMatchGambleItem*>& vGambleItemList );


	void							Release();

	const DWORD						GetGambleItesSize() const { return static_cast<DWORD>(m_GambleItemVec.size()); }
	const CCMatchGambleItem*			GetGambleItemByIndex( const DWORD dwIndex ) const;
	const CCMatchGambleItem*			GetGambleItemByGambleItemID( const DWORD dwGambleItemID ) const;
	const CCMatchGambleItem*			GetGambleItemByName( const string& strGambleItemName ) const;

	const CCMatchGambleRewardItem*	Gamble( const DWORD dwGambleItemID ) const;

	// �׺� �������� ��� �ð��� �˻��ؼ� ������ �ϴ� �׺� �������� ���ڷ� �Ѿ�� ���Ϳ� ����ش�. //
	void							GetItemVectorByCheckedItemTime(vector<DWORD>& outItemIndexVec, const DWORD dwCurTime) const;
	void							GetOpenedGambleItemList( vector<DWORD>& outGItemList ) const;
	const bool						IsItTimeoverEventGambleItem( const DWORD dwGambleItemID, const DWORD dwCurTime ) const;	
	const DWORD						GetLastUpdateTime() const { return m_dwLastUpdateTime; }

	void							SetLastUpdateTime( const DWORD dwCurTime ) { m_dwLastUpdateTime = dwCurTime; }

	void							WriteGambleItemInfoToLog() const;
};