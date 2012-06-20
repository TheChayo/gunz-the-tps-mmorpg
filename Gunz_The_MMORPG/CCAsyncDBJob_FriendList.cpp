#include "stdafx.h"
#include "CCAsyncDBJob_FriendList.h"

void CCAsyncDBJob_FriendList::Run(void* pContext)
{
	_ASSERT(m_pFriendInfo);

	CCMatchDBMgr* pDBMgr = (CCMatchDBMgr*)pContext;

	// �ش�ĳ������ ģ����� ��������
	if (!pDBMgr->FriendGetList(m_nCID, m_pFriendInfo)) 
	{
		SetResult(CCASYNC_RESULT_FAILED);
		return;
	}

	SetResult(CCASYNC_RESULT_SUCCEED);
}
