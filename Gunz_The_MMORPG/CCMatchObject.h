#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <string>
using namespace std;
#include "CCMatchItem.h"
#include "CCUID.h"
#include "CCObject.h"
#include "CCMatchGlobal.h"
#include "CCMatchFriendInfo.h"
#include "CCMatchClan.h"
#include "CCMatchChannel.h"
#include "CCSmartRefreshImpl.h"
#include "CCQuestItem.h"
#include "CCMatchAntiHack.h"
#include "CCMatchHShield.h"
#include "CCMatchGambleItem.h"
#include "CCHackingTypes.h"
#include "CCMatchObjectCharBuffInfo.h"

#include "CCMatchAccountPenaltyInfo.h"

#include "CCMatchCharBRInfo.h"

// 등급 - 이것은 디비의 UserGrade테이블과 싱크가 맞아야 한다.
enum CCMatchUserGradeID
{
	CCMUG_FREE			= 0,	// 무료 계정
	CCMUG_REGULAR		= 1,	// 정액 유저
	CCMUG_STAR			= 2,	// 스타유저(게임짱)

	CCMUG_CRIMINAL		= 100,	// 전과자
	CCMUG_WARNING_1		= 101,	// 1차 경고
	CCMUG_WARNING_2		= 102,	// 2차 경고
	CCMUG_WARNING_3		= 103,	// 3차 경고
	CCMUG_CHAT_LIMITED	= 104,  // 채팅 금지
	CCMUG_PENALTY		= 105,	// 기간 정지

	CCMUG_EVENTMASTER	= 252,	// 이벤트 진행자
	CCMUG_BLOCKED		= 253,	// 사용 정지
	CCMUG_DEVELOPER		= 254,	// 개발자
	CCMUG_ADMIN			= 255	// 관리자
};



enum CCMatchDisconnectStatus
{
	CCMDS_CONNECTED = 1,
	CCMDS_DISCONN_WAIT,
	CCMDS_DISCONNECT,

	CCMDS_END,
};



// 유료이용자 권한 - 지금은 DB의 PGradeID를 읽지 않고, 단지 넷마블PC방 보너스 용도로만 사용한다.
enum CCMatchPremiumGradeID
{
	MMPG_FREE			= 0,	// 무료
	MMPG_PREMIUM_IP		= 1		// 넷마블 PC방 보너스
};

// 성별 - 이것은 디비의 성별정보와 싱크가 맞아야 한다.
enum CCMatchSex
{
	MMS_MALE = 0,		// 남자
	MMS_FEMALE = 1		// 여자
};


/// 계정 정보
struct CCMatchAccountInfo
{
	int						m_nAID;
	char					m_szUserID[ MAX_USERID_STRING_LEN ];	// 계정ID
	CCMatchUserGradeID		m_nUGrade;								// 등급
	CCMatchPremiumGradeID	m_nPGrade;								// 유료 이용자 권한

	CCMatchHackingType		m_HackingType;
	SYSTEMTIME				m_EndBlockDate;							// 해킹 유저블럭이 유지되는 시간.
	DWORD					m_dwHackingBlockEndTimeMS;

	bool					m_bIsPowerLevelingHacker;
	int						m_nPowerLevelingRegTimeMin;

	int						m_nCCode;

	CCMatchAccountInfo() : m_nAID(-1), m_nUGrade(CCMUG_FREE), m_nPGrade(MMPG_FREE), m_nCCode(0)
	{
		m_HackingType				= CCMHT_NO;
		m_dwHackingBlockEndTimeMS	= 0;
		m_bIsPowerLevelingHacker	= false;
		m_nPowerLevelingRegTimeMin	= 0;
		
		memset(m_szUserID, 0, MAX_USERID_STRING_LEN);
	}
};


struct CCAccountItemNode
{
	int					nAIID;
	unsigned long int	nItemID;
	int					nRentMinutePeriodRemainder;
	int					nCount;
};

// 플레이어가 현재 위치하고 있는 곳
enum CCMatchPlace
{
	MMP_OUTSIDE	= 0,
	MMP_LOBBY = 1,
	MMP_STAGE	= 2,
	MMP_BATTLE	= 3,
	MMP_END
};

enum CCMatchObjectStageState
{
	MOSS_NONREADY	= 0,
	MOSS_READY		= 1,
	MOSS_SHOP		= 2,
	MOSS_EQUIPMENT	= 3,
	MOSS_END
};

#define DEFAULT_CHAR_HP				100
#define DEFAULT_CHAR_AP				0

#define DBCACHING_REQUEST_POINT			40
#define DBCACHING_REQUEST_TIME_POINT	60 * 5	// 5분마다 업데이트~!

struct DBCharCachingData
{
	int	nAddedXP;
	int	nAddedBP;
	int	nAddedKillCount;
	int	nAddedDeathCount;
	int nAddedPlayTime;

	void Reset()
	{
		nAddedXP = 0;
		nAddedBP = 0;
		nAddedKillCount = 0;
		nAddedDeathCount = 0;
		nAddedPlayTime = 0;
	}
	bool IsRequestUpdate()
	{
		if ((nAddedKillCount > DBCACHING_REQUEST_POINT) || (nAddedDeathCount > DBCACHING_REQUEST_POINT)
			|| nAddedPlayTime > DBCACHING_REQUEST_TIME_POINT)
			return true;

		return false;
	}
};

struct MGamePlayInfo
{
	int nPlayTime;
	int	nKillCount;
	int nDeathCount;
	int	nXP;
	int	nBP;

	void Reset()
	{
		nPlayTime = 0;
		nKillCount = 0;
		nDeathCount = 0;
		nXP = 0;
		nBP = 0;
	}
};

// 캐릭터의 클랜정보
struct CCMatchCharClanInfo
{
	int					m_nClanID;							// db상의 클랜 ID
	char				m_szClanName[ CLAN_NAME_LENGTH ];	// 클랜 이름
	CCMatchClanGrade		m_nGrade;							// 클랜에서의 권한
	int					m_nContPoint;						// 클랜 기여도
	string				m_strDeleteDate;

	CCMatchCharClanInfo() {  Clear(); }
	void Clear()
	{
		m_nClanID = 0; 
		// m_szClanName[0] = 0; 
		m_nGrade=CCG_NONE;
		m_nContPoint = 0;
		m_strDeleteDate.clear();

		memset( m_szClanName, 0, CLAN_NAME_LENGTH );
	}
	bool IsJoined() { return (m_nClanID == 0) ? false : true; }
public:
	int GetClanID() { return m_nClanID; }
};


