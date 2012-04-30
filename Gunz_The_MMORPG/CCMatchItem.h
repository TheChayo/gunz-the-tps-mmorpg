#pragma once

#include "winsock2.h"
#include "CCXml.h"
#include "CCUID.h"
#include "CCSync.h"
#include "CCBaseItem.h"
#include "CCMemoryProxy.h"
#include <map>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

class CCZFileSystem;
class CCMatchCRC32XORCache;


/// ������ Ÿ��
enum CCMatchItemType
{
	MMIT_MELEE = 0,
	MMIT_RANGE,
	MMIT_EQUIPMENT,
	MMIT_CUSTOM,
	MMIT_TICKET,
	MMIT_AVATAR,
	MMIT_COMMUNITY,
	MMIT_LONGBUFF,
	MMIT_END
};

/// ������ ���� Ÿ��
enum CCMatchItemSlotType
{
	MMIST_NONE = 0,
	MMIST_MELEE,
	MMIST_RANGE,
	MMIST_CUSTOM,
	MMIST_HEAD,
	MMIST_CHEST,
	MMIST_HANDS,
	MMIST_LEGS,
	MMIST_FEET,
	MMIST_FINGER,
	MMIST_EXTRA,
	MMIST_AVATAR,
	MMIST_COMMUNITY,
	MMIST_LONGBUFF,
	MMIST_END
};

//�� ������.....
#define MMATCH_PARTS_ITEM		100
#define MMATCH_PARTS_PRIMARY	108
#define MMATCH_PARTS_SECONDARY	109
#define MMATCH_PARTS_CUSTOM1	110
#define MMATCH_PARTS_CUSTOM2	111

/// ������ 
// MMCIP_END�� ���� �� enum�� ������ ���� �ٲ�� ���÷��� �ε��� ������ ��ġ�Ƿ� �����Ϸ��� ���ǰ� �ʿ���..
// Ư�� MMCIP_END�� CCTD_CharInfo�� ũ�⸦ ��ȭ��Ű�Ƿ� ���÷��� �ε� �ڵ带 �������־�� �մϴ�.
// �׸��� �߰� �Ҷ��� ����� �� ������ �ؾ��մϴ�. �߰��� ������ CCMatchCharItemParts ����ϴ� Ŀ�ǵ� �ڵ带 ���� ���÷��̿����� ����� �� ���� ��
enum CCMatchCharItemParts
{
	MMCIP_HEAD		= 0,
	MMCIP_CHEST		= 1,
	MMCIP_HANDS  	= 2,
	MMCIP_LEGS		= 3,
	MMCIP_FEET		= 4,
	MMCIP_FINGERL	= 5,
	MMCIP_FINGERR	= 6,
	MMCIP_MELEE		= 7,
	MMCIP_PRIMARY	= 8,
	MMCIP_SECONDARY	= 9,
	MMCIP_CUSTOM1	= 10,	
	MMCIP_CUSTOM2	= 11,
	MMCIP_AVATAR	= 12,
	MMCIP_COMMUNITY1	= 13,
	MMCIP_COMMUNITY2	= 14,
	MMCIP_LONGBUFF1	= 15,
	MMCIP_LONGBUFF2	= 16,
	MMCIP_END
};

// Ŀ���� ������ Ÿ��
enum CCMatchCustomItemType
{
	MMCIT_MED_KIT		= 0,
	MMCIT_REPAIR_KIT,
	MMCIT_BULLET_KIT,
	MMCIT_DYNAMITE,
	MMCIT_REMOTE_CHARGE,
	MMCIT_DEMOLITION_TOOLS,
	MMCIT_FLASH_BANG,
	MMCIT_FRAGMENTATION,
	MMCIT_SMOKE_GRENADE,
	MMCIT_TEARGAS_GRENADE,
	MMCIT_FOOD,

	MMCIT_ENCHANT_FIRE,			// ��æƮ ������ - fire
	MMCIT_ENCHANT_COLD,			// ��æƮ ������ - cold
	MMCIT_ENCHANT_LIGHTNING,	// ��æƮ ������ - lightning
	MMCIT_ENCHANT_POISON,		// ��æƮ ������ - poison

	MMCIT_POTION,				// �Ҹ� ������ �� ȸ�� �迭 + ���� ����

	MMCIT_END
};

