#ifndef _CCMATCHTRANSDATATYPE_H
#define _CCMATCHTRANSDATATYPE_H

#include "CCMatchObject.h"
#include "CCMatchRule.h"		// CCMATCH_GAMETYPE, CCMATCH_ROUNDSTATE, CCMATCH_ROUNDRESULT ���� -> �������
#include "CCMatchStageSetting.h"
#include "CCMatchGameType.h"
#include "CCMatchGlobal.h"

#pragma pack(push, old)
#pragma pack(1)

// �� ĳ���� ����Ʈ ���� - ĳ���� ���ý� ���
struct CCTD_AccountCharInfo
{
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				nCharNum;
	unsigned char		nLevel;
};

struct CCTD_CharInfo
{
	// ���� ����ü�� ������ �����Ϸ��� ���� ���÷����� �ε��� ���ؼ� ���� ���� ����ü�� ZReplay.cpp�� �����ϰ�
	// �������� �ε� �ڵ带 �ۼ������ �մϴ�. ������ �߰��� ������ �������� �����̴� ���� �׳��� �����մϴ�.

	// ĳ���� ����
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				szClanName[CLAN_NAME_LENGTH];
	CCMatchClanGrade		nClanGrade;
	unsigned short		nClanContPoint;
	char				nCharNum;
	unsigned short		nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nXP;
	int					nBP;
	float				fBonusRate;
	unsigned short		nPrize;
	unsigned short		nHP;
	unsigned short		nAP;
	unsigned short		nMaxWeight;
	unsigned short		nSafeFalls;
	unsigned short		nFR;
	unsigned short		nCR;
	unsigned short		nER;
	unsigned short		nWR;

	// ������ ����
	unsigned long int	nEquipedItemDesc[MMCIP_END];

	// account �� ����
	CCMatchUserGradeID	nUGradeID;

	// ClanCLID
	unsigned int		nClanCLID;

	// ������ �����ʸ�Ʈ ���
	int					nDTLastWeekGrade;	

	// ������ ���� �߰�
	CCUID				uidEquipedItem[MMCIP_END];
	unsigned long int	nEquipedItemCount[MMCIP_END];
};

//���������ӽ��ּ� 
/*
struct CCTD_CharBuffInfo
{
	CCShortBuffInfo	ShortBuffInfo[MAX_CHARACTER_SHORT_BUFF_COUNT];	
};*/

struct CCTD_BuffInfo
{
	unsigned long int	nItemId;		// ������ ����Ų ������ID
	unsigned short		nRemainedTime;	// ���� ���� ���ҳ� (�ʴ����� ����), �����ۿ� ���󼭴� Ƚ���� ��� ���� �ִ�
};

// �� ĳ���� ������ �߰� ����
struct CCTD_MyExtraCharInfo
{
	char	nLevelPercent;		///< ���� ����ġ ���� �ۼ�Ʈ
};

// ���� ������� �ʴ´�.
struct CCTD_SimpleCharInfo
{
	char				szName[32];
	char				nLevel;
	char				nSex;
	char				nHair;
	char				nFace;
	unsigned long int	nEquipedItemDesc[MMCIP_END];
};


struct CCTD_MySimpleCharInfo
{
	unsigned char		nLevel;
	unsigned long int	nXP;
	int					nBP;
};

struct CCTD_CharLevelInfo
{
	unsigned char		nLevel;
	unsigned long int	nCurrLevelExp;
	unsigned long int	nNextLevelExp;
};

struct CCTD_RoundPeerInfo
{
	CCUID			uidChar;
	unsigned char	nHP;
	unsigned char	nAP;
};

struct CCTD_RoundKillInfo
{
	CCUID	uidAttacker;
	CCUID	uidVictim;
};

struct CCTD_ItemNode
{
	CCUID				uidItem;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;		// �Ⱓ�� ������ ��밡�ɽð�(�ʴ���), RENT_MINUTE_PERIOD_UNLIMITED�̸� ������
	int					iMaxUseHour;					// �ִ� ��� �ð�(�ð� ����)
	int					nCount;
};

struct CCTD_RelayMap
{
	int				nMapID;
};

struct CCTD_AccountItemNode
{
	int					nAIID;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;		// �Ⱓ�� ������ ��밡�ɽð�(�ʴ���), RENT_MINUTE_PERIOD_UNLIMITED�̸� ������
	int					nCount;
};