#define DEFAULT_CHARINFO_MAXWEIGHT		100
#define DEFAULT_CHARINFO_SAFEFALLS		0
#define DEFAULT_CHARINFO_BONUSRATE		0.0f
#define DEFAULT_CHARINFO_PRIZE			0




/// 캐릭터 정보
class CCMatchCharInfo
{
public:
	unsigned long int	m_nCID;
	int					m_nCharNum;
	char				m_szName[MATCHOBJECT_NAME_LENGTH];
	int					m_nLevel;
	CCMatchSex			m_nSex;			// 성별
	int					m_nHair;		// 머리
	int					m_nFace;		// 얼굴
	unsigned long int	m_nXP;
	int					m_nBP;
	float				m_fBonusRate;
	int					m_nPrize;
	int					m_nHP;
	int					m_nAP;
	int					m_nMaxWeight;
	int					m_nSafeFalls;
	int					m_nFR;
	int					m_nCR;
	int					m_nER;
	int					m_nWR;
	unsigned long int	m_nEquipedItemCIID[MMCIP_END];
	CCMatchItemMap		m_ItemList;			// 아이템 정보
	CCMatchEquipedItem	m_EquipedItem;		// 장비하고 있는 아이템 정보
	CCMatchCharClanInfo	m_ClanInfo;			// 클랜 정보

	// 퀘스트 아이템.
	CCQuestItemMap		m_QuestItemList;
	DBQuestCachingData	m_DBQuestCachingData;
	DBQuestCachingData& GetDBQuestCachingData() { return m_DBQuestCachingData; }


	// 몬스터 바이블
	CCQuestMonsterBible	m_QMonsterBible;

	// gamble item.
	CCMatchGambleItemManager m_GambleItemManager;

	unsigned long int	m_nTotalPlayTimeSec;		// 케릭터의 총 플레이 시간
	unsigned long int	m_nPlayTimeSec;				// 접속해서 플레이한 시간
	unsigned long int	m_nConnTime;				// 접속한 시간(1초 = 1000)
	unsigned long int	m_nBattleStartTime;			// 배틀 시작시간

	unsigned long int	m_nTotalKillCount;			// 전체 킬수
	unsigned long int	m_nTotalDeathCount;			// 전체 데쓰수
	unsigned long int	m_nConnKillCount;			// 접속이후로 누적된 킬수
	unsigned long int	m_nConnDeathCount;			// 접속이후로 누적된 데쓰수
	unsigned long int   m_nConnXP;					// 접속이후로 누적된 경험치

	unsigned long int   m_nBattleStartXP;			// 배틀 시작 경험치

	bool				m_IsSendMyItemListByRequestClient;

	unsigned int		m_nRank;

protected:
	DBCharCachingData	m_DBCachingData;
public:
	CCMatchCharInfo() : m_nCID(0), m_nCharNum(0), m_nLevel(0), m_nSex(MMS_MALE), m_nFace(0),
		               m_nHair(0), m_nXP(0), m_nBP(0), m_fBonusRate(DEFAULT_CHARINFO_BONUSRATE), m_nPrize(DEFAULT_CHARINFO_PRIZE), m_nHP(0),
					   m_nAP(0), m_nMaxWeight(DEFAULT_CHARINFO_MAXWEIGHT), m_nSafeFalls(DEFAULT_CHARINFO_SAFEFALLS),
					   m_nFR(0), m_nCR(0), m_nER(0), m_nWR(0),
					   m_nConnTime(0), m_nTotalKillCount(0), m_nTotalDeathCount(0), m_nConnKillCount(0), m_nConnDeathCount(0), 
					   m_nConnXP(0), m_nRank(0), m_nBattleStartTime(0), m_nPlayTimeSec(0), m_nTotalPlayTimeSec(0), m_nBattleStartXP(0)
	{
		memset(m_szName, 0, MATCHOBJECT_NAME_LENGTH);
		memset(m_nEquipedItemCIID, 0, sizeof(m_nEquipedItemCIID));
		memset(&m_DBCachingData, 0, sizeof(m_DBCachingData));
		memset(&m_QMonsterBible, 0, sizeof(CCQuestMonsterBible) );

		m_QuestItemList.Clear();
		m_DBQuestCachingData.Reset();
		m_CharGamePlayInfo.Reset();


		m_IsSendMyItemListByRequestClient = false;

		m_EquipedItem.SetOwner( this );
	}
	bool EquipFromItemList();
	void ClearItems();
	void Clear();
	void GetTotalWeight(int* poutWeight, int* poutMaxWeight);


	// db caching까지 함께 더해준다.
	void IncKill()
	{ 
		m_nTotalKillCount += 1;
		m_nConnKillCount += 1;
		m_DBCachingData.nAddedKillCount += 1;
		m_CharGamePlayInfo.nKillCount +=1;
	}
	void IncDeath()
	{ 
		m_nTotalDeathCount += 1;
		m_nConnDeathCount += 1;
		m_DBCachingData.nAddedDeathCount += 1;
		m_CharGamePlayInfo.nDeathCount += 1;
	}
	void IncBP(int nAddedBP)		
	{ 
		m_nBP += nAddedBP;
		m_DBCachingData.nAddedBP += nAddedBP;
		m_CharGamePlayInfo.nBP += nAddedBP;
	}
	void DecBP(int nDecBP)
	{ 
		m_nBP -= nDecBP;
		m_DBCachingData.nAddedBP -= nDecBP;
		m_CharGamePlayInfo.nBP -= nDecBP;
	}
	void IncXP(int nAddedXP)
	{ 
		m_nConnXP += nAddedXP;
		m_nXP += nAddedXP;
		m_DBCachingData.nAddedXP += nAddedXP;
		m_CharGamePlayInfo.nXP += nAddedXP;
	}
	void DecXP(int nDecXP)
	{ 
		m_nConnXP -= nDecXP; 
		m_nXP -= nDecXP; 
		m_DBCachingData.nAddedXP -= nDecXP; 
		m_CharGamePlayInfo.nXP -= nDecXP;
	}

	void UpdatePlayTime(int nPlayTime)
	{
		int nAddedPlayTime = nPlayTime - m_nPlayTimeSec;

		m_nPlayTimeSec = nPlayTime;
		m_nTotalPlayTimeSec += nAddedPlayTime;
		m_DBCachingData.nAddedPlayTime += nAddedPlayTime;
	}

	DBCharCachingData* GetDBCachingData() { return &m_DBCachingData; }

protected:
	CCMatchCharBattleTimeRewardInfoMap m_CharBRInfoMap;