enum CCMatchMeleeItemType
{
	MIT_DAGGER			= 0,
	MIT_DUAL_DAGGER		= 1,
	MIT_KATANA			= 2,
	MIT_GREAT_SWORD		= 3,
	MIT_DOUBLE_KATANA	= 4,

	MIT_END
};

enum CCMatchRangeItemType
{
	RIT_PISTOL			= 0,
	RIT_PISTOLx2		= 1,
	RIT_REVOLVER		= 2,
	RIT_REVOLVERx2		= 3,
	RIT_SMG				= 4,
	RIT_SMGx2			= 5,
	RIT_SHOTGUN			= 6,
	RIT_SAWED_SHOTGUN	= 7,
	RIT_RIFLE			= 8,
	RIT_MACHINEGUN		= 9,
	RIT_ROCKET			= 10,
	RIT_SNIFER			= 11,

	RIT_END
};

// Melee, Range, Custom�� ��ģ ���� Ÿ�� .. �Ѽ� ����� ������ ����..
enum CCMatchWeaponType
{
	MWT_NONE	= 0,

	// melee
	MWT_DAGGER,
	MWT_DUAL_DAGGER,
	MWT_KATANA,
	MWT_GREAT_SWORD,
	MWT_DOUBLE_KATANA,

	// range
	MWT_PISTOL,
	MWT_PISTOLx2,
	MWT_REVOLVER,
	MWT_REVOLVERx2,
	MWT_SMG,
	MWT_SMGx2,
	MWT_SHOTGUN,
	MWT_SAWED_SHOTGUN,
	MWT_RIFLE,
	MWT_MACHINEGUN,
	MWT_ROCKET,
	MWT_SNIFER,

	// custom
	MWT_MED_KIT,
	MWT_REPAIR_KIT,
	MWT_BULLET_KIT,
	MWT_FLASH_BANG,
	MWT_FRAGMENTATION,
	MWT_SMOKE_GRENADE,
	MWT_FOOD,
	MWT_SKILL,				// NPC�� - skill.xml�� ����Ǿ� �ִ� �ɷ� �����Ѵ�.

	// custom - enchant
	MWT_ENCHANT_FIRE,			
	MWT_ENCHANT_COLD,
	MWT_ENCHANT_LIGHTNING,
	MWT_ENCHANT_POISON,

	// custom
	MWT_POTION,
	MWT_TRAP,
	MWT_DYNAMITYE,

	MWT_END
};

enum CCMatchSpendType
{
	MMCT_NONE,
	MMCT_NORMAL,	// �Ϲ� ��� ������(���̳�����Ʈ. ����)
	MMCT_LONGBUFF,	// �� ���� ��� ������
	MMCT_SHORTBUFF,	// �� ���� ��� ������
	MMCT_COMMUNITY,	// Ŀ�´�Ƽ ��� ������
	MMCT_END
};

enum CCMatchDamageType
{
	MMDT_NORMAL,
	MMDT_FIRE,
	MMDT_COLD,
	MMDT_POISION,
	MMDT_LIGHTING,
	MMDT_HEAL,
	MMDT_REPAIR,
	MMDT_HASTE,
	MMDT_END
};

enum CCMatchTicketType
{
	MMTT_NONE = 0,
	MMTT_ADMISSION,
	MMIT_CHANGEHEAD,

	MMTT_END,
};

enum CCMatchItemEffectId
{
	// zitem.xml���� �Ʒ� ����� ����ϹǷ� ������ ��ġ�� ���Ƿ� �����ϸ� �ȵ˴ϴ�
	MMIEI_NONE = 0,
	
	// �Ƿ�ĸ��, �Ƿ����, ����ĸ��, �������� ����Ʈ ����
	MMIEI_POTION_HEAL_INSTANT		= 100,
	MMIEI_POTION_REPAIR_INSTANT		= 101,
	MMIEI_POTION_HEAL_OVERTIME		= 102,
	MMIEI_POTION_REPAIR_OVERTIME	= 103,

	// ����ĸ��
	MMIEI_POTION_HASTE				= 130,
};

