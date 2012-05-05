#include "stdafx.h"
#include "ZShopEquipListbox.h"
#include "ZShopEquipInterface.h"
#include "ZItemSlotView.h"
#include "ZShop.h"


ZShopEquipListItem::ZShopEquipListItem(ZShopEquipItem* pItemData)
: m_pItemData(pItemData)
{
	_ASSERT(m_pItemData);
	m_pItemData->GetName(m_szName);
	m_pItemData->GetLevelResText(m_szLevel);
	m_pItemData->GetPriceText(m_szPrice);
}

ZShopEquipListItem::~ZShopEquipListItem()
{
	delete m_pItemData;
}

void ZShopEquipListItem::OnDraw( sRect& r, CCDrawContext* pDC, bool bSelected, bool bMouseOver )
{
	// ����
	if (bSelected)
		pDC->SetColor(220, 220, 220);
	else if (bMouseOver)
		pDC->SetColor(60, 60, 60);
	else
		pDC->SetColor(30, 30, 30);

	pDC->FillRectangle(r);

	sRect rcIcon;
	GetIconRect(rcIcon, r);
	rcIcon.x += r.x;
	rcIcon.y += r.y;

	// ������ ����
	pDC->SetColor(15, 15, 15);
	pDC->FillRectangle(rcIcon);

	pDC->SetBitmap(m_pItemData->GetIcon());
	pDC->Draw(rcIcon);

	sRect rc;	// ��Ʈ���� ������ ����
	rc.x = rcIcon.x + rcIcon.w + 2;
	rc.w = r.w - rcIcon.w - 6;
	rc.y = r.y + 2;
	rc.h = r.h - 4;

	// �ؽ�Ʈ ��
	if (!bSelected)
		pDC->SetColor(200, 200, 200);
	else
		pDC->SetColor(20, 20, 20);

	// �����۸�, ����
	pDC->TextMultiLine2(rc, GetString(), CONVERT600(2), true, CCAM_LEFT | MAM_TOP);
	pDC->Text(rc, m_szPrice, CCAM_RIGHT | CCAM_BOTTOM);

	// ���� (������ ���� ��� ���� ǥ��)
	// ����� ������ ���� ��� ������ �׸��ڸ� �׷��� �þȼ��� ���δ�

	pDC->SetColor(20, 20, 20);
	pDC->Text(sRect(rcIcon.x+1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, CCAM_LEFT | CCAM_BOTTOM);	// 1�ȼ��� ��������
	pDC->Text(sRect(rcIcon.x-1, rcIcon.y, rcIcon.w, rcIcon.h), m_szLevel, CCAM_LEFT | CCAM_BOTTOM);
	pDC->Text(sRect(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h+1), m_szLevel, CCAM_LEFT | CCAM_BOTTOM);
	pDC->Text(sRect(rcIcon.x, rcIcon.y, rcIcon.w, rcIcon.h-1), m_szLevel, CCAM_LEFT | CCAM_BOTTOM);

	if (ZGetMyInfo()->GetLevel() < m_pItemData->GetLevelRes())
		pDC->SetColor(200, 10, 10);
	else
		pDC->SetColor(200, 200, 200);

	pDC->Text(rcIcon, m_szLevel, CCAM_LEFT | CCAM_BOTTOM);
}

bool ZShopEquipListItem::GetDragItem( CCBitmap** ppDragBitmap, char* szDragString, char* szDragItemString )
{
	if (!m_pItemData) return true;

	*ppDragBitmap = m_pItemData->GetIcon();
	strcpy(szDragString, m_szName);
	strcpy(szDragItemString, m_szName);
	return true;
}

int ZShopEquipListItem::GetSortHint()
{
	// ����Ʈ�ڽ��� �������� �������� �����ϵ��� ������ ���� ������ �����Ѵ�
	// (�� �������� ���� Ŭ������ ���� sorthint�� �����ϵ��� ���� ���� ������ ������ �л�Ǿ� ������ �ڵ� �б� ����ϱ�..)
	if (ZSEIT_GAMBLE == m_pItemData->GetType()) 
		return 1000000;

	if (ZSEIT_MATCH == m_pItemData->GetType())
	{
		int hint = 2000000;
		int nResLv = m_pItemData->GetLevelRes();
		CCMatchItemDesc* pItemDesc = ((ZShopEquipItem_Match*)m_pItemData)->GetDesc();
		if (!pItemDesc) { _ASSERT(0); return 0; }

		if (pItemDesc->m_nType.Ref() == MMIT_MELEE)
			return hint + 100000 + pItemDesc->m_nWeaponType.Ref() * 1000 + nResLv;
		if (pItemDesc->m_nType.Ref() == MMIT_RANGE)
			return hint + 200000 + pItemDesc->m_nWeaponType.Ref() * 1000 + nResLv;
		if (pItemDesc->m_nType.Ref() == MMIT_EQUIPMENT)
			return hint + 300000 + pItemDesc->m_nSlot * 1000 + nResLv;

		hint = 4000000;
		if (pItemDesc->m_nType.Ref() == MMIT_CUSTOM)
		{
			hint = hint + 400000 + pItemDesc->m_nWeaponType.Ref() * 1000 + nResLv;
			if (pItemDesc->m_nWeaponType.Ref() == MWT_POTION)
			{
				hint = hint + pItemDesc->m_nEffectId * 100;
			}
			return hint;
		}
		if (pItemDesc->m_nType.Ref() == MMIT_COMMUNITY)
			return hint + 500000 + pItemDesc->m_nWeaponType.Ref() * 1000 + nResLv;
		if (pItemDesc->m_nType.Ref() == MMIT_LONGBUFF)
			return hint + 600000 + pItemDesc->m_nWeaponType.Ref() * 1000 + nResLv;

		return hint + 700000 + nResLv;
	}

	if (ZSEIT_SET == m_pItemData->GetType())
	{
		int nResLv = ((ZShopEquipItem_Set*)m_pItemData)->GetLevelRes();
		return 3000000 + nResLv;
	}

	if (ZSEIT_QUEST == m_pItemData->GetType())
		return 5000000;

	return 9999999;
}

void ZShopEquipListItem::GetIconRect(sRect& out, const sRect& rcItem)
{
	int len = rcItem.h-4;
	out.Set(2, 2, len, len);
}

bool ZShopEquipListItem::IsPtInRectToShowToolTip(sRect& rcItem, MPOINT& pt)
{
	// pt�� ����� ������ ������ �ִ��� �Ǵ�
	sRect rcIcon;
	GetIconRect(rcIcon, rcItem);
	// ����Ʈ ��ǥ��� ��ȯ
	rcIcon.x += rcItem.x;
	rcIcon.y += rcItem.y;
	// pt-in-rect ����
	return (rcIcon.x < pt.x && pt.x < rcIcon.x+rcIcon.w &&
		rcIcon.y < pt.y && pt.y < rcIcon.y+rcIcon.h);
}



ZShopEquipListbox::ZShopEquipListbox(const char* szName, CCWidget* pParent, CCListener* pListener)
: MMultiColListBox(szName, pParent, pListener)
{
	m_idxItemLastTooltip = -1;
}

bool ZShopEquipListbox::OnEvent( CCEvent* pEvent, CCListener* pListener )
{
	sRect rtClient = GetClientRect();

	if(pEvent->nMessage==MWM_MOUSEMOVE)
	{
		SetupItemDescTooltip();
		//return true;			  // �޽����� �Ծ������ ������ �ȳ��´�
	}

	return MMultiColListBox::OnEvent(pEvent, pListener);
}

char* ZShopEquipListbox::GetItemDescTooltipName()
{
	if (GetParent() && 0==strcmp("Shop", GetParent()->m_szIDLName))
		return "Shop_ItemDescription";

	if (GetParent() && 0==strcmp("Equipment", GetParent()->m_szIDLName))
		return "Equip_ItemDescription";

	_ASSERT(0);
	return "_noExistWidgetName_";
}

void ZShopEquipListbox::SetupItemDescTooltip()
{
	const char* szTextAreaName = GetItemDescTooltipName();
	CCTextArea* pItemDescTextArea = (CCTextArea*)ZGetGameInterface()->GetIDLResource()->FindWidget(szTextAreaName);
	if (pItemDescTextArea)
	{
		MPOINT ptInList = MScreenToClient(this, CCEvent::LatestPos);
		int idxItem = FindItem(ptInList);
		if (idxItem!=-1)
		{
			ZShopEquipListItem* pItem = (ZShopEquipListItem*)Get(idxItem);
			sRect rcListBox = GetRect();
			sRect rcItem;
			if (pItem && CalcItemRect(idxItem, rcItem))	// �׸��� ǥ�õǰ� �ִ� ������ �˾Ƴ� (����Ʈ ��ǥ��)
			{
				if (pItem->IsPtInRectToShowToolTip(rcItem, ptInList)) // �׸񿡰� �� ��ǥ�� ����� �̹��� �������� ���
				{
					if (m_idxItemLastTooltip==idxItem) return;
					m_idxItemLastTooltip = idxItem;

					pItem->GetItemData()->FillItemDesc(pItemDescTextArea);

					// ������ ��ġ
					sRect rcTextArea = pItemDescTextArea->GetRect();
					MPOINT posDesc(rcItem.x, rcItem.y);
					posDesc = CCClientToScreen(this, posDesc);
					posDesc.x -= pItemDescTextArea->GetClientWidth();			// �ϴ� �������� ��������
					if (posDesc.y+rcTextArea.h > rcListBox.y + rcListBox.h)		// ����Ʈ �ڽ� �ϴ��� �Ѿ�� �ʰ� ����
						posDesc.y = rcListBox.y + rcListBox.h - rcTextArea.h;
					if (posDesc.y < 0)											// �׷��ٰ� ȭ�� ���� �հ� ���� �ȵȴ�
						posDesc.y = 0;
					pItemDescTextArea->SetPosition(posDesc);
					pItemDescTextArea->SetZOrder(MZ_TOP);
					ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(true, pItemDescTextArea, this);
					return;
				}
			}
		}
	}

	m_idxItemLastTooltip = -1;
	ZGetGameInterface()->GetShopEquipInterface()->ShowItemDescription(false, pItemDescTextArea, this);
}


// ============ ZShopPurchaseItemListBoxListener ===============================================

void ShopPurchaseItemListBoxOnDrop(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
}

void ShopSaleItemListBoxOnDrop(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
}

void CharacterEquipmentItemListBoxOnDrop(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
	if (pSender == NULL) return;
	if (strcmp(pSender->GetClassName(), MINT_ITEMSLOTVIEW)) return;

	ZItemSlotView* pItemSlotView = (ZItemSlotView*)pSender;

	ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), pItemSlotView->GetParts());
}