// ���Ӿ� ���� ����
struct CCTD_GameInfoPlayerItem
{
	CCUID	uidPlayer;
	bool	bAlive;
	int		nKillCount;
	int		nDeathCount;
};

struct CCTD_GameInfo
{
	char	nRedTeamScore;		// ���������� ����ϴ� ����������
	char	nBlueTeamScore;		// ���������� ����ϴ� ���������

	short	nRedTeamKills;		// ������������ġ������ ����ϴ� ������ų��
	short	nBlueTeamKills;		// ������������ġ������ ����ϴ� �����ų��
};

struct CCTD_RuleInfo	
{
	unsigned char	nRuleType;
};

struct CCTD_RuleInfo_Assassinate : public CCTD_RuleInfo
{
	CCUID	uidRedCommander;
	CCUID	uidBlueCommander;
};

struct CCTD_RuleInfo_Berserker : public CCTD_RuleInfo
{
	CCUID	uidBerserker;
};


enum CCTD_PlayerFlags {
	CCTD_PlayerFlags_AdminHide	= 1,
	CCTD_PlayerFlags_BridgePeer	= 1<<1,
	CCTD_PlayerFlags_Premium		= 1<<2			// �ݸ��� �ǽù� ���ʽ�
};

struct CCTD_ChannelPlayerListNode 
{
	CCUID			uidPlayer;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szClanName[CLAN_NAME_LENGTH];
	char			nLevel;
	char			nDTLastWeekGrade;
	CCMatchPlace		nPlace;
	unsigned char	nGrade;			// �κ񿡼��� uid �� ĳ������ ����� �˼��� ���..
	unsigned char	nPlayerFlags;	// �÷��̾� �Ӽ�(��ڼ����) - CCTD_PlayerFlags ���
	unsigned int	nCLID;			// ClanID
	unsigned int	nEmblemChecksum;// Emblem Checksum
};


struct CCTD_ClanMemberListNode 
{
	CCUID				uidPlayer;
	char				szName[MATCHOBJECT_NAME_LENGTH];
	char				nLevel;
	CCMatchClanGrade		nClanGrade;
	CCMatchPlace			nPlace;
};

enum CCTD_WorldItemSubType
{
	CCTD_Dynamic = 0,
	CCTD_Static  = 1,
};

// ������ ���� ����
struct CCTD_WorldItem
{
	unsigned short	nUID;
	unsigned short	nItemID;
	unsigned short  nItemSubType;
	short			x;
	short			y;
	short			z;
/*
	float			x;
	float			y;
	float			z;
*/
};


// �ߵ��� Ʈ�� ����
struct CCTD_ActivatedTrap
{
	CCUID				uidOwner;
	unsigned short		nItemID;
	unsigned long int	nTimeElapsed;
	short	x;
	short	y;
	short	z;
};


// �ٷΰ����ϱ� ���͸� ����
struct CCTD_QuickJoinParam
{
	unsigned long int	nMapEnum;		// ���ϴ� ���� ��Ʈ���
	unsigned long int	nModeEnum;		// ���ϴ� ���Ӹ���� ��Ʈ���
};


// ĳ������ Ŭ�� ������Ʈ ����
struct CCTD_CharClanInfo
{
	char				szClanName[CLAN_NAME_LENGTH];		// Ŭ�� �̸�
	CCMatchClanGrade		nGrade;
};


// ���� ��������
struct CCTD_CharInfo_Detail
{
	char				szName[32];						// �̸�
	char				szClanName[CLAN_NAME_LENGTH];	// Ŭ���̸�
	CCMatchClanGrade		nClanGrade;						// Ŭ����å
	int					nClanContPoint;					// Ŭ�� �⿩��
	unsigned short		nLevel;							// ����
	char				nSex;							// ����
	char				nHair;							// �Ӹ� �ڽ���
	char				nFace;							// �� �ڽ���
	unsigned long int	nXP;							// xp
	int					nBP;							// bp

	int					nKillCount;
	int					nDeathCount;

	// ���ӻ�Ȳ

	unsigned long int	nTotalPlayTimeSec;				// �� �÷��� �ð�
	unsigned long int	nConnPlayTimeSec;				// ���� ���� �ð�


	unsigned long int	nEquipedItemDesc[MMCIP_END];	// ������ ����

	CCMatchUserGradeID	nUGradeID;						// account UGrade

	// ClanCLID
	unsigned int		nClanCLID;
};


/// �� ����Ʈ �޶�� ��û�Ҷ� ������ ����ü
struct CCTD_StageListNode
{
	CCUID			uidStage;							///< �� UID
	unsigned char	nNo;								///< ���ȣ
	char			szStageName[STAGENAME_LENGTH];		///< ���̸�
	char			nPlayers;							///< �����ο�
	char			nMaxPlayers;						///< �ִ��ο�
	STAGE_STATE		nState;								///< �������
	CCMATCH_GAMETYPE nGameType;							///< ���� Ÿ��
	char			nMapIndex;							///< ��
	int				nSettingFlag;						///< �� ���� �÷���(����, ��й�, ��������)
	char			nMasterLevel;						///< ���� ����
	char			nLimitLevel;						///< ���ѷ���
};

/// Ŭ���̾�Ʈ�� �˾ƾ��� ��Ÿ���� : AdminHide ���¸� ��������� ��ȯ & ��ȭ���Ͽ� ����
struct CCTD_ExtendInfo
{
	char			nTeam;
	unsigned char	nPlayerFlags;	// �÷��̾� �Ӽ�(��ڼ����) - CCTD_PlayerFlags ���
	unsigned char	nReserved1;		// ����
	unsigned char	nReserved2;
};

struct CCTD_PeerListNode
{
	CCUID				uidChar;
	DWORD				dwIP;
	unsigned int		nPort;
	CCTD_CharInfo		CharInfo;
	//���������ӽ��ּ� CCTD_CharBuffInfo	CharBuffInfo;
	CCTD_ExtendInfo		ExtendInfo;
};


// ���� �亯��
struct CCTD_ReplierNode
{
	char szName[MATCHOBJECT_NAME_LENGTH];
};


// ���� ���� ��û �� �׷�
struct CCTD_LadderTeamMemberNode
{
	char szName[MATCHOBJECT_NAME_LENGTH];

};

// Ŭ�� ����
struct CCTD_ClanInfo
{
	char				szClanName[CLAN_NAME_LENGTH];		// Ŭ�� �̸�
	short				nLevel;								// ����
	int					nPoint;								// ����Ʈ
	int					nTotalPoint;						// ��Ż����Ʈ
	int					nRanking;							// ��ŷ
	char				szMaster[MATCHOBJECT_NAME_LENGTH];	// Ŭ�� ������
	unsigned short		nWins;								// ���� - �¼�
	unsigned short		nLosses;							// ���� - �м�
	unsigned short		nTotalMemberCount;					// ��ü Ŭ������
	unsigned short		nConnedMember;						// ���� ���ӵ� Ŭ������
	unsigned int		nCLID;								// ClanID
	unsigned int		nEmblemChecksum;					// Emblem Checksum
};

// Ŭ���� ������� Ŭ�� ����Ʈ
struct CCTD_StandbyClanList
{
	char				szClanName[CLAN_NAME_LENGTH];		// Ŭ�� �̸�
	short				nPlayers;							// ������� �ο���
	short				nLevel;								// ����
	int					nRanking;							// ��ŷ - 0�̸� unranked
	unsigned int		nCLID;								// ClanID
	unsigned int		nEmblemChecksum;					// Emblem Checksum
};


// ����Ʈ, �����̹��� ���� ����
struct CCTD_QuestGameInfo
{
	unsigned short		nQL;												// ����Ʈ ����
	float				fNPC_TC;											// NPC ���̵� ���� ���
	unsigned short		nNPCCount;											// ���ʹ� ������ NPC����

	unsigned char		nNPCInfoCount;										// ������ NPC ���� ����
	unsigned char		nNPCInfo[MAX_QUEST_NPC_INFO_COUNT];					// ������ NPC ����
	unsigned short		nMapSectorCount;									// �� ��� ����
	unsigned short		nMapSectorID[MAX_QUEST_MAP_SECTOR_COUNT];			// �� ��� ID
	char				nMapSectorLinkIndex[MAX_QUEST_MAP_SECTOR_COUNT];	// �� ����� Link Index
	unsigned char		nRepeat;											// �ݺ� Ƚ�� (�����̹���)
	CCMATCH_GAMETYPE		eGameType;											// ����Ÿ��(����Ʈ��, �����̹��̳�)
};

