#ifndef _MMATCHRULE_DUEL_H
#define _MMATCHRULE_DUEL_H


#include "CCMatchRule.h"
#include <list>

using namespace std;

class CCMatchRuleDuel : public CCMatchRule {
private:
	list<CCUID>		WaitQueue;										///< ��⿭ ť
	bool			isChangeChampion;								///< ���ڰ� �ٲ��°�
	bool			isRoundEnd;										///< ���� �����°�
	bool			isTimeover;										///< Ÿ�ӿ��� �ƴ°�
	int				nVictory;										///< ���¼�
protected:	
	virtual bool RoundCount();										///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.

	virtual void OnBegin();											///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();											///< ��ü ���� ����� ȣ��
	virtual void OnRoundBegin();									///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();										/// ���� ���� �� ȣ��
	virtual void OnRoundTimeOut();									///< ���尡 Ÿ�Ӿƿ����� ����� �� OnRoundEnd() ���̴�.

	virtual bool OnCheckRoundFinish();								///< ���尡 �������� üũ

	virtual void OnGameKill(const CCUID& uidAttacker, const CCUID& uidVictim);	///< ų������ �������� ų���� è�Ǿ��� ų���� üũ

	virtual bool OnCheckEnableBattleCondition();					///< ���� �������� üũ

	/// ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnEnterBattle(CCUID& uidChar);
	// ������ �������� ȣ��ȴ�.
	virtual void OnLeaveBattle(CCUID& uidChar);		

	void		 SpawnPlayers();									///< �÷��̾���� ������Ų��.

	void		LogInfo();

	void		SendQueueInfo(bool isRoundEnd = false);				///< �÷��̾�鿡�� ť ������ ����

public:
	int				GetVictory() { return nVictory;	}					///< ���¼� ����
	CCUID			uidChampion;									///< 1������ ���
	CCUID			uidChallenger;									///< ������

	CCMatchRuleDuel(CCMatchStage* pStage);
	virtual ~CCMatchRuleDuel() { }
	virtual CCMATCH_GAMETYPE GetGameType() { return CCMATCH_GAMETYPE_DUEL; }
};


#endif