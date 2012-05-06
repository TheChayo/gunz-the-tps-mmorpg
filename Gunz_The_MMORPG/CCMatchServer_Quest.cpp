#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCMatchStage.h"
#include "CCMatchRule.h"
#include "CCMatchRuleQuest.h"
#include "CCMatchGameType.h"
#include "CCMatchConfig.h"
#include "CCBlobArray.h"
#include "CCMatchShop.h"
#include "CCAsyncDBJob_BuyQuestItem.h"

void CCMatchServer::OnRequestNPCDead(const CCUID& uidSender, const CCUID& uidKiller, CCUID& uidNPC, CCVector& pos)
{
	CCMatchObject* pSender = GetObject(uidSender);
	if (!IsEnabledObject(pSender)) { ASSERT( 0 ); return; }

	CCMatchStage* pStage = FindStage(pSender->GetStageUID());
	if (pStage == NULL) { ASSERT( 0 ); return; }

	CCMatchCharBattleTimeRewardInfoMap::iterator iter = pSender->GetCharInfo()->GetBRInfoMap().begin();
	for( ; iter !=  pSender->GetCharInfo()->GetBRInfoMap().end(); iter++ ) {
		CCMatchCharBRInfo* pInfo = iter->second;
		pInfo->AddKillCount(1);	///< NPC Kill�� 1 Kill�� ���!
	}

	if (MGetGameTypeMgr()->IsQuestDerived(pStage->GetStageSetting()->GetGameType()))
	{
		CCMatchRule* pRule = pStage->GetRule();
		if( NULL == pRule ) return;
		if (false == MGetGameTypeMgr()->IsQuestDerived( pRule->GetGameType() )) return;

		CCMatchRuleBaseQuest* pQuestRule  = reinterpret_cast< CCMatchRuleBaseQuest* >( pRule );

		pQuestRule->OnRequestNPCDead((CCUID&)uidSender, (CCUID&)uidKiller, uidNPC, pos);
	}
	else
	{
		ASSERT( 0 );
	}
}


void CCMatchServer::OnQuestRequestDead(const CCUID& uidVictim)
{
	CCMatchObject* pVictim = GetObject(uidVictim);
	if (pVictim == NULL) return;

	CCMatchStage* pStage = FindStage(pVictim->GetStageUID());
	if (pStage == NULL) return;

	if ( !MGetGameTypeMgr()->IsQuestDerived(pStage->GetStageSetting()->GetGameType())) return;

	CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
	pQuestRule->OnRequestPlayerDead((CCUID&)uidVictim);

	// ������ ������ �˰��־��µ� �� �׾��ٰ� �Ű���°�� �׾��ٴ� �޽����� ������Ѵ�
	if (pVictim->CheckAlive() == false) {	
		CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SUICIDE, CCUID(0,0));
		int nResult = MOK;
		pNew->AddParameter(new CCCommandParameterInt(nResult));
		pNew->AddParameter(new CCCommandParameterUID(pVictim->GetUID()));
		RouteToBattle(pStage->GetUID(), pNew);
		return;
	}

	pVictim->OnDead();


	// �׾��ٴ� �޼��� ����
	CCCommand* pCmd = CreateCommand(MC_MATCH_QUEST_PLAYER_DEAD, CCUID(0,0));
	pCmd->AddParameter(new CCCommandParameterUID(uidVictim));
	RouteToBattle(pStage->GetUID(), pCmd);	
}



void CCMatchServer::OnQuestTestRequestNPCSpawn(const CCUID& uidPlayer, int nNPCType, int nNPCCount)
{
#ifdef _DEBUG
	if (MGetServerConfig()->GetServerMode() != CSM_TEST) return;

	CCMatchObject* pPlayer = GetObject(uidPlayer);
	if (pPlayer == NULL) return;
	CCMatchStage* pStage = FindStage(pPlayer->GetStageUID());
	if (pStage == NULL) return;

#ifndef _DEBUG
	if (!IsAdminGrade(pPlayer)) return;
#endif

	if (MGetGameTypeMgr()->IsQuestDerived(pStage->GetStageSetting()->GetGameType()))
	{
		CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
		pQuestRule->OnRequestTestNPCSpawn(nNPCType, nNPCCount);
	}
#endif
}