// ����Ʈ, ���� ����
struct CCTD_QuestReward
{
	CCUID				uidPlayer;	// �ش� �÷��̾� UID
	int					nXP;		// �ش� �÷��̾ ���� XP
	int					nBP;		// �ش� �÷��̾ ���� BP
};

// ����Ʈ ������ ���� ����
struct CCTD_QuestItemNode
{
	int		m_nItemID;
	int		m_nCount;
};

// ����Ʈ �Ϲ� ������ ���� ����
struct CCTD_QuestZItemNode
{
	unsigned int		m_nItemID;
	int					m_nRentPeriodHour;
	int					m_nItemCnt;
};


// ����Ʈ�� ����� NPC�� ����.
struct CCTD_NPCINFO
{
	BYTE	m_nNPCTID;
	WORD	m_nMaxHP;
	WORD	m_nMaxAP;
	BYTE	m_nInt;
	BYTE	m_nAgility;
	float	m_fAngle;
	float	m_fDyingTime;

	float	m_fCollisonRadius;
	float	m_fCollisonHight;

	BYTE	m_nAttackType;
	float	m_fAttackRange;
	DWORD	m_nWeaponItemID;
	float	m_fDefaultSpeed;
};

// �����̹� ��ŷ ����
struct CCTD_SurvivalRanking
{
	char	m_szCharName[MATCHOBJECT_NAME_LENGTH];
	DWORD	m_dwPoint;
	DWORD	m_dwRank;		// ���������� ���� �� �����Ƿ� ���� ��ũ �������� �̰����� ���

	CCTD_SurvivalRanking() : m_dwPoint(0), m_dwRank(0) { m_szCharName[0] = 0; }
};

#if defined(LOCALE_NHNUSA)
struct CCTD_ServerStatusInfo
{
	DWORD			m_dwIP;
	DWORD			m_dwAgentIP;
	int				m_nPort;
	unsigned char	m_nServerID;
	short			m_nMaxPlayer;
	short			m_nCurPlayer;
	char			m_nType;
	bool			m_bIsLive;
	char			m_szServerName[ 64 ];
};
#else
struct CCTD_ServerStatusInfo
{
	DWORD			m_dwIP;
	int				m_nPort;
	unsigned char	m_nServerID;
	short			m_nMaxPlayer;
	short			m_nCurPlayer;
	char			m_nType;
	bool			m_bIsLive;
	char			m_szServerName[ 64 ];
};
#endif

struct CCTD_ResetTeamMembersData
{
	CCUID			m_uidPlayer;		// �ش� �÷��̾�
	char			nTeam;				// ��
};


// ��� ť ����

struct CCTD_DuelQueueInfo
{
	CCUID			m_uidChampion;
	CCUID			m_uidChallenger;
	CCUID			m_WaitQueue[14];				// ��
	char			m_nQueueLength;
	char			m_nVictory;						// ���¼�
	bool			m_bIsRoundEnd;					// ���� �������ΰ�
};

struct CCTD_DuelTournamentGameInfo
{
	CCUID			uidPlayer1;					// ������ ������ ������ Player1
	CCUID			uidPlayer2;					// ������ ������ ������ Player1
	int				nMatchType;					// ������ ������ MatchType(
	int				nMatchNumber;				// ������ ������ MatchNumber
	int				nRoundCount;				// ������ ������ �����
	bool			bIsRoundEnd;				// ���尡 ����Ǿ��°��� ���� Flag(�÷��̾� ��Ż�� ������..)
	char			nWaitPlayerListLength;		// ����� ����Ʈ�� Length
	byte			dummy[2];					// 4����Ʈ�� ���߱� ���� ����
	CCUID			WaitPlayerList[8];			// ����ڵ��� CCUID
};

struct CCTD_DuelTournamentNextMatchPlayerInfo
{
	CCUID			uidPlayer1;					// ������ ������ ������ Player1
	CCUID			uidPlayer2;					// ������ ������ ������ Player1
};


