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
//	CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
//	if (pItemDesc == NULL)
//	{
//		// Gamble ������
//		CCMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(nItemID);
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
//// ���� �԰� �ִ� ������ �� ���������� �ٲ� ���� �� ����� �ɷ�ġ ��ȭ�� ǥ���Ѵ�
//// pNewItemDesc=NULL �� �ϸ� �⺻ �ɷ�ġ�� �ʱ�ȭ�Ѵ�
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
//// �ϵ��ڵ������� �Լ�.. shop�̳� equipment�Ŀ� ���� �˸��� ���������� �����̸��� ����
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
//	if(pEvent->nMessage==MWM_MOUSEMOVE)
//	{
//		SetupItemDescTooltip();
//		//return true;			  // �޽����� �Ծ������ ������ �ȳ��´�
//	}
//	else if(pEvent->nMessage==MWM_RBUTTONDOWN) {
//		// ���� ���ǰ� ���� ���� �ڵ��ΰ� �����ϴ�. �ּ�ó����
//		/*if(rtClient.InPoint(pEvent->Pos)==true) {
//			int nSelItem = FindItem(pEvent->Pos);
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
//					// ������ ���� ����
//					if (m_pDescFrame && m_pDescFrame->IsVisible())
//						m_pDescFrame->Show(false);
//
//					// �˾��޴� ����
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
//	return MMultiColListBox::OnEvent(pEvent, pListener);
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
//			if (pItem && CalcItemRect(idxItem, rcItem))	// �׸��� ǥ�õǰ� �ִ� ������ �˾Ƴ� (����Ʈ ��ǥ��)
//			{
//				if (pItem->IsPtInRectToShowToolTip(rcItem, ptInList)) // �׸񿡰� �� ��ǥ�� ����� �̹��� �������� ���
//				{
//					// ������ ���� ���� ����
//					if (CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc( pItem->GetItemID() ))
//					{
//						ZMyItemNode* pItemNode = ZGetMyInfo()->GetItemList()->GetItem( pItem->GetUID() );
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription(pItemDesc, szTextAreaName, pItemNode);
//					}
//					if (CCQuestItemDesc* pQuestItemDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc( pItem->GetItemID() ))
//					{
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pQuestItemDesc, szTextAreaName );
//					}
//					if (CCMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(pItem->GetItemID()) )
//					{
////						ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pGItemDesc, szTextAreaName );
//					}
//
//					// ������ ��ġ
//					sRect rcTextArea = pItemDescTextArea->GetRect();
//					sPoint posDesc(rcItem.x, rcItem.y);
//					posDesc = CCClientToScreen(this, posDesc);
//					posDesc.x -= pItemDescTextArea->GetClientWidth();			// �ϴ� �������� ��������
//					if (posDesc.y+rcTextArea.h > rcListBox.y + rcListBox.h)		// ����Ʈ �ڽ� �ϴ��� �Ѿ�� �ʰ� ����
//						posDesc.y = rcListBox.y + rcListBox.h - rcTextArea.h;
//					if (posDesc.y < 0)											// �׷��ٰ� ȭ�� ���� �հ� ���� �ȵȴ�
//						posDesc.y = 0;
//					pItemDescTextArea->SetPosition(posDesc);
//					pItemDescTextArea->SetZOrder(MZ_TOP);
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
//: MMultiColListBox(szName, pParent, pListener)
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
//	((MPopupMenu*)m_pItemMenu)->Show(false);
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
////		if(CCWidget::IsMsg(szMessage, MLB_ITEM_SEL) == true) {
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
////							pDesc = MGetMatchItemDescMgr()->GetItemDesc(partsItemId);
////							if (!pDesc) { _ASSERT(0); continue; }
////
////							CCMatchCharItemParts nCharItemParts = GetSuitableItemParts(pDesc->m_nSlot);
////							pCharacterView->SetParts(nCharItemParts, pDesc->m_nID);
////						}
////					}
////
////					// ������ ������ ���� ���� �ؽ�Ʈ�� ����
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
////					CCMatchGambleItemDesc* pDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(nItemID);
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
////					CCQuestItemDesc* pDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc( nItemID );
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
////					CCMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
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
////		else if ( CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
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
////		if ( CCWidget::IsMsg(szMessage, MLB_ITEM_SEL)==true) {
////			unsigned long int nItemID = 0;
////
////			ZEquipmentListBox_OLD* pEquipmentListBox = (ZEquipmentListBox_OLD*)pWidget;
////			ZEquipmentListItem_OLD* pListItem = (ZEquipmentListItem_OLD*)pEquipmentListBox->GetSelItem();
////			if (pListItem != NULL) 
////				nItemID = ZGetMyInfo()->GetItemList()->GetItemID(pListItem->GetUID());
////
////			ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText();
////
////			// Gamble ���������� Ȯ��
////			CCMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(pListItem->GetItemID());
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
////			// �Ϲ� ������...
////			CCButton* pButtonEquip = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Equip");
////			CCButton* pButtonAccItemBtn1 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountItemBtn");
////			CCButton* pButtonAccItemBtn2 = (CCButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("SendAccountSpendableItemConfirmOpen");
////
////			CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
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
////				// gambleitem�� ������ ���ø� �ϱ� ������ ����Ʈ���� �׺� �����ۼ���ŭ ����� ������ ZItem�� ���ؽ��� �ȴ�.
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
////				// ĳ�� �������� ��� '�߾����࿡ ������'��ư Ȱ��ȭ, �ƴ� ��Ȱ��ȭ
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
////			// ����Ʈ ������
////			CCQuestItemDesc* pQuestItemDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc( pListItem->GetItemID());
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
////		else if ( CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
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
//	// ����
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
//	// ������ ����
//	//if (!bSelected)
//	pDC->SetColor(15, 15, 15);
//	//else
//	//	pDC->SetColor(200, 200, 200);
//	pDC->FillRectangle(rcIcon);
//
//	pDC->SetBitmap(m_pBitmap);
//	pDC->Draw(rcIcon);
//
//	sRect rc;	// ��Ʈ���� ������ ����
//	rc.x = rcIcon.x + rcIcon.w + 2;
//	rc.w = r.w - rcIcon.w - 6;
//	rc.y = r.y + 2;
//	rc.h = r.h - 4;
//
//	// �ؽ�Ʈ ��
//	if (!bSelected)
//		pDC->SetColor(200, 200, 200);
//	else
//		pDC->SetColor(20, 20, 20);
//
//	// �����۸�, ����
//	pDC->TextMultiLine2(rc, GetString(), CONVERT600(2), true, CCD_LEFT | CCD_TOP);
//	pDC->Text(rc, m_szPrice, CCD_RIGHT | CCAM_BOTTOM);
//
//	// ���� (������ ���� ��� ���� ǥ��)
//	// ����� ������ ���� ��� ������ �׸��ڸ� �׷��� �þȼ��� ���δ�
//
//	pDC->SetColor(20, 20, 20);
//	pDC->Text(sRect(rcIcon.x+1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, CCD_LEFT | CCAM_BOTTOM);	// 1�ȼ��� ��������
//	pDC->Text(sRect(rcIcon.x-1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, CCD_LEFT | CCAM_BOTTOM);
//	pDC->Text(sRect(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h+1), m_szLevel, CCD_LEFT | CCAM_BOTTOM);
//	pDC->Text(sRect(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h-1), m_szLevel, CCD_LEFT | CCAM_BOTTOM);
//
//	if (m_bLevelResticted)
//		pDC->SetColor(200, 10, 10);
//	else
//		pDC->SetColor(200, 200, 200);
//
//	pDC->Text(rcIcon, m_szLevel, CCD_LEFT | CCAM_BOTTOM);
//}
//
//bool ZEquipmentListItem_OLD::IsPtInRectToShowToolTip(sRect& rcItem, sPoint& pt)
//{
//	// pt�� ����� ������ ������ �ִ��� �Ǵ�
//	sRect rcIcon;
//	GetIconRect(rcIcon, rcItem);
//	// ����Ʈ ��ǥ��� ��ȯ
//	rcIcon.x += rcItem.x;
//	rcIcon.y += rcItem.y;
//	// pt-in-rect ����
//	return (rcIcon.x < pt.x && pt.x < rcIcon.x+rcIcon.w &&
//		rcIcon.y < pt.y && pt.y < rcIcon.y+rcIcon.h);
//}
//
//int ZEquipmentListItem_OLD::GetSortHint()
//{
//	int itemID = GetItemID();
//
//	// ����Ʈ�ڽ��� �������� �������� �����ϵ��� ������ ���� ������ �����Ѵ�
//	CCMatchGambleItemDesc* pGItemDesc = MGetMatchGambleItemDescMgr()->GetGambleItemDesc(itemID);
//	if (pGItemDesc) return 100000;
//
//	MShopBaseItem* pShopItem = ZGetShopEx()->GetItem(m_UID.Low);
//	if (MDynamicCast(MShopSetItem, pShopItem)) {
//		int nResLv = 0;// ��Ʈ�� ���� ������ �ٽ� ����ؾ� �Ѵ�. �׷��� �ٽ� ����������� �׳� ����Ʈ�����ۿ� ������ int�� ���� �ְ� ����
//		return 300000 + nResLv;
//	}
//
//	CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(itemID);
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
//	CCQuestItemDesc* pQuestItemDesc = MGetMatchQuestItemDescMgr()->FindQItemDesc(itemID);
//	if (pQuestItemDesc) return 500000;
//
//	return 999999;
//}