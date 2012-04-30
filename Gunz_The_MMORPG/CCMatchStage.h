#pragma once

#include <vector>
#include <list>
using namespace std;

#include "CCMatchItem.h"
#include "CCMatchTransDataType.h"
#include "CCUID.h"
#include "CCMatchRule.h"
#include "CCMatchObject.h"
#include "CCMatchWorldItem.h"
#include "CCMatchActiveTrapMgr.h"
#include "CCMatchStageSetting.h"
#include "CCVoteMgr.h"
#include "CCMatchGlobal.h"
#include "CCDuelTournamentGroup.h"

#define CCTICK_STAGE			100

class CCMatchObject;
class CCMatchStage;
class CCMatchServer;
class CCLadderGroup;

// �������� Ÿ��
enum CCMatchStageType
{
	CCST_NORMAL	= 0,			// �Ϲ�
	CCST_LADDER,					// ������ Ŭ������
	
	CCST_MAX
};

struct CCMatchStageTeamBonus
{
	bool		bApplyTeamBonus;		// �ش� ���ӿ��� ����ġ ���ʽ��� ����Ǵ��� ����
};

// Ladder���ϰ�� ������ ����
struct CCMatchLadderTeamInfo
{
	int		nTID;				// Ladder Team ID
	int		nFirstMemberCount;	// �ʱ�����

	// Clan�������� ����ϴ� ����
	int		nCLID;				// Ŭ��ID
	int		nCharLevel;			// ĳ���� ��� ����
	int		nContPoint;			// ĳ���� �⿩�� ���
};


// Stage���� ����ϴ� ���� �߰�����
struct CCMatchStageTeam
{
	int						nTeamBonusExp;			// ������ �� ����ġ
	int						nTeamTotalLevel;		// �������� ���� ���� - �����ʽ� ��н� ����Ѵ�
	int						nScore;					// ���ھ�
	int						nSeriesOfVictories;		// ��������
	int						nTotalKills;			// ���� �� ų�� (���� ����, �� ���� ���۶� �ʱ�ȭ - ������������ġ�� ����)
	CCMatchLadderTeamInfo	LadderInfo;
};


struct CCMatchStageSuicide
{
	CCMatchStageSuicide( const CCUID& uidUser, const DWORD dwExpireTime )
	{
		m_uidUser		= uidUser;
		m_dwExpireTime	= dwExpireTime;
		m_bIsChecked	= false;
	}

	CCUID	m_uidUser;
	DWORD	m_dwExpireTime;
	bool	m_bIsChecked;
};


struct CCMATCH_RESOURCECHECKINFO
{
	DWORD	dwLastRequestTime;
	DWORD	dwResourceCRC32Cache;
	DWORD	dwResourceXORCache;
	bool	bIsEnterBattle;
	bool	bIsChecked;
};

struct CCMatchDuelTournamentMatch
{
	int nMatchNumber;
	int nNextMatchNumber;

	CCUID uidPlayer1;
	CCUID uidPlayer2;
	
	CCDUELTOURNAMENTROUNDSTATE nRoundState;
};

struct CCMatchDuelTournamentStageInfo
{
	CCDUELTOURNAMENTTYPE nDuelTournamentType;

	int nDuelTournamentNumber;
	char szTimeStamp[DUELTOURNAMENT_TIMESTAMP_MAX_LENGTH + 1];

	map<int, CCMatchDuelTournamentMatch*> DuelTournamentMatchMap;
	int nDuelTournamentTotalRound;
};

typedef map<CCUID, CCMATCH_RESOURCECHECKINFO>	ResourceCRC32CacheMap;


class CCMatchCRC32XORCache;


// �������� ���� Ŭ���� - ���������� ���õ� ���� �Ѱ��Ѵ�.
class CCMatchStage {
private:
	int						m_nIndex;
	STAGE_STATE				m_nState;
	CCMatchStageType			m_nStageType;
	CCUID					m_uidStage;
	CCUID					m_uidOwnerChannel;
	char					m_szStageName[ STAGENAME_LENGTH ];
	bool					m_bPrivate;		// ��й�
	char					m_szStagePassword[ STAGEPASSWD_LENGTH ];
	CCMatchStageTeamBonus	m_TeamBonus;
	CCMatchStageTeam			m_Teams[MMT_END];

