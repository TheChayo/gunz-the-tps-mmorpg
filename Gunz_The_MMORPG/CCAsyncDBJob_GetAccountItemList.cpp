#include "stdafx.h"
#include "CCAsyncDBJob_GetAccountItemList.h"


void CCAsyncDBJob_GetAccountItemList::Run( void* pContext )
{
	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	CCAccountItemNode ExpiredItemList[MAX_EXPIRED_ACCOUNT_ITEM];
	int nExpiredItemCount = 0;

	// ��񿡼� AccountItem�� �����´�
	if (!pDBMgr->GetAccountItemInfo(m_dwAID, m_AccountItems, &m_nItemCount, MAX_ACCOUNT_ITEM
		,ExpiredItemList, &nExpiredItemCount, MAX_EXPIRED_ACCOUNT_ITEM))
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	m_vExpiredItems.clear();

	// ���⼭ �߾������� �Ⱓ���� �������� �ִ��� üũ�Ѵ�.
	if (nExpiredItemCount > 0) {
		for (int i = 0; i < nExpiredItemCount; i++) {
			// ��񿡼� �Ⱓ����� AccountItem�� �����.
			if (pDBMgr->DeleteExpiredAccountItem(m_dwAID, ExpiredItemList[i].nAIID)) {
				m_vExpiredItems.push_back(ExpiredItemList[i].nItemID);
			}
		}
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}