#pragma once
#include "CCMatchGlobal.h"
#include <vector>
using namespace std;

class CCMatchObject;
class MLadderGroup;
class CCMatchStage;

class MLadderGameStrategy;
class MClanGameStrategy;
struct CCMatchLadderTeamInfo;

class CCBaseTeamGameStrategy
{
protected:
	CCBaseTeamGameStrategy() { }
	virtual ~CCBaseTeamGameStrategy() { }
public:
	/// ���� �������� üũ�Ѵ�.
	virtual int ValidateChallenge(CCMatchObject** ppMemberObject, int nMemberCount) = 0;

	/// �������ڰ� �ٸ���������� ������ �� �ִ��� üũ�Ѵ�.
	virtual int ValidateRequestInviteProposal(CCMatchObject* pProposerObject, CCMatchObject** ppReplierObjects,
					const int nReplierCount) = 0;
	/// ���ο� LadderGroup ID�� �����ؼ� ��ȯ�Ѵ�.
	virtual int GetNewGroupID(CCMatchObject* pLeaderObject, CCMatchObject** ppMemberObjects, int nMemberCount) = 0;

	/// LadderGroup�� �ʿ��� ������ �����Ѵ�. ID����..
	virtual void SetLadderGroup(MLadderGroup* pGroup, CCMatchObject** ppMemberObjects, int nMemberCount) = 0;

	/// Stage���� �ʿ��� LadderInfo�� �����Ѵ�.
	virtual void SetStageLadderInfo(CCMatchLadderTeamInfo* poutRedLadderInfo, CCMatchLadderTeamInfo* poutBlueLadderInfo,
									MLadderGroup* pRedGroup, MLadderGroup* pBlueGroup) = 0;

	/// ������ �������� ����� DB �����Ѵ�.
	virtual void SavePointOnFinishGame(CCMatchStage* pStage, CCMatchTeam nWinnerTeam, bool bIsDrawGame,
		                               CCMatchLadderTeamInfo* pRedLadderInfo, CCMatchLadderTeamInfo* pBlueLadderInfo) = 0;

	virtual int GetRandomMap(int nTeamMember) = 0;

	/// ������忡 ���� ������ �ڽ� Ŭ������ ��ȯ�Ѵ�. CSM_LADDER, CSM_CLAN�� ����
	static CCBaseTeamGameStrategy* GetInstance(CCMatchServerMode nServerMode);
};


class MLadderGameStrategy : public CCBaseTeamGameStrategy
{
protected:
	MLadderGameStrategy() { }
public:
	static MLadderGameStrategy* GetInstance()
	{
		static MLadderGameStrategy m_stInstance;
		return &m_stInstance;
	}
	virtual int ValidateChallenge(CCMatchObject** ppMemberObject, int nMemberCount);
	virtual int ValidateRequestInviteProposal(CCMatchObject* pProposerObject, CCMatchObject** ppReplierObjects,
					const int nReplierCount);
	virtual int GetNewGroupID(CCMatchObject* pLeaderObject, CCMatchObject** ppMemberObjects, int nMemberCount);
	virtual void SetLadderGroup(MLadderGroup* pGroup, CCMatchObject** ppMemberObjects, int nMemberCount) { }
	virtual void SetStageLadderInfo(CCMatchLadderTeamInfo* poutRedLadderInfo, CCMatchLadderTeamInfo* poutBlueLadderInfo,
									MLadderGroup* pRedGroup, MLadderGroup* pBlueGroup);
	virtual void SavePointOnFinishGame(CCMatchStage* pStage, CCMatchTeam nWinnerTeam, bool bIsDrawGame,
		                               CCMatchLadderTeamInfo* pRedLadderInfo, CCMatchLadderTeamInfo* pBlueLadderInfo);
	virtual int GetRandomMap(int nTeamMember);
};


class MClanGameStrategy : public CCBaseTeamGameStrategy
{
protected:
	MClanGameStrategy();
	vector<int>		m_RandomMapVec[CCLADDERTYPE_MAX];
public:
	static MClanGameStrategy* GetInstance()
	{
		static MClanGameStrategy m_stInstance;
		return &m_stInstance;
	}

	virtual int ValidateChallenge(CCMatchObject** ppMemberObject, int nMemberCount);
	virtual int ValidateRequestInviteProposal(CCMatchObject* pProposerObject, CCMatchObject** ppReplierObjects,
					const int nReplierCount);
	virtual int GetNewGroupID(CCMatchObject* pLeaderObject, CCMatchObject** ppMemberObjects, int nMemberCount);
	virtual void SetLadderGroup(MLadderGroup* pGroup, CCMatchObject** ppMemberObjects, int nMemberCount);
	virtual void SetStageLadderInfo(CCMatchLadderTeamInfo* poutRedLadderInfo, CCMatchLadderTeamInfo* poutBlueLadderInfo,
									MLadderGroup* pRedGroup, MLadderGroup* pBlueGroup);
	virtual void SavePointOnFinishGame(CCMatchStage* pStage, CCMatchTeam nWinnerTeam, bool bIsDrawGame,
		                               CCMatchLadderTeamInfo* pRedLadderInfo, CCMatchLadderTeamInfo* pBlueLadderInfo);
	virtual int GetRandomMap(int nTeamMember);
};