	CCUIDRefCache			m_ObjUIDCaches;
	list<int>				m_BanCIDList;

	unsigned long			m_nStateTimer;
	unsigned long			m_nLastTick;
	unsigned long			m_nChecksum;					// ��Ϲ� ���� ���ſ�
	unsigned long			m_nLastChecksumTick;
	int						m_nAdminObjectCount;
	unsigned long			m_nStartTime;					// ���� ������ �ð�
	unsigned long			m_nLastRequestStartStageTime;

	
	CCMatchStageSetting		m_StageSetting;
	CCMatchRule*				m_pRule;

	CCUID					m_uidAgent;
	bool					m_bAgentReady;
	int						m_nRoundObjCount[MMT_END];

	CCVoteMgr				m_VoteMgr;

	char					m_szFirstMasterName[MATCHOBJECT_NAME_LENGTH];

	ResourceCRC32CacheMap	m_ResourceCRC32CacheMap;
	DWORD					m_dwLastResourceCRC32CacheCheckTime;
	bool					m_bIsUseResourceCRC32CacheCheck;
	
	// �����̸�
	vector<RelayMap>		m_vecRelayMapsRemained;			// ���� ���� �ʵ�
	RELAY_MAP_TYPE			m_RelayMapType;
	RELAY_MAP_REPEAT_COUNT	m_RelayMapRepeatCountRemained;	// ������ ���� �ݺ� ��
	bool					m_bIsLastRelayMap;

	void SetMasterUID(const CCUID& uid)	{ m_StageSetting.SetMasterUID(uid);}
	CCMatchRule* CreateRule(MMATCH_GAMETYPE nGameType);

	vector< MMatchStageSuicide > m_SuicideList;
protected:
	inline bool IsChecksumUpdateTime(unsigned long nTick);
	void UpdateChecksum(unsigned long nTick);
	void OnStartGame();
	void OnFinishGame();
	void OnApplyTeamBonus(MMatchTeam nTeam);
protected:
	friend MMatchServer;
	void SetStageType(MMatchStageType nStageType);
	void SetLadderTeam(MMatchLadderTeamInfo* pRedLadderTeamInfo, MMatchLadderTeamInfo* pBlueLadderTeamInfo);


private :
	const bool SetChannelRuleForCreateStage(bool bIsAllowNullChannel);


public:
	CCMatchWorldItemManager	m_WorldItemManager;
	CCMatchActiveTrapMgr		m_ActiveTrapManager;

	void UpdateStateTimer();
	unsigned long GetStateTimer()	{ return m_nStateTimer; }
	unsigned long GetChecksum()		{ return m_nChecksum; }
	unsigned long GetStartTime()	{ return m_nStartTime; }		///< ���� ������ �ð�
public:
	CCMatchStage();
	virtual ~MMatchStage();

	bool Create(const CCUID& uid, const char* pszName, bool bPrivate, const char* pszPassword, bool bIsAllowNullChannel, 
		const MMATCH_GAMETYPE GameType = MMATCH_GAMETYPE_DEFAULT, const bool bIsCheckTicket = false, const DWORD dwTicketItemID = 0);

