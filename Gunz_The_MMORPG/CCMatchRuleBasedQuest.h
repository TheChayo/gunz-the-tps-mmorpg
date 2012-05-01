#pragma once
#include "CCMatchRule.h"
#include "CCMatchNPCObject.h"
#include "CCQuestPlayer.h"
#include "CCMatchQuestRound.h"


class CCMatchQuestGameLogInfoManager;

/// ����Ʈ����� �θ� Ŭ����
class CCMatchRuleBaseQuest : public CCMatchRule {
protected:
	CCMatchNPCManager			m_NPCManager;				///< NPC ������
	CCQuestPlayerManager			m_PlayerManager;			///< �÷��̾� ������
	bool						m_bQuestCompleted;			///< ����Ʈ�� �������� ����

	unsigned long int			m_nLastNPCSpawnTime;		// for test
	int							m_nNPCSpawnCount;			///< ������ NPC��
	unsigned long int			m_nSpawnTime;				///< ������ NPC ���� �ð�
	int							m_nFirstPlayerCount;		///< ó�� ���ӽ��۽� �÷��̾� ��

	unsigned long int			m_nLastNPCAssignCheckTime;	///< ���������� NPC ���Ҵ� üũ�� �� �ð�
	unsigned long int			m_nLastPingTime;			///< ���������� Ŭ���̾�Ʈ �� �� �ð�

protected:
	virtual void OnBegin();								///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();								///< ��ü ���� ����� ȣ��
	virtual bool OnRun();								///< ����ƽ�� ȣ��
	virtual void OnRoundBegin();						///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();							///< ���� ���� �� ȣ��
	virtual bool OnCheckRoundFinish();					///< ���尡 �������� üũ
	virtual void OnRoundTimeOut();						///< ���尡 Ÿ�Ӿƿ����� ����� �� OnRoundEnd() ���̴�.
	virtual bool RoundCount();							///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.
	virtual bool OnCheckEnableBattleCondition();		///< ���� �������� üũ

	virtual void OnCommand(CCCommand* pCommand);			///< ����Ʈ������ ����ϴ� Ŀ�ǵ� ó��
protected:
	/// npc�� �װ� ���� ��Ӿ������� �ִ��� üũ�ϰ� ������ �������� ��ӽ�Ų��.
	/// @param uidPlayer	������ �÷��̾� UID
	/// @param pDropItem	����� ������
	/// @param pos			��ġ
	void CheckRewards(CCUID& uidPlayer, CCQuestDropItem* pDropItem, CCVector& pos);
	bool CheckPlayersAlive();								///< �÷��̾ ��� �׾����� üũ
	virtual void ProcessNPCSpawn() = 0;						///< NPC �����۾�
	virtual bool CheckNPCSpawnEnable() = 0;					///< NPC�� ���� �������� ����
	virtual void RouteGameInfo() = 0;						///< Ŭ���̾�Ʈ�� ���� ���� �����ش�.
	virtual void RouteStageGameInfo() = 0;					///< ����� ������������ �ٲ� ���� ������ �����ش�.

	/// NPC�� ������Ų��.
	/// @param nNPC			NPC ����
	/// @param nPosIndex	���� ��ġ
	CCMatchNPCObject* SpawnNPC(CCQUEST_NPC nNPC, int nPosIndex, bool bKeyNPC=false);

	virtual void OnCompleted();					///< ����Ʈ ������ ȣ��ȴ�.
	virtual void OnFailed();					///< ����Ʈ ���н� ȣ��ȴ�.
	virtual void RouteCompleted() = 0;			///< ����Ʈ ���� �޽����� ������. - ��������� �Բ� ������.
	virtual void RouteFailed() = 0;				///< ����Ʈ ���� �޽��� ������.
	virtual void DistributeReward() = 0;		///< ����Ʈ ������ ������ ���

	void ReAssignNPC();
	void SendClientLatencyPing();
public:
	// Ŀ�ǵ� ó�� ���� �Լ�

	/// NPC�� �׿����� ȣ��
	/// @param uidSender		�޼��� ���� �÷��̾�
	/// @param uidKiller		���� �÷��̾�
	/// @param uidNPC			���� NPC
	/// @param pos				NPC ��ġ
	virtual void OnRequestNPCDead(CCUID& uidSender, CCUID& uidKiller, CCUID& uidNPC, CCVector& pos);

	/// �÷��̾� �׾��� �� ȣ��
	/// @param uidVictim		���� �÷��̾� UID
	virtual void OnRequestPlayerDead(const CCUID& uidVictim);
	virtual void OnRequestTestNPCSpawn(int nNPCType, int nNPCCount);
	virtual void OnRequestTestClearNPC();

	/// ��� �÷��̾� ��Ȱ �� HP, AP �ʱ�ȭ
	void RefreshPlayerStatus();
	/// ��� NPC�� �ʱ�ȭ
	void ClearAllNPC();

	/// �÷��̾ ��Ż�� �̵����� ��� ȣ��ȴ�.
	/// @param uidPlayer			�̵��� �÷��̾� UID
	virtual void OnRequestMovetoPortal(const CCUID& uidPlayer) = 0;
	/// ��Ż�� �̵��ϰ� ���� �̵��� �Ϸ�Ǿ��� ��� ȣ��ȴ�.
	/// @param uidPlayer			�÷��̾� UID
	virtual void OnReadyToNewSector(const CCUID& uidPlayer) = 0;

	virtual void OnRequestTestSectorClear() = 0;
	virtual void OnRequestTestFinish() = 0;


	// ����Ʈ������ �߰��� ���� �������̽�.
	// ���������� ���� ������ �غ�����. �ʹ� ���� �������̽��� CCMatchRule���� �ö�� ������... - by �߱���.
	virtual void OnRequestDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnResponseDropSacrificeItemOnSlot( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnRequestCallbackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID ) {}
	virtual void OnResponseCallBackSacrificeItem( const CCUID& uidSender, const int nSlotIndex, const unsigned long int nItemID )	{}
	virtual void OnRequestQL( const CCUID& uidSender ) {}
	virtual void OnResponseQL_ToStage( const CCUID& uidStage )	{}
	virtual void OnRequestSacrificeSlotInfo( const CCUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToListener( const CCUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToStage( const CCUID& uidStage )	{}
	virtual void OnChangeCondition() {}

	virtual bool							PrepareStart() { return true; }
	virtual void							PreProcessLeaveStage( const CCUID& uidLeaverUID );


	// ���� ���̺� �߰� �������̽�.
	virtual void CheckMonsterBible( const CCUID& uidUser, const int nMonsterBibleIndex );
	virtual void PostNewMonsterInfo( const CCUID& uidUser, const char nMonIndex );
	
public:
	CCMatchRuleBaseQuest(CCMatchStage* pStage);			///< ������
	virtual ~CCMatchRuleBaseQuest();						///< �Ҹ���
	virtual void OnEnterBattle(CCUID& uidChar);			///< ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnLeaveBattle(CCUID& uidChar);			///< ������ �������� ȣ��ȴ�.

	virtual void RefreshStageGameInfo() = 0;
};


void InsertNPCIDonUnique( vector<CCQUEST_NPC>& outNPCList, CCQUEST_NPC nNPCID );
void MakeJacoNPCList( vector<CCQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps );
void MakeSurvivalKeyNPCList( vector<CCQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps );
void MakeNomalNPCList( vector<CCQUEST_NPC>& outNPCList, CCQuestScenarioInfoMaps& ScenarioInfoMaps, CCMatchQuest* pQuest );
void CopyCCTD_NPCINFO( CCTD_NPCINFO* pDest, const CCQuestNPCInfo* pSource );
