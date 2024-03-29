#include "stdafx.h"

/*
#include <winsock2.h>
#include "MErrorTable.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include "CCSharedCommandTable.h"
#include "CCCommandLogFrame.h"
#include "ZIDLResource.h"
#include "CCBlobArray.h"
#include "ZInterface.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "CCMatchChannel.h"
#include "CCMatchStage.h"
#include "ZPost.h"
#include "CCComboBox.h"
#include "CCTextArea.h"
#include "CCDebug.h"
#include "ZMyInfo.h"
#include "ZNetRepository.h"
#include "ZCountDown.h"

#include "ZLanguageConf.h"
*/

// Added R347a
#include "ZGlobal.h"
// Added R349a
#include "ZGameClient.h"
#include "CCBlobArray.h"
#include "ZGameInterface.h"

void ZGameClient::OnDuelTournamentPrepare(CCDUELTOURNAMENTTYPE nType, CCUID uidStage, void* pBlobPlayerInfo)
{
	m_uidStage = uidStage;

	vector<DTPlayerInfo> vecDTPlayerInfo;

	int nCount = CCGetBlobArrayCount(pBlobPlayerInfo);
	if(nCount != GetDTPlayerCount(nType)) { _ASSERT(!"듀얼 토너먼트 타입에 대한 인원수가 안맞음");}

	for (int i=0; i<nCount; ++i)
	{
		DTPlayerInfo *pPlayerInfo = reinterpret_cast<DTPlayerInfo*>(CCGetBlobArrayElement(pBlobPlayerInfo, i));
		if (!pPlayerInfo) { _ASSERT(0); break; }

		DTPlayerInfo playerInfo;
		strcpy(playerInfo.m_szCharName, pPlayerInfo->m_szCharName);
		playerInfo.uidPlayer = pPlayerInfo->uidPlayer;
		playerInfo.m_iTP = pPlayerInfo->m_iTP;
		vecDTPlayerInfo.push_back(playerInfo);

		//////////////////////////////////////////////////// LOG ////////////////////////////////////////////////////
#ifdef _DUELTOURNAMENT_LOG_ENABLE_
		char szbuf[32] = {0, };
		switch(nType)
		{
		case CCDUELTOURNAMENTTYPE_FINAL:			sprintf(szbuf, "TYPE_FINAL"); break;
		case CCDUELTOURNAMENTTYPE_SEMIFINAL:		sprintf(szbuf, "TYPE_SEMIFINAL"); break;
		case CCDUELTOURNAMENTTYPE_QUATERFINAL:	sprintf(szbuf, "TYPE_QUATERFINAL"); break;
		default:								sprintf(szbuf, "TYPE_FAIL"); break;
		}
		cclog("[MC_MATCH_DUELTOURNAMENT_PREPARE_MATCH] Type:%s, Count:%d, Player(%d:%d)%s, TP:%d \n", 
			szbuf, i, pPlayerInfo->uidPlayer.High, pPlayerInfo->uidPlayer.Low, pPlayerInfo->m_szCharName, pPlayerInfo->m_iTP);
#endif
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	ZApplication::GetGameInterface()->SetDuelTournamentCharacterList((CCDUELTOURNAMENTTYPE)nType, vecDTPlayerInfo);
}

void ZGameClient::OnDuelTournamentLaunch(const CCUID& uidStage, const char* pszMapName)
{	
	m_uidStage = uidStage;
	strcpy(m_szStageName, "DuelTournament_Stage");

	SetAllowTunneling(false);

	m_MatchStageSetting.SetMapName(const_cast<char*>(pszMapName));

	// 암호화 키 설정  - 클랜전은 OnStageJoin대신 여기서 암호화키를 설정한다.
	unsigned int nStageNameChecksum = m_szStageName[0] + m_szStageName[1] + m_szStageName[2] + m_szStageName[3];
	InitPeerCrypt(uidStage, nStageNameChecksum);

	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME)
	{
		ZChangeGameState(GUNZ_GAME);		// thread safely
	}
}