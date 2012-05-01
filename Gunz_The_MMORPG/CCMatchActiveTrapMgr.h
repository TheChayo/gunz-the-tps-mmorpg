#pragma once

// CCMatchActiveTrapMgr�� ������ ������ �� �� �ߵ����̰ų� Ȥ�� ���� �ߵ��Ǳ� ���� Ʈ������ ����� �����Ѵ�.
// (���� ���� ������ �������� ���� ���忡 �����ϴ� Ʈ������ �˷��ֱ� ���ؼ� ����صδ� ���̴�)


class CCMatchActiveTrap
{
public:
	unsigned long m_nTimeThrowed;
	int m_nLifeTime;

	// Ʈ���� �����ٴ� Ŀ�ǵ�� ��� ����
	CCUID m_uidOwner;
	int m_nTrapItemId;

	// ���� Ʈ���� �ߵ��Ǿ��ٴ� Ŀ�ǵ�� ������ ����
	CCVector3 m_vPosActivated;
	unsigned long m_nTimeActivated;

	// �� Ʈ���� ���������� �ߵ��Ǳ� ���� �ð��뿡 ������ ������ uid�� ���� ����� ��
	vector<CCUID> m_vecUidForcedEntered;

public:
	CCMatchActiveTrap();
	bool IsActivated() { return m_nTimeActivated!=0; }
	void AddForcedEnteredPlayer(const CCUID& uid);
};

class CCMatchActiveTrapMgr
{
	typedef list<CCMatchActiveTrap*>		ListTrap;
	typedef ListTrap::iterator			ItorTrap;
	list<CCMatchActiveTrap*> m_listTrap;

	CCMatchStage* m_pStage;

public:
	CCMatchActiveTrapMgr();
	~CCMatchActiveTrapMgr();

	void Create(CCMatchStage* pStage);
	void Destroy();
	void Clear();

	void AddThrowedTrap(const CCUID& uidOwner, int nItemId);
	void OnActivated(const CCUID& uidOwner, int nItemId, const CCVector3& vPos);

	void Update(unsigned long nClock);

	void RouteAllTraps(CCMatchObject* pObj);
	void RouteTrapActivationForForcedEnterd(CCMatchActiveTrap* pTrap);
};