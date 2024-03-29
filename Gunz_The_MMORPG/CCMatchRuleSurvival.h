#pragma once

#include "CCMatchRule.h"
#include "CCMatchRuleBaseQuest.h"
#include "CCMatchNPCObject.h"
#include "CCMatchQuestRound.h"
#include "CCSacrificeQItemTable.h"
#include "CCQuestItem.h"
#include "CCMatchSurvivalGameLog.h"
#include "CCQuestNPCSpawnTrigger.h"
#include "CCBaseGameType.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCQuestLevel;

/// 서바이벌 룰 클래스
class CCMatchRuleSurvival : public CCMatchRuleBaseQuest {

	typedef pair< CCUID, unsigned long int > SacrificeSlot;	// <아이템을 올려놓은 유저의 UID, 올려놓은 아이템의 ItemID.>

	class CCQuestSacrificeSlot
	{
	public :
		CCQuestSacrificeSlot() 
		{
			Release();
		}

		~CCQuestSacrificeSlot()
		{
		}

		CCUID&				GetOwnerUID()	{ return m_SacrificeSlot.first; }
		unsigned long int	GetItemID()		{ return m_SacrificeSlot.second; }

		bool IsEmpty() 
		{
			if( (CCUID(0, 0) == m_SacrificeSlot.first) && (0 == m_SacrificeSlot.second) )
				return true;
			return false;
		}


		void SetOwnerUID( const CCUID& uidItemOwner )		{ m_SacrificeSlot.first = uidItemOwner; }
		void SetItemID( const unsigned long int nItemID )	{ m_SacrificeSlot.second = nItemID; }
		void SetAll( const CCUID& uidItemOwner, const unsigned long int nItemID )
		{
			SetOwnerUID( uidItemOwner );
			SetItemID( nItemID );
		}


		bool IsOwner( const CCUID& uidRequester, const unsigned long int nItemID )
		{
			if( (m_SacrificeSlot.first == uidRequester) && (m_SacrificeSlot.second == nItemID) )
				return true;
			return false;
		}

		void Release()
		{
			m_SacrificeSlot.first	= CCUID( 0, 0 );
			m_SacrificeSlot.second	= 0;
		}

	private :
		SacrificeSlot	m_SacrificeSlot;
	};

private:
	// 타입정의 ---------------------

	/// 대기방에서 필요한 현재 스테이지의 방정보. 게임중에서는 m_pQuestLevel에 진짜값이 저장된다.
	struct CCQuestStageGameInfo
	{
		int				nQL;
		int				nPlayerQL;
		int				nMapsetID;
		unsigned int	nScenarioID;
	};
	/// 섹터내 게임 결과
	enum COMBAT_PLAY_RESULT
	{
		CPR_PLAYING = 0,		///< 게임 진행중
		CPR_COMPLETE,			///< 게임 클리어
		CPR_FAILED				///< 실패
	};

	/// 서바이벌 시나리오 반복할 때마다 강화되는 NPC의 능력치를 보관
	struct CCQuestLevelReinforcedNPCStat
	{
		float			fMaxHP;
		float			fMaxAP;
		CCQuestLevelReinforcedNPCStat() : fMaxAP(1), fMaxHP(1) {}
	};

	// 멤버변수 ---------------------

	unsigned long	m_nPrepareStartTime;	///< 포탈 이동 시간 계산하기 위한 변수
	unsigned long	m_nCombatStartTime;		///< 섹터이동하고서 새로운 전투 시작한 시간
	unsigned long	m_nQuestCompleteTime;	///< 퀘스트 클리어때 시간

	CCQuestSacrificeSlot				m_SacrificeSlot[ MAX_SACRIFICE_SLOT_COUNT ];
	//int								m_iScenarioState;
	int								m_nPlayerCount;
	CCMatchSurvivalGameLogInfoManager	m_SurvivalGameLogInfoMgr;

	CCQuestStageGameInfo				m_StageGameInfo;	///< 대기방에서 필요한 현재 스테이지의 방정보

	vector< CCQUEST_NPC >			m_vecNPCInThisScenario;		///< 이 시나리오에서 쓰일 NPC 목록
	map<CCQUEST_NPC, CCQuestLevelReinforcedNPCStat>	m_mapReinforcedNPCStat;	///< 시나리오 반복을 거치며 강화된 NPC 능력치의 상태

	typedef map<CCQUEST_NPC, CCQuestLevelReinforcedNPCStat>	MapReinforcedNPCStat;
	typedef MapReinforcedNPCStat::iterator		ItorReinforedNPCStat;

