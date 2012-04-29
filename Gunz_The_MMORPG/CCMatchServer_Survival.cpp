//////////////////////////////////////////////////////////////////////////////////////////////
// 2009. 6. 3 - Added By Hong KiJu

// Notice By Hong KiJu - ȫ���ֿ��� �����ϼ���

// Test Code�Դϴ�. �Ʒ��� ���� ȣ���Ͻø� �˴ϴ�..-.,-;
// �Լ��� ��ġ�� CCMatchServer Class ���� ��ġ���ѳ�����,
// ���� ���ϴ� ��ġ�� �Լ��� �ű�ø� �˴ϴ�.
// �Լ��� �Ķ���� � �����ϼž� �ɰ̴ϴ�. �ϴ� ������ ������Ÿ�� ���Դϴ�.

#include "stdafx.h"
#include "CCMatchServer.h"

#include "CCAsyncDBJob_SurvivalMode.h"

bool CCMatchServer::OnRequestSurvivalModeGroupRanking()
{
	CCAsyncDBJob_GetSurvivalModeGroupRanking *pAsyncDbJob_GetSurvivalModeGroupRanking = new CCAsyncDBJob_GetSurvivalModeGroupRanking;
	if( 0 == pAsyncDbJob_GetSurvivalModeGroupRanking )
		return false;

	pAsyncDbJob_GetSurvivalModeGroupRanking->Input();
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob_GetSurvivalModeGroupRanking );

	return true;
}

bool CCMatchServer::OnRequestSurvivalModePrivateRanking( const CCUID& uidStage, const CCUID& uidPlayer, DWORD dwScenarioID, DWORD dwCID )
{
	CCAsyncDBJob_GetSurvivalModePrivateRanking *pAsyncDbJob_GetSurvivalModePrivateRanking = new CCAsyncDBJob_GetSurvivalModePrivateRanking;
	if( 0 == pAsyncDbJob_GetSurvivalModePrivateRanking )
		return false;

	pAsyncDbJob_GetSurvivalModePrivateRanking->Input( uidStage, uidPlayer, dwScenarioID, dwCID );
	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob_GetSurvivalModePrivateRanking );

	return true;
}

// DB�� SurvivalModeGameLog�� ���� �� ȣ��Ǵ� �Լ��Դϴ�.
bool CCMatchServer::OnPostSurvivalModeGameLog()
{
	char *szGameName = "'Survival Mode Test Room";
	DWORD dwScenarioID = 1;			// ���� Define�ؼ� ���ø� ���� �� �����ϴ�. 
									// 1�� �Ǽ�, 2�� ������, 3�� �������� DB������ ���õǾ� ����
	DWORD dwTotalRound = 20;		// �� ������ ������Դϴ�. 

	DWORD dwMasterPlayerCID			= 1;	// ������ CID�Դϴ�(1�� �׽�Ʈ �ڵ�, ���� DB���� �߷�Ÿ��)
	DWORD dwMasterPlayerRankPoint	= 1000;
	DWORD dwPlayer2CID				= 4;	// ������ Player�� CID�Դϴ�(4�� �������)
	DWORD dwPlayer2RankPoint		= 4000;
	DWORD dwPlayer3CID				= 6;	// ������ Player�� CID�Դϴ�(6�� ���d)
	DWORD dwPlayer3RankPoint		= 6000;
	DWORD dwPlayer4CID				= 8;	// ������ Player�� CID�Դϴ�
	DWORD dwPlayer4RankPoint		= 8000;

	DWORD dwGamePlayTime =	10;		// ������ �÷����� �ð��Դϴ�.

	CCAsyncDBJob_InsertSurvivalModeGameLog *pAsyncDbJob_InsertSurvivalGameLog = new CCAsyncDBJob_InsertSurvivalModeGameLog;
	if( 0 == pAsyncDbJob_InsertSurvivalGameLog )
		return false;

	pAsyncDbJob_InsertSurvivalGameLog->Input( szGameName, dwScenarioID, dwTotalRound, 
		dwMasterPlayerCID, dwMasterPlayerRankPoint, dwPlayer2CID, dwPlayer2RankPoint, 
		dwPlayer3CID, dwPlayer3RankPoint, dwPlayer4CID, dwPlayer4RankPoint, 
		dwGamePlayTime);

	CCMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob_InsertSurvivalGameLog );

	return true;
}
