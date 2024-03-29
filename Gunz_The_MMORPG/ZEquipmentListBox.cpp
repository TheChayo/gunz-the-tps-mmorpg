#include "stdafx.h"
//
//#include "ZEquipmentListBox.h"
//#include "ZPost.h"
//#include "ZApplication.h"
//#include "ZGameClient.h"
//#include "ZItemSlotView.h"
//#include "ZShopEx.h"
//#include "ZCharacterView.h"
//#include "CCTextArea.h"
//#include "ZMyInfo.h"
//#include "ZMyItemList.h"
//#include "CCComboBox.h"
//#include "ZItemMenu.h"
//#include "ZShopEquipInterface.h"
//
//
//bool ZGetIsCashItem( unsigned long nItemID)
//{
//	CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(nItemID);
//	if (pItemDesc == NULL)
//	{
//		// Gamble 아이템
//		CCMatchGambleItemDesc* pGItemDesc = CCGetMatchGambleItemDescMgr()->GetGambleItemDesc(nItemID);
//		if( pGItemDesc ) {
//			if( pGItemDesc->IsCash() ) {
//				return true;
//			}
//		}
//
//		return false;
//	}
//	if (pItemDesc->IsCashItem())
//		return true;
//	return false;
//}
//
//
//
//// 현재 입고 있는 부위를 새 아이템으로 바꿔 입을 때 생기는 능력치 변화를 표시한다
//// pNewItemDesc=NULL 로 하면 기본 능력치로 초기화한다
//
//bool ZEquipmentListBox_OLD::IsDropable(CCWidget* pSender)
//{
//	if (pSender == NULL) return false;
//	if (strcmp(pSender->GetClassName(), CORE_ITEMSLOTVIEW)) return false;
//
//	return true;
//}
//
////bool ZEquipmentListBox_OLD::OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
////{
////	if (m_pOnDropFunc != NULL)
////	{
////		m_pOnDropFunc(this, pSender, pBitmap, szString, szItemString);
////	}
////
////	return true;
////}
//
//#define SHOW_DESCRIPTION		"showdesc"
//#define HIDE_DESCRIPTION		"hidedesc"
//
//// 하드코딩스러운 함수.. shop이냐 equipment냐에 따라 알맞은 아이템툴팁 위젯이름을 얻어낸다
//char* ZEquipmentListBox_OLD::GetItemDescriptionWidgetName()
//{
//	if (GetParent() && 0==strcmp("Shop", GetParent()->m_szIDLName))
//		return "Shop_ItemDescription";
//
//	if (GetParent() && 0==strcmp("Equipment", GetParent()->m_szIDLName))
//		return "Equip_ItemDescription";
//
//	_ASSERT(0);
//	return "_noExistWidgetName_";
//}
//
//bool ZEquipmentListBox_OLD::OnEvent(CCEvent* pEvent, CCListener* pListener)
//{
//	sRect rtClient = GetClientRect();
//
//	if(pEvent->iMessage==CCWM_MOUSEMOVE)
//	{
//		SetupItemDescTooltip();
//		//return true;			  // 메시지를 먹어버리면 툴팁이 안나온다
//	}
//	else if(pEvent->iMessage==CCWM_RBUTTONDOWN) {
//		// 현재 사용되고 있지 않은 코드인것 같습니다. 주석처리함
//		/*if(rtClient.InPoint(pEvent->sPos)==true) {
//			int nSelItem = FindItem(pEvent->sPos);
//			if ( (nSelItem != -1) && GetItemMenu())
//			{
//				SetSelIndex(nSelItem);
//
//				ZEquipmentListItem_OLD* pNode = (ZEquipmentListItem_OLD*)Get(nSelItem);
//				if (ZGetIsCashItem(pNode->GetItemID())) {
//					ZItemMenu* pMenu = GetItemMenu();
//					pMenu->SetupMenu();
//					pMenu->SetTargetName(pNode->GetString());
//					pMenu->SetTargetUID(pNode->GetUID());
//
//					// 아이템 설명 끄고
//					if (m_pDescFrame && m_pDescFrame->IsVisible())
//						m_pDescFrame->Show(false);
//
//					// 팝업메뉴 띄운다
//					sPoint posItem;
//					GetItemPos(&posItem, nSelItem);
//					sPoint posMenu;
//					posMenu.x = GetRect().w/4;
//					posMenu.y = posItem.y + GetItemHeight()/4;
//					pMenu->Show(posMenu.x, posMenu.y, true);
//
//					return true;
//				}
//			}
//		}*/
//	}
//
//	return CCMultiColListBox::OnEvent(pEvent, pListener);
//}
//
//void ZEquipmentListBox_OLD::SetupItemDescTooltip()
//{
//	const char* szTextAreaName = GetItemDescriptionWidgetName();
//	CCTextArea* pItemDescTextArea = (CCTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget(szTextAreaName);
//	if (pItemDescTextArea)
//	{
//		sPoint ptInList = MScreenToClient(this, CCEvent::LatestPos);
//		int idxItem = FindItem(ptInList);
//		if (idxItem!=-1)
//		{
//			if (m_idxItemLastTooltip==idxItem) return;
//
//			m_idxItemLastTooltip = idxItem;
//			ZEquipmentListItem_OLD* pItem = (ZEquipmentListItem_OLD*)Get(idxItem);
//			sRect rcListBox = GetRect();
//			sRect rcItem;
//			if (pItem && CalcItemRect(idxItem, rcItem))	// 항목이 표시되고 있는 영역을 알아냄 (리스트 좌표계)
//			{
//				if (pItem->IsPtInRectToShowToolTip(rcItem, ptInList)) // 항목에게 이 좌표가 썸네일 이미지 영역인지 물어봄
//				{
//					// 아이템 설명 툴팁 세팅
//					if (CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc( pItem->GetItemID() ))
//					{
//						ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem( pItem->GetUID() );
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription(pItemDesc, szTextAreaName, pItemNode);
//					}
//					if (CCQuestItemDesc* pQuestItemDesc = CCGetMatchQuestItemDescMgr()->FindQItemDesc( pItem->GetItemID() ))
//					{
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pQuestItemDesc, szTextAreaName );
//					}
//					if (CCMatchGambleItemDesc* pGItemDesc = CCGetMatchGambleItemDescMgr()->GetGambleItemDesc(pItem->GetItemID()) )
//					{
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pGItemDesc, szTextAreaName );
//					}
//
//					// 툴팁의 위치
//					sRect rcTextArea = pItemDescTextArea->GetRect();
//					sPoint posDesc(rcItem.x, rcItem.y);
//					posDesc = CCClientToScreen(this, posDesc);
//					posDesc.x -= pItemDescTextArea->GetClientWidth();			// 일단 아이콘의 왼쪽으로
//					if (posDesc.y+rcTextArea.h > rcListBox.y + rcListBox.h)		// 리스트 박스 하단을 넘어가지 않게 하자
//						posDesc.y = rcListBox.y + rcListBox.h - rcTextArea.h;
//					if (posDesc.y < 0)											// 그렇다고 화면 위로 뚫고 가면 안된다
//						posDesc.y = 0;
//					pItemDescTextArea->SetPosition(posDesc);
//					pItemDescTextArea->SetZOrder(CC_TOP);
//					ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(true, pItemDescTextArea, this);
//					return;
//				}
//			}
//		}
//	}
//
//	m_idxItemLastTooltip = -1;
//	ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(false, pItemDescTextArea, this);
//}
//
//ZEquipmentListBox_OLD::ZEquipmentListBox_OLD(const char* szName, CCWidget* pParent, CCListener* pListener)
//: CCMultiColListBox(szName, pParent, pListener)
//{
//	SetDesiredNumRow(4);
//
//	//	m_pOnDropFunc = NULL;
//
//	//	m_nLastItem=-1;
//	//	m_dwLastMouseMove=timeGetTime();
//	m_pDescFrame=NULL;
//
//	m_pItemMenu = NULL;
//	m_idxItemLastTooltip = -1;
//}
//
//ZEquipmentListBox_OLD::~ZEquipmentListBox_OLD()
//{
//	if (m_pItemMenu) {
//		delete m_pItemMenu;
//		m_pItemMenu = NULL;
//	}
//}
//
//void ZEquipmentListBox_OLD::AttachMenu(ZItemMenu* pMenu) 
//{ 
//	m_pItemMenu = pMenu;
//	((CCPopupMenu*)m_pItemMenu)->Show(false);
//}
//
//// Listener //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//
///////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////
////
////class MShopPurchaseItemListBoxListener : public CCListener{
////public:
////	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
////	{
////		CCButton* pButton1 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuyConfirmCaller");
////		CCButton* pButton2 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuySpendableItemConfirmCaller");
////		CCButton* pButton3 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuyCashConfirmCaller");
////
////		if(CCWidget::IsMsg(szMessage, CCLB_ITEM_SEL) == true) {
////
////			ZEquipmentListBox_OLD* pEquipmentListBox = (ZEquipmentListBox_OLD*)pWidget;
////			ZEquipmentListItem_OLD* pListItem = (ZEquipmentListItem_OLD*)pEquipmentListBox->GetSelItem();
////			if (pListItem == NULL) return true;
////
////			int nMSID = pListItem->GetUID().Low;
////			int nItemID = ZGetShopEx()->GetItemID(nMSID);
////			MShopItemType nType = ZGetShopEx()->GetItemType(nMSID);
////
////			switch(nType)
////			{
////			case MSIT_SITEM :
////				{
////					MShopBaseItem* pBaseItem = ZGetShopEx()->GetItem(nMSID);
////					if (!pBaseItem) { _ASSERT(0); return false; }
////
////					MShopSetItem* pSetItem = MDynamicCast(MShopSetItem, pBaseItem);
////					if (!pSetItem) { _ASSERT(0); return false; }
////
////					ZCharacterView* pCharacterView = 
////						(ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformationShop");
////					if( pCharacterView == NULL ) return false;
////
////					pCharacterView->SetSelectMyCharacter();
////					CCMatchItemDesc* pDesc;
////					for (int i=0; i<MAX_SET_ITEM_COUNT; ++i)
////					{
////						int partsItemId = pSetItem->GetItemID(i);
////						if (partsItemId != 0)
////						{
////							pDesc = CCGetMatchItemDescMgr()->GetItemDesc(partsItemId);
////							if (!pDesc) { _ASSERT(0); continue; }
////
////							CCMatchCharItemParts nCharItemParts = GetSuitableItemParts(pDesc->m_nSlot);
////							pCharacterView->SetParts(nCharItemParts, pDesc->m_nID);
////						}
////					}
////
////					// 아이템 툴팁과 성능 변동 텍스트를 갱신
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( , "Shop_ItemDescription", NULL);
////					UpdateCharInfoTextWithNewItem( );
////
////					if(pButton1) { pButton1->Enable(false); pButton1->Show(false); }
////					if(pButton2) { pButton2->Enable(false);  pButton2->Show(false);  }
////					if(pButton3) { pButton1->Enable(true); pButton3->Show(true); }
////				}
////				return true;
////
////			case MSIT_GITEM :
////				{
////					CCMatchGambleItemDesc* pDesc = CCGetMatchGambleItemDescMgr()->GetGambleItemDesc(nItemID);
////					if( pDesc == NULL ) return false;
////
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pDesc, "Shop_ItemDescription" );
////					
////					if(pButton1) { pButton1->Enable(false); pButton1->Show(false); }
////					if(pButton2) { pButton2->Enable(true);  pButton2->Show(true);  }
////					if(pButton3) { pButton1->Enable(false); pButton3->Show(false); }
////				}
////				return true;
////
////			case MSIT_QITEM :
////				{
////#ifdef _QUEST_ITEM
////					CCQuestItemDesc* pDesc = CCGetMatchQuestItemDescMgr()->FindQItemDesc( nItemID );
////					if( pDesc == NULL ) return false;
////
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pDesc, "Shop_ItemDescription");
////
////					if (pButton1) { pButton1->Enable(false); pButton1->Show(false); }
////					if (pButton2) { pButton2->Enable(true);  pButton2->Show(true);  }
////					if (pButton3) { pButton3->Enable(false); pButton3->Show(false); }
////#endif
////				}
////				return true;
////
////			case MSIT_ZITEM :
////				{
////					CCMatchItemDesc* pDesc = CCGetMatchItemDescMgr()->GetItemDesc(nItemID);
////					if( pDesc == NULL ) return false;
////
////					ZCharacterView* pCharacterView = 
////						(ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformationShop");
////					if( pCharacterView == NULL ) return false;
////
////					CCMatchCharItemParts nCharItemParts = GetSuitableItemParts(pDesc->m_nSlot);
////
////					pCharacterView->SetSelectMyCharacter();
////					pCharacterView->SetParts(nCharItemParts, pDesc->m_nID);
////
////					if (IsWeaponCharItemParts(nCharItemParts)) {
////						pCharacterView->ChangeVisualWeaponParts(nCharItemParts);
////					}
////
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pDesc, "Shop_ItemDescription", NULL);
////
////					UpdateCharInfoTextWithNewItem(pDesc);
////
////					if ( pDesc->IsCashItem() ) {
////						if( pButton1 ) { pButton1->Enable(false);  pButton1->Show(false);  }
////						if( pButton2 ) { pButton2->Enable(false); pButton2->Show(false); }
////						if( pButton3 ) { pButton3->Enable(true); pButton3->Show(true); }
////					} else {
////						if( pDesc->IsSpendableItem() ) {
////							if( pButton1 ) { pButton1->Enable(false); pButton1->Show(false); }
////							if( pButton2 ) { pButton2->Enable(true);  pButton2->Show(true);  }
////						} else { 
////							if( pButton1 ) { pButton1->Enable(true);  pButton1->Show(true);  }
////							if( pButton2 ) { pButton2->Enable(false); pButton2->Show(false); }
////						}
////						if( pButton3 ) { pButton3->Enable(false); pButton3->Show(false); }
////					}
////				}
////
////				return true;
////			}
////		}
////		else if ( CCWidget::IsMsg(szMessage, CCLB_ITEM_DBLCLK)==true)
////		{
////			CCWidget* pWidget = (CCWidget*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Shop_BuyConfirm");
////			//			if ( pWidget)
////			//				pWidget->Show( true);
////
////			return true;
////		}
////
////		return false;
////	}
////};
////MShopPurchaseItemListBoxListener g_ShopPurchaseItemListBoxListener;
////
////CCListener* ZGetShopPurchaseItemListBoxListener()
////{
////	return &g_ShopPurchaseItemListBoxListener;
////}
//
//
//
//////////
////class MEquipmentItemListBoxListener : public CCListener{
////public:
////	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
////	{
////		if ( CCWidget::IsMsg(szMessage, CCLB_ITEM_SEL)==true) {
////			unsigned long int nItemID = 0;
////
////			ZEquipmentListBox_OLD* pEquipmentListBox = (ZEquipmentListBox_OLD*)pWidget;
////			ZEquipmentListItem_OLD* pListItem = (ZEquipmentListItem_OLD*)pEquipmentListBox->GetSelItem();
////			if (pListItem != NULL) 
////				nItemID = ZGetMyInfo()->GetItemList()->GetItemID(pListItem->GetUID());
////
////			ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText();
////
////			// Gamble 아이템인지 확인
////			CCMatchGambleItemDesc* pGItemDesc = CCGetMatchGambleItemDescMgr()->GetGambleItemDesc(pListItem->GetItemID());
////			if ( pGItemDesc )
////			{
////				ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pGItemDesc, "Equip_ItemDescription");
////
////				ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( MMIST_NONE);
////
////				CCButton *pButton = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Equip");
////				if (pButton) pButton->Enable( true);
////
////				CCButton *pButton1 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountItemBtn");
////				CCButton *pButton2 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountSpendableItemConfirmOpen");
////				if (pButton1) { pButton1->Show(false); pButton1->Enable(false); }
////				if (pButton2) { pButton2->Show(true);  pButton2->Enable( pGItemDesc->IsCash() ? true : false); }
////
////				return true;
////			}
////
////
////			// 일반 아이템...
////			CCButton* pButtonEquip = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Equip");
////			CCButton* pButtonAccItemBtn1 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountItemBtn");
////			CCButton* pButtonAccItemBtn2 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountSpendableItemConfirmOpen");
////
////			CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(nItemID);
////			ZCharacterView* pCharacterView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformation");
////			if ((pCharacterView) && (pItemDesc)) {
////
////				UpdateCharInfoTextWithNewItem(pItemDesc);
////
////				CCMatchCharItemParts nCharItemParts = GetSuitableItemParts(pItemDesc->m_nSlot);
////
////				pCharacterView->SetSelectMyCharacter();
////				pCharacterView->SetParts(nCharItemParts, pItemDesc->m_nID);
////
////				if (IsWeaponCharItemParts(nCharItemParts))
////					pCharacterView->ChangeVisualWeaponParts(nCharItemParts);
////
////				// gambleitem은 위에서 무시를 하기 때문에 리스트에서 겜블 아이템수만큼 빼줘야 선택한 ZItem의 인텍스가 된다.
////				//const int nGItemListCount = ZGetMyInfo()->GetItemList()->GetGambleItemCount();
////				//ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem( pEquipmentListBox->GetSelIndex() - nGItemListCount );				
////
////				ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem(pListItem->GetUID());				
////				if ( pItemDesc && pItemNode)
////					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pItemDesc, "Equip_ItemDescription", pItemNode);
////
////				ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( pItemDesc->m_nSlot);
////
////				if ( pButtonEquip && (pItemDesc->m_nSlot != MMIST_NONE))	pButtonEquip->Enable( true);
////				else														pButtonEquip->Enable( false);
////
////				// 캐쉬 아이템일 경우 '중앙은행에 보내기'버튼 활성화, 아님 비활성화
////				if( pItemDesc->IsSpendableItem() ) {
////					if( ZGetIsCashItem(nItemID)) {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(false); pButtonAccItemBtn1->Enable(false); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(true);  pButtonAccItemBtn2->Enable(true);  }
////					} else {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(false); pButtonAccItemBtn1->Enable(false); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(true);  pButtonAccItemBtn2->Enable(false); }
////					} 					
////				} else {
////					if( ZGetIsCashItem(nItemID)) {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(true);  pButtonAccItemBtn1->Enable(true); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(false); pButtonAccItemBtn2->Enable(false);}
////					} else {
////						if(pButtonAccItemBtn1) { pButtonAccItemBtn1->Show(true);  pButtonAccItemBtn1->Enable(false); }
////						if(pButtonAccItemBtn2) { pButtonAccItemBtn2->Show(false); pButtonAccItemBtn2->Enable(false); }
////					}
////				}
////			}
////
////			// 퀘스트 아이템
////			CCQuestItemDesc* pQuestItemDesc = CCGetMatchQuestItemDescMgr()->FindQItemDesc( pListItem->GetItemID());
////			if ( pQuestItemDesc) {
////				ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pQuestItemDesc, "Equip_ItemDescription");
////
////				ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( MMIST_NONE);
////
////				if ( pButtonEquip) pButtonEquip->Enable( false);
////
////				if (pButtonAccItemBtn1) pButtonAccItemBtn1->Enable( false);
////				if (pButtonAccItemBtn2) pButtonAccItemBtn2->Enable( false);
////			}
////
////			return true;
////		}
////		else if ( CCWidget::IsMsg(szMessage, CCLB_ITEM_DBLCLK)==true)
////		{
////			ZGetGameInterface()->GetShopEquipInterface()->Equip();		
////			return true;
////		}
////
////		return false;
////	}
////};
////MEquipmentItemListBoxListener g_EquipmentItemListBoxListener;
////
////CCListener* ZGetEquipmentItemListBoxListener()
////{
////	return &g_EquipmentItemListBoxListener;
////}
//
//void ZEquipmentListItem_OLD::GetIconRect(sRect& out, const sRect& rcItem)
//{
//	int len = rcItem.h-4;
//	out.Set(2, 2, len, len);
//	//out.Set(2, 2, CONVERT800(60), CONVERT600(60));
//}
//
//void ZEquipmentListItem_OLD::OnDraw(sRect& r, CCDrawContext* pDC, bool bSelected, bool bMouseOver)
//{
//	// 배경색
//	if (bSelected)
//		pDC->SetColor(220, 220, 220);
//	else if (bMouseOver)
//		pDC->SetColor(60, 60, 60);
//	else
//		pDC->SetColor(30, 30, 30);
//
//	pDC->FillRectangle(r);
//
//	sRect rcIcon;
//	GetIconRect(rcIcon, r);
//	rcIcon.x += r.x;
//	rcIcon.y += r.y;
//
//	// 아이콘 배경색
//	//if (!bSelected)
//	pDC->SetColor(15, 15, 15);
//	//else
//	//	pDC->SetColor(200, 200, 200);
//	pDC->FillRectangle(rcIcon);
//
//	pDC->SetBitmap(m_pBitmap);
//	pDC->Draw(rcIcon);
//
//	sRect rc;	// 비트맵을 제외한 영역
//	rc.x = rcIcon.x + rcIcon.w + 2;
//	rc.w = r.w - rcIcon.w - 6;
//	rc.y = r.y + 2;
//	rc.h = r.h - 4;
//
//	// 텍스트 색
//	if (!bSelected)
//		pDC->SetColor(200, 200, 200);
//	else
//		pDC->SetColor(20, 20, 20);
//
//	// 아이템명, 가격
//	pDC->TextMultiLine2(rc, GetString(), CONVERT600(2), true, CCD_LEFT | CCD_TOP);
//	pDC->Text(rc, m_szPrice, CCD_RIGHT | CCD_BOTTOM);
//
//	// 레벨 (레벨이 낮은 경우 붉은 표시)
//	// 썸네일 아이콘 위에 찍기 때문에 그림자를 그려서 시안성을 높인다
//
//	pDC->SetColor(20, 20, 20);
//	pDC->Text(sRect(rcIcon.x+1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, CCD_LEFT | CCD_BOTTOM);	// 1픽셀씩 빗겨찍음
//	pDC->Text(sRect(rcIcon.x-1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, CCD_LEFT | CCD_BOTTOM);
//	pDC->Text(sRect(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h+1), m_szLevel, CCD_LEFT | CCD_BOTTOM);
//	pDC->Text(sRect(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h-1), m_szLevel, CCD_LEFT | CCD_BOTTOM);
//
//	if (m_bLevelResticted)
//		pDC->SetColor(200, 10, 10);
//	else
//		pDC->SetColor(200, 200, 200);
//
//	pDC->Text(rcIcon, m_szLevel, CCD_LEFT | CCD_BOTTOM);
//}
//
//bool ZEquipmentListItem_OLD::IsPtInRectToShowToolTip(sRect& rcItem, sPoint& pt)
//{
//	// pt가 썸네일 아이콘 영역에 있는지 판단
//	sRect rcIcon;
//	GetIconRect(rcIcon, rcItem);
//	// 리스트 좌표계로 변환
//	rcIcon.x += rcItem.x;
//	rcIcon.y += rcItem.y;
//	// pt-in-rect 판정
//	return (rcIcon.x < pt.x && pt.x < rcIcon.x+rcIcon.w &&
//		rcIcon.y < pt.y && pt.y < rcIcon.y+rcIcon.h);
//}
//
//int ZEquipmentListItem_OLD::GetSortHint()
//{
//	int itemID = GetItemID();
//
//	// 리스트박스가 아이템을 종류별로 정렬하도록 종류에 따라서 순번을 리턴한다
//	CCMatchGambleItemDesc* pGItemDesc = CCGetMatchGambleItemDescMgr()->GetGambleItemDesc(itemID);
//	if (pGItemDesc) return 100000;
//
//	MShopBaseItem* pShopItem = ZGetShopEx()->GetItem(m_UID.Low);
//	if (MDynamicCast(MShopSetItem, pShopItem)) {
//		int nResLv = 0;// 세트의 제한 레벨을 다시 계산해야 한다. 그러나 다시 계산하지말고 그냥 리스트아이템에 렙제를 int로 갖고 있게 하자
//		return 300000 + nResLv;
//	}
//
//	CCMatchItemDesc* pItemDesc = CCGetMatchItemDescMgr()->GetItemDesc(itemID);
//	if (pItemDesc) {
//		int hint = 200000;
//		int nResLv = pItemDesc->m_nResLevel.Ref();
//		if (pItemDesc->m_nType.Ref() == MMIT_MELEE)
//			return hint + 10000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_RANGE)
//			return hint + 20000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_EQUIPMENT)
//			return hint + 30000 + pItemDesc->m_nSlot * 100 + nResLv;
//
//		hint = 400000;
//		if (pItemDesc->m_nType.Ref() == MMIT_CUSTOM)
//			return hint + 40000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_COMMUNITY)
//			return hint + 50000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		if (pItemDesc->m_nType.Ref() == MMIT_LONGBUFF)
//			return hint + 60000 + pItemDesc->m_nWeaponType.Ref() * 100 + nResLv;
//		
//		return hint + 70000 + nResLv;
//	}
//
//	CCQuestItemDesc* pQuestItemDesc = CCGetMatchQuestItemDescMgr()->FindQItemDesc(itemID);
//	if (pQuestItemDesc) return 500000;
//
//	return 999999;
//}