struct CCMatchItemEffectDesc
{
	unsigned long int	m_nID;
	char				m_szName[128];
	int					m_nArea;
	unsigned long int	m_nTime;
	int					m_nModHP;
	int					m_nModAP;
	int					m_nModMaxWT;
	int					m_nModSF;
	int					m_nModFR;
	int					m_nModCR;
	int					m_nModPR;
	int					m_nModLR;
	int					m_nResAP;
	int					m_nResFR;
	int					m_nResCR;
	int					m_nResPR;
	int					m_nResLR;
	int					m_nStun;
	int					m_nKnockBack;
	int					m_nSmoke;
	int					m_nFlash;
	int					m_nTear;
	int					m_nFlame;
};

enum CCMatchItemBonusType
{
	MIBT_SOLO = 0,
	MIBT_TEAM = 1,
	MIBT_QUEST = 2
};

struct CCMatchItemBonus
{
	float				m_fXP_SoloBonus;
	float				m_fXP_TeamBonus;
	float				m_fXP_QuestBonus;

	float				m_fBP_SoloBonus;
	float				m_fBP_TeamBonus;
	float				m_fBP_QuestBonus;
};

struct CCMatchItemName
{
	char m_szItemName[128];
	char m_szMeshName[128];
};

struct CCMatchAvatarMeshName
{
	char m_szHeadMeshName[128];
	char m_szChestMeshName[128];
	char m_szHandMeshName[128];
	char m_szLegsMeshName[128];
	char m_szFeetMeshName[128];	
};

struct CCMatchItemDesc
{
	unsigned long int		m_nID;
//	char					m_szItemName[128];
	MProtectValue<CCMatchItemName>* m_pMItemName;
	MProtectValue<int>		m_nTotalPoint;
	MProtectValue<CCMatchWeaponType>	m_nWeaponType;
	MProtectValue<CCMatchItemType>	m_nType;
	MProtectValue<int>		m_nResSex;
	MProtectValue<int>		m_nResLevel;
	CCMatchItemSlotType		m_nSlot;
	
	MProtectValue<int>		m_nWeight;
	MProtectValue<int>		m_nBountyPrice;
	bool					m_bIsCashItem;	
	MProtectValue<int>		m_nDelay;
	CCMatchItemEffectDesc*	m_pEffect;
	CCMatchItemEffectId		m_nEffectId;
	MProtectValue<int>		m_nControllability;
	MProtectValue<int>		m_nMagazine;
	MProtectValue<int>		m_nMaxBullet;
	MProtectValue<int>		m_nReloadTime;
	bool					m_bSlugOutput;
	MProtectValue<int>		m_nGadgetID;
	MProtectValue<int>		m_nHP;
	MProtectValue<int>		m_nAP;
	MProtectValue<int>		m_nMaxWT;
	MProtectValue<int>		m_nSF;
	MProtectValue<int>		m_nFR;
	MProtectValue<int>		m_nCR;
	MProtectValue<int>		m_nPR;
	MProtectValue<int>		m_nLR;
	MProtectValue<int>		m_nLimitSpeed;
	MProtectValue<int>		m_nLimitJump;
	MProtectValue<int>		m_nLimitTumble;
	MProtectValue<int>		m_nLimitWall;
	MProtectValue<int>		m_nRange;		// melee���� ���ݹ���
	MProtectValue<int>		m_nAngle;		// melee���� ���ݰ���
	MProtectValue<int>		m_nEffectLevel;


	MProtectValue<int>				m_nDamage;			// ������� ���ݷ�
	MProtectValue<int>				m_nItemPower;		// �Ҹ� �������� ������ �Ǵ� ȸ����
	MProtectValue<int>				m_nDamageTime;		// ��Ʈ ������ �������� ȸ���� ���� �ð�(0�� ���, �� �游 ����)
	MProtectValue<CCMatchDamageType>	m_nDamageType;		// �Ҹ� �������� ������ Ÿ�� (������ �Ӽ� Ȥ�� ȸ�� �Ӽ�)
	MProtectValue<int>		m_nLifeTime;				// �۵� �ð� (����� Ʈ���� �۵��ð����� ���)

	char					m_szDesc[8192];	// ����

	bool								m_bIsSpendableItem;
	MProtectValue<CCMatchSpendType>		m_nSpendType;

	MProtectValue<CCMatchAvatarMeshName>*		m_pAvatarMeshName;

	unsigned long int		m_nColor;
	int						m_nImageID;
	int						m_nBulletImageID;
	int						m_nMagazineImageID;
	char					m_szReloadSndName[256];
	char					m_szFireSndName[256];
	char					m_szDryfireSndName[256];
	char					m_szWeaponByFiber[256];		// melee ������ ������ ���� �Ҹ�

	CCMatchItemBonus			m_Bonus;
	CCMatchTicketType		m_TicketType;
	MProtectValue<int>		m_nMaxRentPeriod;	// �Ⱓ�� �ٿ�Ƽ �������� �ִ� �Ⱓ ���� (��¥����)

	bool					m_bIsEnableMoveToAccountItem;

	CCMatchItemDesc();
	~CCMatchItemDesc();

	int GetSellBountyValue(int nCnt = 1) { return int(m_nBountyPrice.Ref() * 0.25) * nCnt; }

	bool IsCashItem()		{ if ((m_nID>=500000) || (m_bIsCashItem)) return true; return false; }
	bool IsEnchantItem()	{ if (m_nWeaponType.Ref() >= MWT_ENCHANT_FIRE && m_nWeaponType.Ref() <= MWT_ENCHANT_POISON) return true; 
								return false; }
	bool IsUnLimitItem()		{ return RENT_PERIOD_UNLIMITED == m_nMaxRentPeriod.Ref(); }	
	bool IsSpendableItem()		{ return m_bIsSpendableItem; }


	void CacheCRC32( CCMatchCRC32XORCache& crc );

	void ShiftFugitiveValues();

	void DumpBinary(FILE* fp);
	void LoadBinary(FILE* fp);
};


struct CCMatchItemDescForDatabase
{
	unsigned long int m_nID;

	int m_nResSex;
	int	m_nResLevel;

	CCMatchItemSlotType	m_nSlot;

	int m_nWeight;
	int m_nBountyPrice;
	int m_nDamage;
	int m_nDelay;

	int m_nControllability;
	int m_nMaxBullet;
	int m_nMagazine;
	int m_nReloadTime;	
	
	int m_nHP;
	int m_nAP;

	bool m_bIsCashItem;
	bool m_bIsSpendableItem;
};

// ���԰� ������ �������� üũ
bool IsSuitableItemSlot(CCMatchItemSlotType nSlotType, CCMatchCharItemParts nParts);
CCMatchCharItemParts GetSuitableItemParts(CCMatchItemSlotType nSlotType);
CCMatchItemSlotType	GetSuitableItemSlot(CCMatchCharItemParts nParts);
bool IsWeaponItemSlotType(CCMatchItemSlotType nSlotType);
bool IsWeaponCharItemParts(CCMatchCharItemParts nParts);

char* GetItemSlotTypeStr(CCMatchItemSlotType nSlotType);
char* GetCharItemPartsStr(CCMatchCharItemParts nParts);

// ����Ÿ�� �˾Ƴ���
CCMatchWeaponType GetWeaponType(CCMatchMeleeItemType nMeleeItemType);
CCMatchWeaponType GetWeaponType(CCMatchRangeItemType nRangeItemType);
CCMatchWeaponType GetWeaponType(CCMatchCustomItemType nCustomItemType);

// ��æƮ���������� Ȯ��
bool IsEnchantItem(CCMatchItemDesc* pItemDesc);

/*
class CCMatchItemEffectDescMgr : public map<int, CCMatchItemEffectDesc*>
{
protected:
	void ParseEffect(CCXmlElement& element);
public:
	CCMatchItemEffectDescMgr();
	virtual ~CCMatchItemEffectDescMgr();
	bool ReadXml(const char* szFileName);
	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);
	void Clear();
	CCMatchItemEffectDesc* GetEffectDesc(int nID);
	static CCMatchItemEffectDescMgr* GetInstance();
};

inline CCMatchItemEffectDescMgr* MGetMatchItemEffectDescMgr() { return CCMatchItemEffectDescMgr::GetInstance(); }
*/

class CCMatchItemDescMgr : public map<int, CCMatchItemDesc*>
{
	unsigned long m_nChecksum;

	int m_nextItemIdToMemoryShift;
	
protected:
	bool ParseItem(CCXmlElement& element);
public:
	CCMatchItemDescMgr();
	virtual ~CCMatchItemDescMgr();
	bool ReadXml(const char* szFileName);
	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);
	void Clear();
	CCMatchItemDesc* GetItemDesc(unsigned long int nID);
	static CCMatchItemDescMgr* GetInstance();

	unsigned long GetChecksum() { return m_nChecksum; }

	void ShiftMemoryGradually();

	// ���� ���õ� Validation Check
	bool ValidateItemBuff();

	bool ReadCache();
	void WriteCache();
};
inline CCMatchItemDescMgr* MGetMatchItemDescMgr() { return CCMatchItemDescMgr::GetInstance(); }



