#ifndef _ZMATCH_H
#define _ZMATCH_H

#include <list>
using namespace std;

#include "ZPrerequisites.h"
#include "CCUID.h"
#include "CCMatchStage.h"

class ZCharacter;
class CCMatchStageSetting;
class ZRule;

/// Ŭ���̾�Ʈ ���� �� ���� Ŭ����
class ZMatch
{
private:
protected:
	ZRule*					m_pRule;
	CCMatchStageSetting*		m_pStageSetting;
	CCMATCH_ROUNDSTATE		m_nRoundState;
	int						m_nCurrRound;
	int						m_nTeamScore[MMT_END];
	int						m_nRoundKills;			// ��������� ų�� - FirstKillüũ�Ϸ��� ����
	int						m_nTeamKillCount[MMT_END]; // �� ���� ų �� ����

	unsigned long			m_nNowTime;
	unsigned long			m_nStartTime;
	unsigned long int		m_nLastDeadTime;
	int						m_nSoloSpawnTime;

	DWORD					m_dwStartTime;			// ���带 �������� ���� �ð���

	void SoloSpawn();				///< �� ĳ���� ����
	void InitCharactersProperties();	///< ĳ���� HP, �Ѿ� �� �ʱ�ȭ
	void InitRound();				///< ���� ó�� ���۽� �ʱ�ȭ
	void ProcessRespawn();
public:
	ZMatch();
	virtual ~ZMatch();
	bool Create();
	void Destroy();

	void Update(float fDelta);
	bool OnCommand(CCCommand* pCommand);
	void OnResponseRuleInfo(CCTD_RuleInfo* pInfo);
	void SetRound(int nCurrRound);						///< ���� ���� ����
	void OnForcedEntry(ZCharacter* pCharacter);			///< �������� ���
	void InitCharactersPosition();						///< ���� �����Ҷ� ĳ���� ��ġ ����
	void OnDrawGameMessage();
	void RespawnSolo();

	int GetRemainedSpawnTime();
	int GetRoundCount(); 
	int GetRoundReadyCount(void);
	void GetTeamAliveCount(int* pnRedTeam, int* pnBlueTeam);
	const char* GetTeamName(int nTeamID);
	void SetRoundState(CCMATCH_ROUNDSTATE nRoundState, int nArg=0);

	void SetRoundStartTime( void);
	DWORD GetRemaindTime( void);

	inline int GetCurrRound();
	inline bool IsTeamPlay();
	inline bool IsWaitForRoundEnd();
	inline bool IsQuestDrived();
	inline CCMATCH_ROUNDSTATE GetRoundState();
	inline CCMATCH_GAMETYPE GetMatchType();
	inline bool GetTeamKillEnabled();
	inline const char* GetMapName();
	inline int GetTeamScore(CCMatchTeam nTeam);
	inline void SetTeamScore(CCMatchTeam nTeam, int nScore);
	inline int GetTeamKills(CCMatchTeam nTeam);		// ���� ����ų��
	inline void AddTeamKills(CCMatchTeam nTeam, int amount = 1);
	inline void SetTeamKills(CCMatchTeam nTeam, int amount);
	inline int GetRoundKills();
	inline void AddRoundKills();
	inline bool IsRuleGladiator();
	inline ZRule* GetRule();
};



#define DEFAULT_ONETURN_GAMETIME		300000		// ���� �⺻ ���ð� = 5��
#define DEFAULT_WAITTIME				120000		// �⺻ ��� �ð� = 2��
#define DEFAULT_READY_TIME				5000		// �غ� �ð�


// inline func ////////////////////////////////////////////////////////////////////////
inline int ZMatch::GetCurrRound() 
{ 
	return m_nCurrRound; 
}
inline bool ZMatch::IsTeamPlay() 
{ 
	return m_pStageSetting->IsTeamPlay(); 
}

inline bool ZMatch::IsWaitForRoundEnd() 
{ 
	return m_pStageSetting->IsWaitforRoundEnd(); 
}

inline CCMATCH_ROUNDSTATE ZMatch::GetRoundState()
{ 
	return m_nRoundState; 
}
inline CCMATCH_GAMETYPE ZMatch::GetMatchType()
{ 
	return m_pStageSetting->GetStageSetting()->nGameType; 
}
inline bool ZMatch::GetTeamKillEnabled()
{ 
	return m_pStageSetting->GetStageSetting()->bTeamKillEnabled; 
}
inline const char* ZMatch::GetMapName()
{
	return m_pStageSetting->GetStageSetting()->szMapName;
}
inline int ZMatch::GetTeamScore(CCMatchTeam nTeam)
{ 
	return m_nTeamScore[nTeam]; 
}

inline int ZMatch::GetTeamKills(CCMatchTeam nTeam)
{ 
	return m_nTeamKillCount[nTeam]; 
}

inline void ZMatch::AddTeamKills(CCMatchTeam nTeam, int amount)
{ 
	m_nTeamKillCount[nTeam]+=amount; 
}

inline void ZMatch::SetTeamKills(CCMatchTeam nTeam, int amount)
{ 
	m_nTeamKillCount[nTeam]=amount; 
}

inline void ZMatch::SetTeamScore(CCMatchTeam nTeam, int nScore)
{ 
	m_nTeamScore[nTeam] = nScore; 
}
inline int ZMatch::GetRoundKills()
{ 
	return m_nRoundKills; 
}
inline void ZMatch::AddRoundKills()
{ 
	m_nRoundKills++; 
}
inline bool ZMatch::IsRuleGladiator()
{
	return ((GetMatchType() == CCMATCH_GAMETYPE_GLADIATOR_SOLO) || 
			(GetMatchType() == CCMATCH_GAMETYPE_GLADIATOR_TEAM));
}
inline ZRule* ZMatch::GetRule() 
{ 
	return m_pRule; 
}

inline bool ZMatch::IsQuestDrived()
{
	return m_pStageSetting->IsQuestDrived();
}

#endif