struct CCTD_DuelTournamentRoundResultInfo
{
	CCUID			uidWinnerPlayer;			// ���� ������ �¸���
	CCUID			uidLoserPlayer;				// ���� ������ �й���(180cm ����...)
	bool			bIsTimeOut;					// ���� ���尡 Ÿ�� �ƿ��̾���?
	bool			bDraw;						// ���� ���尡 ����� ��� true
	bool			bIsMatchFinish;				// ���� ���尡 ����Ǹ鼭, Match�� ����Ǿ��� ��� true
	byte			dummy[2];					// ���� ����

	///////////////////////////////////////////////////////////
	// Notice 
	// bDraw�� ����� ��, True�̴�.
	// bIsMatchFinish�� Match�� ����Ǿ��� ��, True�̴�.
	// ������ ���� ����� ���� ������ �� �ִ�.
	// bDraw = true,  bIsMatchFinish = true  => �÷��̾� �θ��� ��� ��Ż���� ���
	// bDraw = true,  bIsMatchFinish = false => ���� �ڻ�� ���Ͽ� ����� ���
	// bDraw = false, bIsMatchFinish = true  => �׳� �Ѹ��� �̰��, ��ġ�� ������ ���
	// bDraw = false, bIsMatchFinish = false => �׳� �Ѹ��� �̰�µ�, ��ġ�� ��� ����Ǿߵ� ���(4��, ���)
	///////////////////////////////////////////////////////////
};

struct CCTD_DuelTournamentMatchResultInfo
{
	int				nMatchNumber;
	int				nMatchType;
	CCUID			uidWinnerPlayer;			// ���� ������ �¸���
	CCUID			uidLoserPlayer;				// ���� ������ �й���(180cm ����...)
	int				nGainTP;
	int				nLoseTP;
};

#pragma pack(pop, old)


// admin ����
enum ZAdminAnnounceType
{
	ZAAT_CHAT = 0,
	ZAAT_MSGBOX
};

// �׺� ������ ������
struct CCTD_GambleItemNode
{
	CCUID			uidItem;
	unsigned int	nItemID;							// ������ ID
	unsigned int	nItemCnt;
};


// �׺� ������ ������
struct CCTD_DBGambleItmeNode
{
	unsigned int	nItemID;							// ������ ID
	char			szName[ MAX_GAMBLEITEMNAME_LEN ];	// ������ �̸�
	char			szDesc[ MAX_GAMBLEITEMDESC_LEN ];	// ������ ����.
	int				nBuyPrice;							// ������ ���԰���
	bool			bIsCash;							// Cash������ ����.
};

// Shop ������ ������ - Added by 2010-03-18 ȫ����
struct CCTD_ShopItemInfo
{
	unsigned int	nItemID;
	int				nItemCount;
};
/////////////////////////////////////////////////////////
void Make_MTDItemNode(CCTD_ItemNode* pout, CCUID& uidItem, unsigned long int nItemID, int nRentMinutePeriodRemainder, int iMaxUseHour, int nCount);
void Make_MTDAccountItemNode(CCTD_AccountItemNode* pout, int nAIID, unsigned long int nItemID, int nRentMinutePeriodRemainder, int nCount);

void Make_MTDQuestItemNode( CCTD_QuestItemNode* pOut, const unsigned long int nItemID, const int nCount );

struct CCMatchWorldItem;
void Make_MTDWorldItem(CCTD_WorldItem* pOut, CCMatchWorldItem* pWorldItem);

class CCMatchActiveTrap;
void Make_MTDActivatedTrap(CCTD_ActivatedTrap *pOut, CCMatchActiveTrap* pTrapItem);

// ����ġ, ����ġ ������ 4byte�� ����
// ���� 2����Ʈ�� ����ġ, ���� 2����Ʈ�� ����ġ�� �ۼ�Ʈ�̴�.
inline unsigned long int MakeExpTransData(int nAddedXP, int nPercent)
{
	unsigned long int ret = 0;
	ret |= (nAddedXP & 0x0000FFFF) << 16;
	ret |= nPercent & 0xFFFF;
	return ret;
}
inline int GetExpFromTransData(unsigned long int nValue)
{
	return (int)((nValue & 0xFFFF0000) >> 16);

}
inline int GetExpPercentFromTransData(unsigned long int nValue)
{
	return (int)(nValue & 0x0000FFFF);
}

#endif