	void Destroy();
	void OnCommand(MCommand* pCommand);
	void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);
	bool CheckAutoTeamBalancing();	// true�̸� ������ ����� �Ѵ�.
	void ShuffleTeamMembers();


	const char* GetName()		{ return m_szStageName; }
	const char* GetPassword()	{ return m_szStagePassword; }
	void SetPassword(const char* pszPassword)	{ strcpy(m_szStagePassword, pszPassword); }
	const bool IsPrivate()		{ return m_bPrivate; }
	void SetPrivate(bool bVal)	{ m_bPrivate = bVal; }
	CCUID GetUID()				{ return m_uidStage; }

	const char* GetMapName()	{ return m_StageSetting.GetMapName(); }
	bool SetMapName(char* pszMapName); // 	{ m_StageSetting.SetMapName(pszMapName); }
	
	char* GetFirstMasterName()	{ return m_szFirstMasterName; }
	void SetFirstMasterName(char* pszName)	{ strcpy(m_szFirstMasterName, pszName); }

	CCMatchObject* GetObj(const CCUID& uid)	{ if (m_ObjUIDCaches.count(uid) == 0) return NULL; else return (MMatchObject*)(m_ObjUIDCaches[uid]); }			///< �߰�by ����, ����� ���� -_-
	size_t GetObjCount()					{ return m_ObjUIDCaches.size(); }
	int GetPlayers();
	CCUIDRefCache::iterator GetObjBegin()	{ return m_ObjUIDCaches.begin(); }
	CCUIDRefCache::iterator GetObjEnd()		{ return m_ObjUIDCaches.end(); }
	int GetObjInBattleCount();															///< �����ϰ� �ִ� �÷��̾��
	int GetCountableObjCount()				{ return ((int)GetObjCount() - m_nAdminObjectCount); }	///< ��ڸ� ������ �÷��̾��


	void AddBanList(int nCID);
	bool CheckBanList(int nCID);

	void AddObject(const CCUID& uid, const MMatchObject* pObj);
	CCUIDRefCache::iterator RemoveObject(const CCUID& uid);
	bool KickBanPlayer(const char* pszName, bool bBanPlayer=true);

	const CCUID RecommandMaster(bool bInBattleOnly);
	void EnterBattle(MMatchObject* pObj);
	void LeaveBattle(MMatchObject* pObj);

	STAGE_STATE GetState()					{ return m_nState; }
	void ChangeState(STAGE_STATE nState)	{ m_nState = nState; UpdateStateTimer(); }

	bool CheckTick(unsigned long nClock);
	void Tick(unsigned long nClock);

	CCMatchStageSetting* GetStageSetting() { return &m_StageSetting; }

	CCMatchRule* GetRule()			{ return m_pRule; }
	void ChangeRule(MMATCH_GAMETYPE nRule);
	void GetTeamMemberCount(int* poutnRedTeamMember, int* poutnBlueTeamMember, int* poutSpecMember, bool bInBattle);
	CCMatchTeam GetRecommandedTeam();

	CCVoteMgr* GetVoteMgr()			{ return &m_VoteMgr; }

	CCUID GetAgentUID()				{ return m_uidAgent; }
	void SetAgentUID(CCUID uid)		{ m_uidAgent = uid; }
	bool GetAgentReady()			{ return m_bAgentReady; }
	void SetAgentReady(bool bReady)	{ m_bAgentReady = bReady; }

	CCUID GetMasterUID()				{ return m_StageSetting.GetMasterUID(); }
	int GetIndex()					{ return m_nIndex; }

	void SetOwnerChannel(CCUID& uidOwnerChannel, int nIndex);
	CCUID GetOwnerChannel() { return m_uidOwnerChannel; }

	void PlayerTeam(const CCUID& uidPlayer, MMatchTeam nTeam);
	void PlayerState(const CCUID& uidPlayer, MMatchObjectStageState nStageState);
	bool StartGame( const bool bIsUseResourceCRC32CacheCheck );
	bool StartRelayGame( const bool bIsUseResourceCRC32CacheCheck );
	bool FinishGame();
	bool CheckBattleEntry();

	void RoundStateFromClient(const CCUID& uidStage, int nState, int nRound);
	void ObtainWorldItem(MMatchObject* pObj, const int nItemID);
	void RequestSpawnWorldItem(MMatchObject* pObj, const int nItemID, 
							   const float x, const float y, const float z, float fDropDelayTime);
	void SpawnServerSideWorldItem(MMatchObject* pObj, const int nItemID, 
							   const float x, const float y, const float z, 
							   int nLifeTime, int* pnExtraValues );
	void OnNotifyThrowTrapItem(const CCUID& uidPlayer, const int nItemID);
	void OnNotifyActivatedTrapItem(const CCUID& uidPlayer, const int nItemID, const MVector3& pos);


	bool IsApplyTeamBonus();	// ���� ���ʽ� ���뿩�� Ȯ��
	void AddTeamBonus(int nExp, MMatchTeam nTeam);
	int GetTeamScore(MMatchTeam nTeam)		{ return m_Teams[nTeam].nScore; }

	void InitTeamKills()									{ m_Teams[MMT_BLUE].nTotalKills = m_Teams[MMT_RED].nTotalKills = 0; }
	int GetTeamKills(MMatchTeam nTeam)						{ return m_Teams[nTeam].nTotalKills; }
	void AddTeamKills(MMatchTeam nTeam, int amount=1)		{ m_Teams[nTeam].nTotalKills+=amount; }	

	const MMatchStageType GetStageType()	{ return m_nStageType; }
	int GetMinPlayerLevel();	// �濡 �ִ� �÷��̾��� �ּ� ������ ���Ѵ�.

	bool CheckUserWasVoted( const CCUID& uidPlayer );

	bool CheckDuelMap();
	bool CheckTicket( MMatchObject* pObj );
	bool CheckQuestGame();

	// �����̸�
	bool IsRelayMap()									{ return m_StageSetting.IsRelayMap(); }
	bool IsStartRelayMap()								{ return m_StageSetting.IsStartRelayMap(); }
	int						GetRelayMapListCount()		{ return m_StageSetting.GetRelayMapListCount(); }
	const RelayMap*			GetRelayMapList()			{ return m_StageSetting.GetRelayMapList(); }
	RELAY_MAP_TYPE			GetRelayMapType()			{ return m_StageSetting.GetRelayMapType(); }
	RELAY_MAP_REPEAT_COUNT  GetRelayMapRepeatCount()	{ return m_StageSetting.GetRelayMapRepeatCount(); }

	void InitCurrRelayMap();

	void SetIsRelayMap(bool bIsRelayMap)									{ m_StageSetting.SetIsRelayMap(bIsRelayMap); }	
	void SetIsStartRelayMap(bool bVal)										{ m_StageSetting.SetIsStartRelayMap(bVal); }
	void SetRelayMapType(RELAY_MAP_TYPE nRelayMapType)						{ m_StageSetting.SetRelayMapType(nRelayMapType); }
	void SetRelayMapRepeatCount(RELAY_MAP_REPEAT_COUNT nRelayRepeatCount)	{ m_StageSetting.SetRelayMapRepeatCount(nRelayRepeatCount); }
	void SetRelayMapCurrList(const RelayMap* relayMapList);
	void SetRelayMapListCount(int nRelayMapListCount);
	void SetRelayMapList(RelayMap* RelayMapList);	
	

