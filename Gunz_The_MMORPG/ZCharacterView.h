#ifndef ZCHARACTERVIEW_H
#define ZCHARACTERVIEW_H

#include "ZPrerequisites.h"
#include "ZMeshView.h"
#include "CCUID.h"
#include "CCMatchItem.h"

#include "RCharCloth.h"

class ZItemSlot{
public:
	unsigned long int	m_nItemID;
	sRect				m_Rect;
};

struct ZCharacterViewInfo
{
	CCUID		UID;
	CCMatchSex	nSex;
	int			nHair;
	int			nFace;
	int			nLevel;

	//jintriple3 �� �ٲٱ� �� ������..�޸� ���Ͻ÷�..
	CCProtectValue<int>* m_pMnTeam;
//	CCMatchTeam	nTeam;
	CCMatchObjectStageState		nStageState;
//	bool		bReady;
	bool		bMaster;
	bool		bFireWall;
	bool		bNAT;
};

class ZCharacterView : public ZMeshView{
public:
	bool				m_bDrawInfo;
	ZCharacterViewInfo	m_Info;
	ZItemSlot			m_ItemSlots[MMCIP_END];
	ZMeshView*			m_pItemMeshView[MMCIP_END];
protected:
	bool				m_bVisibleEquipment;
	CCMatchCharItemParts	m_nVisualWeaponParts;	// ���� �ִ� ������ �����ִ� ���� ����

	bool				m_bAutoRotate;
	DWORD				m_dwTime;				// ȸ�� �ð�
	
protected:
	void		 RepositionItemSlots();

	virtual void OnSize(int w, int h);
	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool IsDropable(CCWidget* pSender);
	virtual bool OnDrop(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString);
public:
	ZCharacterView(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZCharacterView();

	void SetDrawInfo(bool bVal)	{ m_bDrawInfo = bVal; }
	bool GetDrawInfo()			{ return m_bDrawInfo; }

	void InitCharParts(ZCharacterView* pCharView, CCMatchCharItemParts nVisualWeaponParts = MMCIP_PRIMARY);
	void InitCharParts(CCMatchSex nSex, unsigned int nHair, unsigned int nFace, 
				       const unsigned long int* nEquipItemIDs, CCMatchCharItemParts nVisualWeaponParts = MMCIP_PRIMARY);
	void SetParts(CCMatchCharItemParts nParts, unsigned int nItemID);
	void ChangeVisualWeaponParts(CCMatchCharItemParts nVisualWeaponParts);

	void SetVisibleEquipment(bool bVisible)		{ m_bVisibleEquipment = bVisible; }
	void SetSelectMyCharacter();

	void EnableAutoRotate( bool bAutoRotate)	{ m_bAutoRotate = bAutoRotate; }
	bool IsAutoRotate()							{ return m_bAutoRotate; }

	virtual void OnInvalidate();
	virtual void OnRestore();

public:
	#define CORE_CHARACTERVIEW	"CharacterView"
	virtual const char* GetClassName(){ return CORE_CHARACTERVIEW; }
	
	void SetCharacter( CCUID uid );
	const CCUID& GetCharacter() { return m_Info.UID; }
};

unsigned long int GetVisualWeaponID(unsigned long int nMeleeItemID, unsigned long int nPrimaryItemID,
									unsigned long int nSecondaryItemID, unsigned long int nCustom1ItemID,
									unsigned long int nCustom2ItemID);



#endif