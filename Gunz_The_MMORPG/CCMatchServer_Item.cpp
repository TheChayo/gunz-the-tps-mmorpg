#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "CCMatchItem.h"
#include "CCAgentObject.h"
#include "CCMatchNotify.h"
#include "Msg.h"
#include "CCMatchObjCache.h"
#include "CCMatchStage.h"
#include "CCMatchTransDataType.h"
#include "CCMatchFormula.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCMatchShop.h"
#include "CCMatchTransDataType.h"
#include "CCDebug.h"	
#include "CCMatchAuth.h"
#include "CCMatchStatus.h"
#include "CCAsyncDBJob.h"
#include "CCAsyncDBJob_GetAccountItemList.h"


// �÷��̾�� ������ ����
bool CCMatchServer::DistributeZItem(const CCUID& uidPlayer, const unsigned long int nItemID, bool bRentItem, int nRentPeriodHour, int nItemCount)
{
	CCMatchObject* pObject = GetObject(uidPlayer);
	if (!IsEnabledObject(pObject)) return false;

	CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if ( pItemDesc == NULL ) return false;

	if ( !pItemDesc->IsSpendableItem() ) 
	{
		unsigned long int nNewCIID = 0;
		if (!m_MatchDBMgr.InsertDistributeItem(pObject->GetCharInfo()->m_nCID, nItemID, bRentItem, nRentPeriodHour, &nNewCIID)) return false;

		// ������Ʈ�� ������ �߰�
		int nRentMinutePeriodRemainder = nRentPeriodHour * 60;

		CCUID uidNew = CCMatchItemMap::UseUID();
		pObject->GetCharInfo()->m_ItemList.CreateItem(uidNew, nNewCIID, nItemID, bRentItem, nRentMinutePeriodRemainder, nRentPeriodHour);
	}
	else 
	{
		// TodoH(��)
		// �ϴ� �Ҹ� �������� ������ ���� �������� �ʴ´�.

		return false;
	}

	return true;
}


// ���� ���� ������Ʈ���� �������� ����
bool CCMatchServer::RemoveExpiredCharItem(CCMatchObject* pObject, CCUID& uidItem)
{
	CCMatchItem* pItem = pObject->GetCharInfo()->m_ItemList.GetItem(uidItem);
	if (pItem == NULL) return false;

	// ��񿡼� ������ ����
	if (!m_MatchDBMgr.DeleteExpiredCharItem(pObject->GetCharInfo()->m_nCID, pItem->GetCIID())) {
		return false;
	}

	// ���� ������̸� ��ü
	CCMatchCharItemParts nCheckParts = MMCIP_END;
	if (pObject->GetCharInfo()->m_EquipedItem.IsEquipedItem(uidItem, nCheckParts)) {
		pObject->GetCharInfo()->m_EquipedItem.Remove(nCheckParts);
	}

	// ������Ʈ���� ������ ����
	pObject->GetCharInfo()->m_ItemList.RemoveItem(uidItem);

	return true;
}


void CCMatchServer::OnRequestAccountItemList(const CCUID& uidPlayer)
{
	ResponseAccountItemList(uidPlayer);
}
void CCMatchServer::ResponseAccountItemList(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL) || (pObj->GetAccountInfo() == NULL)) return;

	CCAsyncDBJob_GetAccountItemList* pGetAccountItemListJob = new CCAsyncDBJob_GetAccountItemList(uidPlayer);
	if( NULL == pGetAccountItemListJob ) return;

	pGetAccountItemListJob->Input( uidPlayer, pObj->GetAccountInfo()->m_nAID );
	pObj->m_DBJobQ.DBJobQ.push_back( pGetAccountItemListJob );
}


