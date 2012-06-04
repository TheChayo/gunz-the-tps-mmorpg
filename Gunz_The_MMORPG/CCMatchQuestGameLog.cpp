#include "stdafx.h"
#include "CCMatchQuestGameLog.h"
#include "CCQuestConst.h"
#include "CCAsyncDBJob.h"


void CCQuestPlayerLogInfo::AddUniqueItem( const unsigned long int nItemID, int nCount )
{
	if (IsQuestItemID(nItemID))
	{
		// ����ũ �����۵��� �ٸ� ���̺� �߰����� ������ �����ϱ����ؼ� ���� ������ ���ƾ� ��.
		CCQuestItemDesc* pQItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID );
		if( 0 == pQItemDesc )
			return;

		if( !pQItemDesc->m_bUnique )
			return;
	}

	m_UniqueItemList.insert( map<unsigned long int, int>::value_type(nItemID, nCount) );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

CCMatchQuestGameLogInfoManager::CCMatchQuestGameLogInfoManager() : m_nMasterCID( 0 ), m_nScenarioID( 0 ), m_dwStartTime( 0 ), m_dwEndTime( 0 ), m_nTotalRewardQItemCount( 0 )
{
	memset( m_szStageName, 0, 64 );
}


CCMatchQuestGameLogInfoManager::~CCMatchQuestGameLogInfoManager()
{
	Clear();
}


void CCMatchQuestGameLogInfoManager::AddQuestPlayer( const CCUID& uidPlayer, CCMatchObject* pPlayer )
{
	if( (0 == pPlayer) || (uidPlayer != pPlayer->GetUID()) )
		return;

	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = new CCQuestPlayerLogInfo;
	if( 0 == pQuestPlayerLogInfo )
		return;

	pQuestPlayerLogInfo->SetCID( pPlayer->GetCharInfo()->m_nCID );

	insert( value_type(uidPlayer, pQuestPlayerLogInfo) );
}

///
// ����Ʈ�� Ŭ�����ϰ� ������� �������� �����.
// ������ �α׿� �����.
///
bool CCMatchQuestGameLogInfoManager::AddRewardQuestItemInfo( const CCUID& uidPlayer, CCQuestItemMap* pObtainQuestItemList )
{
	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = Find( uidPlayer );
	if( 0 == pQuestPlayerLogInfo )
		return false;

	if( 0 == pObtainQuestItemList )
		return false;

	// ������ ���ؼ� ����Ʈ �����ۿ� ���õ� ��� ������ �����.
	pQuestPlayerLogInfo->ClearQItemInfo();

	CCQuestItemMap::iterator itQItem, endQItem;
	endQItem = pObtainQuestItemList->end();
	for( itQItem = pObtainQuestItemList->begin(); itQItem != endQItem; ++itQItem )
	{
		if( GetQuestItemDescMgr().FindQItemDesc(itQItem->second->GetItemID())->m_bUnique )
			pQuestPlayerLogInfo->AddUniqueItem( itQItem->second->GetItemID(), itQItem->second->GetCount() );

		m_nTotalRewardQItemCount += itQItem->second->GetCount();
	}

	return true;
}

bool CCMatchQuestGameLogInfoManager::AddRewardZItemInfo( const CCUID& uidPlayer, CCQuestRewardZItemList* pObtainZItemList )
{
	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = Find( uidPlayer );
	if (( 0 == pQuestPlayerLogInfo ) || ( 0 == pObtainZItemList )) return false;

	for(CCQuestRewardZItemList::iterator itor = pObtainZItemList->begin(); itor != pObtainZItemList->end(); ++itor )
	{
		RewardZItemInfo iteminfo = (*itor);
		CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(iteminfo.nItemID);
		if (pItemDesc == NULL) continue;

		// ����ũ �α� ����
		pQuestPlayerLogInfo->AddUniqueItem( iteminfo.nItemID, 1);
	}

	return true;
}

void CCMatchQuestGameLogInfoManager::Clear()
{
	if( empty() )
		return;

	// Player����.
	CCMatchQuestGameLogInfoManager::iterator It, End;
	for( It = begin(), End = end(); It != End; ++It )
		delete It->second;
	
	clear();

	m_nTotalRewardQItemCount = 0;
}

CCQuestPlayerLogInfo* CCMatchQuestGameLogInfoManager::Find( const CCUID& uidPlayer )
{
	CCMatchQuestGameLogInfoManager::iterator It = find( uidPlayer );
	if( end() == It )
		return 0;

	CCQuestPlayerLogInfo* pQuestPlayerLogInfo = It->second;
	if( 0 == pQuestPlayerLogInfo )
		return 0;

	return pQuestPlayerLogInfo;
}


///
// First : 2005.04.18 �߱���.
// Last  : 2005.04.18 �߱���.
//
// ����Ʈ�� �Ϸ�Ǹ� ������ִ� ������ ������ ��� �α׸� ����� �۾��� ��.
///
bool CCMatchQuestGameLogInfoManager::PostInsertQuestGameLog()
{
	const int nElapsedPlayTime = (m_dwEndTime - m_dwStartTime) / 60000; // �д����� ����� ��.

	CCAsyncDBJob_InsertQuestGameLog* pAsyncDbJob_InsertGameLog = new CCAsyncDBJob_InsertQuestGameLog;
	if( 0 == pAsyncDbJob_InsertGameLog )
		return false;

	pAsyncDbJob_InsertGameLog->Input( m_szStageName, 
									  m_nScenarioID, 
									  m_nMasterCID, 
									  this,
                                      m_nTotalRewardQItemCount, 
									  nElapsedPlayTime );

	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob_InsertGameLog );

	return true;
}


void CCMatchQuestGameLogInfoManager::SetStageName( const char* pszStageName )
{
	if( (0 == pszStageName) || (64 < strlen(pszStageName)) )
		return;

	strcpy( m_szStageName, pszStageName );
}