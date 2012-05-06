#ifndef ZITEMSLOTVIEW_H
#define ZITEMSLOTVIEW_H

#include "ZPrerequisites.h"
#include "CCWidget.h"
#include "RMesh.h"
#include "RVisualMeshMgr.h"
#include "ZMeshView.h"
#include "CCButton.h"
#include "CCMatchItem.h"

using namespace RealSpace2;


class ZItemSlotView : public CCButton{
protected:
	CCBitmap*				m_pBackBitmap;

	// 1) m_nItemID�� ������ �����۾��̵� ���õ� ��� ���� ������ item�� ����ϰ�,
	// 2) m_nItemID==-1�̸� �� ĳ������ ������ ������ �����ٰ� ����Ѵ�.

	unsigned long int		m_nItemID;
	unsigned int			m_nItemCount;

	CCMatchCharItemParts		m_nParts;

	bool					m_bSelectBox;			// ����Ʈ �ڽ� ��� ����
	bool					m_bDragAndDrop;			// �巡�� �� ��� ���� ����
	bool					m_bKindable;
	bool					m_bHorizonalInverse;	// �¿� �����ؼ� �׸����̳� (�������� ������)

	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool IsDropable(CCWidget* pSender);
	virtual bool OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);

	void SetDefaultText(CCMatchCharItemParts nParts);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	bool IsEquipableItem(unsigned long int nItemID, int nPlayerLevel, CCMatchSex nPlayerSex);

	virtual void OnMouseIn(void);
	virtual void OnMouseOut(void);

	const char* GetItemDescriptionWidgetName();

public:
	char					m_szItemSlotPlace[128];

	ZItemSlotView(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZItemSlotView(void);
	CCMatchCharItemParts GetParts() { return m_nParts; }
	void SetParts(CCMatchCharItemParts nParts);

	void SetBackBitmap(CCBitmap* pBitmap);
	void SetIConBitmap(CCBitmap* pBitmap);

	void EnableDragAndDrop( bool bEnable);

	void SetKindable( bool bKindable);

	void SetItemID(unsigned long int id) { m_nItemID = id; }
	void SetItemCount(unsigned long int nCnt) { m_nItemCount = nCnt; }
	void SetHorizontalInverse(bool b) { m_bHorizonalInverse = b; }


#define CORE_ITEMSLOTVIEW	"ItemSlotView"
	virtual const char* GetClassName(void){ return CORE_ITEMSLOTVIEW; }
};



#endif