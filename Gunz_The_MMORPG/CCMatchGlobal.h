#ifndef _MMATCHGLOBAL_H
#define _MMATCHGLOBAL_H

#include "CCBaseLocale.h"

#define MAX_CHAR_COUNT							4		// ���� �� �ִ� ĳ���� ��

#define MAX_TRAP_THROWING_LIFE					10.f		// ������ Ʈ���� �� �ð� ���� �ߵ����� ���ϸ� �ҹ�ź���� ó��

#define CYCLE_STAGE_UPDATECHECKSUM				500		// �渮��Ʈ ���� ������� �����ִ� ������ - 0.5��

// ���� Rule ����
#define NUM_APPLYED_TEAMBONUS_TEAM_PLAYERS		3		// �������� ���ο��� 3�� �̻��϶��� ����ġ ���ʽ��� �����Ѵ�
#define RESPAWN_DELAYTIME_AFTER_DYING			7000	///< �װ��� ������ �Ǵ� �⺻ �����̽ð�
#define RESPAWN_DELAYTIME_AFTER_DYING_MIN		2000	///< �װ��� ������ �Ǵ� �ּ� �����̽ð�
#define RESPAWN_DELAYTIME_AFTER_DYING_MAX		20000	///< �װ��� ������ �Ǵ� �ִ� �����̽ð�

#define MAX_XP_BONUS_RATIO						2.0f	///< ����ġ ���ʽ��� �ִ� 2���̴�.
#define MAX_BP_BONUS_RATIO						2.0f	///< �ٿ�Ƽ ���ʽ��� �ִ� 2���̴�.

#define TRANS_STAGELIST_NODE_COUNT				8	// �ѹ��� Ŭ���̾�Ʈ���� �����ִ� ����������� ����
#define TRANS_STANDBY_CLANLIST_NODE_COUNT		4	// Ŭ�������� �ѹ��� Ŭ���̾�Ʈ���� �����ִ� ����� Ŭ�� ����


#define MAX_REPLIER	16			// proposal �亯�ڴ� �ִ� 16��

#define CLAN_SPONSORS_COUNT						4		// Ŭ�������� �ʿ��� �߱��� ��
#define CLAN_CREATING_NEED_BOUNTY				1000	// Ŭ�������� �ʿ��� �ٿ�Ƽ
#define CLAN_CREATING_NEED_LEVEL				10		// Ŭ�������� �ʿ��� ����



#define MAX_CHAR_LEVEL							99

#define MATCH_SIMPLE_DESC_LENGTH				64


// �׼Ǹ��� ����
#define ACTIONLEAGUE_TEAM_MEMBER_COUNT			4		// �׼Ǹ��״� 4���� ��� �Բ� �����ؾߵȴ�.
#define MAX_LADDER_TEAM_MEMBER					4		// �������� 1~4����� ���� �� �ִ�.
#define MAX_CLANBATTLE_TEAM_MEMBER				8		// Ŭ������ �ִ� 8����� ���� �� �ִ�.

//#define LIMIT_ACTIONLEAGUE		// �ݸ��� �׼Ǹ��� ���� ������
//#define LEAK_TEST
#define CLAN_BATTLE					// Ŭ���� ���߿� ������ - ������ ������ ����� ����





enum CCMatchTeam
{
	CCMT_ALL			= 0,
	CCMT_SPECTATOR	= 1,
	CCMT_RED			= 2,
	CCMT_BLUE		= 3,
	CCMT_END
};


// �������
enum CCMatchServerMode
{
	MSM_NORMAL		= 0,		// �Ϲ�
	MSM_CLAN		= 1,		// Ŭ���� ���� ����
	MSM_LADDER		= 2,		// ���� ���� ����
	MSM_EVENT		= 3,		// �̺�Ʈ ����
	MSM_TEST		= 4,		// �׽�Ʈ ����
	MSM_MAX,

	MSM_ALL			= 100,		// event���� ���ȴ�.
};

// ���� ����
enum CCMatchProposalMode
{
	MPROPOSAL_NONE = 0,				// ������� ����
	MPROPOSAL_LADDER_INVITE,		// �������� ��û
	MPROPOSAL_CLAN_INVITE,			// Ŭ���� ��û
	MPROPOSAL_END
};


// ���� Ÿ��
enum CCLADDERTYPE {
	CCLADDERTYPE_NORMAL_2VS2		= 0,
	CCLADDERTYPE_NORMAL_3VS3,
	CCLADDERTYPE_NORMAL_4VS4,
//	CCLADDERTYPE_NORMAL_8VS8,
#ifdef _DEBUG
	CCLADDERTYPE_NORMAL_1VS1,
#endif
	CCLADDERTYPE_MAX
};

