#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCAsyncDBJob.h"
#include "CCSharedCommandTable.h"
#include "CCBlobArray.h"
#include "CCMatchConfig.h"
#include "CCMatchQuestGameLog.h"

void CCAsyncDBJob_Test::Run(void* pContext)
{
	char szLog[128];
	sprintf(szLog, "Thread=%d , CCAsyncDBJob_Test BEGIN \n", GetCurrentThreadId());
	cclog(szLog);

	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetDatabase()->IsOpen()) {
		cclog("m_DB is not opened \n");
	}
	
	CString strSQL;
	strSQL.Format("SELECT TOP 5 * from Character");

	CODBCRecordset rs(pDBMgr->GetDatabase());

	bool bException = false;
	try 
	{
		rs.Open(strSQL, CRecordset::forwardOnly, CRecordset::readOnly);
	} 
	catch(CDBException* e)
	{
		bException = true;
		cclog("AsyncTest: Exception rs.Open \n");
		cclog(e->m_strError);
	}

	try
	{
		while(!rs.IsEOF()) {
			rs.MoveNext();
		}
	}	
	catch(CDBException* e)
	{
		bException = true;
		cclog(e->m_strError);
		cclog("AsyncTest: Exception rs.MoveNext \n");
	}

	int nCount = rs.GetRecordCount();

	sprintf(szLog, "Thread=%d , CCAsyncDBJob_Test END RecordCount=%d \n", 
			GetCurrentThreadId(), nCount);
	cclog(szLog);
}

void CCAsyncDBJob_GetAccountCharList::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetAccountCharList(m_nAID, m_CharList, &m_nCharCount))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_GetCharInfo::Run(void* pContext)
{
	_ASSERT(m_pCharInfo);
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	//int nWaitHourDiff;

	if (!pDBMgr->GetCharInfoByAID(m_nAID, m_nCharIndex, m_pCharInfo)) {
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	unsigned int nEquipedItemID[MMCIP_END];
	unsigned int nEquipedItemCIID[MMCIP_END];

	if( !pDBMgr->GetCharEquipmentInfoByAID(m_nAID, m_nCharIndex, nEquipedItemID, nEquipedItemCIID)) {
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	for(int i = 0; i < MMCIP_END; i++) {
		m_pCharInfo->m_nEquipedItemCIID[i] = nEquipedItemCIID[i];
	}

#ifdef _DELETE_CLAN
	// Ŭ���� ���Ե� ĳ�����̸� ����û�� Ŭ������ �˻縦 ����� �Ѵ�.
	if( 0 != m_pCharInfo->m_ClanInfo.m_nClanID ) 
	{
		/*
		// nWaitHourDiff�� ���� 0�̸� ���� Ŭ��.
		// 0 �̻��̸� ����û�� ������ Ŭ���̴�. 
		// ��� ��û�� Ŭ���� ���� Ŭ������ �ٲ��ֱ� ���ؼ���, 
		//  Clan���̺��� DeleteDate�� NULL�� ������ ��� �Ѵ�. - by SungE.

		if( UNDEFINE_DELETE_HOUR == nWaitHourDiff )
		{
			// ���� Ŭ��.
		}
		else if( 0 > nWaitHourDiff )
		{
			SetDeleteState( MMCDS_WAIT );
		}
		//else if( MAX_WAIT_CLAN_DELETE_HOUR < nWaitHourDiff )
		//{
		//	// Ŭ�������� DB���� ����.
		//	// ���۾��� DB�� Agent server�۾����� �ϰ� ó�����Ѵ�.
		//}
		else if( 0 <= nWaitHourDiff) 
		{
			// ���� DB�� �������� �ʰ�, ������ �Ϲ� ������ ó����.
			SetDeleteState( MMCDS_NORMAL );
			m_pCharInfo->m_ClanInfo.Clear();
		}
		*/
	}
#endif

	// ��񿡼� ������ ������ �����´�. 
	// �̰��� �����ս� ������ ���߿� �÷��̾ �ڱ� ������ ���� �Ҷ��� �����;� �ҵ�	
	m_pCharInfo->ClearItems();
	if (!pDBMgr->GetCharItemInfo(m_pCharInfo))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

#ifdef _QUEST_ITEM
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		m_pCharInfo->m_QuestItemList.Clear();
		if( !pDBMgr->GetCharQuestItemInfo(m_pCharInfo) )
		{
			cclog( "CCAsyncDBJob_GetCharInfo::Run - ��񿡼� ����Ʈ ������ ����� �������µ� ��������.\n" );
			SetResult(CCASYNC_RESULT_FAILED);
			return;
		}
	}
#endif

	if( !pDBMgr->GetCharBRInfoAll(m_pCharInfo->m_nCID, m_pCharInfo->GetBRInfoMap()) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}



void CCAsyncDBJob_UpdateCharClanContPoint::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;


	if (!pDBMgr->UpdateCharClanContPoint(m_nCID, m_nCLID, m_nAddedContPoint))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}


	SetResult(CCASYNC_RESULT_SUCCEED);
}