void CCMatchServer::OnQuestTestRequestClearNPC(const CCUID& uidPlayer)
{
#ifndef _DEBUG
	return;
#endif

	if (MGetServerConfig()->GetServerMode() != CSM_TEST) return;

	CCMatchObject* pPlayer = GetObject(uidPlayer);
	if (pPlayer == NULL) return;
	CCMatchStage* pStage = FindStage(pPlayer->GetStageUID());
	if (pStage == NULL) return;

#ifndef _DEBUG
	if (!IsAdminGrade(pPlayer)) return;
#endif

	if (MGetGameTypeMgr()->IsQuestDerived(pStage->GetStageSetting()->GetGameType()))
	{
		CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
		pQuestRule->OnRequestTestClearNPC();
	}

}


void CCMatchServer::OnQuestTestRequestSectorClear(const CCUID& uidPlayer)
{
#ifndef _DEBUG
	return;
#endif

	if (MGetServerConfig()->GetServerMode() != CSM_TEST) return;

	CCMatchObject* pPlayer = GetObject(uidPlayer);
	if (pPlayer == NULL) return;
	CCMatchStage* pStage = FindStage(pPlayer->GetStageUID());
	if (pStage == NULL) return;

#ifndef _DEBUG
	if (!IsAdminGrade(pPlayer)) return;
#endif

	if (MGetGameTypeMgr()->IsQuestDerived( pStage->GetStageSetting()->GetGameType() ))
	{
		CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
		pQuestRule->OnRequestTestSectorClear();
	}

}

void CCMatchServer::OnQuestTestRequestQuestFinish(const CCUID& uidPlayer)
{
#ifndef _DEBUG
	return;
#endif

	if (MGetServerConfig()->GetServerMode() != CSM_TEST) return;

	CCMatchObject* pPlayer = GetObject(uidPlayer);
	if (pPlayer == NULL) return;
	CCMatchStage* pStage = FindStage(pPlayer->GetStageUID());
	if (pStage == NULL) return;

#ifndef _DEBUG
	if (!IsAdminGrade(pPlayer)) return;
#endif

	if (MGetGameTypeMgr()->IsQuestDerived( pStage->GetStageSetting()->GetGameType() ))
	{
		CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
		pQuestRule->OnRequestTestFinish();
	}

}



void CCMatchServer::OnQuestRequestMovetoPortal(const CCUID& uidPlayer)
{
	CCMatchObject* pPlayer = GetObject(uidPlayer);
	if (pPlayer == NULL) return;
	CCMatchStage* pStage = FindStage(pPlayer->GetStageUID());
	if (pStage == NULL) return;

	if (false == MGetGameTypeMgr()->IsQuestDerived( pStage->GetStageSetting()->GetGameType() )) return;

	CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
	pQuestRule->OnRequestMovetoPortal(uidPlayer);
}

void CCMatchServer::OnQuestReadyToNewSector(const CCUID& uidPlayer)
{
	CCMatchObject* pPlayer = GetObject(uidPlayer);
	if (pPlayer == NULL) return;
	CCMatchStage* pStage = FindStage(pPlayer->GetStageUID());
	if (pStage == NULL) return;

	if (false == MGetGameTypeMgr()->IsQuestDerived( pStage->GetStageSetting()->GetGameType() )) return;

	CCMatchRuleBaseQuest* pQuestRule = (CCMatchRuleBaseQuest*)pStage->GetRule();
	pQuestRule->OnReadyToNewSector(uidPlayer);

}