	MGamePlayInfo m_CharGamePlayInfo;
public:
	CCMatchCharBattleTimeRewardInfoMap& GetBRInfoMap() { return m_CharBRInfoMap; }
	MGamePlayInfo* GetCharGamePlayInfo() { return &m_CharGamePlayInfo; }

};

class CCMatchTimeSyncInfo {
protected:
	int				m_nFoulCount;
	unsigned long	m_nLastSyncClock;
public:
	CCMatchTimeSyncInfo()				{ m_nFoulCount=0; m_nLastSyncClock=0; }
	virtual ~CCMatchTimeSyncInfo()		{}

	int GetFoulCount()					{ return m_nFoulCount; }
	void AddFoulCount()					{ m_nFoulCount++; }
	void ResetFoulCount()				{ m_nFoulCount = 0; }
	unsigned long GetLastSyncClock()	{ return m_nLastSyncClock; }
	void Update(unsigned long nClock)	{ m_nLastSyncClock = nClock; }
};

// MatchObject가 게임안에서 사용하는 변수들
struct CCMatchObjectGameInfo
{
	bool		bJoinedGame;		// 게임에 참가중인지 여부 - 팀전에서 난입했을때
};


class CCMatchDisconnStatusInfo
{
public :
	CCMatchDisconnStatusInfo() 
	{
		m_DisconnStatus						= CCMDS_CONNECTED;
		m_dwLastDisconnStatusUpdatedTime	= timeGetTime();
		m_bIsSendDisconnMsg					= false;
		m_dwDisconnSetTime					= 0;
		m_bIsUpdateDB						= false;
	}

	~CCMatchDisconnStatusInfo() {}

	const CCMatchDisconnectStatus	GetStatus()				{ return m_DisconnStatus; }
	const DWORD						GetLastUpdatedTime()	{ return m_dwLastDisconnStatusUpdatedTime; }
	const DWORD						GetMsgID()				{ return m_dwMsgID; }
	const CCMatchHackingType			GetHackingType()		{ return m_HackingType; } 
	const CCMatchBlockLevel			GetBlockLevel()			{ return m_BlockLevel; }
	const string&					GetEndDate()			{ return m_strEndDate; }
	const string&					GetComment()			{ return m_strComment; }
		
	const bool	IsSendDisconnMsg()	{ return m_bIsSendDisconnMsg; }
	void		SendCompleted()		{ m_bIsSendDisconnMsg = false; }	// CCMatchServer에서 커맨드 처리를 위해서 사용...
																		// IsSendDisconnMsg로 접속 종료 메시지를 보내야 하는지 검사후,
																		// 커맨드를 보내후에는 SendCompleted()를 호출하여 다음에 중복으로 보내는 것을 막는다.
																		// 더 좋은 방법을 생각해 봐야 함. -by SungE. 2006-03-07.

	const bool IsUpdateDB()			{ return m_bIsUpdateDB; }
	void UpdateDataBaseCompleted()	{ m_bIsUpdateDB = false; } // Update되면은 다음 BlockType이 설정전까진 false로 설정.

	const bool IsDisconnectable( const DWORD dwTime = timeGetTime() )
	{
		if( (CCMDS_DISCONNECT == GetStatus()) && (CCINTERVAL_DISCONNECT_STATUS_MIN < (dwTime - m_dwDisconnSetTime)) )
			return true;
		return false;
	}
	
	void SetStatus( const CCMatchDisconnectStatus Status )	
	{
		m_DisconnStatus = Status; 
		if( CCMDS_DISCONN_WAIT == Status )
			SendDisconnMsgPrepared();

		if( CCMDS_DISCONNECT == Status )
			m_dwDisconnSetTime = (timeGetTime() - 2000);
		
	}
	void SetUpdateTime( const DWORD dwTime )					{ m_dwLastDisconnStatusUpdatedTime = dwTime; }
	void SetMsgID( const DWORD dwMsgID )						{ m_dwMsgID = dwMsgID; }
	void SetHackingType( const CCMatchHackingType HackingType )	{ m_HackingType = HackingType; m_bIsUpdateDB = true; } // 새로운 BlockType이 설정되면 
																													   //  DB업데이트 준비도 같이함.
	void SetBlockLevel( const CCMatchBlockLevel BlockLevel )		{ m_BlockLevel = BlockLevel; }
	void SetEndDate( const string& strEndDate )					{ m_strEndDate = strEndDate; }
	void SetComment( const string& strComment )					{ m_strComment = strComment; }
	
	void Update( const DWORD dwTime )
	{
		if( (dwTime - GetLastUpdatedTime()) > CCINTERVAL_DISCONNECT_STATUS_MIN ) 
		{
			if( CCMDS_DISCONN_WAIT == GetStatus() ) {
				SetStatus( CCMDS_DISCONNECT );		
			}
			
			SetUpdateTime( dwTime );
		}
	}

private :
	void SendDisconnMsgPrepared()	{ m_bIsSendDisconnMsg = true; }

private :
	CCMatchDisconnectStatus	m_DisconnStatus;
	DWORD					m_dwLastDisconnStatusUpdatedTime;
	DWORD					m_dwDisconnSetTime;
	DWORD					m_dwMsgID;
	CCMatchHackingType		m_HackingType;
	CCMatchBlockLevel		m_BlockLevel;	// Level에따라 계정 블럭, 로그작업만 하는등으로 나눠짐.
	string					m_strEndDate;
	string					m_strComment;
	bool					m_bIsSendDisconnMsg;
	bool					m_bIsUpdateDB;

	const static DWORD CCINTERVAL_DISCONNECT_STATUS_MIN;
};


struct CCMatchObjectChannelInfo
{
	CCUID			uidChannel;
	CCUID			uidRecentChannel;
	bool			bChannelListTransfer;
	CCCHANNEL_TYPE	nChannelListType;
	unsigned long	nChannelListChecksum;
	int				nTimeLastChannelListTrans;
	void Clear()
	{
		uidChannel = CCUID(0,0);
		uidRecentChannel = CCUID(0,0);
		bChannelListTransfer = false;
		nChannelListType = CCCHANNEL_TYPE_PRESET;
		nChannelListChecksum = 0;
		nTimeLastChannelListTrans = 0;
	}
};

class CCAsyncJob;
class MBMatchGameguard;


struct CCASYNCJOBQ
{
	list<CCAsyncJob*>	DBJobQ;
	bool				bIsRunningAsyncJob;
	int					nLastJobID;
};


#define MAX_CHAT_FLOOD_COUNT	5
#define MAX_BAN_CHAT_TICK		30 * 1000

class CCMatchChatFloodInfo 
{
protected:
	DWORD			m_dwBanTick;
	unsigned short	m_dwBanCount;

