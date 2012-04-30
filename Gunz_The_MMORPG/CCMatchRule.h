#ifndef _MMATCHRULE_H
#define _MMATCHRULE_H

#include "CCMatchItem.h"
#include "CCMatchTransDataType.h"
#include "CCUID.h"
#include "CCMatchGameType.h"
#include "MQuestPlayer.h"
#include "CCMatchEventManager.h"

class CCMatchObject;
class CCMatchStage;


/// ���� ���� ����
enum MMATCH_ROUNDSTATE {
	MMATCH_ROUNDSTATE_PREPARE		= 0,			///< �غ� �ܰ�
	MMATCH_ROUNDSTATE_COUNTDOWN		= 1,			///< �÷��� ī��Ʈ 
	MMATCH_ROUNDSTATE_PLAY			= 2,			///< ������
	MMATCH_ROUNDSTATE_FINISH		= 3,			///< ����
	MMATCH_ROUNDSTATE_EXIT			= 4,			///< �������� ����
	MMATCH_ROUNDSTATE_FREE			= 5,			///< �������࿡ �ʿ��� ����� �������� ���� ������� ����
	MMATCH_ROUNDSTATE_FAILED		= 6,			///< ����Ŭ���� ����.
	MMATCH_ROUNDSTATE_PRE_COUNTDOWN = 7,			///< �÷��� ���� ī��Ʈ �ٿ�(�ϴ�, �����ʸ�Ʈ���� ���)

	MMATCH_ROUNDSTATE_END	// Not using, just a END mark
};

enum MMATCH_ROUNDRESULT {
	MMATCH_ROUNDRESULT_DRAW = 0,
	MMATCH_ROUNDRESULT_REDWON,
	MMATCH_ROUNDRESULT_BLUEWON,
	MMATCH_ROUNDRESULT_RED_ALL_OUT,   // Ŭ����忡�� �̱�� �ִ����� �� �����������
	MMATCH_ROUNDRESULT_BLUE_ALL_OUT,
	MMATCH_ROUNDRESULT_END
};


///////////////////////////////////////////////////////////////////////////////////////////////
/// ���� �� �⺻ ���̽� Ŭ����
class CCMatchRule {
protected:
	// CCMatchGameTypeInfo*	m_pGameTypeInfo;							///< ���� Ÿ�� ����

	CCMatchStage*		m_pStage;									///< �������� Ŭ����
	MMATCH_ROUNDSTATE	m_nRoundState;								///< ���� ���� ����
	int					m_nRoundCount;								///< ���� ��
	int					m_nRoundArg;								///< ������ �߰� ����
	unsigned long		m_tmRoundStateTimer;
	int					m_nLastTimeLimitAnnounce;					// 60, 30, 10 �� �ϳ�

	CCMatchEventManager m_OnBeginEventManager;						/// ������ �����Ҷ� �̺�Ʈ.
	CCMatchEventManager m_OnGameEventManager;						/// ��Ʋ�� �̺�Ʈ.
	CCMatchEventManager m_OnEndEventManager;							/// ������ ������ �̺�Ʈ.

protected:
	virtual bool RoundCount() { return false; }						///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.
	virtual bool OnRun();											///< ����ƽ�� ȣ��
	virtual void OnBegin();											///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();											///< ��ü ���� ����� ȣ��
	virtual void OnRoundBegin();									///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();										///< ���� ���� �� ȣ��
	virtual void OnRoundTimeOut();									///< ���尡 Ÿ�Ӿƿ����� ����� �� OnRoundEnd() ���̴�.

	virtual bool OnCheckRoundFinish() = 0;							///< ���尡 �������� üũ
	virtual bool OnCheckEnableBattleCondition() { return true; }	///< ���� �������� üũ
	virtual bool OnCheckBattleTimeOut(unsigned int tmTimeSpend);	///< ���� Ÿ�Ӿƿ����� üũ