// ============ ZShopPurchaseItemListBoxListener ===============================================

class ZShopPurchaseItemListBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if(CCWidget::IsMsg(szMessage, MLB_ITEM_SEL) == true)
		{
			ZShopEquipListbox* pEquipmentListBox = (ZShopEquipListbox*)pWidget;
			ZShopEquipListItem* pListItem = (ZShopEquipListItem*)pEquipmentListBox->GetSelItem();
			if (pListItem == NULL) return true;

			ZCharacterView* pCharacterView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformationShop");
			if (pCharacterView)
				pListItem->GetItemData()->UpdateCharacterView(pCharacterView);

			pListItem->GetItemData()->UpdateCharInfoText();

			ZGetGameInterface()->GetShopEquipInterface()->SelectArmorWeaponTabWithSlotType( pListItem->GetItemData()->GetSlotType());

			WidgetEnableShow("BuyConfirmCaller", true, true);

			return true;
		}
		else if(CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK) == true)
		{
			ZGetGameInterface()->GetShopEquipInterface()->OnBuyButton();
			return true;
		}

		return false;
	}
};
ZShopPurchaseItemListBoxListener g_ShopPurchaseItemListBoxListener;

CCListener* ZGetShopPurchaseItemListBoxListener(void)
{
	return &g_ShopPurchaseItemListBoxListener;
}

class ZEquipMyItemListBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if ( CCWidget::IsMsg(szMessage, MLB_ITEM_SEL)==true) {

			ZShopEquipListbox* pEquipmentListBox = (ZShopEquipListbox*)pWidget;
			ZShopEquipListItem* pListItem = (ZShopEquipListItem*)pEquipmentListBox->GetSelItem();
			if (!pListItem) return false;

			ZCharacterView* pCharacterView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformation");
			if (pCharacterView)
				pListItem->GetItemData()->UpdateCharacterView(pCharacterView);

			pListItem->GetItemData()->UpdateCharInfoText();

			ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( pListItem->GetItemData()->GetSlotType());
			ZGetGameInterface()->GetShopEquipInterface()->SelectArmorWeaponTabWithSlotType( pListItem->GetItemData()->GetSlotType());

			WidgetEnableShow("Equip",				pListItem->GetItemData()->CanEquip(), true);
			WidgetEnableShow("SendAccountItemBtn",	pListItem->GetItemData()->CanSendAccount(), true);

			return true;
		}
		else if ( CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
		{
			ZGetGameInterface()->GetShopEquipInterface()->Equip();		
			return true;
		}

		return false;
	}
};
ZEquipMyItemListBoxListener g_EquipmentItemListBoxListener;

CCListener* ZGetEquipmentMyItemListBoxListener(void)
{
	return &g_EquipmentItemListBoxListener;
}


class ZShopSellItemListBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if(CCWidget::IsMsg(szMessage, MLB_ITEM_SEL)==true)
		{
			ZShopEquipListbox* pListbox = (ZShopEquipListbox*)pWidget;
			ZShopEquipListItem* pListItem = (ZShopEquipListItem*)pListbox->GetSelItem();
			
			if (!pListItem) return false;

			ZGetGameInterface()->GetShopEquipInterface()->DrawCharInfoText();
			ZGetGameInterface()->GetShopEquipInterface()->SetKindableItem( pListItem->GetItemData()->GetSlotType());

			WidgetEnableShow("SellConfirmCaller", pListItem->GetItemData()->CanSell(), true);
			return true;
		}
		else if ( CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
		{
			ZGetGameInterface()->GetShopEquipInterface()->OnSellButton();
			return true;
		}

		return false;
	}
};

ZShopSellItemListBoxListener g_ShopSellItemListBoxListener;

CCListener* ZGetShopSellItemListBoxListener(void)
{
	return &g_ShopSellItemListBoxListener;
}

class ZAccountItemListBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if ( CCWidget::IsMsg(szMessage, MLB_ITEM_SEL)==true) {

			ZShopEquipListbox* pListbox = (ZShopEquipListbox*)pWidget;
			ZShopEquipListItem* pListItem = (ZShopEquipListItem*)pListbox->GetSelItem();
			if (!pListItem) return false;

			unsigned long int nItemID = ZGetMyInfo()->GetItemList()->GetAccountItemID( pListbox->GetSelIndex());

			ZCharacterView* pCharacterView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget("EquipmentInformation");
			if (pCharacterView)
				pListItem->GetItemData()->UpdateCharacterView(pCharacterView);

			pListItem->GetItemData()->UpdateCharInfoText();
			ZGetGameInterface()->GetShopEquipInterface()->SelectArmorWeaponTabWithSlotType( pListItem->GetItemData()->GetSlotType());

			// �߾����࿡�� �ٿ�Ƽ �������� ����ִٶ� ���Ǿ��� ������ �ű���ִ�.
			WidgetEnableShow("BringAccountItemBtn", true, true);

			return true;
		}
		else if ( CCWidget::IsMsg(szMessage, MLB_ITEM_DBLCLK)==true)
		{
			ZGetGameInterface()->GetShopEquipInterface()->OnBringAccountButton();
			return true;
		}

		return false;
	}
};

ZAccountItemListBoxListener g_AccountItemListBoxListener;

CCListener* ZGetAccountItemListBoxListener(void)
{
	return &g_AccountItemListBoxListener;
}


class ZShopListFilterListener : public CCListener {
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if(CCWidget::IsMsg(szMessage, MCMBBOX_CHANGED)==true)
		{
			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

			MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("Shop_AllEquipmentFilter");
			if ( pComboBox)
			{
				int sel = pComboBox->GetSelIndex();

				// �����¶��
				ZGetShop()->m_ListFilter = sel;
				ZGetShop()->Serialize();

				// �ȱ���¶�� - �ȱ�� �� �����ش�..
				ZMyItemList* pil = ZGetMyInfo()->GetItemList();
				pil->m_ListFilter = sel;
				pil->Serialize();
			}

			ZGetGameInterface()->GetShopEquipInterface()->SelectShopTab(-1);
		}
		return true;
	}
};

ZShopListFilterListener g_ShopListFilterListener;

CCListener* ZGetShopListFilterListener()
{
	return &g_ShopListFilterListener;
}

/////////////////////////////////////////////////////////////////

class MEquipListFilterListener : public CCListener {
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage)
	{
		if(CCWidget::IsMsg(szMessage, MCMBBOX_CHANGED)==true) {

			ZIDLResource* pResource = ZGetGameInterface()->GetIDLResource();

			MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("Equip_AllEquipmentFilter");
			if ( pComboBox)
			{
				int sel = pComboBox->GetSelIndex();

				ZMyItemList* pil = ZGetMyInfo()->GetItemList();
				pil->m_ListFilter = sel;
				pil->Serialize();
			}

			ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentTab( -1);
		}
		return true;
	}
};

MEquipListFilterListener g_EquipListFilterListener;

CCListener* ZGetEquipListFilterListener()
{
	return &g_EquipListFilterListener;
}


//// �����̾��̶�� �Ǿ� �ִ� �� : ������ �����ϴ� �ߴ��� �κ��ΰ� ����. ������ ����
//class MCashShopItemListBoxListener : public CCListener
//{
//public:
//	virtual bool OnCommand( CCWidget* pWidget, const char* szMessage)
//	{
//		if ( CCWidget::IsMsg( szMessage, MLB_ITEM_SEL)==true)
//		{
//			unsigned long int nItemID = 0;
//
//			ZEquipmentListBox_OLD* pEquipmentListBox = (ZEquipmentListBox_OLD*)pWidget;
//			ZEquipmentListItem_OLD* pListItem = ( ZEquipmentListItem_OLD*)pEquipmentListBox->GetSelItem();
//			if ( pListItem)
//			{
//				CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc( pListItem->GetItemID());
//				ZCharacterView* pCharacterView = (ZCharacterView*)ZGetGameInterface()->GetIDLResource()->FindWidget( "EquipmentInformationShop");
//				if ( pItemDesc && pCharacterView)
//				{
//					CCMatchCharItemParts nCharItemParts = GetSuitableItemParts( pItemDesc->m_nSlot);
//
//					pCharacterView->SetSelectMyCharacter();
//					pCharacterView->SetParts(nCharItemParts, pItemDesc->m_nID);
//
//					if (IsWeaponCharItemParts( nCharItemParts))
//						pCharacterView->ChangeVisualWeaponParts( nCharItemParts);
//
//					ZGetGameInterface()->GetShopEquipInterface()->SetupItemDescription( pItemDesc,
//						"Shop_ItemDescription",
//						NULL);
//				}
//
//
//				MButton* pButton = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget( "BuyCashConfirmCaller");
//				if ( pButton)
//					pButton->Enable( true);
//
//
//				return true;
//			}
//		}
//
//		else if ( CCWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
//		{
//			return true;
//		}
//
//
//		return false;
//	}
//};
//
//MCashShopItemListBoxListener g_CashShopItemListBoxListener;
//
//CCListener* ZGetCashShopItemListBoxListener(void)
//{
//	return &g_CashShopItemListBoxListener;
//}