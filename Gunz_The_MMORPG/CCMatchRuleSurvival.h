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

/// �����̹� �� Ŭ����
class CCMatchRuleSurvival : public CCMatchRuleBaseQuest {

	typedef pair< CCUID, unsigned long int > SacrificeSlot;	// <�������� �÷����� ������ UID, �÷����� �������� ItemID.>

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
	// Ÿ������ ---------------------

	/// ���濡�� �ʿ��� ���� ���������� ������. �����߿����� m_pQuestLevel�� ��¥���� ����ȴ�.
	struct CCQuestStageGameInfo
	{
		int				nQL;
		int				nPlayerQL;
		int				nMapsetID;
		unsigned int	nScenarioID;
	};
	/// ���ͳ� ���� ���
	enum COMBAT_PLAY_RESULT
	{
		CPR_PLAYING = 0,		///< ���� ������
		CPR_COMPLETE,			///< ���� Ŭ����
		CPR_FAILED				///< ����
	};

	/// �����̹� �ó����� �ݺ��� ������ ��ȭ�Ǵ� NPC�� �ɷ�ġ�� ����
	struct CCQuestLevelReinforcedNPCStat
	{
		float			fMaxHP;
		float			fMaxAP;
		CCQuestLevelReinforcedNPCStat() : fMaxAP(1), fMaxHP(1) {}
	};

	// ������� ---------------------

	unsigned long	m_nPrepareStartTime;	///< ��Ż �̵� �ð� ����ϱ� ���� ����
	unsigned long	m_nCombatStartTime;		///< �����̵��ϰ� ���ο� ���� ������ �ð�
	unsigned long	m_nQuestCompleteTime;	///< ����Ʈ Ŭ��� �ð�

	CCQuestSacrificeSlot				m_SacrificeSlot[ MAX_SACRIFICE_SLOT_COUNT ];
	//int								m_iScenarioState;
	int								m_nPlayerCount;
	CCMatchSurvivalGameLogInfoManager	m_SurvivalGameLogInfoMgr;

	CCQuestStageGameInfo				m_StageGameInfo;	///< ���濡�� �ʿ��� ���� ���������� ������

	vector< CCQUEST_NPC >			m_vecNPCInThisScenario;		///< �� �ó��������� ���� NPC ���
	map<CCQUEST_NPC, CCQuestLevelReinforcedNPCStat>	m_mapReinforcedNPCStat;	///< �ó����� �ݺ��� ��ġ�� ��ȭ�� NPC �ɷ�ġ�� ����

	typedef map<CCQUEST_NPC, CCQuestLevelReinforcedNPCStat>	MapReinforcedNPCStat;
	typedef MapReinforcedNPCStat::iterator		ItorReinforedNPCStat;

	// �Լ� -------------------------
	void ClearQuestLevel();
	void MakeStageGameInfo();
	void InitJacoSpawnTrigger();
	void MakeNPCnSpawn(CCQUEST_NPC nNPCID, bool bAddQuestDropItem, bool bKeyNPC);
	int GetRankInfo(int nKilledNpcHpApAccum, int nDeathCount);
protected:
	CCQuestLevel*			m_pQuestLevel;			///< ����Ʈ ���� ����
	CCQuestNPCSpawnTrigger	m_JacoSpawnTrigger;		///< �������� ��� ���� �Ŵ���
	CCQuestCombatState		m_nCombatState;			///< ���ͳ� ���� ����

	virtual void ProcessNPCSpawn();				///< NPC �����۾�
	virtual bool CheckNPCSpawnEnable();			///< NPC�� ���� �������� ����
	virtual void RouteGameInfo();				///< Ŭ���̾�Ʈ�� ���� ���� �����ش�.
	virtual void RouteStageGameInfo();			///< ����� ������������ �ٲ� ���� ������ �����ش�.
	virtual void RouteCompleted();				///< ����Ʈ ���� �޽����� ������. - ��������� �Բ� ������.
	virtual void RouteFailed();					///< ����Ʈ ���� �޽��� ������.
	virtual void OnCompleted();					///< ����Ʈ ������ ȣ��ȴ�.
	virtual void OnFailed();					///< ����Ʈ ���н� ȣ��ȴ�.
	virtual void DistributeReward() {}			///< ����Ʈ ������ ������ ��� - �����̹��� ������ ����

	void SendGameResult();						///< ���� ������ ����� �뺸