typedef struct _DBCharItemCachingData
{
	bool	bNeedDBUpdate;
	int		nAddedItemCount;

	void Reset() { 
		nAddedItemCount = 0; 
		bNeedDBUpdate = false;
	}

	void IncCnt(int nVal) { 
		nAddedItemCount += nVal; 
		bNeedDBUpdate = true; 
	}

	void DecCnt(int nVal) {
		nAddedItemCount -= nVal; 
		bNeedDBUpdate = true; 
	}
	
	bool IsNeedDBUpdate() { return bNeedDBUpdate; }
} DBCharItemCachingData;


class CCMatchItem : public MBaseItem
{
private:
protected:
	CCUID				m_uidItem;	
	CCMatchItemDesc*		m_pDesc;					///< ��ũ����
	bool				m_bEquiped;					///< ����ϰ� �ִ��� ����
	unsigned long int	m_nRentItemRegTime;			///< �Ⱓ�� �������� ��� ����� �ð�. �� �ð��� ������ ��ϵǴ� �ð���.

	// ����ϴ� �߰��� description�� NULL�� �Ǵ��� �˻��ϱ� ���ؼ�.
	unsigned int		m_nItemID;

	WORD				m_nCountOfNonDestroyItem;		///< ����ϸ� ������ ������ ��������� �ʴ� �������� �ִ� ��� ����(ex ����Ŷ).
	WORD				m_nUseCountOfNonDestroyItem;	///< ����ϸ� ������ ������ ��������� �ʴ� �������� ���� ����(ex ����Ŷ).


	unsigned long int	m_nCIID;					///< DB�� ���� CIID

	DBCharItemCachingData m_CharItemCachingData;

protected:
	void SetDesc(CCMatchItemDesc* pDesc) { m_pDesc = pDesc; }

public:
	CCMatchItem();
	virtual ~CCMatchItem();

	bool Create( const CCUID& uid, CCMatchItemDesc* pDesc, const WORD nCountOfNonDesctroyItem, int nCount = 1);
	void Destroy();

	CCUID				GetUID() const								{ return m_uidItem; }
	unsigned long int	GetCIID() const								{ return m_nCIID; }
	unsigned long int	GetDescID() const;
	
	unsigned long int	GetRentItemRegTime() const					{ return m_nRentItemRegTime; }
	CCMatchItemDesc*		GetDesc() const;
	CCMatchItemType		GetItemType();
	
	void				SetCIID(unsigned long int nCIID)			{ m_nCIID = nCIID; }	
	void				SetEquiped(bool bIsEquiped)					{ m_bEquiped = bIsEquiped; }
	void				SetRentItemRegTime(unsigned long int nTime)	{ m_nRentItemRegTime = nTime; }	

	bool				IsEmpty()									{ return (((m_pDesc == NULL) || (m_nCount <= 0)) ? true : false); }
	bool				IsEquiped() const							{ return m_bEquiped; }			

	const WORD			GetCountOfNonDestroyItem()					{ return m_nCountOfNonDestroyItem; }
	const WORD			GetUseCountOfNonDestroyItem()				{ return m_nUseCountOfNonDestroyItem; }
	void				ResetUseCountOfNonDestroyItem()				{ m_nUseCountOfNonDestroyItem = 0; }
	void				IncreaseUseCountOfNonDestroyItem();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DB Caching�� ���Ͽ� ������� ��� �Լ���..	
	void				IncreaseCount(int nVal);
	void				DecreaseCount(int nVal);
	void				DecreaseCountWithCaching(int nVal);

	bool				IsNeedDBUpdate()							{ return m_CharItemCachingData.IsNeedDBUpdate(); }
	void				DBUpdateDone()								{ m_CharItemCachingData.Reset(); }	
	DBCharItemCachingData* GetItemCachingData()						{ return &m_CharItemCachingData; }
};


class CCMatchItemMap;
class CCMatchCharInfo;

typedef struct _EquipedParts
{
	CCUID	uidParts;
	int		nItemCount;
} EquipedParts;