	void SetRoundStateTimer(unsigned long tmTime)	{ m_tmRoundStateTimer = tmTime; }
	void InitRound();												///< ���ο� ���� �ʱ�ȭ
	void SetRoundState(MMATCH_ROUNDSTATE nState);					///< ���� ���� ����

	void InitOnBeginEventManager();
	void InitOnGameEventManager();
	void InitOnEndEventManager();

	void CheckOnBeginEvent();
	void CheckOnGameEvent();
	void CheckOnEndEvent();

	void RunOnBeginEvent();
	void RunOnGameEvent();
	void RunOnEndEvent();
public:
	CCMatchRule()					{ _ASSERT(false); }				///< �� �����ڴ� ���� ������� �ʴ´�.
	CCMatchRule(CCMatchStage* pStage);								///< ������
	virtual ~CCMatchRule()			{}								///< �Ҹ���
	CCMatchStage* GetStage()			{ return m_pStage; }			///< �������� ��ȯ

	int GetRoundCount()				{ return m_nRoundCount; }		///< �� ���� �� ��ȯ
	void SetRoundCount(int nRound)	{ m_nRoundCount = nRound; }		///< �� ���� �� ����
	int GetRoundArg()				{ return m_nRoundArg; }			///< ���� ���� ��ȯ
	void SetRoundArg(int nArg)		{ m_nRoundArg = nArg; }			///< ���� ���� ����

	CCMatchEventManager& GetOnBeginEventManager()	{ return m_OnBeginEventManager; }
	CCMatchEventManager& GetOnGameEventManager()		{ return m_OnGameEventManager; }
	CCMatchEventManager& GetOnEndEventManager()		{ return m_OnEndEventManager; }

	MMATCH_ROUNDSTATE GetRoundState()	{ return m_nRoundState; }				///< ���� ���� ��ȯ
	unsigned long GetRoundStateTimer()	{ return m_tmRoundStateTimer; }
	unsigned long GetLastTimeLimitAnnounce()	{ return m_nLastTimeLimitAnnounce; }
	void SetLastTimeLimitAnnounce(int nSeconds)	{ m_nLastTimeLimitAnnounce = nSeconds; }

	virtual void* CreateRuleInfoBlob()		{ return NULL; }

	/// �� ���ʽ� ���
	/// @param pAttacker		������
	/// @param pVictim			������
	/// @param nSrcExp			���� ����ġ
	/// @param poutAttackerExp	�����ڰ� ���� ����ġ
	/// @param poutTeamExp		���� ���� ����ġ
	virtual void CalcTeamBonus(CCMatchObject* pAttacker,
		                       CCMatchObject* pVictim,
							   int nSrcExp,
							   int* poutAttackerExp,
							   int* poutTeamExp);
	/// ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnEnterBattle(CCUID& uidChar) {}		
	// ������ �������� ȣ��ȴ�.
	virtual void OnLeaveBattle(CCUID& uidChar) {}		
	/// �ش�꿡���� ����ϴ� Ŀ�ǵ�� ���� ó���Ѵ�.
	virtual void OnCommand(MCommand* pCommand) {}		
	/// ��������� �Ծ��� ��� ȣ��ȴ�.
	virtual void OnObtainWorldItem(CCMatchObject* pObj, int nItemID, int* pnExtraValues) {}
	/// Kill�� ȣ��
	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim) {}

	
	bool Run();														///< ����ƽ
	void Begin();													///< ����
	void End();														///< ��

	void DebugTest();												///< ����� �׽�Ʈ

	virtual bool CheckPlayersAlive() { return true; }
	virtual void OnFailed() {}
	virtual MMATCH_GAMETYPE GetGameType() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////








inline bool IsGameRuleDeathMatch(MMATCH_GAMETYPE nGameType)
{
	return (
		(nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) || 
		(nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_TRAINING)
	);
}
inline bool IsGameRuleGladiator(MMATCH_GAMETYPE nGameType)
{
	return ((nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) || 
			(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM));
}

#endif