// �� ����Ÿ�Ժ� �ʿ��� �ο���
const int g_nNeedLadderMemberCount[CCLADDERTYPE_MAX] = {	2, 3, 4/*, 8*/
#ifdef _DEBUG
, 1 
#endif
};


/// Clan����.
#define DEFAULT_CLAN_POINT			1000			// �⺻ Ŭ�� ����Ʈ
#define DAY_OF_DELETE_CLAN			(7)				// Ŭ�� ����û�� DAY_OF_DELETE_CLAN�ϸ�ŭ ������ �м��۾��� �����.
#define MAX_WAIT_CLAN_DELETE_HOUR	(24)			// DAY_OF_DELETE_CLAN + MAX_WAIT_CLAN_DELETE_HOUR�� ��񿡼� Ŭ������.
#define UNDEFINE_DELETE_HOUR		(2000000000)	// �������� Ŭ���� DeleteTime�� null��ó����.

enum CCMatchClanDeleteState
{
	CCMCDS_NORMAL = 1,
	CCMCDS_WAIT,
	CCMCDS_DELETE,

	CCMCDS_END,
};


// �ɼ� ����
enum CCBITFLAG_USEROPTION {
	CCBITFLAG_USEROPTION_REJECT_WHISPER	= 1,
	CCBITFLAG_USEROPTION_REJECT_INVITE	= 1<<1
};

// ����Ʈ ���� ///////////////////////////////////////////////////////////////////////////////

#define MAX_QUEST_MAP_SECTOR_COUNT				16			// ����Ʈ���� �ִ� ������� �� �ִ� �� ����

#ifdef _DEBUG_QUEST
#define MAX_QUEST_NPC_INFO_COUNT				100			// ����Ʈ���� �ִ� ���� NPC ���� ����
#else
#define MAX_QUEST_NPC_INFO_COUNT				14			// ����Ʈ���� �ִ� ���� NPC ���� ����
#endif



#define ALL_PLAYER_NOT_READY					1	// ��� ������ ���� ���� ���ؼ� ������ �������� ����.
#define QUEST_START_FAILED_BY_SACRIFICE_SLOT	2	// ��� ������ ���� �˻�� ������ �־ ������ ������.
#define INVALID_TACKET_USER						3	// ������� ������ �ִ� ���� ������ ����.
#define INVALID_MAP								4	// ���� ����Ÿ�Կ��� ����� �� ���� ����.

#define MIN_QUESTITEM_ID							200001	// item id�� 200001���ʹ� ����Ʈ �������̴�
#define MAX_QUESTITEM_ID							299999

// Keeper Manager���� Schedule����. ////////////////////////////////////////////////////////////

enum KMS_SCHEDULE_TYPE
{
	KMST_NO = 0,
	KMST_REPEAT,
	KMST_COUNT,
	KMST_ONCE,

	KMS_SCHEDULE_TYPE_END,
};

enum KMS_COMMAND_TYPE
{
	KMSC_NO = 0,
	KMSC_ANNOUNCE,
	KMSC_STOP_SERVER,
	KMSC_RESTART_SERVER,
	
	KMS_COMMAND_TYPE_END,
};

enum SERVER_STATE_KIND
{
	SSK_OPENDB = 0,

	SSK_END,
};

enum SERVER_ERR_STATE
{
	SES_NO = 0,
	SES_ERR_DB,
    
	SES_END,
};

enum SERVER_TYPE
{
	ST_NULL = 0,
	ST_DEBUG,
	ST_NORMAL,
	ST_CLAN,
	ST_QUEST,
	ST_EVENT,
};


enum CCMatchBlockLevel
{
	CCMBL_NO = 0,
	CCMBL_ACCOUNT,
	CCMBL_LOGONLY,