public:
	// Rule���� ȣ���ϴ� �Լ���
	void OnRoundEnd_FromTeamGame(MMatchTeam nWinnerTeam);
	void OnInitRound();			// ���� ���۽� RuleŬ�������� ȣ��

public :
	// suicide.
	void ReserveSuicide( const CCUID& uidUser, const DWORD dwExpireTime );
	void CheckSuicideReserve( const DWORD dwCurTime );

public :
	void ResetPlayersCustomItem();
	void ClearGabageObject();

private :
	CCChannelRule*	GetStageChannelRule();
	bool			IsValidMap( const char* pMapName );
	bool			IsClanServer();

	void			MakeResourceCRC32Cache( const DWORD dwKey, DWORD& out_crc32, DWORD& out_xor );
	void			MakeItemResourceCRC32Cache( MMatchCRC32XORCache& CRC32Cache );
	void			SetResourceCRC32Cache( const CCUID& uidPlayer, const DWORD dwCRC32Cache, const DWORD dwXORCache );
	void			RequestResourceCRC32Cache( const CCUID& uidPlayer );
	void			DeleteResourceCRC32Cache( const CCUID& uidPlayer );
	const bool 		IsValidResourceCRC32Cache( const CCUID& uidPlayer, const DWORD dwResourceCRC32Cache, const DWORD dwResourceXORCache);
	void			SetDisableCheckResourceCRC32Cache( const CCUID& uidPlayer );
	void			SetDisableAllCheckResourceCRC32Cache();

	void			CheckResourceCRC32Cache( const DWORD dwClock );