//void CCMatchServer::ResponseTakeoffItem(const CCUID& uidPlayer, const CCMatchCharItemParts parts)
//{
//	CCMatchObject* pObj = GetObject(uidPlayer);
//	if (pObj == NULL) return;
//	int nResult = MOK;
//
//	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
//	if (pCharInfo == NULL) return;
//	if (pCharInfo->m_EquipedItem.IsEmpty(parts)) return;
//
//
//	
//	CCMatchItem* pItem = pCharInfo->m_EquipedItem.GetItem(parts);
//	if( NULL == pItem )
//	{
//		return;
//	}
//
//	CCMatchItemDesc* pItemDesc = pItem->GetDesc();
//	if (pItemDesc == NULL) 
//	{
//		return;
//	}
//
//	// ���� üũ
//	int nWeight=0, nMaxWeight=0;
//	pCharInfo->m_EquipedItem.GetTotalWeight(&nWeight, &nMaxWeight);
//	nMaxWeight = pCharInfo->m_nMaxWeight + nMaxWeight - pItemDesc->m_nMaxWT;
//	nWeight -= pItemDesc->m_nWeight;
//
//	if (nWeight > nMaxWeight) nResult = MERR_CANNOT_TAKEOFF_ITEM_BY_WEIGHT;
//
//	if( MOK == nResult )
//	{
//		bool bRet = false;
//		if (!m_MatchDBMgr.UpdateEquipedItem(pCharInfo->m_nCID, parts, 0, 0, &bRet))
//		{
//			cclog("DB Query(ResponseEquipItem > UpdateEquipedItem) Failed\n");
//			nResult = MERR_CANNOT_TAKEOFF_ITEM;
//		}
//
//		if( bRet )
//		{
//			pCharInfo->m_EquipedItem.Remove( parts );
//		}
//		else
//		{
//			nResult = MERR_CANNOT_TAKEOFF_ITEM;
//		}
//	}
//
//	CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_TAKEOFF_ITEM, CCUID(0,0));
//	pNew->AddParameter(new CCCommandParameterInt(nResult));
//	RouteToListener(pObj, pNew);	
//
//	if( MOK == nResult )
//	{
//		ResponseCharacterItemList( uidPlayer );
//
//		// ���� �������� ���̸� ���������� �˷��� ������ Look�� ������Ʈ ����� �Ѵ�.
//		if( FindStage(pObj->GetStageUID())  )
//		{
//			CCCommand* pEquipInfo = CreateCommand( MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK, CCUID(0, 0) );
//			pEquipInfo->AddParameter( new CCCmdParaCCUID(uidPlayer) );
//			pEquipInfo->AddParameter( new CCCmdParamInt(parts) );
//			pEquipInfo->AddParameter( new CCCmdParamInt(0) );
//			RouteToStage( pObj->GetStageUID(), pEquipInfo );
//		}
//	}
//}
//
//
//bool CCMatchServer::ResponseCharacterItemList(const CCUID& uidPlayer)
//{
//	CCMatchObject* pObj = GetObject(uidPlayer);
//	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) 
//	{
//		cclog("ResponseCharacterItemList > pObj or pObj->GetCharInfo() IS NULL\n");
//		return false;
//	}
//
//	// ���� DB�� ������Ʈ �Ǿ��ٸ� ������ �ٽ� �����Ѵ�.
//	// �뷮�� ũ���� ���� �Ͼ�� ���� �ƴϴ�. - by SungE 2007-06-28
//	if( SendGambleItemList(pObj) )
//	{
//		ResponseShopItemList( uidPlayer, 0, 0 );
//	}
//
//	// ������ ��� �＼���� ���߾����� ��񿡼� ������ ������ �����´�
//	if (!pObj->GetCharInfo()->m_ItemList.IsDoneDbAccess())
//	{
//		if (!m_MatchDBMgr.GetCharItemInfo(pObj->GetCharInfo()))
//		{
//			cclog("DB Query(ResponseCharacterItemList > GetCharItemInfo) Failed\n");
//			return false;
//		}
//	}
//
//	if( CSM_TEST == MGetServerConfig()->GetServerMode() ) 
//	{
//		if( !pObj->GetCharInfo()->m_QuestItemList.IsDoneDbAccess() )
//		{
//			if( !m_MatchDBMgr.GetCharQuestItemInfo(pObj->GetCharInfo()) )
//			{
//				cclog( "ResponseCharacterItemList - GetCharQuestItemInfo fail.\n" );
//				return false;
//			}
//		}
//	}
//
//	CCCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_CHARACTER_ITEMLIST, uidPlayer);
//
//	// �ٿ�Ƽ ����
//	pNew->AddParameter(new CCCommandParameterInt(pObj->GetCharInfo()->m_nBP));
//
//	// ����� ������ ����
//	int nRealEquipedItemCount = 0;
//	int nIndex = 0;
//	void* pEquipItemArray = CCMakeBlobArray(sizeof(CCUID), MMCIP_END);
//	for (int i = 0; i < MMCIP_END; i++)
//	{
//		CCUID* pUID = (CCUID*)CCGetBlobArrayElement(pEquipItemArray, nIndex++);
//
//		if (!pObj->GetCharInfo()->m_EquipedItem.IsEmpty(CCMatchCharItemParts(i)))
//		{
//			*pUID = pObj->GetCharInfo()->m_EquipedItem.GetItem(CCMatchCharItemParts(i))->GetUID();
//			nRealEquipedItemCount++;
//		}
//		else
//		{
//			*pUID = CCUID(0,0);
//		}
//	}
//
//	pNew->AddParameter(new CCCommandParameterBlob(pEquipItemArray, CCGetBlobArraySize(pEquipItemArray)));
//	CCEraseBlobArray(pEquipItemArray);
//
//
//	// ���� �ִ� ������ ����Ʈ ����
//	int nItemCount = pObj->GetCharInfo()->m_ItemList.GetCount();
//
//	void*			pItemArray					= CCMakeBlobArray(sizeof(CCTD_ItemNode), nItemCount);
//	CCMatchItemMap*	pItemList					= &pObj->GetCharInfo()->m_ItemList;
//	CCMatchItem*		pItem						= NULL;
//	CCTD_ItemNode*	pItemNode					= NULL;
//	unsigned long	nPassTime					= 0;
//	int				nPassMinuteTime				= 0;
//	int				iMaxUseHour					= 0;
//	int				nRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
//
//	nIndex = 0;
//	for (CCMatchItemMap::iterator itor = pItemList->begin(); itor != pItemList->end(); ++itor)
//	{
//		pItem = (*itor).second;
//
//		pItemNode					= (CCTD_ItemNode*)CCGetBlobArrayElement(pItemArray, nIndex++);
//		nPassTime					= CCGetTimeDistance(pItem->GetRentItemRegTime(), GetTickTime());
//		nPassMinuteTime				= nPassTime / (1000 * 60);
//		iMaxUseHour					= 0;		// �ִ� ���ð� ������(�ǸŰ��� ����� ����)
//		nRentMinutePeriodRemainder	= RENT_MINUTE_PERIOD_UNLIMITED;
//
//		if (pItem->IsRentItem())
//		{
//			nRentMinutePeriodRemainder	= pItem->GetRentMinutePeriodRemainder() - nPassMinuteTime;
//			iMaxUseHour					= pItem->GetRentHourPeriod();
//		}
//		
//		Make_MTDItemNode(pItemNode
//			, pItem->GetUID()
//			, pItem->GetDescID()
//			, nRentMinutePeriodRemainder
//			, iMaxUseHour);	
//
//		//pItemNode->uidItem						= pItem->GetUID();
//		//pItemNode->nItemID						= pItem->GetDescID();
//		//pItemNode->nRentMinutePeriodRemainder	= nRentMinutePeriodRemainder;		// �ʴ���
//		//pItemNode->iMaxUseHour					= iMaxUseHour;						// �ִ� ���ð� �߰�
//	}
//
//
//	pNew->AddParameter(new CCCommandParameterBlob(pItemArray, CCGetBlobArraySize(pItemArray)));
//	CCEraseBlobArray(pItemArray);
//
//	// ������ �ִ� �׺� ������ ����Ʈ ����
//	int nGambleItemCount = pObj->GetCharInfo()->m_GambleItemManager.GetCount();
//	void* pGambleItemArray = CCMakeBlobArray(sizeof(CCTD_GambleItemNode), nGambleItemCount);
//
//	for (int j = 0; j < nGambleItemCount; ++j)
//	{
//		const CCMatchCharGambleItem* pGambleItem = pObj->GetCharInfo()->m_GambleItemManager.GetGambleItem(j);
//		CCTD_GambleItemNode* pSendGambleItem = (CCTD_GambleItemNode*)CCGetBlobArrayElement(pGambleItemArray, j);
//		if( pSendGambleItem != NULL )
//		{
//			pSendGambleItem->uidItem = pGambleItem->GetUID();
//			pSendGambleItem->nItemID = pGambleItem->GetGambleItemID();			
//		}
//		else
//		{
//			ASSERT( 0 );
//		}
//	}
//
//	pNew->AddParameter(new CCCommandParameterBlob(pGambleItemArray, CCGetBlobArraySize(pGambleItemArray)));
//	CCEraseBlobArray(pGambleItemArray);
//
//	PostSafeQueue( pNew );
//
//	return true;
//}

void CCMatchServer::OnRequestUseSpendableNormalItem(const CCUID& uidPlayer, const CCUID& uidItem)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	CCMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);	///< UID�� ������ ã��
	if( pItem == NULL ) {
		cclog("Use Spendable Item Failed[CID : %d, CCUID(%d%d)]\n", pCharInfo->m_nCID, uidItem.High, uidItem.Low);
		return;
	}

	if( pItem->GetDesc()->IsSpendableItem() ) 
	{ 
		UseSpendableItem(uidPlayer, uidItem);	
	}	
}

void CCMatchServer::UseSpendableItem(const CCUID& uidPlayer, const CCUID& uidItem)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	CCMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);	///< UID�� ������ ã��
	if( pItem == NULL ) {
		cclog("Use Spendable Item Failed[CID : %d, CCUID(%d%d)]\n", pCharInfo->m_nCID, uidItem.High, uidItem.Low);
		return;
	}

	if( pItem->GetItemCount() > 0 ) 
	{
		pItem->DecreaseCountWithCaching(1);

		/*
		if( pItem->GetItemCount() == 0 ) 
		{
			UpdateCharItemDBCachingData(pObj);
			
			CCMatchCharItemParts nCheckParts = MMCIP_END;
			if (pCharInfo->m_EquipedItem.IsEquipedItem(uidItem, nCheckParts)) {
				pCharInfo->m_EquipedItem.Remove(nCheckParts);
			}

			if (!m_MatchDBMgr.UpdateEquipedItem(pCharInfo->m_nCID, parts, 0, 0))
			{
				cclog("DB Query(ResponseEquipItem > UpdateEquipedItem) Failed\n");
				// nResult = MERR_CANNOT_TAKEOFF_ITEM;
			}
			ResponseTakeoffItem

			pCharInfo->m_ItemList.RemoveItem(uidItem);

			ResponseCharacterItemList(uidPlayer);
		}
		*/

#ifdef _DEBUG
		cclog("Item UID(%d%d)�� ���Ǿ����ϴ�. ������ �׸� ����\n", uidItem.High, uidItem.Low);
#endif
	} else {
		cclog("Item UID(%d%d)�� ������������ ���Ǿ����ϴ�.\n", uidItem.High, uidItem.Low);
	}
}