	list<int>	m_ChatFloodList;
public:
	CCMatchChatFloodInfo() : m_dwBanTick(0), m_dwBanCount(0)
	{}

	~CCMatchChatFloodInfo()
	{
		m_dwBanTick = 0;
		m_dwBanCount = 0;
		
		m_ChatFloodList.clear();
	}

	bool PushChatTickCount()
	{
		DWORD dwFirstTick = 0;
		DWORD dwCurTickCount = GetTickCount();

		if( m_ChatFloodList.size() >= MAX_CHAT_FLOOD_COUNT )
		{
			dwFirstTick = m_ChatFloodList.front();
			m_ChatFloodList.pop_front();			
		}

		m_ChatFloodList.push_back(dwCurTickCount);

		// 맨 처음의 Tick과 삽입하려는 Tick과의 차이가 1초 이하면.. Flooding이다!
		return (dwCurTickCount - dwFirstTick < 1000)? true : false;
	}

	bool IsBanUser()
	{
		DWORD dwCurTickCount = GetTickCount();

		if( dwCurTickCount - m_dwBanTick < MAX_BAN_CHAT_TICK) return true;

		m_dwBanTick = 0;
		return false;
	}

	void SetBanUser()				{ m_dwBanTick = GetTickCount(); m_dwBanCount++;}
};

class CCMatchObjectDuelTournamentCharInfo
{
protected:
	bool m_bIsChallenge;
	bool m_bIsSettingData;

	int m_nTP;
	int m_nWins;
	int m_nLoses;
	int m_nRanking;
	int m_nRankingIncrease;
	int m_nQuaterFinalCount;
	int m_nQuaterFinalWins; 
	int m_nSemiFinalCount;
	int m_nSemiFinalWins;
	int m_nFinalCount;
	int m_nFinalWins;
	int	m_nLeaveCount;
	int	m_nLastWeekGrade;					// 듀얼토너먼트 지난주 등급 ([1~10] 1이 우수한 등급, 0은 무효값)

	char m_szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

	list<DTRankingInfo*> m_SideRankingList;
public:
	CCMatchObjectDuelTournamentCharInfo()
	{
		m_nTP = 0;
		m_nWins = 0;
		m_nLoses = 0;
		m_nRanking = 0;
		m_nRankingIncrease = 0;
		m_nQuaterFinalCount = 0;
		m_nQuaterFinalWins = 0;
		m_nSemiFinalCount = 0;
		m_nSemiFinalWins = 0;
		m_nFinalCount = 0;
		m_nFinalWins = 0;

		m_nLeaveCount = 0;
		m_nLastWeekGrade = 0;

		m_bIsChallenge = false;
		m_bIsSettingData = false;

		memset(m_szTimeStamp, 0, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}
	CCMatchObjectDuelTournamentCharInfo(CCMatchObjectDuelTournamentCharInfo *pDTCharInfo)
	{
		SetCharInfo(pDTCharInfo->GetTP()
			, pDTCharInfo->GetWins()
			, pDTCharInfo->GetLoses()
			, pDTCharInfo->GetRanking()
			, pDTCharInfo->GetRankingIncrease()
			, pDTCharInfo->GetFinalCount()
			, pDTCharInfo->GetFinalWins()
			, pDTCharInfo->GetSemiFinalCount()
			, pDTCharInfo->GetSemiFinalWins()
			, pDTCharInfo->GetQuaterFinalCount()
			, pDTCharInfo->GetQuaterFinalWins()
			, pDTCharInfo->GetLeaveCount()
			, pDTCharInfo->GetLastWeekGrade()
			, pDTCharInfo->GetTimeStamp());
	}

	~CCMatchObjectDuelTournamentCharInfo()
	{
		RemoveSideRankingAll();
	}

	void SetCharInfo(int nTP, int nWins, int nLoses, int nRanking, int nRankingIncrease, 
		int nFinalCount, int nFinalWins, int nSemiFinalCount, int nSemiFinalWins, int nQuaterFinalCount, int nQuaterFinalWins, int nLeaveCount, int nLastWeekGrade, char* szTimeStamp)
	{
		m_nTP = nTP;
		m_nWins = nWins;
		m_nLoses = nLoses;
		m_nRanking = nRanking;
		m_nRankingIncrease = nRankingIncrease;
		m_nQuaterFinalCount = nQuaterFinalCount;
		m_nQuaterFinalWins = nQuaterFinalWins;
		m_nSemiFinalCount = nSemiFinalCount;
		m_nSemiFinalWins = nSemiFinalWins;
		m_nFinalCount = nFinalCount;
		m_nFinalWins = nFinalWins;

		m_nLeaveCount = nLeaveCount;
		m_nLastWeekGrade = nLastWeekGrade;

		m_bIsChallenge = false;
		m_bIsSettingData = true;

		memcpy(m_szTimeStamp, szTimeStamp, DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1);
	}

	list<DTRankingInfo*>* GetSideRankingList()	{ return &m_SideRankingList;}

	void AddSideRankingInfo(DTRankingInfo* pRankingInfo)
	{
		DTRankingInfo *pNewInfo = new DTRankingInfo;
		memcpy(pNewInfo, pRankingInfo, sizeof(DTRankingInfo));
		m_SideRankingList.push_back(pNewInfo);
	}

	void RemoveSideRankingAll()
	{	
		list<DTRankingInfo*>::iterator iter = m_SideRankingList.begin();
		for(; iter != m_SideRankingList.end();){
			DTRankingInfo* pInfo = (*iter);
			iter = m_SideRankingList.erase(iter);
			delete pInfo;
		}
	}

	int GetTP()					{ return m_nTP; }
	int GetWins()				{ return m_nWins; }
	int GetLoses()				{ return m_nLoses; }
	int GetRanking()			{ return m_nRanking; }
	int GetRankingIncrease()	{ return m_nRankingIncrease; }
	int GetQuaterFinalCount()	{ return m_nQuaterFinalCount; }
	int GetQuaterFinalWins()	{ return m_nQuaterFinalWins; }
	int GetSemiFinalCount()		{ return m_nSemiFinalCount; }
	int GetSemiFinalWins()		{ return m_nSemiFinalWins; }
	int GetFinalCount()			{ return m_nFinalCount; }
	int GetFinalWins()			{ return m_nFinalWins; }
	int GetLeaveCount()			{ return m_nLeaveCount; }
	int GetLastWeekGrade()		{ return m_nLastWeekGrade; }
	char *GetTimeStamp()		{ return m_szTimeStamp; }
	
	void SetTP(int nTP)					{ m_nTP = nTP; }
	void SetLastWeekGrade(int grade)	{ m_nLastWeekGrade = grade; }

	void IncreaseWins()					{ ++m_nWins; }
	void IncreaseLoses()				{ ++m_nLoses; }
	void IncreaseQuaterFinalCount()		{ ++m_nQuaterFinalCount; }
	void IncreaseQuaterFinalWins()		{ ++m_nQuaterFinalWins; }
	void IncreaseSemiFinalCount()		{ ++m_nSemiFinalCount; }
	void IncreaseSemiFinalWins()		{ ++m_nSemiFinalWins; }
	void IncreaseFinalCount()			{ ++m_nFinalCount; }
	void IncreaseFinalWins()			{ ++m_nFinalWins; }
	void IncreaseLeaveCount()			{ ++m_nLeaveCount; }

	bool IsChallengeDuelTournament()	{ return m_bIsChallenge; }
	void SetChallengeDuelTournament(bool bValue) { m_bIsChallenge = bValue; }

	bool IsJoinDuelTournament()	{ return m_bIsChallenge; }
	void SetJoinDuelTournament(bool bValue) { m_bIsChallenge = bValue; }

	bool IsSettingData() { return m_bIsSettingData; }
};

class CCMatchObject : public CCObject {
protected:
	CCMatchAccountInfo			m_AccountInfo;		// 계정 정보
	CCMatchCharInfo*				m_pCharInfo;		// 캐릭터 정보
	CCMatchFriendInfo*			m_pFriendInfo;		// 친구정보
	CCMatchPlace					m_nPlace;			// 위치 정보
	CCMatchTimeSyncInfo			m_nTimeSyncInfo;	// 스피드핵 감시	
	CCMatchObjectChannelInfo		m_ChannelInfo;		// 채널 정보
	CCMatchObjectGameInfo		m_GameInfo;			// 게임안에서의 정보
	CCMatchDisconnStatusInfo		m_DisconnStatusInfo;// 오브젝으 접속종료 상태 정보.
	CCMatchObjectHShieldInfo		m_HSieldInfo;