void CCMatchServer::OnRequestCharQuestItemList( const CCUID& uidSender )
{
	if( CSM_TEST != MGetServerConfig()->GetServerMode() ) 
		return;

	OnResponseCharQuestItemList( uidSender );
}
void CCMatchServer::OnResponseCharQuestItemList( const CCUID& uidSender )
{
	CCMatchObject* pPlayer = GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
		return;

	// ������ ��� �＼���� ���߾����� ��񿡼� ����Ʈ ������ ������ �����´�
	if( !pPlayer->GetCharInfo()->m_QuestItemList.IsDoneDbAccess() )
	{
		if( !m_MatchDBMgr.GetCharQuestItemInfo(pPlayer->GetCharInfo()) )
		{
			cclog( "DB Query(ResponseCharacterItemList > GetcharQuestItemInfo) failed\n" );
			return;
		}
	}

	CCCommand* pNewCmd = CreateCommand( MC_MATCH_RESPONSE_CHAR_QUEST_ITEM_LIST, CCUID(0, 0) );
	if( 0 == pNewCmd )
	{
		cclog( "CCMatchServer::OnResponseCharQuestItemList - Command���� ����.\n" );
		return;
	}

	// ���� �ִ� ����Ʈ ������ ����Ʈ ����.
	int					nIndex			= 0;
	CCTD_QuestItemNode*	pQuestItemNode	= 0;
	void*				pQuestItemArray = CCMakeBlobArray( static_cast<int>(sizeof(CCTD_QuestItemNode)), 
														  static_cast<int>(pPlayer->GetCharInfo()->m_QuestItemList.size()) );

	CCQuestItemMap::iterator itQItem, endQItem;
	endQItem = pPlayer->GetCharInfo()->m_QuestItemList.end();
	for( itQItem = pPlayer->GetCharInfo()->m_QuestItemList.begin(); itQItem != endQItem; ++itQItem )
	{
		pQuestItemNode = reinterpret_cast< CCTD_QuestItemNode* >( CCGetBlobArrayElement(pQuestItemArray, nIndex++) );
		Make_MTDQuestItemNode( pQuestItemNode, itQItem->second->GetItemID(), itQItem->second->GetCount() );
	}

	pNewCmd->AddParameter( new CCCommandParameterBlob(pQuestItemArray, CCGetBlobArraySize(pQuestItemArray)) );
	CCEraseBlobArray( pQuestItemArray );

	RouteToListener( pPlayer, pNewCmd );
}