/// ����ϰ��ִ� ������
class CCMatchEquipedItem
{
protected:
	// CCMatchItem*		m_pParts[MMCIP_END];
	//EquipedParts	m_Parts[MMCIP_END];
	CCUID			m_uidParts[ MMCIP_END ];	
	
	CCMatchCharInfo*	m_pOwner;

public:
	CCMatchEquipedItem() 
	{ 
		// memset(m_pParts, 0, sizeof(m_pParts)); 
		memset( m_uidParts, 0, sizeof(m_uidParts) );
		// memset(m_Parts, 0, sizeof(EquipedParts) * MMCIP_END);


		m_pOwner = NULL;
	}

	virtual ~CCMatchEquipedItem() 
	{ 
		m_pOwner = NULL;
	}
public:
	// bool SetItem(CCMatchCharItemParts parts, CCMatchItem* pMatchItem);
	bool SetItem( CCMatchCharItemParts parts, const CCUID& uidItem, CCMatchItem* pItem );

	CCMatchItem* GetItem(CCMatchCharItemParts parts); //  { return m_pParts[parts]; }
	void Remove(CCMatchCharItemParts parts);
	void Remove(const CCUID& uidParts);
	// bool IsEmpty(CCMatchCharItemParts parts) { if (m_pParts[parts] != NULL) return false; return true; }
	bool IsEmpty(CCMatchCharItemParts parts); //  { if (m_pParts[parts] != NULL) return false; return true; }
	void GetTotalWeight(int* poutWeight, int* poutMaxWeight);
	// bool IsEquipedItem(CCMatchItem* pCheckItem, CCMatchCharItemParts& outParts); // �ش� �������� ��������� üũ
	bool IsEquipedItem(const CCUID& uidItem, CCMatchCharItemParts& outParts); // �ش� �������� ��������� üũ
	void Clear();
	void SetOwner( CCMatchCharInfo* pOwner ) { _ASSERT( NULL != pOwner ); m_pOwner = pOwner; }


private :
	// CCMatchItem* GetMyItem( CCMatchCharItemParts parts );
};

/// ĳ���Ͱ� ���� �ִ� �����۵�
class CCMatchItemMap : public map<CCUID, CCMatchItem*>
{
private:
protected:
	static CCUID				m_uidGenerate;
	static CCCriticalSection	m_csUIDGenerateLock;
	bool					m_bDoneDbAccess;		// ��񿡼� ������ �����Ծ����� ����

	/// �Ⱓ�� �������� �ϳ��� �ִ��� ����
	/// ���� - �Ⱓ�� �������� �ϳ��� ������ true�� ���� �ִ�. 
	bool					m_bHasRentItem;			

public:
	CCMatchItemMap();
	virtual ~CCMatchItemMap();
	bool IsEmpty() const { return empty(); }
	int GetCount() const { return (int)size(); }
	virtual bool CreateItem( const CCUID& uid
		, int nCIID
		, int nItemDescID
		, bool bRentItem = false
		, DWORD dwRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED
		, const WORD wRentHourPeriod = RENT_PERIOD_UNLIMITED
		, int nCount = 1);
	bool			RemoveItem(const CCUID& uidItem);

	virtual void	Clear();
	CCMatchItem*		GetItem(const CCUID& uidItem) const;
	CCMatchItem*		GetItemByItemID(const DWORD dwItemID) const;		//< �������� ItemID�� �־, 1���� �����ش�(Spendable ������ ����)
																		//< �� ��ȣ�� ���� ������, �׷��� �������..

	CCMatchItem*		GetItemByCIID(const DWORD dwCIID) const;


	bool			IsDoneDbAccess() const			{ return m_bDoneDbAccess; }
	void			SetDbAccess(bool bVal)			{ m_bDoneDbAccess = bVal; }
	bool			HasRentItem() const				{ return m_bHasRentItem; }
	const bool		IsHave( const DWORD dwItemID ) const;

	int	 GetItemCount(const CCUID& uidItem);

public:
	static CCUID UseUID() {
		m_csUIDGenerateLock.Lock();
			m_uidGenerate.Increase();	
		m_csUIDGenerateLock.Unlock();
		return m_uidGenerate;
	}
};


bool IsExpiredRentItem( const CCMatchItem* pMItem, const DWORD dwTick );

#include "cxr_CCMatchItem.h"
