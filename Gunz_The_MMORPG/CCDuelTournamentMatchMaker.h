#include "MDuelTournamentGroup.h"

//////////////////////////////////////////////////////////////////////
// Duel Tournament�� Match-Up�� �����ִ� MatchMaker Ŭ�����̴�.
// 
// �⺻������ TP�� ���ؼ� ��� ���� �̳��̸� ��ġ�� ��Ű��,
// ����� TP�� Ÿ ������ ������ �׷� ������ ��Ÿ�� ������ ��ٸ���
// ��� �ð��� ���� �ð��̻� ������� �׸� ��ٸ��� ������ ����� TP�� ������ ��ġ�ȴ�.
//////////////////////////////////////////////////////////////////////

// �����׽�Ʈ �ۼ��� ���� ���� Ŭ���� : MatchObject���� ������ �������� �뵵
class CCMatchObjectContainer
{
public:
	// playerUID -> CCMatchObject -> DuelTournamentCharInfo ������ ������ ������ ����
	virtual CCMatchObjectDuelTournamentCharInfo* GetDuelTournamentCharInfo(const MUID& uid) { 
		CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject(uid);
		return pObj ? pObj->GetDuelTournamentCharInfo() : NULL;
	}

	// uidPlayer�� ����Ű�� MatchObject�� ��ȿ���� �˻��ϴ� ������ ����
	virtual bool IsEnabledUid(const MUID& uid) {
		CCMatchObject* pObj = CCMatchServer::GetInstance()->GetObject(uid);
		return IsEnabledObject(pObj);
	}
};

class MDuelTournamentMatchMaker
{
public:

	struct DTUser {
		MUID uid;	// player uid
		int tp;		// tournament point

		DTUser() : uid(0,0), tp(0) {}
		DTUser(const MUID& uid_, int tp_) : uid(uid_), tp(tp_) {}

		bool operator <(const DTUser &o) const {
			if (tp < o.tp) return true;
			if (tp == o.tp && uid < o.uid) return true;
			return false;
		}
	};

protected:
	CCMatchObjectContainer* m_pMatchObjectContainer;

	typedef map<DTUser, int>	MapDTUser;
	typedef MapDTUser::iterator	ItorDTUser;
	typedef pair<DTUser, int>	PairDTUser;
	typedef pair<DTUser,DTUser>	PairDTMatch;	// MapDTUser���� ��ġ�׷��� ��Ÿ���� Ű�� (�׷��� ù° ����Ű, ������ ����Ű)

	MapDTUser m_mapUser;

	struct GroupEval { int tpDiff; PairDTMatch group; };

public:

	MDuelTournamentMatchMaker();
	MDuelTournamentMatchMaker(CCMatchObjectContainer* moc);
	~MDuelTournamentMatchMaker();

	void SetMatchObjectContainer(CCMatchObjectContainer* p) { m_pMatchObjectContainer = p; }

	bool AddPlayer(const MUID& uid, int tp, DWORD curTime);
	bool RemovePlayer(const MUID& uid);
	void RemoveAll();

	int GetNumPlayer() { return (int)m_mapUser.size(); }

	void PickMatchableGroupsAndRemove(MDuelTournamentPickedGroup& out_matchGroup, int numPick, int acceptableTpGap);
	bool PickGroupForPlayerAndRemove(MDuelTournamentPickedGroup& out_matchGroup, int numPick, const DTUser& dtUser);

	// uid�� ��ȿ�� �� ����ڵ��� �����Ѵ� (��ġ��Ű�� �Լ����� ����ϱ� ���� �ѹ� ������ ��)
	void CleanDisabledUid();

	// ���� �ð� �̻� ����� ������ ã�Ƴ���
	const MDuelTournamentMatchMaker::DTUser* FindLongWaitPlayer(DWORD limitWaitTime, DWORD curTime);

	// �÷��̾� ���� ���� - ��Ī�� �÷��̾� �׷��� ����ڿ��� ���� ���
	void RemovePlayers(const DTUser& begin, const DTUser& last);

	// ���� �ð��� �������� ��ϵ� �������� �������ش�.
	void ServiceTimeClose();

protected:
	// TP�� ����� �������� �׷��� ���´�, ���� �׷��� ������ �� �ִ�
	void PickMatchableGroups(vector<PairDTMatch>& out_vecMatchGroup, int numPick, int acceptableTpGap);

	// Ư�� ������ �����ϴ� ���� TP���̰� ���� �׷��� ������
	bool PickGroupForPlayer(PairDTMatch& out_MatchGroup, int numPick, const DTUser& dtUser);
};

// �����׽�Ʈ���� ���θ� ��� ���� ����� Ŭ����
class MMockDTMatchMaker : public MDuelTournamentMatchMaker
{
public:
	using MDuelTournamentMatchMaker::MapDTUser;
	using MDuelTournamentMatchMaker::ItorDTUser;
	using MDuelTournamentMatchMaker::PairDTUser;
	using MDuelTournamentMatchMaker::PairDTMatch;

	using MDuelTournamentMatchMaker::PickMatchableGroups;
	using MDuelTournamentMatchMaker::PickGroupForPlayer;
	
	using MDuelTournamentMatchMaker::m_mapUser;

	MMockDTMatchMaker(CCMatchObjectContainer* moc) : MDuelTournamentMatchMaker(moc) {}
};