	// 함수 -------------------------
	void ClearQuestLevel();
	void MakeStageGameInfo();
	void InitJacoSpawnTrigger();
	void MakeNPCnSpawn(CCQUEST_NPC nNPCID, bool bAddQuestDropItem, bool bKeyNPC);
	int GetRankInfo(int nKilledNpcHpApAccum, int nDeathCount);
protected:
	CCQuestLevel*			m_pQuestLevel;			///< 퀘스트 월드 레벨
	CCQuestNPCSpawnTrigger	m_JacoSpawnTrigger;		///< 보스방일 경우 자코 매니져
	CCQuestCombatState		m_nCombatState;			///< 섹터내 전투 상태

	virtual void ProcessNPCSpawn();				///< NPC 스폰작업
	virtual bool CheckNPCSpawnEnable();			///< NPC가 스폰 가능한지 여부
	virtual void RouteGameInfo();				///< 클라이언트에 게임 정보 보내준다.
	virtual void RouteStageGameInfo();			///< 대기중 스테이지에서 바뀐 게임 정보를 보내준다.
	virtual void RouteCompleted();				///< 퀘스트 성공 메시지를 보낸다. - 리워드까지 함께 보낸다.
	virtual void RouteFailed();					///< 퀘스트 실패 메시지 보낸다.
	virtual void OnCompleted();					///< 퀘스트 성공시 호출된다.
	virtual void OnFailed();					///< 퀘스트 실패시 호출된다.
	virtual void DistributeReward() {}			///< 퀘스트 성공시 리워드 배분 - 서바이벌엔 보상이 없다

	void SendGameResult();						///< 게임 끝날때 결과를 통보

	/// 섹터 라운드 시작되었다고 메세지 보낸다.
	void RouteMapSectorStart();
	/// 해당 플레이어가 포탈로 이동했다고 메세지 보낸다.
	/// @param uidPlayer 이동한 플레이어 UID
	void RouteMovetoPortal(const CCUID& uidPlayer);
	/// 해당 플레이어가 포탈로 이동이 완료되었다고 메세지 보낸다.
	/// @param uidPlayer 이동한 플레이어 UID
	void RouteReadyToNewSector(const CCUID& uidPlayer);
	/// 해당 퀘스트 아이템을 먹었다고 메세지 보낸다.
	/// @param nQuestItemID  퀘스트 아이템 ID
	void RouteObtainQuestItem(unsigned long int nQuestItemID);
	/// 해당 일반 아이템을 먹었다고 메세지 보낸다.
	/// @param nItemID  일반 아이템 ID
	void RouteObtainZItem(unsigned long int nItemID);
	/// 섹터 경험치 라우트
	void RouteSectorBonus(const CCUID& uidPlayer, unsigned long int nEXPValue, unsigned long int nBP);
	/// 섹터 전투 상태 변화시 메세지 보낸다.
	void RouteCombatState();
	/// 퀘스트 레벨 생성
	bool MakeQuestLevel();
	/// 섹터 전투 처리 작업
	/// - 나중에 일련의 Combat 상태 관리는 Survival만들때 CCMatchRuleBaseQuest로 옮겨져야 한다.
	void CombatProcess();
	/// 다음 섹터로 이동
	void MoveToNextSector();			
	/// 섹터 전투 상태 변환
	void SetCombatState(CCQuestCombatState nState);
	/// 다음 섹터로 이동완료되었는지 체크
	bool CheckReadytoNewSector();
	/// 섹터 라운드가 끝났는지 체크한다.
	COMBAT_PLAY_RESULT CheckCombatPlay();
	/// 퀘스트가 모두 끝났는지 체크한다.
	bool CheckQuestCompleted();
	/// 퀘스트 Complete하고 나서 아이템 먹을 수 있는 지연시간 계산
	bool CheckQuestCompleteDelayTime();
	/// 섹터 클리어시 호출된다.
	void OnSectorCompleted();
	/// 섹터 클리어시 보상 Xp Bp 를 처리
	void RewardSectorXpBp();
	/// 섹터 전투 처리 작업
	void ProcessCombatPlay();

	/// 해당 전투 상태 처음 시작할때
	void OnBeginCombatState(CCQuestCombatState nState);
	/// 해당 전투 상태 끝났을때
	void OnEndCombatState(CCQuestCombatState nState);