	bool			m_bHacker;						// 핵이 검출된 유저
	bool			m_bBridgePeer;
	bool			m_bRelayPeer;
	CCUID			m_uidAgent;

	DWORD			m_dwIP;
	char 			m_szIP[64];
	unsigned int	m_nPort;
	bool			m_bFreeLoginIP;					// 인원제한 상관없이 로그인 허용된 클라이언트

	unsigned char	m_nPlayerFlags;					// 휘발성 속성등 (CCTD_PlayerFlags)
	unsigned long	m_nUserOptionFlags;				// 귓말,초대 거부등의 옵션

	CCUID			m_uidStage;
	CCUID			m_uidChatRoom;

	bool			m_bStageListTransfer;
	unsigned long	m_nStageListChecksum;
	unsigned long	m_nStageListLastChecksum;
	int				m_nTimeLastStageListTrans;
	int				m_nStageCursor;

	CCRefreshClientChannelImpl		m_RefreshClientChannelImpl;
	CCRefreshClientClanMemberImpl	m_RefreshClientClanMemberImpl;

	CCMatchObjectStageState	m_nStageState;	// 대기방에서의 상태정보
	CCMatchTeam		m_nTeam;
	int				m_nLadderGroupID;
	bool			m_bLadderChallenging;	// 클랜전 상대팀 대기중인지 여부

//	bool			m_bStageMaster;	
	bool			m_bEnterBattle;
	bool			m_bAlive;
	unsigned int	m_nDeadTime;

	bool			m_bNewbie;				// 자신의 캐릭터들의 최고레벨이 10레벨이상이면 입문채널에 들어갈 수 없다.
	bool			m_bForcedEntried;		// 난입한 플레이어인지 여부.
	bool			m_bLaunchedGame;

	unsigned int			m_nKillCount;	// 한라운드 죽인수
	unsigned int			m_nDeathCount;	// 한라운드 죽은수
	unsigned long int		m_nAllRoundKillCount;	// 전체 라운드의 킬수
	unsigned long int		m_nAllRoundDeathCount;	// 전체 라운드의 데쓰수

	bool			m_bWasCallVote;		// 한 게임 안에서 투표건의를 했는가?

	bool			m_bDBFriendListRequested;		// 친구리스트 DB에 정보요청을 했는지 여부
	unsigned long int	m_nTickLastPacketRecved;	// 최근 패킷 받은시간
	unsigned long int	m_nLastHShieldMsgRecved;
	bool			m_bHShieldMsgRecved;
	string				m_strCountryCode3; // filter에서 설정됨.

	//DWORD	m_dwLastHackCheckedTime;
	//DWORD	m_dwLastRecvNewHashValueTime;
	//bool	m_bIsRequestNewHashValue;

	DWORD	m_dwLastSpawnTime;				// 마지막으로 스폰 되었던 시간

	unsigned long int m_nLastPingTime;		// 퀘스트모드, 핑 보낸 시간
	unsigned long int m_nQuestLatency;		// 퀘스트모드

	bool				m_bIsLoginCompleted;

	unsigned long		m_dwLastSendSuicideCommand;	// 비정상 유저는 MC_MATCH_REQUEST_SUICIDE를 무한으로 보낼수 있다.	
													// 이것을 막기 위해서, 일정 시간이 지나야 응답을 하는 방법으로 수정하였다.

	DWORD				m_dwLastSendGambleItemTime;

	bool				m_IsSendFirstGameguardRequest;
	bool				m_IsRecvFirstGameguardResponse;

public :
	CCASYNCJOBQ			m_DBJobQ;


protected:
	void UpdateChannelListChecksum(unsigned long nChecksum)	{ m_ChannelInfo.nChannelListChecksum = nChecksum; }
	unsigned long GetChannelListChecksum()					{ return m_ChannelInfo.nChannelListChecksum; }

	void UpdateStageListChecksum(unsigned long nChecksum)	{ m_nStageListChecksum = nChecksum; }
	unsigned long GetStageListChecksum()					{ return m_nStageListChecksum; }
	CCMatchObject() : CCObject() 
	{
	}
	void DeathCount()				{ m_nDeathCount++; m_nAllRoundDeathCount++; }
	void KillCount()				{ m_nKillCount++; m_nAllRoundKillCount++; }

//	void CheckClientHashValue( const DWORD dwTime );
public:
	CCMatchObject(const CCUID& uid);
	virtual ~CCMatchObject();

	char* GetName() { 
		if (m_pCharInfo)
			return m_pCharInfo->m_szName; 
		else
			return "Unknown";
	}
	char* GetAccountName()			{ return m_AccountInfo.m_szUserID; }

	bool IsLoginCompleted() { return m_bIsLoginCompleted; }
	void LoginCompleted() { m_bIsLoginCompleted = true; }
	void LoginNotCompleted() { m_bIsLoginCompleted = false; }

	bool GetBridgePeer()			{ return m_bBridgePeer; }
	void SetBridgePeer(bool bValue)	{ m_bBridgePeer = bValue; }
	bool GetRelayPeer()				{ return m_bRelayPeer; }
	void SetRelayPeer(bool bRelay)	{ m_bRelayPeer = bRelay; }
	const CCUID& GetAgentUID()		{ return m_uidAgent; }
	void SetAgentUID(const CCUID& uidAgent)	{ m_uidAgent = uidAgent; }

	void SetPeerAddr(DWORD dwIP, char* szIP, unsigned short nPort)	{ m_dwIP=dwIP; strcpy(m_szIP, szIP); m_nPort = nPort; }
	DWORD GetIP()					{ return m_dwIP; }
	char* GetIPString()				{ return m_szIP; }
	unsigned short GetPort()		{ return m_nPort; }
	bool GetFreeLoginIP()			{ return m_bFreeLoginIP; }
	void SetFreeLoginIP(bool bFree)	{ m_bFreeLoginIP = bFree; }

	void ResetPlayerFlags()						{ m_nPlayerFlags = 0; }
	unsigned char GetPlayerFlags()				{ return m_nPlayerFlags; }
	bool CheckPlayerFlags(unsigned char nFlag)	{ return (m_nPlayerFlags&nFlag?true:false); }
	void SetPlayerFlag(unsigned char nFlagIdx, bool bSet)	
	{ 
		if (bSet) m_nPlayerFlags |= nFlagIdx; 
		else m_nPlayerFlags &= (0xff ^ nFlagIdx);
	}

	void SetUserOption(unsigned long nFlags)	{ m_nUserOptionFlags = nFlags; }
	bool CheckUserOption(unsigned long nFlag)	{ return (m_nUserOptionFlags&nFlag?true:false); }

	CCUID GetChannelUID()						{ return m_ChannelInfo.uidChannel; }
	void SetChannelUID(const CCUID& uid)			{ SetRecentChannelUID(m_ChannelInfo.uidChannel); m_ChannelInfo.uidChannel = uid; }
	CCUID GetRecentChannelUID()					{ return m_ChannelInfo.uidRecentChannel; }
	void SetRecentChannelUID(const CCUID& uid)	{ m_ChannelInfo.uidRecentChannel = uid; }

	CCUID GetStageUID()					{ return m_uidStage; }
	void SetStageUID(const CCUID& uid)	{ m_uidStage = uid; }
	CCUID GetChatRooCCUID()				{ return m_uidChatRoom; }
	void SetChatRooCCUID(const CCUID& uid){ m_uidChatRoom = uid; }

	bool CheckChannelListTransfer()	{ return m_ChannelInfo.bChannelListTransfer; }
	void SetChannelListTransfer(const bool bVal, const CCCHANNEL_TYPE nChannelType=CCCHANNEL_TYPE_PRESET);

	bool CheckStageListTransfer()	{ return m_bStageListTransfer; }
	void SetStageListTransfer(bool bVal)	{ m_bStageListTransfer = bVal; UpdateStageListChecksum(0); }

	CCRefreshClientChannelImpl* GetRefreshClientChannelImplement()		{ return &m_RefreshClientChannelImpl; }
	CCRefreshClientClanMemberImpl* GetRefreshClientClanMemberImplement()	{ return &m_RefreshClientClanMemberImpl; }

	CCMatchTeam GetTeam()			{ return m_nTeam; }
	void SetTeam(CCMatchTeam nTeam);
	CCMatchObjectStageState GetStageState()	{ return m_nStageState; }
	void SetStageState(CCMatchObjectStageState nStageState)	{ m_nStageState = nStageState; }
	bool GetEnterBattle()			{ return m_bEnterBattle; }
	void SetEnterBattle(bool bEnter){ m_bEnterBattle = bEnter; }
	bool CheckAlive()				{ return m_bAlive; }
	void SetAlive(bool bVal)		{ m_bAlive = bVal; }
	void SetKillCount(unsigned int nKillCount) { m_nKillCount = nKillCount; }
	unsigned int GetKillCount()		{ return m_nKillCount; }
	void SetDeathCount(unsigned int nDeathCount) { m_nDeathCount = nDeathCount; }
	unsigned int GetDeathCount()	{ return m_nDeathCount; }
	unsigned int GetAllRoundKillCount()	{ return m_nAllRoundKillCount; }
	unsigned int GetAllRoundDeathCount()	{ return m_nAllRoundDeathCount; }
	void SetAllRoundKillCount(unsigned int nAllRoundKillCount) { m_nAllRoundKillCount = nAllRoundKillCount; }
	void SetAllRoundDeathCount(unsigned int nAllRoundDeathCount) { m_nAllRoundDeathCount = nAllRoundDeathCount; }
	void FreeCharInfo();
	void FreeFriendInfo();
	void OnDead();
	void OnKill();
	bool IsEnabledRespawnDeathTime(unsigned int nNowTime);

	void SetForcedEntry(bool bVal) { m_bForcedEntried = bVal; }
	bool IsForcedEntried() { return m_bForcedEntried; }
	void SetLaunchedGame(bool bVal) { m_bLaunchedGame = bVal; }
	bool IsLaunchedGame() { return m_bLaunchedGame; }
	void CheckNewbie(int nCharMaxLevel);
	bool IsNewbie()					{ return m_bNewbie; }
	bool IsHacker()					{ return m_bHacker; }
	void SetHacker(bool bHacker)	{ m_bHacker = bHacker; }

	inline bool WasCallVote()						{ return m_bWasCallVote; }
	inline void SetVoteState( const bool bState )	{ m_bWasCallVote = bState; }

	inline unsigned long int	GetTickLastPacketRecved()		{ return m_nTickLastPacketRecved; }
	inline unsigned long int	GetLastHShieldMsgRecved()		{ return m_nLastHShieldMsgRecved; }
	inline bool				GetHShieldMsgRecved()			{ return m_bHShieldMsgRecved; }
	inline void				SetHShieldMsgRecved(bool bSet)	{ m_bHShieldMsgRecved = bSet; }


	void UpdateTickLastPacketRecved();
	void UpdateLastHShieldMsgRecved();

