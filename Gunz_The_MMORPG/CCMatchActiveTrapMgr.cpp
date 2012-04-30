#include "stdafx.h"
#include "CCMatchActiveTrapMgr.h"
#include "CCBlobArray.h"


CCMatchActiveTrap::CCMatchActiveTrap()
: m_vPosActivated(0,0,0)
{
	m_uidOwner.SetZero();
	m_nTrapItemId = 0;

	m_nTimeThrowed = 0;
	m_nLifeTime = 0;
	m_nTimeActivated = 0;
}

void CCMatchActiveTrap::AddForcedEnteredPlayer(const CCUID& uid)
{
	// �� �Լ��� ���������� ���� �ߵ����� ���� Ʈ���� �����ڿ��� ���߿� �˷��ֱ� ���� ���ȴ�.
	_ASSERT(!IsActivated());

	int n = (int)m_vecUidForcedEntered.size();
	for (int i=0; i<n; ++i)
		if (m_vecUidForcedEntered[i] == uid) return;
	
	m_vecUidForcedEntered.push_back(uid);
}


CCMatchActiveTrapMgr::CCMatchActiveTrapMgr()
{
	m_pStage = NULL;
}

CCMatchActiveTrapMgr::~CCMatchActiveTrapMgr()
{
	Destroy();
}

void CCMatchActiveTrapMgr::Create( CCMatchStage* pStage )
{
	m_pStage = pStage;
}

void CCMatchActiveTrapMgr::Destroy()
{
	m_pStage = NULL;
	Clear();
}

void CCMatchActiveTrapMgr::Clear()

{
	for (ItorTrap it=m_listTrap.begin(); it!=m_listTrap.end(); ++it)
		delete *it;
	m_listTrap.clear();
}

void CCMatchActiveTrapMgr::AddThrowedTrap( const CCUID& uidOwner, int nItemId )
{
	if (!m_pStage) return;
	if (!m_pStage->GetObj(uidOwner)) return;

	CCMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemId);
	if (!pItemDesc) return;

	CCMatchActiveTrap* pTrap = new CCMatchActiveTrap;
	
	pTrap->m_nTimeThrowed = MGetMatchServer()->GetGlobalClockCount();
	pTrap->m_uidOwner = uidOwner;
	pTrap->m_nTrapItemId = nItemId;

	pTrap->m_nLifeTime = pItemDesc->m_nLifeTime.Ref();

	m_listTrap.push_back(pTrap);

	OutputDebugStr("AddThrowedTrap\n");
}

void CCMatchActiveTrapMgr::OnActivated( const CCUID& uidOwner, int nItemId, const MVector3& vPos )
{
	if (!m_pStage) return;

	CCMatchActiveTrap* pTrap;
	for (ItorTrap it=m_listTrap.begin(); it!=m_listTrap.end(); ++it)
	{
		pTrap = *it;
		if (pTrap->m_uidOwner == uidOwner &&
			pTrap->m_nTrapItemId == nItemId &&
			!pTrap->IsActivated())
		{
			pTrap->m_nTimeActivated = MGetMatchServer()->GetGlobalClockCount();
			pTrap->m_vPosActivated = vPos;

			OutputDebugStr("OnActivated trap\n");

			// �ߵ����� ���� Ʈ���� �����ϴ� ������ �����ߴ� �������� ������ �� Ʈ���� �ߵ� ����� �˷��ش�
			RouteTrapActivationForForcedEnterd(pTrap);
			return;
		}
	}
}

void CCMatchActiveTrapMgr::Update( unsigned long nClock )
{
	CCMatchActiveTrap* pTrap;
	for (ItorTrap it=m_listTrap.begin(); it!=m_listTrap.end(); )
	{
		pTrap = *it;

		if (pTrap->IsActivated())
		{
			// �ߵ� �� ������ ���� Ʈ���� ����
			if (nClock - pTrap->m_nTimeActivated > pTrap->m_nLifeTime)
			{
				it = m_listTrap.erase(it);
				OutputDebugStr("Trap deactivated\n");
				continue;
			}
		}
		else
		{
			// ���������� ��ȿ�ð� ���� �ߵ� Ŀ�ǵ尡 ���� ���� �͵� ����
			if (nClock - pTrap->m_nTimeThrowed > MAX_TRAP_THROWING_LIFE * 1000)
			{
				it = m_listTrap.erase(it);
				OutputDebugStr("Trap Removed without activation\n");
				continue;
			}
		}

		++it;
	}
}