void CCMatchServer::OnRequestBuyQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nItemCount )
{
	if (MGetServerConfig()->GetServerMode() == CSM_TEST) {
		OnResponseBuyQuestItem( uidSender, nItemID, nItemCount );
	}
}
void CCMatchServer::OnResponseBuyQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nItemCount )
{
	CCMatchObject* pPlayer = GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) ) return;

	CCQuestItemDescManager::iterator itQItemDesc = GetQuestItemDescMgr().find( nItemID );
	if( GetQuestItemDescMgr().end() == itQItemDesc ) {
		cclog( "CCMatchServer::OnResponseBuyQuestItem - %d������ description�� ã�� ���߽��ϴ�.\n", nItemID );
		return;
	}

	CCQuestItemDesc* pQuestItemDesc = itQItemDesc->second;
	if( 0 == pQuestItemDesc ) {
		cclog( "CCMatchServer::OnRequestBuyQuestItem - %d�� item description�� ���������Դϴ�.\n", nItemID );
		return;
	}

	// �������� �Ǹŵǰ� �ִ� ���������� �˻�.
	if( !MGetMatchShop()->IsSellItem(pQuestItemDesc->m_nItemID) ) {
		cclog( "CCMatchServer::OnRequestBuyQuestItem - %d�� �������� �Ǹŵǰ� �ִ� �������� �ƴ�.\n", pQuestItemDesc->m_nItemID );
		return;
	}

	// ����� �ٿ�Ƽ�� �Ǵ��� �˻�.
	if( pPlayer->GetCharInfo()->m_nBP < (itQItemDesc->second->m_nPrice * nItemCount) ) {
		// �ٿ�Ƽ�� �����Ѵٴ� ������ �˷���� ��.
		// �ӽ÷� CCMatchItem���� ����ϴ°� �������.
		// �ʿ��ϸ� Quest item�� �´� Ŀ�ǵ�� �����ؾ� ��.
		CCCommand* pBPLess = CreateCommand( MC_MATCH_RESPONSE_BUY_QUEST_ITEM, CCUID(0,0) );
		pBPLess->AddParameter( new CCCmdParamInt(MERR_TOO_EXPENSIVE_BOUNTY) );
		pBPLess->AddParameter( new CCCmdParamInt(pPlayer->GetCharInfo()->m_nBP) );
		RouteToListener(pPlayer, pBPLess);
		return;
	}

	CCQuestItemMap::iterator itQItem = pPlayer->GetCharInfo()->m_QuestItemList.find( nItemID );
	if( pPlayer->GetCharInfo()->m_QuestItemList.end() != itQItem ) 
	{
		// �ִ� ������ �Ѵ��� �˻�.
		if( MAX_QUEST_ITEM_COUNT > itQItem->second->GetCount() + nItemCount ) {			
			itQItem->second->Increase(nItemCount); // ���� ����
		} else {
			// ������ �ִ� �������� �ִ� ���� �Ѿ��.
			// �ӽ÷� CCMatchItem���� ����ϴ°� �������. �ʿ��ϸ� Quest item�� �´� Ŀ�ǵ�� �����ؾ� ��.
			CCCommand* pTooMany = CreateCommand( MC_MATCH_RESPONSE_BUY_QUEST_ITEM, CCUID(0,0) );
			pTooMany->AddParameter( new CCCmdParamInt(MERR_TOO_MANY_ITEM) );
			pTooMany->AddParameter( new CCCmdParamInt(pPlayer->GetCharInfo()->m_nBP) );
			RouteToListener(pPlayer, pTooMany);
			return;
		}
	} else {
		CCQuestItem* pNewQuestItem = new CCQuestItem;
		if( 0 == pNewQuestItem ) {
			cclog( "CCMatchServer::OnResponseBuyQuestItem - ���ο� ����Ʈ ������ ���� ����.\n" );
			return;
		}

		if( !pNewQuestItem->Create(nItemID, nItemCount, GetQuestItemDescMgr().FindQItemDesc(nItemID)) ) {
			delete pNewQuestItem;
			cclog( "CCMatchServer::OnResponseBuyQeustItem - %d��ȣ ������ Create( ... )�Լ� ȣ�� ����.\n" );
			return;
		}

		pPlayer->GetCharInfo()->m_QuestItemList.insert( CCQuestItemMap::value_type(nItemID, pNewQuestItem) );
	}

	UpdateCharDBCachingData(pPlayer);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
	//UpdateCharItemDBCachingData(pPlayer);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ <-���ʿ���


	CCAsyncDBJob_BuyQuestItem* pBuyQuestItemJob = new CCAsyncDBJob_BuyQuestItem( uidSender );
	if( NULL == pBuyQuestItemJob ) { return; }	
	pBuyQuestItemJob->Input( uidSender, pPlayer->GetCharInfo()->m_nCID, nItemCount, pQuestItemDesc->m_nPrice);	// ��� �ٿ�Ƽ �����ش�
	pPlayer->m_DBJobQ.DBJobQ.push_back( pBuyQuestItemJob );
}

void CCMatchServer::OnRequestSellQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nCount )
{
	if (MGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		OnResponseSellQuestItem( uidSender, nItemID, nCount );
	}
}

