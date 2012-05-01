#pragma once
/// ������Ʈ�� Ÿ��
enum CCObjectType 
{
	CCOT_NONE		= 0,
	CCOT_PC			= 1,
	CCOT_NPC			= 2,		
	CCOT_ITEM		= 3,
	CCOT_CHARACTER
};

/// ĳ���� ����
enum CCCharacterRace
{
	CCCR_NONE		= 0,
	CCCR_HUMAN,
	CCCR_ELF,
	CCCR_DARKELF,
	CCCR_DWARF,
	CCCR_OGRE
};
typedef unsigned long int CCCharacterRaces;

/// ĳ���� Ŭ����
enum CCCharacterClass
{
	CCCC_NONE			= 0,
	CCCC_FIGHTER,
	CCCC_ROGUE,
	CCCC_ACOLYTE,
	CCCC_MAGE,

	// 2������
	CCCC_KNIGHT,
	CCCC_PALADIN,
	CCCC_BESERKER,
	CCCC_WARRIOR,
	CCCC_RANGER,
	CCCC_ASSASSIN,
	CCCC_HUNTER,
	CCCC_SHADOWWALKER,
	CCCC_SCOUT,
	CCCC_THIEF,
	CCCC_CLERIC,
	CCCC_MONK,
	CCCC_DOCTOR,
	CCCC_SHAMON,
	CCCC_DRUID,
	CCCC_SORCERER,
	CCCC_ENCHANTER,
	CCCC_WIZARD,
	CCCC_MAGICIAN,
	CCCC_WARLOCK
};
typedef unsigned long int CCCharacterClasses;

/// ĳ������ Ÿ��
enum CCCharacterType
{
	CCCT_NONE		= 0,
	CCCT_HUMANOID	= 1,	// �÷��̾�
};

enum CCCharacterMoveMode
{
	CCCMM_WALK		= 0,
	CCCMM_RUN		= 1
};

enum CCCharacterMode
{
	CCCM_PEACE		= 0,
	CCCM_OFFENSIVE	= 1
};

enum CCCharacterState
{
	CCCS_STAND		= 0,
	CCCS_SIT			= 1,
	CCCS_DEAD		= 2
};

/// ĳ���� ����
enum CCCharacterAbility
{
	CCCAB_LEVITATE			= 0,
	CCCAB_MOUNT				= 1,
	CCCAB_INVISIBILITY		= 2,
	CCCAB_STEALTH			= 4,
	CCCAB_SEE_INVISIBILITY	= 8,
	CCCAB_DETECT			= 16,
	CCCAB_INVINCIBILITY		= 32,
	CCCAB_DISABLE_ACTIVE	= 64,
	CCCAB_DISABLE_PASSIVE	= 128,
	CCCAB_STUN				= 256,
	CCCAB_SLEEP				= 512
};
typedef unsigned long int CCCharacterStates;


/*
/// ĳ���� ���°�
enum CCCharacterStatus
{
	CCCS_NONE			= 0,
	CCCS_SIT				= 1,
	CCCS_RUN				= 2,
	CCCS_MOVE			= 3,

	CCCS_ATTACK			= 5,
	CCCS_SKILL			= 6,
	CCCS_USEDISCIPLINE	= 7,
	CCCS_CONCENTRATE		= 8,

	CCCS_LEVITATE		= 17,
	CCCS_POISON			= 18,
	CCCS_CURSE			= 19,
	CCCS_STEALTH			= 20,
	CCCS_INVISIBILITY	= 21,
	CCCS_INVINCIBILITY	= 22,
	CCCS_PARALYSIS		= 23,
	CCCS_SILENCE			= 24,
	CCCS_STUN			= 25,
	CCCS_SLEEP			= 26
};
*/



/// ĳ���� �⺻ Ư��ġ
struct CCCharacterBasicAttr
{
	int			nSTR;
	int			nCON;
	int			nDEX;
	int			nAGI;
	int			nINT;
	int			nCHA;
	CCCharacterBasicAttr(): nSTR(0), nCON(0), 
			   nDEX(0), nAGI(0), nINT(0), nCHA(0) {    }
};

struct CCCharacterRepeatInfo
{
	int			nLastTime;
	float		fHP;
	float		fEN;
	CCCharacterRepeatInfo(): nLastTime(0), fHP(0), fEN(0) {	}
};

struct MCHARACTERDATA
{
	// �ĺ�
	char		szName[24];
	char		szSurname[24];
	int			iClass;				// enum
	int			iRace;				// enum
	int			iType;				// enum
	int			iSex;				// enum
	int			iSpecialization;	// enum
	int			iBindingSpot;		// pos

	// ����ġ
	int			iCurrentXP;
	int			iNextXP;

	// ����
	int			iRelationships;		// list
	int			iRshipRaise;		// list
	int			iRshipLower;		// list

	// ���׷�
	int			iMR;
	int			iFR;
	int			iCR;
	int			iPR;
	int			iLR;

	// ����
	int			iBUFFS;		// list
	int			iSkillList;	// list

	// ����
	int			iAC;
	int			iMFlee;
	int			iRFlee;
	int			iSFlee;
	int			iHPRegen;
	int			iENRegen;

	// ���ݷ�
	int			iMATK;
	int			iRATK;
	int			iCCCritical;
	int			iRCritical;
	int			iMAspd;
	int			iRAspd;
	int			iMAccu;
	int			iRAccu;

	// �̵�
	int			iDSight;
	int			iNSight;
	int			iSpd;

	// ����
	int			iLevitate;			// flag
	int			iInvisibility;		// flag
	int			iInvincibility;		// flag
	int			iStun;				// flag
	int			iStealth;			// flag
	int			iPoisoned;			// flag

	// �̹�
	int			iIVMelee;			// flag
	int			iIVRange;			// flag
	int			iIVMagic;			// flag
	int			iIVCold;			// flag
	int			iIVLightning;		// flag
	int			iIVPoison;			// flag
	int			iIVFire;			// flag
	int			iIVStun;			// flag
	int			iIVSleep;			// flag
	int			iIVCrit;			// flag
	int			iIVParalysis;		// flag

	// ����ȭ
	int			iOCS;
	int			iDCS;
	int			iRCS;
	int			iECS;
	int			iOMS;
	int			iDMS;
	int			iEMS;
};
