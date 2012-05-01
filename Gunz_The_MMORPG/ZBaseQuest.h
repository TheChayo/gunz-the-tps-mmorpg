#ifndef _ZBASEQUEST_H
#define _ZBASEQUEST_H

#include "ZGlobal.h"
#include "CCBaseQuest.h"
#include "ZQuestMap.h"
#include "ZQuestGameInfo.h"
#include "ZMyItemList.h"
#include "ZNPCInfoFromServer.h"



#include <set>

enum ZQuestCheetType
{
	ZQUEST_CHEET_GOD			= 0,		// �������
	ZQUEST_CHEET_WEAKNPCS		= 1,		// �������� 1
	ZQUEST_CHEET_AMMO			= 2,		// �Ѿ˸���
	ZQUEST_CHEET_MAX
};

// ����Ʈ�� �����̹��� ���� �������̽�

// ���� ����Ʈ���� ������ �����̹���带 �߰��ϸ鼭, ����Ʈ�� Ŀ�ǵ峪 npc�ڵ� ���� ������ �� �������� �̰͵���
// ZGetQuest()�� �ʹ� ��������� ����ϰ� �־ �����̹��� �߰��Ϸ��� ū �Ը��� �����丵�� �Ͼ �� �ۿ� ����.
// �׷��� �̹� �� ���� �ǰ� �ִ� ����Ʈ �ڵ带 �����丵�ϴ� ���� ���� �߻� ���� ������ �ſ� �δ㽺����.
// �׷��� �����丵 ��� �����̹������� ����Ʈ �ڵ带 �ѹ� �� �����ؼ� �װ��� �����̹��� �°� �����ϱ�� �Ͽ���
// ���� �ߺ� �ڵ尡 ��������� ������ �� �����ϴٰ� �Ǵ���..
// ������,
// Ŭ���̾�Ʈ �ڵ��� ���� �κ��� ZGetQuest()�� ����ϰ� ������ ��ġ �̰��� �̱����̳� ���������� ���� ���ۿ��� ���Ҵ�.
// ����Ʈ �ڵ尡 �ִ� ���� ������ �ǵ帮�� �������� �Ͽ����� ������ ZGetQuest()����ϴ� ���� ��� ������ ������ �� ���
// ::ZGetQuest()�� ����Ÿ���� ZSurvival�ϼ��� �ֵ��� �ϱ� ���ؼ� ���� ZQuest���� ������ �Ʒ��� �������̽��� �߰��Ͽ���.
// ����Ʈ �ڵ带 ������ �ǵ帮�� �ʱ� ���ؼ� ����Ʈ�� �����̹��� �������� ���� �� �ִ� �ڵ尡 �ִ��� ����� �ű��� �ʾƼ�
// ��� �Լ��� ���������Լ��̴�.

class ZBaseQuest : public CCBaseQuest
{
#ifdef _QUEST_ITEM
    virtual bool OnRewardQuest( CCCommand* pCmd ) = 0;
	virtual bool OnNewMonsterInfo( CCCommand* pCmd ) = 0;

	virtual void GetMyObtainQuestItemList( int nRewardXP, int nRewardBP, void* pMyObtainQuestItemListBlob, void* pMyObtainZItemListBlob ) = 0;

public :
	virtual int GetRewardXP( void) = 0;
	virtual int GetRewardBP( void) = 0;
	virtual bool IsQuestComplete( void) = 0;
	virtual bool IsRoundClear( void) = 0;
	virtual DWORD GetRemainedTime( void) = 0;

	virtual CCQuestCombatState GetQuestState() = 0;

	virtual ZNPCInfoFromServerManager& GetNPCInfoFromServerMgr() = 0;

#endif

	virtual bool OnNPCSpawn(CCCommand* pCommand) = 0;
	virtual bool OnNPCDead(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCDead(CCCommand* pCommand) = 0;
	virtual bool OnEntrustNPCControl(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCBasicInfo(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCHPInfo(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCAttackMelee(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCAttackRange(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCSkillStart(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCSkillExecute(CCCommand* pCommand) = 0;
	virtual bool OnPeerNPCBossHpAp(CCCommand* pCommand) = 0;
	virtual bool OnRefreshPlayerStatus(CCCommand* pCommand) = 0;
	virtual bool OnClearAllNPC(CCCommand* pCommand) = 0;
	virtual bool OnQuestRoundStart(CCCommand* pCommand) = 0;
	virtual bool OnQuestPlayerDead(CCCommand* pCommand) = 0;
	virtual bool OnQuestGameInfo(CCCommand* pCommand) = 0;
	virtual bool OnQuestCombatState(CCCommand* pCommand) = 0;
	virtual bool OnMovetoPortal(CCCommand* pCommand) = 0;
	virtual bool OnReadyToNewSector(CCCommand* pCommand) = 0;
	virtual bool OnSectorStart(CCCommand* pCommand) = 0;
	virtual bool OnObtainQuestItem(CCCommand* pCommand) = 0;
	virtual bool OnObtainZItem(CCCommand* pCommand) = 0;
	virtual bool OnSectorBonus(CCCommand* pCommand) = 0;
	virtual bool OnQuestCompleted(CCCommand* pCommand) = 0;
	virtual bool OnQuestFailed(CCCommand* pCommand) = 0;
	virtual bool OnQuestPing(CCCommand* pCommand) = 0;


	//ZQuestMap			m_Map;
	virtual void LoadNPCMeshes() = 0;
	virtual void LoadNPCSounds() = 0;
	virtual void MoveToNextSector() = 0;
	virtual void UpdateNavMeshWeight(float fDelta) = 0;
protected:
	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual bool OnCreateOnce() = 0;
	virtual void OnDestroyOnce() = 0;
public:
	ZBaseQuest() {}
	virtual ~ZBaseQuest() {}
public:
	virtual void OnGameCreate() = 0;
	virtual void OnGameDestroy() = 0;
	virtual void OnGameUpdate(float fElapsed) = 0;
	virtual bool OnCommand(CCCommand* pCommand) = 0;				///< ���� �̿ܿ� ������� Ŀ�ǵ� ó��
	virtual bool OnGameCommand(CCCommand* pCommand) = 0;			///< ������ ������� Ŀ�ǵ� ó��

	virtual void SetCheet(ZQuestCheetType nCheetType, bool bValue) = 0;
	virtual bool GetCheet(ZQuestCheetType nCheetType) = 0;

	virtual void Reload() = 0;
	virtual bool Load() = 0;

	
	// interface
	virtual ZQuestGameInfo* GetGameInfo() = 0;

	// ���¿� ������� ���ɼ� �ִ� ����Ʈ ���õ� Ŀ�ǵ�.
	virtual bool OnSetMonsterBibleInfo( CCCommand* pCmd ) = 0;


	virtual bool OnPrePeerNPCAttackMelee(CCCommand* pCommand) = 0;	// ������ ó���ϴ°� ��Ÿ�̹� �ʴ�
	
};


#endif