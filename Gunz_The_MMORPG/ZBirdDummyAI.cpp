#include "stdafx.h"

#include "ZBirdDummyAI.h"
#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZPost.h"
#include "ZConsole.h"
#include "CCCommandLogFrame.h"
#include "ZConfiguration.h"
#include "FileInfo.h"
#include "ZInterfaceItem.h"
#include "ZInterfaceListener.h"
#include "CCDebug.h"
#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"
#include "CCTCPSocket.h"
#include "ZGameClient.h"
#include "time.h"
#include "ZBirdDummyClient.h"

#define BIRDDUMMY_AI_KILL_DELAY		300

ZBirdDummyAI::ZBirdDummyAI()
{
	m_nID = 0;
	m_bInCombat = false;
	m_nKillCount = 0;
	m_pClient = NULL;
	m_nLastCommandTime = 0;
	m_bCreated = false;
	if ((rand() % 100) < 20) m_nLobbyType = ZBDAI_MASTER; else m_nLobbyType = ZBDAI_GUEST;
	if ((rand() % 100) < 20) m_nMasterType = ZBDAI_STARTALONE; else m_nMasterType = ZBDAI_WAIT;
	if ((rand() % 100) < 30) m_nGuestType = ZBDAI_READY; else m_nGuestType = ZBDAI_FORCEDENTRY;
}	

ZBirdDummyAI::~ZBirdDummyAI()
{

}

void ZBirdDummyAI::OnCommand(CCCommand* pCmd)
{
	Sleep(1);
	if (m_pClient == NULL) return;

	m_nLastCommandTime = timeGetTime();


}

void ZBirdDummyAI::Run()
{
	if (!m_bCreated) return;

	OnRun();


}