void CCMatchServer::OnResponseSellQuestItem( const CCUID& uidSender, const unsigned long int nItemID, const int nCount )
{
	CCMatchObject* pPlayer = GetObject( uidSender );
	if( !IsEnabledObject(pPlayer) )
	{
		cclog( "CCMatchServer::OnResponseSellQuestItem - find user fail.\n" );
		return;
	}

	CCQuestItemDescManager::iterator itQItemDesc = GetQuestItemDescMgr().find( nItemID );
	if( GetQuestItemDescMgr().end() == itQItemDesc )
	{
		cclog( "CCMatchServer::OnResponseSellQuestItem - find item(%u) description fail.\n", nItemID );
		return;
	}
	
	CCQuestItemDesc* pQItemDesc = itQItemDesc->second;
	if( 0 == pQItemDesc )
	{
		cclog( "CCMatchServer::OnResponseSellQuestItem - item(%u) description is null point.\n", nItemID );
		return;
	}

	// ������ ī��Ʈ �˻�.
	CCQuestItemMap::iterator itQItem = pPlayer->GetCharInfo()->m_QuestItemList.find( nItemID );
	if( pPlayer->GetCharInfo()->m_QuestItemList.end() != itQItem ) {
		if( nCount > itQItem->second->GetCount() ) {
			return;
		}

		UpdateCharDBCachingData(pPlayer);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
		//UpdateCharItemDBCachingData(pPlayer);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ <-���ʿ���

		// ��� �ٿ�Ƽ �����ش�
		int nPrice = ( pQItemDesc->GetSellBountyValue(nCount) );
		if (!m_MatchDBMgr.UpdateCharBP(pPlayer->GetCharInfo()->m_nCID, nPrice))	{
			/*
			CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SELL_ITEM, CCUID(0,0));
			pNew->AddParameter(new CCCmdParamInt(MERR_CANNOT_SELL_ITEM));
			RouteToListener(pObj, pNew);

			return false;
			*/
			return;
		}

		itQItem->second->Decrease( nCount );
		pPlayer->GetCharInfo()->m_nBP += nPrice;		// ���Ƚô� 1/4�� ������ ����.

	} else {
		// �������� �ʴ� �������� �ȷ��� �Ͽ���.
		cclog( "CCMatchServer::OnResponseSellQuestItem - user is not owner. itemid(%u)\n", nItemID );
		ASSERT( 0 );
		return;
	}

	// ������ �ŷ� ī��Ʈ ����. ���ο��� ��� ������Ʈ ����.
	pPlayer->GetCharInfo()->GetDBQuestCachingData().IncreaseShopTradeCount();

	CCCommand* pCmd = CreateCommand( MC_MATCH_RESPONSE_SELL_QUEST_ITEM, CCUID(0, 0) );
	if( 0 == pCmd ) {
		return;
	}

	pCmd->AddParameter( new CCCmdParamInt(MOK) );
	pCmd->AddParameter( new CCCmdParamInt(pPlayer->GetCharInfo()->m_nBP) );
	RouteToListener( pPlayer, pCmd );

	// ����Ʈ ������ ����Ʈ�� �ٽ� ������.
	OnRequestCharQuestItemList( pPlayer->GetUID() );
}


void CCMatchServer::OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
#ifdef _QUEST_ITEM
	if (MGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		CCMatchObject* pPlayer = GetObject( uidSender );
		if( !IsEnabledObject(pPlayer) )
		{
			cclog( "CCMatchServer::OnRequestDropSacrificeItemOnSlot - invalid user.\n" );
			return;
		}

		CCMatchStage* pStage = FindStage( pPlayer->GetStageUID() );
		if( 0 != pStage )
		{
			if(STAGE_STATE_RUN == pStage->GetState())
				return; // ���ӽ��۽� ����������� ���Կ� �ø��� ���� ��� ���� �߻��Ǳ� ������ ���⼭ ������

			const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
			if( 0 == pNode )
			{
				cclog( "CCMatchServer::OnRequestDropSacrificeItemOnSlot - find stage fail.\n" );
				return;
			}

			if( MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType) )
			{
				CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
				if( 0 == pRuleQuest )
					return;

				pRuleQuest->OnRequestDropSacrificeItemOnSlot( uidSender, nSlotIndex, nItemID );
			}
		}
	}	
#endif
}


void CCMatchServer::OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )
{
#ifdef _QUEST_ITEM
	if (MGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		CCMatchObject* pPlayer = GetObject( uidSender );
		if( !IsEnabledObject(pPlayer) )
		{
			cclog( "CCMatchServer::OnRequestDropSacrificeItemOnSlot - invalid user.\n" );
			return;
		}

		CCMatchStage* pStage = FindStage( pPlayer->GetStageUID() );
		if( 0 != pStage )
		{
			const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
			if( 0 == pNode )
			{
				cclog( "CCMatchServer::OnRequestCallbackSacrificeItem - find stage fail.\n" );
				return;
			}

			if( MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType) )
			{
				CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
				if( 0 == pRuleQuest )
					return;

				pRuleQuest->OnRequestCallbackSacrificeItem( uidSender, nSlotIndex, nItemID );
			}
		}
	}
#endif
}

void CCMatchServer::OnRequestQL( const CCUID& uidSender )
{
#ifdef _QUEST_ITEM
	if (MGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		CCMatchObject* pPlayer = GetObject( uidSender );
		if( !IsEnabledObject(pPlayer) )
		{
			cclog( "CCMatchServer::OnRequestQL - invlaid user.\n" );
			return;
		}

		CCMatchStage* pStage = FindStage( pPlayer->GetStageUID() );
		if( 0 != pStage )
		{
			const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
			if( 0 == pNode )
			{
				cclog( "CCMatchServer::OnRequestQL - find stage fail.\n" );
				return;
			}

			if( MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType) )
			{
				CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
				if( 0 == pRuleQuest )
					return;

				pRuleQuest->OnRequestQL( uidSender );
			}
		}
	}	
#endif
}


void CCMatchServer::OnRequestSacrificeSlotInfo( const CCUID& uidSender )
{
#ifdef _QUEST_ITEM
	if (MGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		CCMatchObject* pPlayer = GetObject( uidSender );
		if( !IsEnabledObject(pPlayer) )
		{
			cclog( "CCMatchServer::OnRequestSacrificeSlotInfo - invalid user.\n" );
			return;
		}

		CCMatchStage* pStage = FindStage( pPlayer->GetStageUID() );
		if( 0 != pStage )
		{
			const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
			if( 0 == pNode )
			{
				cclog( "CCMatchServer::OnRequestSacrificeSlotInfo - find stage fail.\n" );
				return;
			}

			if( MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType) )
			{
				CCMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< CCMatchRuleBaseQuest* >( pStage->GetRule() );
				if( 0 == pRuleQuest )
					return;

				pRuleQuest->OnRequestSacrificeSlotInfo( uidSender );
			}
		}
	}
#endif
}

void CCMatchServer::OnQuestStageMapset(const CCUID& uidStage, int nMapsetID)
{
	if (QuestTestServer())
	{



	}
}


void CCMatchServer::OnRequestMonsterBibleInfo( const CCUID& uidSender )
{
	if (MGetServerConfig()->GetServerMode() == CSM_TEST)
	{
		CCMatchObject* pPlayer = GetObject( uidSender );
		if( !IsEnabledObject(pPlayer) )
		{
			cclog( "CCMatchServer::OnRequestMonsterBibleInfo - invalid user.\n" );
			return;
		}

		OnResponseMonsterBibleInfo( uidSender );
	}
}


void CCMatchServer::OnResponseMonsterBibleInfo( const CCUID& uidSender )
{
	CCMatchObject* pObj = GetObject( uidSender );
	if( !IsEnabledObject(pObj) )
		return;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if( 0 == pCharInfo )
		return;

	// ��񿡼� �ɸ��� ����Ʈ ���������� �������ִ��� �˻縦 ��.
	if( !pCharInfo->m_QuestItemList.IsDoneDbAccess() )
	{
		cclog( "CCMatchServer::OnResponseMonsterBibleInfo - not load db monsterbible info.\n" );
		return;
	}	

	void* pMonBibleInfoBlob = CCMakeBlobArray( MONSTER_BIBLE_SIZE, 1 );
	if( 0 == pMonBibleInfoBlob )
	{
		cclog( "CCMatchServer::OnResponseMonsterBibleInfo - make blob fail.\n" );
		return;
	}

	CCQuestMonsterBible* pMonBible = reinterpret_cast< CCQuestMonsterBible * >( CCGetBlobArrayElement(pMonBibleInfoBlob, 0) );
	if( 0 == pMonBible )
	{
		cclog( "CCMatchServer::OnResponseMonsterBibleInfo - typecast fail.\n" );
		return;
	}

	memcpy( pMonBible, &(pCharInfo->m_QMonsterBible), MONSTER_BIBLE_SIZE );


	CCCommand* pCmd = CreateCommand( MC_MATCH_RESPONSE_MONSTER_BIBLE_INFO, CCUID(0, 0) );
	if( 0 == pCmd )
	{
		cclog( "CCMatchServer::OnResponseMonsterBibleInfo - create command fail.\n" );
		return;
	}

	pCmd->AddParameter( new CCCmdParamUID(uidSender) );
	pCmd->AddParameter( new CCCommandParameterBlob(pMonBibleInfoBlob, CCGetBlobArraySize(pMonBibleInfoBlob)) );

	RouteToListener( pObj, pCmd );

	CCEraseBlobArray( pMonBibleInfoBlob );
}


void CCMatchServer::OnQuestPong( const CCUID& uidSender )
{
	CCMatchObject* pObj = GetObject( uidSender );
	if( 0 == pObj ) 
		return;

	pObj->SetQuestLatency(GetGlobalClockCount());
	pObj->m_bQuestRecvPong = true;
}