/*
void CCMatchServer::OnRequestUseSpendableBuffItem(const CCUID& uidPlayer, const CCUID& uidItem)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;

	CCMatchItem *pItem = pCharInfo->m_ItemList.GetItem(uidItem);	///< UID�� ������ ã��
	if( pItem == NULL ) {
		cclog("Use Spendable Item Failed[CID : %d, CCUID(%d%d)]\n", pCharInfo->m_nCID, uidItem.High, uidItem.Low);
		return;
	}

	if( pItem->GetDesc()->IsSpendableItem() == false ) {
		_ASSERT(0); 
		return; 
	} 

	CCMatchBuffDesc* pBuffDesc = MGetMatchBuffDescMgr()->GetBuffDesc(pItem->GetDesc()->m_nID);
	if( pBuffDesc == NULL ) {
		_ASSERT(0);
		return;
	}

	int nBuffID		= pBuffDesc->m_nBuffID;
	int nBuffPeriod	= pBuffDesc->m_nBuffPeriod.Ref();

	bool bResult = false;
	if( pBuffDesc->m_nBuffPeriodType.Ref() == MMBPT_LONG ) {
		_ASSERT(0);
		return;
	} else if( pBuffDesc->m_nBuffPeriodType.Ref() == MMBPT_SHORT ) {
		bResult = pObj->GetCharBuff()->ApplyShortBuffInfo(nBuffID, nBuffPeriod);
	} else {
		_ASSERT(0);
		return;
	}

	if( bResult ) 
	{
		UseSpendableItem(uidPlayer, uidItem);

		CCCommand *pCmd = CreateCommand( MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM, uidPlayer );
		pCmd->AddParameter(new CCCommandParameterUID(uidItem));
		pCmd->AddParameter(new CCCommandParameterInt(MOK));
		PostSafeQueue(pCmd);

		PostCmdCharacterBuffInfo(uidPlayer);
	} 
	else 
	{
		CCCommand *pCmd = CreateCommand( MC_MATCH_RESPONSE_USE_SPENDABLE_BUFF_ITEM, uidPlayer );
		pCmd->AddParameter(new CCCommandParameterUID(uidItem));
		pCmd->AddParameter(new CCCommandParameterInt(MERR_CANNOT_APPLY_BUFF_INFO));
		PostSafeQueue(pCmd);
	}	
}*/
//���������ӽ��ּ� 
/*
void CCMatchServer::PostCmdCharacterBuffInfo(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	CCMatchCharInfo* pCharInfo = pObj->GetCharInfo();
	if (pCharInfo == NULL) return;		

	CCCommand *pCmd = CreateCommand(MC_MATCH_SPENDABLE_BUFF_ITEM_STATUS, uidPlayer);	

	void* pCharBuffInfoArray = CCMakeBlobArray(sizeof(CCTD_CharBuffInfo), 1);
	CCTD_CharBuffInfo* pCCTD_CharBuffInfo = (CCTD_CharBuffInfo*)CCGetBlobArrayElement(pCharBuffInfoArray, 0);
	CopyCharBuffInfoForTrans(pCCTD_CharBuffInfo, pCharInfo, pObj);

	pCmd->AddParameter(new CCCommandParameterUID(pObj->GetUID()));
	pCmd->AddParameter(new CCCommandParameterBlob(pCharBuffInfoArray, CCGetBlobArraySize(pCharBuffInfoArray)));
	Post(pCmd);

	CCEraseBlobArray(pCharBuffInfoArray);
}*/