void CCAsyncDBJob_GetAccountCharInfo::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->GetAccountCharInfo(m_nAID, m_nCharNum, &m_CharInfo)) {
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	unsigned int nEquipedItemID[MMCIP_END];
	unsigned int nEquipedItemCIID[MMCIP_END];

	if( !pDBMgr->GetCharEquipmentInfoByAID(m_nAID, m_nCharNum, nEquipedItemID, nEquipedItemCIID)) {
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	for(int i = 0; i < MMCIP_END; i++) {
		m_CharInfo.nEquipedItemDesc[i] = nEquipedItemID[i];
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_CreateChar::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	// ĳ���� ���� �α״� ĳ���͸� �����ϴ� SP�ȿ��� ����.
	if( !pDBMgr->CheckDuplicateCharactername(&m_nResult, m_szCharName) ) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
	}

	if( m_nResult == MOK ) 
	{
		if( !pDBMgr->CreateCharacter(&m_nResult, m_nAID, m_szCharName, m_nCharNum, m_nSex, m_nHair, m_nFace, m_nCostume) ) 
		{
			SetResult(CCASYNC_RESULT_FAILED);
		}
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

void CCAsyncDBJob_DeleteChar::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->DeleteCharacter(m_nAID, m_nCharNum, m_szCharName))
	{
		m_nDeleteResult = MERR_CANNOT_DELETE_CHAR;
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	m_nDeleteResult = MOK;
	SetResult(CCASYNC_RESULT_SUCCEED);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CCAsyncDBJob_InsertGameLog::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;
/*
	pDBMgr->InsertGameLog(m_szGameName, m_szMap, 
							m_szGameType,
							m_nRound,
							m_nMasterCID,
							m_nPlayerCount,
							m_szPlayers);
*/

	if( !pDBMgr->InsertGameLog(m_nMasterCID, m_szMap, m_szGameType, &m_nID) )
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

/*
bool CCAsyncDBJob_InsertGameLog::Input(const char* szGameName, const char* szMap, const char* szGameType,const int nRound, 
										const unsigned int nMasterCID, const int nPlayerCount, const char* szPlayers)
{
	strcpy(m_szGameName, szGameName);
	strcpy(m_szMap, szMap);
	strcpy(m_szGameType, szGameType);
	m_nRound = nRound;
	m_nMasterCID = nMasterCID;
	m_nPlayerCount = nPlayerCount;
	strcpy(m_szPlayers, szPlayers);

	return true;
}
*/
bool CCAsyncDBJob_InsertGameLog::Input(const unsigned int nMasterCID, const char* szMap, const char* szGameType)
{
	m_nMasterCID = nMasterCID;

	strcpy(m_szMap, szMap);
	strcpy(m_szGameType, szGameType);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CCAsyncDBJob_CreateClan::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	if (!pDBMgr->CreateClan(m_szClanName, 
							m_nMasterCID, 
							m_nMember1CID, 
							m_nMember2CID, 
							m_nMember3CID, 
							m_nMember4CID, 
							&m_bDBResult, 
							&m_nNewCLID))
	{
		SetResult(CCASYNC_RESULT_FAILED);

		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

bool CCAsyncDBJob_CreateClan::Input(const TCHAR* szClanName, 
									const int nMasterCID, 
									const int nMember1CID, 
									const int nMember2CID,
									const int nMember3CID, 
									const int nMember4CID,
									const CCUID& uidMaster,
									const CCUID& uidMember1,
									const CCUID& uidMember2,
									const CCUID& uidMember3,
									const CCUID& uidMember4)
{
	strcpy(m_szClanName, szClanName);
	m_nMasterCID = nMasterCID;
	m_nMember1CID = nMember1CID;
	m_nMember2CID = nMember2CID;
	m_nMember3CID = nMember3CID;
	m_nMember4CID = nMember4CID;

	m_uidMaster = uidMaster;
	m_uidMember1 = uidMember1;
	m_uidMember2 = uidMember2;
	m_uidMember3 = uidMember3;
	m_uidMember4 = uidMember4;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void CCAsyncDBJob_ExpelClanMember::Run(void* pContext)
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	// ������ ���󿡼� ���� ����
	if (!pDBMgr->ExpelClanMember(m_nCLID, m_nClanGrade, m_szTarMember, &m_nDBResult))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}

bool CCAsyncDBJob_ExpelClanMember::Input(const CCUID& uidAdmin, int nCLID, int nClanGrade, const char* szTarMember)
{
	m_uidAdmin = uidAdmin;
	strcpy(m_szTarMember, szTarMember);
	m_nCLID = nCLID;
	m_nClanGrade = nClanGrade;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CCAsyncDBJob_InsertQuestGameLog::~CCAsyncDBJob_InsertQuestGameLog()
{
	vector< CCQuestPlayerLogInfo* >::iterator it, end;
	it = m_Player.begin();
	end = m_Player.end();
	for(; it != end; ++it )
		delete (*it);
}


bool CCAsyncDBJob_InsertQuestGameLog::Input( const char* pszStageName, 
										    const int nScenarioID,
										    const int nMasterCID,
											CCMatchQuestGameLogInfoManager* pQGameLogInfoMgr,
											const int nTotalRewardQItemCount,
											const int nElapsedPlayerTime )
{
	if( 0 == pQGameLogInfoMgr )
		return false;

	strcpy( m_szStageName, pszStageName );

	m_nScenarioID				= nScenarioID;
	m_nMasterCID				= nMasterCID;
    m_nElapsedPlayTime			= nElapsedPlayerTime;
	m_nTotalRewardQItemCount	= nTotalRewardQItemCount;

	int										i;
	CCQuestPlayerLogInfo*					pPlayer;
	CCMatchQuestGameLogInfoManager::iterator itPlayer, endPlayer;
	
	m_Player.clear();
	memset( m_PlayersCID, 0, 12 );

	i = 0;
	itPlayer  = pQGameLogInfoMgr->begin();
	endPlayer = pQGameLogInfoMgr->end();
	for(; itPlayer != endPlayer; ++itPlayer )
	{
		if( nMasterCID != itPlayer->second->GetCID() )
			m_PlayersCID[ i++ ] = itPlayer->second->GetCID();

		if( itPlayer->second->GetUniqueItemList().empty() )
			continue;	// ���������� �׳� ����.
		
		if( 0 == (pPlayer = new CCQuestPlayerLogInfo) )
		{
			continue;
		}

		pPlayer->SetCID( itPlayer->second->GetCID() );
		pPlayer->GetUniqueItemList().insert( itPlayer->second->GetUniqueItemList().begin(),
											 itPlayer->second->GetUniqueItemList().end() );

		m_Player.push_back( pPlayer );

		if( MAX_QUEST_LOG_PLAYER_COUNT <= i )
		{
			_ASSERT( 0 && "�ִ��ο� �ʰ�" );
			break;
		}
	}

	return true;
}


void CCAsyncDBJob_InsertQuestGameLog::Run( void* pContext )
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		CCMatchDBMgr* pDBMgr = reinterpret_cast< CCMatchDBMgr* >( pContext );

		int nQGLID;

		// �켱 ����Ʈ ���� �α׸� ������.
		if( !pDBMgr->InsertQuestGameLog(m_szStageName, 
			m_nScenarioID,
			m_nMasterCID, m_PlayersCID[0], m_PlayersCID[1], m_PlayersCID[2],
			m_nTotalRewardQItemCount,
			m_nElapsedPlayTime,
			nQGLID) )
		{
			SetResult(CCASYNC_RESULT_FAILED);
			return;
		}

		// ����ũ �����ۿ� ���� �����ʹ� QUniqueItecclog�� ���� ������ ����� ��.
		int											i;
		int											nCID;
		int											nQIID;
		int											nQUItemCount;
		QItecclogMapIter								itQUItem, endQUItem;
		vector< CCQuestPlayerLogInfo* >::iterator	itPlayer, endPlayer;

		itPlayer  = m_Player.begin();
		endPlayer = m_Player.end();

		for( ; itPlayer != endPlayer; ++itPlayer )
		{
			if( (*itPlayer)->GetUniqueItemList().empty() )
				continue;	// ����ũ �������� ������ ���� ������ ����.

			nCID		= (*itPlayer)->GetCID();
			itQUItem	= (*itPlayer)->GetUniqueItemList().begin();
			endQUItem	= (*itPlayer)->GetUniqueItemList().end();

			for( ; itQUItem != endQUItem; ++itQUItem )
			{
				nQIID			= itQUItem->first;
				nQUItemCount	= itQUItem->second;

				for( i = 0; i < nQUItemCount; ++i )
				{
					if( !pDBMgr->InsertQUniqueGameLog(nQGLID, nCID, nQIID) )
					{
						cclog( "CCAsyncDBJob_InsertQuestGameLog::Run - ����ũ ������ �α� ���� ����. CID:%d QIID:%d\n", 
							nCID, nQIID );

						SetResult(CCASYNC_RESULT_FAILED);
					}
				}
			}

			// �۾��� ���� ������ �޸𸮿��� �����Ѵ�.
			delete (*itPlayer);
		}
	}

	m_Player.clear();
	
	SetResult(CCASYNC_RESULT_SUCCEED);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CCAsyncDBJob_UpdateQuestItemInfo::~CCAsyncDBJob_UpdateQuestItemInfo()
{
	m_QuestItemList.Clear();
}


bool CCAsyncDBJob_UpdateQuestItemInfo::Input( const int nCID, CCQuestItemMap& QuestItemList, CCQuestMonsterBible& QuestMonster )
{
	m_nCID = nCID;

	CCQuestItemMap::iterator it, end;

	m_QuestItemList.Clear();

	it  = QuestItemList.begin();
	end = QuestItemList.end();

	for( ; it != end; ++it )
	{
		CCQuestItem* pQuestItem = it->second;
		m_QuestItemList.CreateQuestItem( pQuestItem->GetItemID(), pQuestItem->GetCount(), pQuestItem->IsKnown() );
	}

	m_QuestItemList.SetDBAccess( QuestItemList.IsDoneDbAccess() );
	m_QuestMonster.Clear();
	return true;
}


void CCAsyncDBJob_UpdateQuestItemInfo::Run( void* pContext )
{
	if( CSM_TEST == CCGetServerConfig()->GetServerMode() ) 
	{
		if( m_QuestItemList.IsDoneDbAccess() )
		{
			CCMatchDBMgr* pDBMgr = reinterpret_cast< CCMatchDBMgr* >( pContext );
			if( !pDBMgr->UpdateQuestItem(m_nCID, m_QuestItemList, m_QuestMonster) )
			{
				SetResult( CCASYNC_RESULT_FAILED );
				return;
			}
		}
	}

	SetResult( CCASYNC_RESULT_SUCCEED );
}


bool CCAsyncDBJob_SetBlockHacking::Input( const DWORD dwAID
										, const DWORD dwCID
										, const BYTE btBlockType
										, const BYTE btBlockLevel
										, const string& strComment
										, const string& strIP
										, const string& strEndDate
										, const BYTE nServerID
										, const string& strChannelName )
{
	m_dwAID				= dwAID;
	m_dwCID				= dwCID;
	m_btBlockType 		= btBlockType;
	m_btBlockLevel		= btBlockLevel;
	m_strComment		= strComment;
	m_strIP				= strIP;
	m_strEndDate		= strEndDate;
	m_nServerID			= nServerID;
	m_strChannelName	= strChannelName;

	return true;
}

void CCAsyncDBJob_SetBlockHacking::Run( void* pContext )
{
	CCMatchDBMgr* pDBMgr = reinterpret_cast< CCMatchDBMgr* >( pContext );

	if( !pDBMgr->SetBlockHacking(
		m_dwAID
		, m_dwCID
		, m_btBlockType
		, m_strIP
		, m_strEndDate
		, m_nServerID
		, m_strChannelName
		, m_strComment
		, m_btBlockLevel) )
	{
		SetResult( CCASYNC_RESULT_FAILED );
		return;
	}


	//if( MMBL_ACCOUNT == m_btBlockLevel )
	//{
	//	if( !pDBMgr->SetBlockAccount( m_dwAID, m_dwCID, m_btBlockType, m_strComment, m_strIP, m_strEndDate) )
	//	{
	//		SetResult( CCASYNC_RESULT_FAILED );
	//		return;
	//	}
	//}
	//else if( MMBL_LOGONLY == m_btBlockLevel )
	//{
	//	if( pDBMgr->InsertBlockLog(m_dwAID, m_dwCID, m_btBlockType, m_strComment, m_strIP) )
	//	{
	//		SetResult( CCASYNC_RESULT_FAILED );
	//		return;
	//	}
	//}

	SetResult( CCASYNC_RESULT_SUCCEED );
}


bool CCAsyncDBJob_ResetAccountHackingBlock::Input( const DWORD dwAID, const BYTE btBlockType )
{
	m_dwAID			= dwAID;
	m_btBlockType	= btBlockType;

	return true;
}


void CCAsyncDBJob_ResetAccountHackingBlock::Run( void* pContext )
{
	CCMatchDBMgr* pDBMgr = reinterpret_cast< CCMatchDBMgr* >( pContext );

	if( !pDBMgr->ResetAccountHackingBlock(m_dwAID, m_btBlockType) )
	{
		SetResult( CCASYNC_RESULT_FAILED );
		return;
	}

	SetResult( CCASYNC_RESULT_SUCCEED );
}