private:
	CCMatchDuelTournamentStageInfo m_nDTStageInfo;

	void MakeDuelTournamentMatchMap(MDUELTOURNAMENTROUNDSTATE nRoundState, int nMatchNumber);
	void ClearDuelTournamentMatchMap();

	CCDUELTOURNAMENTROUNDSTATE GetDuelTournamentRoundState(MDUELTOURNAMENTTYPE nType);
	CCDUELTOURNAMENTROUNDSTATE GetDuelTournamentNextRoundState(MDUELTOURNAMENTROUNDSTATE nRoundState);

	int GetDuelTournamentNextOrder(MDUELTOURNAMENTROUNDSTATE nRoundState, int nOrder, int nTemp);
public:
	void SetDuelTournamentMatchList(MDUELTOURNAMENTTYPE nType, MDuelTournamentPickedGroup *pPickedGroup);
	void SetDuelTournamentMatchNumber(int nValue)						{ m_nDTStageInfo.nDuelTournamentNumber = nValue; }	
	void SetDuelTournamentMatchTimeStamp(char* szTimeStamp)				{ strcpy(m_nDTStageInfo.szTimeStamp, szTimeStamp); }
	
	const MDUELTOURNAMENTTYPE GetDuelTournamentType()							{ return m_nDTStageInfo.nDuelTournamentType; }
	const int	GetDuelTournamentTotalRound()									{ return m_nDTStageInfo.nDuelTournamentTotalRound; }
	const int	GetDuelTournamentMatchNumber()									{ return m_nDTStageInfo.nDuelTournamentNumber; }	
	const char* GetDuelTournamentMatchTimeStamp()								{ return m_nDTStageInfo.szTimeStamp; }
	map<int, MMatchDuelTournamentMatch*>* GetDuelTournamentMatchMap()			{ return &(m_nDTStageInfo.DuelTournamentMatchMap); }

	int	GetDuelTournamentRandomMapIndex();	

private:
	unsigned int m_nGameLogID;

public:
	void SetGameLogID(unsigned int nVal)	{ m_nGameLogID = nVal; }
	unsigned int GetGameLogID()				{ return m_nGameLogID; }

};


inline CCDUELTOURNAMENTROUNDSTATE MMatchStage::GetDuelTournamentNextRoundState(MDUELTOURNAMENTROUNDSTATE nRoundState)
{
	switch(nRoundState){
		case CCDUELTOURNAMENTROUNDSTATE_QUATERFINAL :		return CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL;
		case CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL :			return CCDUELTOURNAMENTROUNDSTATE_FINAL;
		default :											return CCDUELTOURNAMENTROUNDSTATE_MAX;
	}
}

inline CCDUELTOURNAMENTROUNDSTATE CCMatchStage::GetDuelTournamentRoundState(MDUELTOURNAMENTTYPE nType)
{
	switch(nType) {		
		case CCDUELTOURNAMENTTYPE_QUATERFINAL :		return CCDUELTOURNAMENTROUNDSTATE_QUATERFINAL;
		case CCDUELTOURNAMENTTYPE_SEMIFINAL :		return CCDUELTOURNAMENTROUNDSTATE_SEMIFINAL;		
		case CCDUELTOURNAMENTTYPE_FINAL :			return CCDUELTOURNAMENTROUNDSTATE_FINAL;
		default : ASSERT(0);
	}

	return CCDUELTOURNAMENTROUNDSTATE_MAX;
}


class CCMatchStageMap : public map<CCUID, CCMatchStage*> {
	CCUID	m_uidGenerate;
public:
	CCMatchStageMap()			{	m_uidGenerate = CCUID(0,0);	}
	virtual ~CCMatchStageMap()	{	}
	CCUID UseUID()				{	m_uidGenerate.Increase();	return m_uidGenerate;	}
	void Insert(const CCUID& uid, CCMatchStage* pStage)	{	insert(value_type(uid, pStage));	}
};

CCMatchItemBonusType GetStageBonusType(CCMatchStageSetting* pStageSetting);


#define TRANS_STAGELIST_NODE_COUNT	8	// �ѹ��� Ŭ���̾�Ʈ���� �����ִ� ����������� ����
