#ifndef ZEQUIPMENTLISTBOX_H
#define ZEQUIPMENTLISTBOX_H

//#include "ZPrerequisites.h"
//#include "MMultiColListBox.h"
//
//bool ZGetIsCashItem(unsigned long nItemID);
//
//
//
//class ZEquipmentListItem_OLD : public MMultiColListItem{
//protected:
//	CCBitmap*			m_pBitmap;
//
//	int					m_nAIID;		// �߾����࿡�� ����Ѵ�
//	unsigned long		m_nItemID;
//	bool				m_bLevelResticted;	// ���� �� �������� ���� ������
//public:
//	CCUID				m_UID;
//public:
//	char	m_szName[256];
//	char	m_szLevel[256];
//	char	m_szPrice[256];
//public:
//	ZEquipmentListItem_OLD(const CCUID& uidItem, const unsigned long nItemID, CCBitmap* pBitmap, const char* szName, const char* szLevel, const char* szPrice, bool bLeveRestricted)
//	{
//		m_nAIID = 0;
//		m_nItemID = nItemID;
//
//		m_pBitmap = pBitmap;
//		m_UID = uidItem;
//		strcpy(m_szName, szName);
//		strcpy(m_szLevel, szLevel);
//		strcpy(m_szPrice, szPrice);
//		m_bLevelResticted = bLeveRestricted;
//	}
//	ZEquipmentListItem_OLD(const int nAIID, const unsigned long nItemID, CCBitmap* pBitmap, const char* szName, const char* szLevel, const char* szPrice, bool bLeveRestricted)
//	{
//		m_nAIID = nAIID;
//		m_nItemID = nItemID;
//
//		m_pBitmap = pBitmap;
//		m_UID = CCUID(0,0);
//		strcpy(m_szName, szName);
//		strcpy(m_szLevel, szLevel);
//		strcpy(m_szPrice, szPrice);
//		m_bLevelResticted = bLeveRestricted;
//	}
//
//	ZEquipmentListItem_OLD()
//	{
//		m_nAIID = 0;
//		m_nItemID = 0;
//
//		m_pBitmap = NULL;
//		m_UID = CCUID(0,0);
//		m_szName[0] = 0;
//		m_szLevel[0] = 0;
//		m_szPrice[0] = 0;
//		m_bLevelResticted = false;
//	}
//	virtual const char* GetString()
//	{
//		return m_szName;
//	}
//
//	virtual bool GetDragItem(CCBitmap** ppDragBitmap, char* szDragString, char* szDragItemString)
//	{
//		*ppDragBitmap = GetBitmap(0);
//		strcpy(szDragString, m_szName);
//		strcpy(szDragItemString, m_szName);
//
//		return true;
//	}
//	virtual CCBitmap* GetBitmap(int i)
//	{
//		if (i == 0) return m_pBitmap;
//		return NULL;
//	}
//	CCUID& GetUID() { return m_UID; }
//	int GetAIID() { return m_nAIID; }
//
//	int GetMSID() { return 0; }	/// �ӽ÷� �߰��ص�
//
//	unsigned long	GetItemID()		{ return m_nItemID; }
//
//	virtual void OnDraw(sRect& r, CCDrawContext* pDC, bool bSelected, bool bMouseOver);
//
//	// Ŀ�� ��ġ�� ������ ���� ������ ������ ��ġ�ΰ� �Ǵ��Ѵ� - ���� ������ �������� ���� ������
//	// ���ڴ� ��� ����Ʈ ������ ���� ��ǥ��� �־����
//	bool IsPtInRectToShowToolTip(sRect& rcItem, sPoint& pt);
//	
//	// ������ ������ rect �˾Ƴ��� (�׸� ��ǥ��)
//	void GetIconRect(sRect& out, const sRect& rcItem);
//
//	virtual int GetSortHint();
//};
//
//class ZItemMenu;
////typedef void (*ZCB_ONDROP)(void* pSelf, CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
//
//class ZEquipmentListBox_OLD : public MMultiColListBox
//{
//protected:
//	virtual bool IsDropable(CCWidget* pSender);
////	virtual bool OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
//	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
//
//protected:
////	ZCB_ONDROP			m_pOnDropFunc;
//	CCWidget*			m_pDescFrame;
//	int					m_idxItemLastTooltip;
//protected:
//	ZItemMenu*			m_pItemMenu;	// ZEquipmentList�� Exclusive�� Popup�� Child�϶��� Show()�����ϴ�
//	ZItemMenu* GetItemMenu()	{ return m_pItemMenu; }
//
//public:
//	ZEquipmentListBox_OLD(const char* szName, CCWidget* pParent=NULL, CCListener* pListener=NULL);
//	virtual ~ZEquipmentListBox_OLD();
//	void AttachMenu(ZItemMenu* pMenu);
//
//	char* GetItemDescriptionWidgetName();
//	void SetupItemDescTooltip();
//
////	void Add(const CCUID& uidItem, CCBitmap* pIconBitmap, const char* szName, const char* szWeight, const char* szSlot, const char* szPrice);
////	void Add(const CCUID& uidItem, CCBitmap* pIconBitmap, const char* szName, int nWeight, CCMatchItemSlotType nSlot, int nBountyPrice);
//
////	void SetOnDropCallback(ZCB_ONDROP pCallback) { m_pOnDropFunc = pCallback; }
//	void SetDescriptionWidget(CCWidget *pWidget)	{ m_pDescFrame = pWidget; }
//
//public:
//	#define CORE_EQUIPMENTLISTBOX	"EquipmentListBox"
//	virtual const char* GetClassName(){ return CORE_EQUIPMENTLISTBOX; }
//	//DWORD	m_dwLastMouseMove;
//	//int		m_nLastItem;
//};
//
////CCListener* ZGetShopAllEquipmentFilterListener();
////CCListener* ZGetEquipAllEquipmentFilterListener();
//
////CCListener* ZGetShopSaleItemListBoxListener();
////CCListener* ZGetCashShopItemListBoxListener();
////CCListener* ZGetShopPurchaseItemListBoxListener();
////CCListener* ZGetEquipmentItemListBoxListener();
////CCListener* ZGetAccountItemListBoxListener();
#endif