void CCMatchActiveTrapMgr::RouteAllTraps(CCMatchObject* pObj)
{
	// ������ �������� ���� ���忡 �ߵ��Ǿ� �ִ� Ʈ�� �����۵��� �˷��ֱ� ���� �Լ�

	OutputDebugStr("Trap RouteAllTrap to ForcedEntered\n");

	CCMatchActiveTrap* pTrap;

	// ���� �ߵ����� ���� Ʈ��(������ ���ư��� �ִ� ��)�� ���� �ߵ��� �� ���� �˷��� �� �ֵ��� ǥ���صд�
	for (ItorTrap it=m_listTrap.begin(); it!=m_listTrap.end(); ++it)
	{
		pTrap = *it;
		if (!pTrap->IsActivated())
		{
			pTrap->AddForcedEnteredPlayer(pObj->GetUID());

			OutputDebugStr("Trap RESERVE To NOTIFY AddForcedEnteredPlayer\n");
		}
	}

	// �ߵ��Ǿ� �ִ� Ʈ���� ����� �����ش�
	int num = 0;
	for (ItorTrap it=m_listTrap.begin(); it!=m_listTrap.end(); ++it)
		if ((*it)->IsActivated())
			++num;

	if (num <= 0) return;

	void* pTrapArray = MMakeBlobArray(sizeof(MTD_ActivatedTrap), num);

	MTD_ActivatedTrap* pNode;
	int nIndex = 0;
	for (ItorTrap it=m_listTrap.begin(); it!=m_listTrap.end(); ++it)
	{
		pTrap = *it;
		if (pTrap->IsActivated())
		{
			pNode = (MTD_ActivatedTrap*)MGetBlobArrayElement(pTrapArray, nIndex++);
			Make_MTDActivatedTrap(pNode, pTrap);
		}
		else
		{
			// ���� �ߵ����� ���� Ʈ��(������ ���ư��� �ִ� ��)�� ���� �ߵ��� �� ���� �˷��� �� �ֵ��� ǥ���صд�
			pTrap->AddForcedEnteredPlayer(pObj->GetUID());

			OutputDebugStr("Trap RESERVE To NOTIFY AddForcedEnteredPlayer\n");
		}
	}

	MCommand* pCmd = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_NOTIFY_ACTIATED_TRAPITEM_LIST, CCUID(0,0));
	pCmd->AddParameter(new MCommandParameterBlob(pTrapArray, MGetBlobArraySize(pTrapArray)));
	MEraseBlobArray(pTrapArray);

	CCMatchServer::GetInstance()->RouteToListener(pObj, pCmd);
}

void CCMatchActiveTrapMgr::RouteTrapActivationForForcedEnterd(CCMatchActiveTrap* pTrap)
{
	OutputDebugStr("Notify Trap activation to ForcedEnteredPlayer\n");

	if (!pTrap || !pTrap->IsActivated()) { _ASSERT(0); return; }
	if (!m_pStage) return;
	
	int numTarget = (int)pTrap->m_vecUidForcedEntered.size();
	if (numTarget <= 0) return;

	void* pTrapArray = MMakeBlobArray(sizeof(MTD_ActivatedTrap), 1);
	
	MTD_ActivatedTrap* pNode = (MTD_ActivatedTrap*)MGetBlobArrayElement(pTrapArray, 0);
	Make_MTDActivatedTrap(pNode, pTrap);

	MCommand* pCommand = CCMatchServer::GetInstance()->CreateCommand(MC_MATCH_NOTIFY_ACTIATED_TRAPITEM_LIST, CCUID(0,0));
	pCommand->AddParameter(new MCommandParameterBlob(pTrapArray, MGetBlobArraySize(pTrapArray)));

	CCMatchObject* pObj;
	for (int i=0; i<numTarget; ++i)
	{
		pObj = m_pStage->GetObj( pTrap->m_vecUidForcedEntered[i]);
		if (!pObj) continue;

		MCommand* pSendCmd = pCommand->Clone();
		CCMatchServer::GetInstance()->RouteToListener(pObj, pSendCmd);
	}

	delete pCommand;
	MEraseBlobArray(pTrapArray);

	pTrap->m_vecUidForcedEntered.clear();
}