void ZBirdDummyAI::Create(ZBirdDummyClient* pClient)
{
	m_pClient = pClient;
	m_nID = m_pClient->GetDummyID();
	m_bCreated = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ZBirdDummyAIMakeRoomFlood::OnCommand(CCCommand* pCmd)
{
	Sleep(1);
	if (m_pClient == NULL) return;
	m_nLastCommandTime = timeGetTime();

	switch (pCmd->GetID())
	{
	case MC_MATCH_CHANNEL_RESPONSE_RULE:
		{


		}
		break;
	case MC_MATCH_CHANNEL_RESPONSE_JOIN:
		{
			CCUID uidChannel;
			char szChannelName[256];

			pCmd->GetParameter(&uidChannel,		0, MPT_UID);
			pCmd->GetParameter(szChannelName,	2, MPT_STR, sizeof(szChannelName) );

			static int nChannelID = 0;
			char szStageName[256];
			sprintf(szStageName, "%s_stage%d", m_szUserID, nChannelID);
			nChannelID++;


			ZBIRDPOSTCMD4(m_pClient, MC_MATCH_STAGE_CREATE, 
				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
				CCCmdParamStr(szStageName),
				CCCmdParamBool(false), 
				CCCmdParamStr(""));

		}
		break;
	case MC_MATCH_STAGE_LIST:
		{


		}
		break;
	case MC_MATCH_RESPONSE_STAGE_JOIN:
		{
			int nResult;
			pCmd->GetParameter(&nResult, 0, MPT_INT);

			// ����� ���и� �ٽ� �����
			if (nResult != MOK)
			{
				int nRandNum = rand() % 100000;
				char szStageName[256];
				sprintf(szStageName, "%s_stage%d", "������", nRandNum);


				ZBIRDPOSTCMD4(m_pClient, MC_MATCH_STAGE_CREATE, 
					CCCommandParameterUID(m_pClient->GetPlayerUID()), 
					CCCmdParamStr(szStageName),
					CCCmdParamBool(false), 
					CCCmdParamStr(""));

			}
		}
		break;
	case MC_MATCH_STAGE_JOIN:
		{
			CCUID uidChar, uidStage;
			char szStageName[256];

			pCmd->GetParameter(&uidChar, 0, MPT_UID);
			pCmd->GetParameter(&uidStage, 1, MPT_UID);
			pCmd->GetParameter(szStageName, 2, MPT_STR, sizeof(szStageName) );

			if (uidChar == m_pClient->GetPlayerUID())
			{
				m_nKillCount = 0;
				ZBIRDPOSTCMD3(m_pClient, MC_MATCH_STAGE_START, 
					CCCommandParameterUID(m_pClient->GetPlayerUID()), 
					CCCommandParameterUID(m_pClient->GetStageUID()), 
					CCCommandParameterInt(3));


				static unsigned long int stJoinCount = 0;
				stJoinCount++;
				char szTemp[512];
				if ((stJoinCount % 100) == 0)
				{
					sprintf(szTemp, "Join Flood(%u)", stJoinCount++);
					AddToLogFrame(m_nID, szTemp);
				}
			}
		}
		break;
	case MC_MATCH_STAGE_LAUNCH:
		{

			ZBIRDPOSTCMD2(m_pClient, MC_MATCH_LOADING_COMPLETE, 
				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
				CCCmdParamInt(100));

			// ���ӿ� ���ٰ� �˸�
			ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_REQUEST_ENTERBATTLE, 
				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
				CCCommandParameterUID(m_pClient->GetStageUID()));

		}
		break;
	case MC_MATCH_STAGE_ENTERBATTLE:
		{
			CCUID uidChar, uidStage;
			int nParam;

			pCmd->GetParameter(&uidChar, 0, MPT_UID);
			pCmd->GetParameter(&uidStage, 1, MPT_UID);
			pCmd->GetParameter(&nParam, 2, MPT_INT);

			CCCommandParameter* pParam = pCmd->GetParameter(3);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();

			CCTD_PeerListNode* pPeerNode = (CCTD_PeerListNode*)CCGetBlobArrayElement(pBlob, 0);

			//OnStageEnterBattle(uidChar, uidStage, CCCmdEnterBattleParam(nParam), pPeerNode);

			if (uidChar == m_pClient->GetPlayerUID())
			{
				m_bInCombat = true;
			}

			ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 
				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
				CCCommandParameterUID(m_pClient->GetStageUID()));

		}
		break;
	case MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT:
		{
			CCUID uidChar, uidStage;

			pCmd->GetParameter(&uidChar, 0, MPT_UID);
			pCmd->GetParameter(&uidStage, 1, MPT_UID);

			if (uidChar == m_pClient->GetPlayerUID())
			{
				ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 
					CCCommandParameterUID(m_pClient->GetPlayerUID()), 
					CCCommandParameterUID(m_pClient->GetStageUID()));

				m_bInCombat = false;
			}
		}
		break;
	case MC_MATCH_STAGE_FINISH_GAME:
		{
			CCUID uidStage;
			pCmd->GetParameter(&uidStage, 0, MPT_UID);
			
			ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 
				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
				CCCommandParameterUID(m_pClient->GetStageUID()));
		}
		break;

	}
}