	CCMBL_END,
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Util �Լ�

/// ����� ��ȯ
inline CCMatchTeam NegativeTeam(CCMatchTeam nTeam)
{
	if (nTeam == CCMT_RED) return CCMT_BLUE;
	else if (nTeam == CCMT_BLUE) return CCMT_RED;
	return nTeam;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// string buffer length define.

#define ANNOUNCE_STRING_LEN					64
#define MSG_STRING_LEN						64
#define CHAT_STRING_LEN						64
#define VOTE_DISCUSS_STRING_LEN				64
#define VOTE_ARG_STRING_LEN					64
// stage
#define STAGENAME_LENGTH					64	// ���̸� ����
#define STAGEPASSWD_LENGTH					8	// �н����� �ִ����
#define STAGE_QUEST_MAX_PLAYER				4	// ����Ʈ ���Ӹ���� �ִ��ο�
#define STAGE_MAX_PLAYERCOUNT				16  // �⺻���(����Ʈ, Ŭ���� ����) �ִ� �ο�. - by SungE 2007-05-14.
#define STAGE__MAX_ROUND					100	// ������������ ����� �� �ִ� �ִ� ���� ��. �̼��� ���� �� ����. - By SungE 2007-11-07
// clan
#define CLAN_NAME_LENGTH					16	// Ŭ�� �̸� �ִ� ���� - �̰� ����Ǹ� �������� ������ ����Ǿ�� ��.
											// �� �κ��� ���� ����ڵ鶧���� ��� ������ ����. - by SungE. 2007-03-20
#define MIN_CLANNAME						4	// �ּ� 4���̻� 12�����ϸ� Ŭ�� �̸��� ���� �� �ִ�.
#define MAX_CLANNAME						12
// character
#define MIN_CHARNAME						4	// �ּ� 4���̻� 12�� ���ϸ� ĳ���� �̸��� ���� �� �ִ�.
#define MAX_CHARNAME						12
#define MATCHOBJECT_NAME_LENGTH				32	// ĳ���� �̸� ���� - ���� ���÷��̶����� DB�� ����� ����. ���� �߰��ϴ� ������ ����. - by SungE. 2007-03-20
#define MAX_CHARNAME_LENGTH					24	// DB�� ������ ����. Ŀ�ǵ�� �� ���̸� ����Ѵ�.
// chat room
#define MAX_CHATROOMNAME_STRING_LEN			64	
// #define MAX_USERID_STRING_LEN			20
#define MAX_USERID_STRING_LEN				21	///< ��Ʈ�� ���� 20����Ʈ + ���� ���� 1����Ʈ
#define MAX_USER_PASSWORD_STRING_LEN		20
#define USERNAME_STRING_LEN					50
// channel
#define CHANNELNAME_LEN						64
#define CHANNELRULE_LEN						64
// map
#define MAPNAME_LENGTH						32
// Auth
#define NHN_GAMEID							"cc_gunz"
#define NHN_AUTH_LENGTH						4096
#define NHN_OUTBUFF_LENGTH					1024
// item
#define MAX_ACCOUNT_ITEM					1000		// �ְ� 1000���� �����Ѵ�.
#define MAX_EXPIRED_ACCOUNT_ITEM			100
#define MAX_ITEM_COUNT						100			// �ѻ���� �ִ�� ���� �� �ִ� ������ ����(�������� ������ ���Խ�)
#define MAX_QUEST_REWARD_ITEM_COUNT			500			// �ѻ���� �ִ�� ���� �� �ִ� ������ ����(����Ʈ�� ������ ȹ���)
#define MAX_SPENDABLE_ITEM_COUNT			999			// �Ҹ� �������� �ִ�� ���� �� �ִ� ����
// gamble item 
#define MAX_GAMBLEITEMNAME_LEN				(65)		// 
#define MAX_GAMBLEITEMDESC_LEN				(65)		// ������ �߰� ����.
#define MAX_BUYGAMBLEITEM_ELAPSEDTIME_MIN	(5)			// �̺�Ʈ �Ⱓ�� ������� �������� ��� �ð��� ����ؼ� �� �ð��� �ʰ��ϸ� �ȵ�. 
														// - by SungE 2007-06-22
// time define.
#define MIN_REQUEST_STAGESTART_TIME			(1 * 1000)			// ������ �������� ������ 1�ʰ� ������ �ٽ� ������ �� �� �ִ�.
#define MIN_REQUEST_SUICIDE_TIME			(1000 * 60 * 3)		// �ڽ��� �׾��ٴ� ��û�� 3�п� �ѹ��� ��û�� �� �ִ�.

//#define UPDATE_KILLTRACKER_ELAPSED_TIME	(1000 * 60 * 5)
//#define TRACKERINFO_LIFETIME				(1000 * 60 * 20)	
//#define KILLCOUNT_TRACE_TIME				(1000 * 60 * 10)
//#define MAX_KILLCOUNT_ON_TRACETIME		(40)

// md5 
#define MAX_MD5LENGH						(16)
/////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu
// �����̹� ��ŷ ����
#define MAX_SURVIVAL_SCENARIO_COUNT 3
#define MAX_SURVIVAL_RANKING_LIST 10

typedef struct _RankingInfo
{
	char szCharName[MATCHOBJECT_NAME_LENGTH];
	DWORD dwCID;
	DWORD dwRanking;
	DWORD dwRankingPoint;
} RANKINGINFO;

//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 9. 25 - Added By Hong KiJu
// Duel Tournament ����
#define DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH 8

enum CCDUELTOURNAMENTTYPE
{
	CCDUELTOURNAMENTTYPE_FINAL				= 0,		//< ���������
	CCDUELTOURNAMENTTYPE_SEMIFINAL,						//< 4��������
	CCDUELTOURNAMENTTYPE_QUATERFINAL,					//< 8��������
	CCDUELTOURNAMENTTYPE_MAX	
};

enum CCDUELTOURNAMENTROUNDSTATE {
	CCDUELTOURNAMENTROUNDSTATE_FINAL = 0,	
	CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL,
	CCDUELTOURNAMENTROUNDSTATE_QUATERFINAL,
	CCDUELTOURNAMENTROUNDSTATE_MAX
};

enum CCDUELTOURNAMENTMATCHMAKINGFACTOR			// ��ġ�� ������� �ٰ�
{
	CCDUELTOURNAMENTMATCHMAKINGFACTOR_TPGAP = 0,			//< TP gap ����
	CCDUELTOURNAMENTMATCHMAKINGFACTOR_OVERWAIT,			//< ���ð� �ʰ�
};

// ����ī��Ʈ�ٿ� ���¿��� �ð��� ��� �ɰ��� ����� ������ (��� ��ģ ���� ��ü �ð�)
#define DUELTOURNAMENT_PRECOUNTDOWN_WINLOSE_SHOWTIME	4000
#define DUELTOURNAMENT_PRECOUNTDOWN_NEXTMATCH_SHOWTIME	6000

inline int GetDTPlayerCount(CCDUELTOURNAMENTTYPE nType)
{
	switch(nType) {
		case CCDUELTOURNAMENTTYPE_QUATERFINAL :		return 8;
		case CCDUELTOURNAMENTTYPE_SEMIFINAL :		return 4;
		case CCDUELTOURNAMENTTYPE_FINAL :			return 2;			
	}

	return 0;
}

inline int GetDTRoundCount(CCDUELTOURNAMENTROUNDSTATE nRoundState)
{
	switch(nRoundState) {
		case CCDUELTOURNAMENTROUNDSTATE_QUATERFINAL :		return 4;
		case CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL :			return 2;
		case CCDUELTOURNAMENTROUNDSTATE_FINAL :				return 1;			
	}

	return 0;
}

typedef struct _DTRankingInfo
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	int m_nTP;
	int m_nWins;
	int m_nLoses;
	int m_nRanking;
	int m_nRankingIncrease;
	int m_nFinalWins;
	int m_nGrade;
} DTRankingInfo;

typedef struct _DTPlayerInfo
{
	char m_szCharName[MATCHOBJECT_NAME_LENGTH];
	MUID uidPlayer;
	int m_nTP;
} DTPlayerInfo;


//////////////////////////////////////////////////////////////////////////////////////////////
// 2010. 04. 12 - Added By ȫ����

// ������ ������ ����

#define ACCOUNTITEM_INCREASE_LOG_MOVE			100
#define ACCOUNTITEM_INCREASE_LOG_BUY			101
#define ACCOUNTITEM_INCREASE_LOG_REWARD_BY_GM	102

#define ACCOUNTITEM_DECREASE_LOG_MOVE			200
#define ACCOUNTITEM_DECREASE_LOG_SELL			201
#define ACCOUNTITEM_DECREASE_LOG_EXPIRE			202

#define CHARITEM_INCREASE_LOG_MOVE		100
#define CHARITEM_INCREASE_LOG_BUY		101
#define CHARITEM_INCREASE_LOG_REWARD	102

#define CHARITEM_DECREASE_LOG_MOVE		200
#define CHARITEM_DECREASE_LOG_SELL		201
#define CHARITEM_DECREASE_LOG_EXPIRE	202
#define CHARITEM_DECREASE_LOG_GAMBLE	203
#define CHARITEM_DECREASE_LOG_SPEND		204

//////////////////////////////////////////////////////////////////////////////////////////////
// 2010. 06. 07 - Added By ȫ����

#define	MAX_CHARACTER_SHORT_BUFF_COUNT		2

typedef struct _CCLongBuffInfoInDB
{	
	int nCBID;
	int nBuffID;
	int nBuffSecondPeriod;
	int nStartPlayTime;	
} CCLongBuffInfoInDB;

typedef struct _CCLongBuffInfo
{	
	MUID uidBuff;

	int nCBID;
	int nBuffID;
	int nBuffPeriod;
	int nBuffPeriodRemainder;	
} CCLongBuffInfo;

typedef struct _CCShortBuffInfo
{	
	MUID uidBuff;

	int nBuffID;
	int nBuffPeriod;
	int nBuffPeriodRemainder;
} CCShortBuffInfo;