	//void SetLastRecvNewHashValueTime( const DWORD dwTime )	
	//{ 
	//	m_dwLastRecvNewHashValueTime = dwTime; 
	//	m_bIsRequestNewHashValue = false; 
	//}

	DWORD GetLastSpawnTime( void)					{ return m_dwLastSpawnTime;		}
	void SetLastSpawnTime( DWORD dwTime)			{ m_dwLastSpawnTime = dwTime;	}

	inline unsigned long int	GetQuestLatency();
	inline void					SetQuestLatency(unsigned long int l);
	inline void					SetPingTime(unsigned long int t);

	const DWORD GetLastSendSuicideCmdTime()							{ return m_dwLastSendSuicideCommand; }
	inline void SetLastSendSuicideCmdTime( const DWORD dwTime )		{ m_dwLastSendSuicideCommand = dwTime; }

	const DWORD GetLastSendGambleItemTime() const { return m_dwLastSendGambleItemTime; }
	void SetLastSendGambleItemTime( const DWORD dwCurTime )	{ m_dwLastSendGambleItemTime = dwCurTime; }
public:
	// Ladder 관련
	int GetLadderGroupID()			{ return m_nLadderGroupID; }
	void SetLadderGroupID(int nID)	{ m_nLadderGroupID = nID; }
	void SetLadderChallenging(bool bVal) { m_bLadderChallenging = bVal; }
	bool IsLadderChallenging()			{ return m_bLadderChallenging; }		// 클랜전 상대팀 대기중인지 여부
public:
	CCMatchAccountInfo* GetAccountInfo() { return &m_AccountInfo; }
	CCMatchCharInfo* GetCharInfo()	{ return m_pCharInfo; }
	void SetCharInfo(CCMatchCharInfo* pCharInfo);
	
	CCMatchFriendInfo* GetFriendInfo()	{ return m_pFriendInfo; }
	void SetFriendInfo(CCMatchFriendInfo* pFriendInfo);
	bool DBFriendListRequested()	{ return m_bDBFriendListRequested; }
	CCMatchPlace GetPlace()			{ return m_nPlace; }
	void SetPlace(CCMatchPlace nPlace);
	CCMatchTimeSyncInfo* GetSyncInfo()	{ return &m_nTimeSyncInfo; }
//	CCMatchObjectAntiHackInfo* GetAntiHackInfo()		{ return &m_AntiHackInfo; }
	CCMatchDisconnStatusInfo& GetDisconnStatusInfo() { return  m_DisconnStatusInfo; }
	CCMatchObjectHShieldInfo* GetHShieldInfo()		{ return &m_HSieldInfo; }

	/// 틱 처리
	virtual void Tick(unsigned long int nTime);

	void OnStageJoin();
	void OnEnterBattle();
	void OnLeaveBattle();
	void OnInitRound();

	void SetStageCursor(int nStageCursor);
	const CCMatchObjectGameInfo* GetGameInfo() { return &m_GameInfo; }

	//filter
	void			SetCountryCode3( const string strCountryCode3 ) { m_strCountryCode3 = strCountryCode3; }
	const string&	GetCountryCode3() const							{ return m_strCountryCode3; }

	void DisconnectHacker( CCMatchHackingType eType );
// 	void SetXTrapHackerDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );
// 	void SetHShieldHackerDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );
// 	void SetBadFileCRCDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );
// 	void SetBadUserDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );
// 	void SetGameguardHackerDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );
// 	void SetDllInjectionDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );
// 	void SetInvalidStageSettingDisconnectWaitInfo( const CCMatchDisconnectStatus DisStatus = CCMDS_DISCONN_WAIT );

public:
	enum CCMO_ACTION
	{
		CCMOA_STAGE_FOLLOW,
		CCMOA_MAX
	};
	bool CheckEnableAction(CCMO_ACTION nAction);		// 해당 액션이 가능한지 여부 - 필요할때마다 추가한다.

	bool m_bQuestRecvPong;
	DWORD m_dwHShieldCheckCount;

public :
	void DeleteGameguard();

	const bool IsSendFirstGameguardRequest() const	{ return m_IsSendFirstGameguardRequest; }
	void FirstGameguardReqeustIsSent()				{ m_IsSendFirstGameguardRequest = true; }

	const bool IsRecvFirstGameguardResponse() const { return m_IsRecvFirstGameguardResponse; }
	void FirstGameguardResponseIsRecved()			{ m_IsRecvFirstGameguardResponse = true; }

	void ResetCustomItemUseCount();
	const bool IsHaveCustomItem();
	const bool IncreaseCustomItemUseCount();

	bool IsEquipCustomItem(int nItemId);


///< 채팅창 도배 및 벙어리 기능 구현
///< 홍기주(2009.08.05)
protected :
	CCMatchChatFloodInfo m_ChatFloodInfo;
public:
	bool CheckChatFlooding()		{ return m_ChatFloodInfo.PushChatTickCount();}
	bool IsChatBanUser()			{ return m_ChatFloodInfo.IsBanUser();}
	void SetChatBanUser()			{ m_ChatFloodInfo.SetBanUser();}

///< 듀얼 토너먼트 관련
///< 홍기주(2009.09.25)
protected :
	CCMatchObjectDuelTournamentCharInfo *m_pDuelTournamentCharInfo;

public:	
	CCMatchObjectDuelTournamentCharInfo* GetDuelTournamentCharInfo()		{ return m_pDuelTournamentCharInfo; }
	void SetDuelTournamentCharInfo(CCMatchObjectDuelTournamentCharInfo *pDTCharInfo);	
	void FreeDuelTournamentInfo();	

	bool IsChallengeDuelTournament() { 
		if( m_pDuelTournamentCharInfo == NULL ) return false;
		return m_pDuelTournamentCharInfo->IsChallengeDuelTournament(); 
	}

	void SetChallengeDuelTournament(bool bValue) { 
		if( m_pDuelTournamentCharInfo == NULL ) return;
		m_pDuelTournamentCharInfo->SetChallengeDuelTournament(bValue); 
	}

	bool IsJoinDuelTournament() { 
		if( m_pDuelTournamentCharInfo == NULL ) return false;
		return m_pDuelTournamentCharInfo->IsJoinDuelTournament(); 
	}