	///< 게임종료후 보상 분배
	void MakeRewardList();
	///< 경험치와 바운티를 배분.
	//void DistributeXPnBP( CCQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL );	// 서바이벌에서 사용않음
	///< 퀘스트에서 얻은 퀘스트 아이템 분배.		
	//bool DistributeQItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob); 	// 서바이벌에서 사용않음
	///< 퀘스트에서 얻은 일반 아이템 분배.		
	//bool DistributeZItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob);	// 서바이벌에서 사용않음
protected:
	virtual void OnBegin();								///< 전체 게임 시작시 호출
	virtual void OnEnd();								///< 전체 게임 종료시 호출
	virtual bool OnRun();								///< 게임틱시 호출
	virtual void OnCommand(CCCommand* pCommand);			///< 퀘스트에서만 사용하는 커맨드 처리
	virtual bool OnCheckRoundFinish();					///< 라운드가 끝났는지 체크
public:
	CCMatchRuleSurvival(CCMatchStage* pStage);				///< 생성자
	virtual ~CCMatchRuleSurvival();							///< 소멸자

	virtual void RefreshStageGameInfo();

	/// NPC를 죽였을때 호출
	/// @param uidSender		메세지 보낸 플레이어
	/// @param uidKiller		죽인 플레이어
	/// @param uidNPC			죽은 NPC
	/// @param pos				NPC 위치
	virtual void OnRequestNPCDead(CCUID& uidSender, CCUID& uidKiller, CCUID& uidNPC, CCVector& pos);

	/// 플레이어 죽었을 때 호출
	/// @param uidVictim		죽은 플레이어 UID
	virtual void OnRequestPlayerDead(const CCUID& uidVictim);

	/// 월드 아이템을 먹었을 경우 호출된다.
	/// @param pObj				플레이어 오브젝트
	/// @param nItemID			월드 아이템 ID
	/// @param nQuestItemID		퀘스트 아이템 ID
	virtual void OnObtainWorldItem(CCMatchObject* pObj, int nItemID, int* pnExtraValues);

	virtual void OnRequestTestSectorClear();
	virtual void OnRequestTestFinish();

	/// 플레이어가 포탈로 이동했을 경우 호출된다.
	/// @param uidPlayer			이동한 플레이어 UID
	virtual void OnRequestMovetoPortal(const CCUID& uidPlayer);
	/// 포탈로 이동하고 나서 이동이 완료되었을 경우 호출된다.
	/// @param uidPlayer			플레이어 UID
	virtual void OnReadyToNewSector(const CCUID& uidPlayer);

	virtual void OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnResponseDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnResponseCallBackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void OnRequestQL( const CCUID& uidSender );
	virtual void OnResponseQL_ToStage( const CCUID& uidStage );
	virtual void OnRequestSacrificeSlotInfo( const CCUID& uidSender );
	virtual void OnResponseSacrificeSlotInfoToListener( const CCUID& uidSender );
	virtual void OnResponseSacrificeSlotInfoToStage( const CCUID& uidStage );
	virtual void OnChangeCondition();

	virtual bool							PrepareStart();
	virtual bool							IsSacrificeItemDuplicated( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID );
	virtual void							PreProcessLeaveStage( const CCUID& uidLeaverUID );
	virtual void							DestroyAllSlot() {} // 서바이벌에서 사용않음
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_SURVIVAL; }


	void InsertNoParamQItemToPlayer( CCMatchObject* pPlayer, CCQuestItem* pQItem );

	void PostInsertQuestGameLogAsyncJob();

	// 게임이 시작할때 필요한 정보 수집.
	void CollectStartingQuestGameLogInfo();
	// 게임이 끝나고 필요한 정보 수집.
	void CollectEndQuestGameLogInfo();

	//void RouteRewardCommandToStage( CCMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob );
	void RouteResultCommandToStage( CCMatchObject* pPlayer, int nReachedRound, int nPoint);

private :
	int CalcuOwnerQItemCount( const CCUID& uidPlayer, const unsigned long nItemID );
	const bool PostNPCInfo();
	bool PostRankingList();

	void ReinforceNPC();					// npc 능력 강화; 시나리오 반복마다 호출
	bool CollectNPCListInThisScenario();	// 게임 시작시 이 시나리오에서 사용할 NPC 목록을 작성
	int GetCurrentRoundIndex();					// 현재 라운드
	void PostPlayerPrivateRanking();			// 디비에 이 방에 있는 유저들의 개인 랭킹 정보를 요청한다
};