	/// ���� ���� ���۵Ǿ��ٰ� �޼��� ������.
	void RouteMapSectorStart();
	/// �ش� �÷��̾ ��Ż�� �̵��ߴٰ� �޼��� ������.
	/// @param uidPlayer �̵��� �÷��̾� UID
	void RouteMovetoPortal(const CCUID& uidPlayer);
	/// �ش� �÷��̾ ��Ż�� �̵��� �Ϸ�Ǿ��ٰ� �޼��� ������.
	/// @param uidPlayer �̵��� �÷��̾� UID
	void RouteReadyToNewSector(const CCUID& uidPlayer);
	/// �ش� ����Ʈ �������� �Ծ��ٰ� �޼��� ������.
	/// @param nQuestItemID  ����Ʈ ������ ID
	void RouteObtainQuestItem(unsigned long int nQuestItemID);
	/// �ش� �Ϲ� �������� �Ծ��ٰ� �޼��� ������.
	/// @param nItemID  �Ϲ� ������ ID
	void RouteObtainZItem(unsigned long int nItemID);
	/// ���� ����ġ ���Ʈ
	void RouteSectorBonus(const CCUID& uidPlayer, unsigned long int nEXPValue, unsigned long int nBP);
	/// ���� ���� ���� ��ȭ�� �޼��� ������.
	void RouteCombatState();
	/// ����Ʈ ���� ����
	bool MakeQuestLevel();
	/// ���� ���� ó�� �۾�
	/// - ���߿� �Ϸ��� Combat ���� ������ Survival���鶧 CCMatchRuleBaseQuest�� �Ű����� �Ѵ�.
	void CombatProcess();
	/// ���� ���ͷ� �̵�
	void MoveToNextSector();			
	/// ���� ���� ���� ��ȯ
	void SetCombatState(CCQuestCombatState nState);
	/// ���� ���ͷ� �̵��Ϸ�Ǿ����� üũ
	bool CheckReadytoNewSector();
	/// ���� ���尡 �������� üũ�Ѵ�.
	COMBAT_PLAY_RESULT CheckCombatPlay();
	/// ����Ʈ�� ��� �������� üũ�Ѵ�.
	bool CheckQuestCompleted();
	/// ����Ʈ Complete�ϰ� ���� ������ ���� �� �ִ� �����ð� ���
	bool CheckQuestCompleteDelayTime();
	/// ���� Ŭ����� ȣ��ȴ�.
	void OnSectorCompleted();
	/// ���� Ŭ����� ���� Xp Bp �� ó��
	void RewardSectorXpBp();
	/// ���� ���� ó�� �۾�
	void ProcessCombatPlay();

	/// �ش� ���� ���� ó�� �����Ҷ�
	void OnBeginCombatState(CCQuestCombatState nState);
	/// �ش� ���� ���� ��������
	void OnEndCombatState(CCQuestCombatState nState);

	///< ���������� ���� �й�
	void MakeRewardList();
	///< ����ġ�� �ٿ�Ƽ�� ���.
	//void DistributeXPnBP( CCQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL );	// �����̹����� ������
	///< ����Ʈ���� ���� ����Ʈ ������ �й�.		
	//bool DistributeQItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob); 	// �����̹����� ������
	///< ����Ʈ���� ���� �Ϲ� ������ �й�.		
	//bool DistributeZItem( CCQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob);	// �����̹����� ������
protected:
	virtual void OnBegin();								///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();								///< ��ü ���� ����� ȣ��
	virtual bool OnRun();								///< ����ƽ�� ȣ��
	virtual void OnCommand(CCCommand* pCommand);			///< ����Ʈ������ ����ϴ� Ŀ�ǵ� ó��
	virtual bool OnCheckRoundFinish();					///< ���尡 �������� üũ
public:
	CCMatchRuleSurvival(CCMatchStage* pStage);				///< ������
	virtual ~CCMatchRuleSurvival();							///< �Ҹ���

	virtual void RefreshStageGameInfo();

	/// NPC�� �׿����� ȣ��
	/// @param uidSender		�޼��� ���� �÷��̾�
	/// @param uidKiller		���� �÷��̾�
	/// @param uidNPC			���� NPC
	/// @param pos				NPC ��ġ
	virtual void OnRequestNPCDead(CCUID& uidSender, CCUID& uidKiller, CCUID& uidNPC, CCVector& pos);

	/// �÷��̾� �׾��� �� ȣ��
	/// @param uidVictim		���� �÷��̾� UID
	virtual void OnRequestPlayerDead(const CCUID& uidVictim);

	/// ���� �������� �Ծ��� ��� ȣ��ȴ�.
	/// @param pObj				�÷��̾� ������Ʈ
	/// @param nItemID			���� ������ ID
	/// @param nQuestItemID		����Ʈ ������ ID
	virtual void OnObtainWorldItem(CCMatchObject* pObj, int nItemID, int* pnExtraValues);

	virtual void OnRequestTestSectorClear();
	virtual void OnRequestTestFinish();

	/// �÷��̾ ��Ż�� �̵����� ��� ȣ��ȴ�.
	/// @param uidPlayer			�̵��� �÷��̾� UID
	virtual void OnRequestMovetoPortal(const CCUID& uidPlayer);
	/// ��Ż�� �̵��ϰ� ���� �̵��� �Ϸ�Ǿ��� ��� ȣ��ȴ�.
	/// @param uidPlayer			�÷��̾� UID
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
	virtual void							DestroyAllSlot() {} // �����̹����� ������
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_SURVIVAL; }


	void InsertNoParamQItemToPlayer( CCMatchObject* pPlayer, CCQuestItem* pQItem );

	void PostInsertQuestGameLogAsyncJob();

	// ������ �����Ҷ� �ʿ��� ���� ����.
	void CollectStartingQuestGameLogInfo();
	// ������ ������ �ʿ��� ���� ����.
	void CollectEndQuestGameLogInfo();

	//void RouteRewardCommandToStage( CCMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob );
	void RouteResultCommandToStage( CCMatchObject* pPlayer, int nReachedRound, int nPoint);

private :
	int CalcuOwnerQItemCount( const CCUID& uidPlayer, const unsigned long nItemID );
	const bool PostNPCInfo();
	bool PostRankingList();

	void ReinforceNPC();					// npc �ɷ� ��ȭ; �ó����� �ݺ����� ȣ��
	bool CollectNPCListInThisScenario();	// ���� ���۽� �� �ó��������� ����� NPC ����� �ۼ�
	int GetCurrentRoundIndex();					// ���� ����
	void PostPlayerPrivateRanking();			// ��� �� �濡 �ִ� �������� ���� ��ŷ ������ ��û�Ѵ�
};
