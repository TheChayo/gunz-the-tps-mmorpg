#ifndef _ZCHARACTERITEM_H
#define _ZCHARACTERITEM_H

#include "CCMatchItem.h"
#include "ZItem.h"
#include "ZFile.h"
#include <list>
#include <algorithm>
using namespace std;


/// ĳ���Ͱ� ����ϰ� �ִ� �����۵�
class ZCharacterItem
{
private:
protected:
	ZItem					m_Items[MMCIP_END];
	CCMatchCharItemParts		m_nSelectedWeapon;		// ������ Item�� ���� m_Items�� �ش��ϴ� �ε���
	bool Confirm(CCMatchCharItemParts parts, CCMatchItemDesc* pDesc);
	bool IsWeaponItem(CCMatchCharItemParts parts);
public:
	ZCharacterItem();
	virtual ~ZCharacterItem();
	void SelectWeapon(CCMatchCharItemParts parts);
	bool EquipItem(CCMatchCharItemParts parts, int nItemDescID, int nItemCount = 1);

	bool Reload();

	ZItem* GetItem(CCMatchCharItemParts parts)
	{
		if ((parts < MMCIP_HEAD) || (parts >= MMCIP_END))
		{
			_ASSERT(0);
			return NULL;
		}
		return &m_Items[(int)parts]; 
	}
	ZItem* GetSelectedWeapon(); 
	CCMatchCharItemParts GetSelectedWeaponParts() { return (CCMatchCharItemParts)m_nSelectedWeapon; }

	CCMatchCharItemParts GetSelectedWeaponType() {
		return m_nSelectedWeapon;
	}

	bool Save(ZFile *file);
	bool Load(ZFile *file, int nReplayVersion);

	void ShiftFugitiveValues();
	
	/*
	void SetWarppingItemDesc(DWORD tick)
	{ 
		for(int i = 0; i < (int)MMCIP_END; ++i)
		{
			CCMatchItemDesc* pDesc = ((CCMatchItem)m_Items[i]).GetDesc();
			if(pDesc)
				pDesc->m_pMItemName->SetWarpingAdd(tick);
		}
	}
	*/
};

#endif