	void SetJoinDuelTournament(bool bValue) { 
		if( m_pDuelTournamentCharInfo == NULL ) return;
		m_pDuelTournamentCharInfo->SetJoinDuelTournament(bValue); 
	}

protected:
	CCMatchObjectCharBuff	m_CharBuffInfo;

public:
	void FreeCharBuff()							{ m_CharBuffInfo.FreeCharBuffInfo(); }
	CCMatchObjectCharBuff*	GetCharBuff()		{ return &m_CharBuffInfo; }
	
///< Punishment 관련
///< 홍기주(2010.08.09)
protected:
	CCMatchAccountPenaltyInfo m_AccountPenaltyInfo;

public:
	CCMatchAccountPenaltyInfo* GetAccountPenaltyInfo() { return &m_AccountPenaltyInfo; }

protected:
	unsigned int m_nLastCheckBattleTimeReward;

	void SetLastCheckBattleTimeReward(unsigned int nVal)	{ m_nLastCheckBattleTimeReward = nVal; }
	unsigned int GetLastCheckBattleTimeReward()				{ return m_nLastCheckBattleTimeReward; }

	void SetBattleTimeReward(bool bVal);
	void BattleTimeReward(unsigned int nTime);

protected:
	void ResetGamePlayInfo() { if( m_pCharInfo != NULL ) m_pCharInfo->GetCharGamePlayInfo()->Reset(); }
};

class CCMatchObjectList : public map<CCUID, CCMatchObject*>{};


// 캐릭터 생성할때 주는 기본 아이템
struct MINITIALCOSTUME
{
	// 무기
	unsigned int nMeleeItemID;
	unsigned int nPrimaryItemID;
	unsigned int nSecondaryItemID;
	unsigned int nCustom1ItemID;
	unsigned int nCustom2ItemID;

	// 장비 아이템
	unsigned int nChestItemID;
	unsigned int nHandsItemID;
	unsigned int nLegsItemID;
	unsigned int nFeetItemID;
};

#define MAX_COSTUME_TEMPLATE		6
const MINITIALCOSTUME g_InitialCostume[MAX_COSTUME_TEMPLATE][2] = 
{ 
	{{1, 5001, 4001, 30301, 0,     21001, 0, 23001, 0},	{1, 5001, 4001, 30301, 0,     21501, 0, 23501, 0}},	// 건나이트
	{{2, 5002, 0,    30301, 0,     21001, 0, 23001, 0},	{2, 5002, 0,    30301, 0,     21501, 0, 23501, 0}},	// 건파이터
	{{1, 4005, 5001, 30401, 0,     21001, 0, 23001, 0},	{1, 4005, 5001, 30401, 0,     21501, 0, 23501, 0}},	// 애서신
	{{2, 4001, 0,    30401, 0,     21001, 0, 23001, 0},	{2, 4001, 0,    30401, 0,     21501, 0, 23501, 0}},	// 스카우트
	{{2, 4002, 0,    30401, 30001, 21001, 0, 23001, 0},	{2, 4002, 0,    30401, 30001, 21501, 0, 23501, 0}},	// 건프리스트
	{{1, 4006, 0,	 30101, 30001, 21001, 0, 23001, 0},	{1, 4006, 4006, 30101, 30001, 21501, 0, 23501, 0}}	// 닥터
};

/*
const unsigned int g_InitialHair[4][2] = 
{
	{10000, 10022},
	{10001, 10023},
	{10002, 10024},
	{10003, 10025}
};
*/

#define MAX_COSTUME_HAIR		5
const string g_szHairMeshName[MAX_COSTUME_HAIR][2] = 
{
	{"eq_head_01", "eq_head_pony"},
	{"eq_head_02", "eq_head_hair001"},
	{"eq_head_08", "eq_head_hair04"},
	{"eq_head_05", "eq_head_hair006"},
	{"eq_head_08", "eq_head_hair002"}		// 이건 현재 사용안함 - 나중에 다른 모델로 대체해도 됨
};

#define MAX_COSTUME_FACE		4
const string g_szFaceMeshName[MAX_COSTUME_FACE][2] = 
{
	// 기본
	{"eq_face_01", "eq_face_001"},
	{"eq_face_02", "eq_face_002"},
	{"eq_face_04", "eq_face_003"},
	{"eq_face_05", "eq_face_004"},
/*
	{"eq_face_newface01", "eq_face_newface01"},
	{"eq_face_newface02", "eq_face_newface02"},
	{"eq_face_newface03", "eq_face_newface03"},
    {"eq_face_newface04", "eq_face_newface04"},
	{"eq_face_newface05", "eq_face_newface05"},
	{"eq_face_newface06", "eq_face_newface06"},
	{"eq_face_newface07", "eq_face_newface07"},
	{"eq_face_newface08", "eq_face_newface08"},
	{"eq_face_newface09", "eq_face_newface09"},
	{"eq_face_newface10", "eq_face_newface10"},
	{"eq_face_newface11", "eq_face_newface11"},
	{"eq_face_newface12", "eq_face_newface12"},
	{"eq_face_newface13", "eq_face_newface13"},
	{"eq_face_newface14", "eq_face_newface14"}, 
	{"eq_face_newface15", "eq_face_newface15"}, */
};
//남자는 eq_face_newface01 - 13
//여자는 eq_face_newface01 - 15

// MC_MATCH_STAGE_ENTERBATTLE 커맨드에서 쓰이는 파라메타
enum CCCmdEnterBattleParam
{
	MCEP_NORMAL = 0,
	MCEP_FORCED = 1,		// 난입
	MCEP_END
};

// 입을 수 있는 아이템인지 체크
bool IsEquipableItem(unsigned long int nItemID, int nPlayerLevel, CCMatchSex nPlayerSex);

inline bool IsEnabledObject(CCMatchObject* pObject) 
{
	if ((pObject == NULL) || (pObject->GetCharInfo() == NULL)) return false;
	return true;
}

inline bool IsAdminGrade(CCMatchUserGradeID nGrade) 
{
	if ((nGrade == CCMUG_EVENTMASTER) || 
		(nGrade == CCMUG_ADMIN) || 
		(nGrade == CCMUG_DEVELOPER))
		return true;

	return false;
}

inline bool IsAdminGrade(CCMatchObject* pObject) 
{
	if (pObject == NULL) return false;

	return IsAdminGrade(pObject->GetAccountInfo()->m_nUGrade);
}


//////////////////////////////////////////////////////////////////////////


unsigned long int CCMatchObject::GetQuestLatency()
{
	unsigned long int nowTime = timeGetTime();

	unsigned long int ret = nowTime - m_nLastPingTime;

	if (ret > m_nQuestLatency)
		m_nQuestLatency = ret;

	return m_nQuestLatency;
}

void CCMatchObject::SetQuestLatency(unsigned long int l)
{
	m_nQuestLatency = l - m_nLastPingTime;
}

void CCMatchObject::SetPingTime(unsigned long int t)
{
	m_nLastPingTime = t;
}