void ZBirdDummyAIMakeRoomFlood::OnRun()
{
	unsigned long int nNowTime = timeGetTime();

	if (m_bInCombat)
	{
		if ((nNowTime - m_nKillLastTime) > BIRDDUMMY_AI_KILL_DELAY)
		{
			m_nKillCount++;
			ZBIRDPOSTCMD1(m_pClient, MC_MATCH_REQUEST_SUICIDE, 
				CCCommandParameterUID(m_pClient->GetPlayerUID()));

			m_nKillLastTime = nNowTime;

			if (m_nKillCount > 50)
			{
				m_nKillCount = 0;
				ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 
					CCCommandParameterUID(m_pClient->GetPlayerUID()), 
					CCCommandParameterUID(m_pClient->GetStageUID()));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ZBirdDummyAIJoinFlood::OnCommand(CCCommand* pCmd)
{
	Sleep(1);
	if (m_pClient == NULL) return;
	m_nLastCommandTime = timeGetTime();

	switch (pCmd->GetID())
	{
	case MC_MATCH_CHANNEL_RESPONSE_RULE:
		{


		}
		break;
	case MC_MATCH_CHANNEL_RESPONSE_JOIN:
		{
			CCUID uidChannel;
			char szChannelName[256];

			pCmd->GetParameter(&uidChannel,		0, MPT_UID);
			pCmd->GetParameter(szChannelName,	2, MPT_STR, sizeof(szChannelName) );

			static int nChannelID = 0;
			char szStageName[256];
			sprintf(szStageName, "%s_stage%d", m_szUserID, nChannelID);
			nChannelID++;
		}
		break;
	case MC_MATCH_STAGE_LIST:
		{
			int nPrevStageCount, nNextStageCount;
			pCmd->GetParameter(&nPrevStageCount, 0, MPT_INT);
			pCmd->GetParameter(&nNextStageCount, 1, MPT_INT);

			CCCommandParameter* pParam = pCmd->GetParameter(2);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();
			int nCount = CCGetBlobArrayCount(pBlob);

			for(int i=0; i<nCount; i++) {
				CCTD_StageListNode* pNode = (CCTD_StageListNode*)CCGetBlobArrayElement(pBlob, i);

				// log debug
				if( pNode ) 
				{
					if (stricmp(pNode->szStageName, m_szLastStage))
					{
						strcpy(m_szLastStage, pNode->szStageName);

						m_uidWantedRoom = pNode->uidStage;
						ZBIRDPOSTCMD2(m_pClient, MC_MATCH_REQUEST_STAGE_JOIN,
							CCCommandParameterUID(m_pClient->GetPlayerUID()), 
							CCCommandParameterUID(pNode->uidStage));
						break;
					}
				}
			}
		}
		break;
	case MC_MATCH_RESPONSE_STAGE_JOIN:
		{
			int nResult;
			pCmd->GetParameter(&nResult, 0, MPT_INT);

			if (nResult != MOK)
			{
				ZBIRDPOSTCMD2(m_pClient, MC_MATCH_REQUEST_STAGE_JOIN,
					CCCommandParameterUID(m_pClient->GetPlayerUID()), 
					CCCommandParameterUID(m_uidWantedRoom));

			}
		}
		break;
	case MC_MATCH_STAGE_JOIN:
		{
			m_nReservedTime = timeGetTime();
			m_bReserved = true;

			CCUID uidChar, uidStage;
			char szStageName[256];

			pCmd->GetParameter(&uidChar, 0, MPT_UID);
			pCmd->GetParameter(&uidStage, 1, MPT_UID);
			pCmd->GetParameter(szStageName, 2, MPT_STR, sizeof(szStageName) );

//			ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_LEAVE,
//				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
//				CCCommandParameterUID(m_uidWantedRoom));

				static unsigned long int stJoinCount = 0;
				stJoinCount++;
				char szTemp[512];
				if ((stJoinCount % 100) == 0)
				{
					sprintf(szTemp, "Join Flood(%u)", stJoinCount++);
					AddToLogFrame(m_nID, szTemp);
				}
		}
		break;
	}
}

void ZBirdDummyAIJoinFlood::OnRun()
{
	unsigned long int nNowTime = timeGetTime();

	if (m_bReserved)
	{
		if ((nNowTime - m_nReservedTime) > 10000)
		{
			ZBIRDPOSTCMD2(m_pClient, MC_MATCH_STAGE_LEAVE,
				CCCommandParameterUID(m_pClient->GetPlayerUID()), 
				CCCommandParameterUID(m_uidWantedRoom));


			m_bReserved = false;
		}
	}
}

ZBirdDummyAIJoinFlood::ZBirdDummyAIJoinFlood() : ZBirdDummyAI()
{
	m_szLastStage[0] = 0;
	m_bReserved = false;
	m_nReservedTime = 0;	
}