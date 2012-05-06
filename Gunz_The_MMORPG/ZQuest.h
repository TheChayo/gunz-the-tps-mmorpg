#pragma once

#include "ZBaseQuest.h"

//bool ZIsLaunchDevelop();
// ����Ʈ ���� ���� Ŭ����
class ZQuest : public ZBaseQuest
{
private:
	set<CCUID>	m_CharactersGone;	// �������ͷ� �̵��� ĳ���͵�

	ZQuestGameInfo		m_GameInfo;
	bool	m_Cheet[ZQUEST_CHEET_MAX];
	bool	m_bCreatedOnce;
	bool	m_bLoaded;
	bool	m_bIsQuestComplete;
	bool	m_bIsRoundClear;
	DWORD	m_tRemainedTime;					// ���尡 ������ ���� ����� �Ѿ����� ���� �ð�
	float	m_fLastWeightTime;

	CCQuestCombatState	m_QuestCombatState;

	ZNPCInfoFromServerManager m_NPCInfoFromServerMgr;


#ifdef _QUEST_ITEM
	int				m_nRewardXP;				// ����Ʈ���� ȹ���� ����ġ.
	int				m_nRewardBP;				// ����Ʈ���� ȹ���� �ٿ�Ƽ.
	
    virtual bool OnRewardQuest( CCCommand* pCmd );
	virtual bool OnNewMonsterInfo( CCCommand* pCmd );	// ���� �𰨿� ��ϵ� ���� ������ ���� ����.

	virtual void GetMyObtainQuestItemList( int nRewardXP, int nRewardBP, void* pMyObtainQuestItemListBlob, void* pMyObtainZItemListBlob );

public :
	virtual int GetRewardXP( void)							{ return m_nRewardXP; }
	virtual int GetRewardBP( void)							{ return m_nRewardBP; }
	virtual bool IsQuestComplete( void)						{ return m_bIsQuestComplete; }
	virtual bool IsRoundClear( void)						{ return m_bIsRoundClear; }
	virtual DWORD GetRemainedTime( void)					{ return m_tRemainedTime; }

	virtual CCQuestCombatState GetQuestState()				{ return m_QuestCombatState; }

	virtual ZNPCInfoFromServerManager& GetNPCInfoFromServerMgr() { return m_NPCInfoFromServerMgr; }

#endif

	virtual bool OnNPCSpawn(CCCommand* pCommand);
	virtual bool OnNPCDead(CCCommand* pCommand);
	virtual bool OnPeerNPCDead(CCCommand* pCommand);
	virtual bool OnEntrustNPCControl(CCCommand* pCommand);
	virtual bool OnPeerNPCBasicInfo(CCCommand* pCommand);
	virtual bool OnPeerNPCHPInfo(CCCommand* pCommand);
	virtual bool OnPeerNPCAttackMelee(CCCommand* pCommand);
	virtual bool OnPeerNPCAttackRange(CCCommand* pCommand);
	virtual bool OnPeerNPCSkillStart(CCCommand* pCommand);
	virtual bool OnPeerNPCSkillExecute(CCCommand* pCommand);
	virtual bool OnPeerNPCBossHpAp(CCCommand* pCommand);
	virtual bool OnRefreshPlayerStatus(CCCommand* pCommand);
	virtual bool OnClearAllNPC(CCCommand* pCommand);
	virtual bool OnQuestRoundStart(CCCommand* pCommand);
	virtual bool OnQuestPlayerDead(CCCommand* pCommand);
	virtual bool OnQuestGameInfo(CCCommand* pCommand);
	virtual bool OnQuestCombatState(CCCommand* pCommand);
	virtual bool OnMovetoPortal(CCCommand* pCommand);
	virtual bool OnReadyToNewSector(CCCommand* pCommand);
	virtual bool OnSectorStart(CCCommand* pCommand);
	virtual bool OnObtainQuestItem(CCCommand* pCommand);
	virtual bool OnObtainZItem(CCCommand* pCommand);
	virtual bool OnSectorBonus(CCCommand* pCommand);
	virtual bool OnQuestCompleted(CCCommand* pCommand);
	virtual bool OnQuestFailed(CCCommand* pCommand);
	virtual bool OnQuestPing(CCCommand* pCommand);


	//ZQuestMap			m_Map;
	virtual void LoadNPCMeshes();
	virtual void LoadNPCSounds();
	virtual void MoveToNextSector();
	virtual void UpdateNavMeshWeight(float fDelta);
protected:
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual bool OnCreateOnce();
	virtual void OnDestroyOnce();
public:
	ZQuest();
	virtual ~ZQuest();
public:
	virtual void OnGameCreate();
	virtual void OnGameDestroy();
	virtual void OnGameUpdate(float fElapsed);
	virtual bool OnCommand(CCCommand* pCommand);				///< ���� �̿ܿ� ������� Ŀ�ǵ� ó��
	virtual bool OnGameCommand(CCCommand* pCommand);			///< ������ ������� Ŀ�ǵ� ó��

	virtual void SetCheet(ZQuestCheetType nCheetType, bool bValue);
	virtual bool GetCheet(ZQuestCheetType nCheetType);

	virtual void Reload();
	virtual bool Load();

	
	// interface
	virtual ZQuestGameInfo* GetGameInfo()		{ return &m_GameInfo; }

	// ���¿� ������� ���ɼ� �ִ� ����Ʈ ���õ� Ŀ�ǵ�.
	virtual bool OnSetMonsterBibleInfo( CCCommand* pCmd );


	virtual bool OnPrePeerNPCAttackMelee(CCCommand* pCommand);	// ������ ó���ϴ°� ��Ÿ�̹� �ʴ�
	
};




/////////////////////////////////////////////////////////////////////

inline void ZQuest::SetCheet(ZQuestCheetType nCheetType, bool bValue) 
{ 
	m_Cheet[nCheetType] = bValue; 
}

inline bool ZQuest::GetCheet(ZQuestCheetType nCheetType) 
{ 
	if (!ZIsLaunchDevelop()) return false;
	return m_Cheet[nCheetType];
}