#include "stdafx.h"

#include "ZGameInterface.h"
#include "ZApplication.h"
//#include "ZPost.h"
//#include "ZConsole.h"
//#include "CCCommandLogFrame.h"
#include "ZConfiguration.h"
#include "FileInfo.h"
#include "ZInterfaceItem.h"
#include "ZInterfaceListener.h"
#include "CCDebug.h"
//#include "CCMatchTransDataType.h"
#include "CCBlobArray.h"
//#include "CCTCPSocket.h"
//#include "ZGameClient.h"
#include "time.h"
//#include "ZSecurity.h"

// 여기서부터 테스트를 위한 코드 - Bird ////////////////////////////////////////////////
#ifdef _BIRDTEST

#include "ZBirdDummyClient.h"
#include "ZBirdDummyAI.h"


#define MAX_CLIENT		2000
#define MAX_DUMMYCASE	6

int g_nDummyCount = MAX_CLIENT;
int g_nDummyCaseCount[MAX_DUMMYCASE] = {0, };

ZBirdDummyClient			g_DummyClient[MAX_CLIENT];
ZBirdDummyAIJoinFlood		g_DummyAI[MAX_CLIENT];


////////////////////////////////////////////////////////////////////////////////////////////
typedef void (*ZBT_ONCommand)(CCCommand* pCmd);


char g_szBT_ID[256] = "";
ZGameInterface* g_pGIBirdTest = NULL;


// 무한 더미 클라이언트 플러딩
void OnBTDummyTest(CCCommand* pCmd);
void InitBTDummyClient();
void UpdateBTDummyClient();
void OnBTDummyConnFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd);
void OnBTDummyJoinRoomFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd);
void OnBTDummyGameFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd);
void OnBTDummyChannelChatFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd);
void OnBTDummyEchoFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd);
void OnBTDummyChannelChangeOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd);

// 현재 테스트하고 있는 항목 설정
ZBT_ONCommand g_pBTCommandCallBack = NULL;

void ZGameInterface::OnBirdTestCreate()
{
	g_pGIBirdTest = this;

	char szID[256], szPassword[256];
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	CCWidget* pWidget = pResource->FindWidget("LoginID");
	strcpy(szID, pWidget->GetText());

	pWidget = pResource->FindWidget("LoginPassword");
	strcpy(szPassword, pWidget->GetText());

	
	g_nDummyCount = GetPrivateProfileInt("OPTION", "DummyCount", 100, "./BirdTest.ini");

	for (int i = 0; i < MAX_DUMMYCASE; i++)
	{
		char szKey[256];
		sprintf(szKey, "DummyCase%d", i+1);
		g_nDummyCaseCount[i] = GetPrivateProfileInt("OPTION", szKey, 0, "./BirdTest.ini");
	}
	
	g_pBTCommandCallBack = OnBTDummyTest;
	InitBTDummyClient();
}

void ZGameInterface::OnBirdTestDestroy()
{

}

void ZGameInterface::OnBirdTestUpdate()
{
	UpdateBTDummyClient();
}

void ZGameInterface::OnBirdTestDraw()
{

}

void ZGameInterface::OnBirdTestCommand(CCCommand* pCmd)
{
	if (g_pBTCommandCallBack != NULL) g_pBTCommandCallBack(pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////
void InitBTDummyClient()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	CCWidget* pAddr = pResource->FindWidget("ServerAddress");
	CCWidget* pPort = pResource->FindWidget("ServerPort");
	if (!pAddr || !pPort) return;

	ZBT_DummyONCommand pCallBackFunc;

	int nCaseCounter = 0;
	for (int nCase = 0; nCase < MAX_DUMMYCASE; nCase++)
	{
		switch (nCase)
		{
		case 0: pCallBackFunc = OnBTDummyConnFloodOnCommand; break;
		case 1: pCallBackFunc = OnBTDummyJoinRoomFloodOnCommand; break;
		case 2: pCallBackFunc = OnBTDummyGameFloodOnCommand; break;
		case 3: pCallBackFunc = OnBTDummyChannelChatFloodOnCommand; break;
		case 4: pCallBackFunc = OnBTDummyEchoFloodOnCommand; break;
		case 5: pCallBackFunc = OnBTDummyChannelChangeOnCommand; break;
		}

		for (int i = 0; i < g_nDummyCaseCount[nCase]; i++)
		{
			g_DummyClient[nCaseCounter].Create(nCaseCounter, pCallBackFunc);
			ZGetGameClient()->GetCommandManager()->AssignDescs(g_DummyClient[nCaseCounter].GetCommandManager());

			if (nCase == 1)
			{
				g_DummyAI[nCaseCounter].Create(&g_DummyClient[nCaseCounter]);
			}

			// 서버에 연결한다
			char szCmd[256];
			sprintf(szCmd, "%s:%u", pAddr->GetText(), atoi(pPort->GetText()));
			ZBIRDPOSTCMD1(&g_DummyClient[nCaseCounter], MC_NET_CONNECT, CCCmdParamStr(szCmd));

			nCaseCounter++;
		}
	}
}

void UpdateBTDummyClient()
{
	for (int i = 0; i < g_nDummyCount; i++)
	{
		g_DummyAI[i].Run();
		g_DummyClient[i].Run();
	}
}

// 무한 더미 클라이언트 플러딩 - 메인쓰레드
void OnBTDummyTest(CCCommand* pCmd)
{
	switch(pCmd->GetID())
	{
	case MC_MATCH_RESPONSE_LOGIN:
		{

		}
		break;
	case MC_NET_ONCONNECT:
		{
			
		}
		break;
	case MC_NET_ONDISCONNECT:
		{

		}
		break;
	case MC_NET_ONERROR:
		{

		}
		break;

	}
}

bool OnCommonLogin(ZBirdDummyClient* pClient, CCCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_NET_ONCONNECT:
		{
			sprintf(g_szBT_ID, "BirdTest%d", pClient->GetDummyID());
			char szPassword[256] = "1111";

			ZBIRDPOSTCMD4(pClient, MC_MATCH_LOGIN, CCCmdParamStr(g_szBT_ID), CCCmdParamStr(szPassword), CCCommandParameterInt(MCOMMAND_VERSION),
						CCCommandParameterUInt(0));
		}
		return true;
	case MC_MATCH_RESPONSE_LOGIN:
		{
			ZBIRDPOSTCMD1(pClient, MC_MATCH_REQUEST_ACCOUNT_CHARLIST, CCCommandParameterUID(pClient->GetPlayerUID()));
		}
		return true;
	case MC_NET_ONDISCONNECT:
		{
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			CCWidget* pAddr = pResource->FindWidget("ServerAddress");
			CCWidget* pPort = pResource->FindWidget("ServerPort");
			if (!pAddr || !pPort) return true;

			char szCmd[256];
			sprintf(szCmd, "%s:%u", pAddr->GetText(), atoi(pPort->GetText()));
			ZBIRDPOSTCMD1(pClient, MC_NET_CONNECT, CCCmdParamStr(szCmd));
		}
		return true;
	case MC_NET_ONERROR:
		{

		}
		return true;
	case MC_MATCH_RESPONSE_ACCOUNT_CHARLIST:
		{
			CCCommandParameter* pParam = pCmd->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pCharListBlob = pParam->GetPointer();
			int nCount = CCGetBlobArrayCount(pCharListBlob);

			bool bExistChar = false;
			for (int i = 0; i < nCount; i++)
			{
				CCTD_CharInfo* pCharInfo = (CCTD_CharInfo*)CCGetBlobArrayElement(pCharListBlob, i);
				if (pCharInfo->nCharNum == 0)
				{
					ZBIRDPOSTCMD2(pClient, MC_MATCH_REQUEST_SELECT_CHAR,
						CCCommandParameterUID(pClient->GetPlayerUID()), CCCommandParameterUInt(0));


					bExistChar = true;
					break;
				}
			}

			if (!bExistChar)
			{
				// 캐릭이 없으면 새로 생성
				char szCharName[256];
				sprintf(szCharName, "버드꼬붕%d", pClient->GetDummyID());

				ZBIRDPOSTCMD7(pClient, MC_MATCH_REQUEST_CREATE_CHAR, 
					CCCommandParameterUID(pClient->GetPlayerUID()), 
					CCCommandParameterUInt(0),
					CCCommandParameterString(szCharName), 
					CCCommandParameterUInt(0), 
					CCCommandParameterUInt(0),
					CCCommandParameterUInt(0), 
					CCCommandParameterUInt(0));
			}
		}
		return true;
	case MC_MATCH_RESPONSE_CREATE_CHAR:
		{
			ZBIRDPOSTCMD1(pClient, MC_MATCH_REQUEST_ACCOUNT_CHARLIST, CCCommandParameterUID(pClient->GetPlayerUID()));
		}
		return true;
	case MC_MATCH_RESPONSE_SELECT_CHAR:
		{
			int nResult;
			pCmd->GetParameter(&nResult, 0, MPT_INT);
			if (nResult == MOK)
			{
				CCCommandParameter* pParam = pCmd->GetParameter(1);
				if(pParam->GetType()!=MPT_BLOB) break;

				void* pCharBlob = pParam->GetPointer();
				CCTD_CharInfo* pCharInfo = (CCTD_CharInfo*)CCGetBlobArrayElement(pCharBlob, 0);

				// 내정보 저장
				pClient->SetPlayerName(pCharInfo->szName);


				ZBIRDPOSTCMD0(pClient, MC_MATCH_REQUEST_RECOMMANDED_CHANNEL);
			}
			else
			{
				ZBIRDPOSTCMD1(pClient, MC_MATCH_REQUEST_ACCOUNT_CHARLIST, CCCommandParameterUID(pClient->GetPlayerUID()));
			}
		}
		return true;
	case MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL:
		{
			CCUID uidChannel;
			pCmd->GetParameter(&uidChannel, 0, MPT_UID);

//			strcpy(szChannelName, pClient->GetChannelName());

			ZBIRDPOSTCMD2(pClient, MC_MATCH_CHANNEL_REQUEST_JOIN,
				CCCommandParameterUID(pClient->GetPlayerUID()), 
				CCCommandParameterUID(pClient->GetChannelUID()));

		}
		return true;
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// 로그인, 로그아웃 반복
void OnBTDummyConnFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_NET_ONCONNECT:
		{
			sprintf(g_szBT_ID, "BirdTest%d", pClient->GetDummyID());
			char szPassword[256] = "1111";

			ZBIRDPOSTCMD4(pClient, MC_MATCH_LOGIN, CCCmdParamStr(g_szBT_ID), CCCmdParamStr(szPassword), CCCommandParameterInt(MCOMMAND_VERSION),
						CCCommandParameterUInt(0));


			static unsigned long int stConnCount = 0;
			stConnCount++;
			char szTemp[512];
			if ((stConnCount % 100) == 0)
			{
				sprintf(szTemp, "ConnFlood OnConnected(%u)", stConnCount++);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}

			return;
		}
		break;
	case MC_NET_ONDISCONNECT:
		{
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			CCWidget* pAddr = pResource->FindWidget("ServerAddress");
			CCWidget* pPort = pResource->FindWidget("ServerPort");
			if (!pAddr || !pPort) return;

			char szCmd[256];
			sprintf(szCmd, "%s:%u", pAddr->GetText(), atoi(pPort->GetText()));
			ZBIRDPOSTCMD1(pClient, MC_NET_CONNECT, CCCmdParamStr(szCmd));
			return;
		}
		break;
	case MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL:
		{
			CCUID uidChannel;
			char szChannelName[64];
			pCmd->GetParameter(&uidChannel, 0, MPT_UID);
			pCmd->GetParameter(szChannelName, 1, MPT_STR, sizeof(szChannelName) );

			ZBIRDPOSTCMD0(pClient, MC_NET_DISCONNECT);
			return;
		}
		break;
	}
	if (OnCommonLogin(pClient, pCmd)) return;
}

// 방 들어갔다 나왔다 하기
void OnBTDummyJoinRoomFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd)
{
	if (OnCommonLogin(pClient, pCmd)) return;

	g_DummyAI[pClient->GetDummyID()].OnCommand(pCmd);
}

// 게임 열심히 플레이 하자
void OnBTDummyGameFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd)
{

}

char g_szDummyChattingMsg[10][256] =
{ 
"님 즐 하셈..캬캬캬",
"나 버드 꼬붕이당...버드대마왕님 만쉐..~(-_-)~",
"이번 건즈 운영자님들 힘드셨겠어요. 건즈 인기를 반영한다고 하지만...",
"표현방법이 잘못됐다고 말하고 싶은겁니다. 충격을 준답시고 욕을 해대는것은 잘못됐다고 생각합니다만..",
"오픈이 된거예여? 처음이라...",
"괜찮은 클랜을 찾고 있습니다.",
"계속 날라 댕기는 버그요... 그거 고쳐 주세요.짜증남...",
"이게임무료예요?",
"건즈 제대로 할려고 256램 삿다 -_-ㅋ",
"렉이많이걸려여패치좀해주새요"
};

// 채널에서 죽어라 채팅하자
void OnBTDummyChannelChatFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd)
{
	if (OnCommonLogin(pClient, pCmd)) return;

	switch (pCmd->GetID())
	{
	case MC_MATCH_CHANNEL_RESPONSE_JOIN:
		{
			int nChatIndex = rand() % 10;
			ZBIRDPOSTCMD3(pClient, MC_MATCH_CHANNEL_REQUEST_CHAT,
                CCCommandParameterUID(pClient->GetPlayerUID()), 
				CCCommandParameterUID(pClient->GetChannelUID()), 
				CCCmdParamStr(g_szDummyChattingMsg[nChatIndex])	);
		}
		break;
	case MC_MATCH_CHANNEL_CHAT:
		{
			CCUID uidChannel;
			char szPlayerName[256];
			pCmd->GetParameter(&uidChannel,		0, MPT_UID);
			pCmd->GetParameter(szPlayerName,	1, MPT_STR, sizeof(szPlayerName) );

			// 내 채팅 메세지이면 또 보낸다
			
			if (!stricmp(szPlayerName, pClient->GetPlayerName()))
			{
				int nChatIndex = rand() % 10;
				ZBIRDPOSTCMD3(pClient, MC_MATCH_CHANNEL_REQUEST_CHAT,
					CCCommandParameterUID(pClient->GetPlayerUID()), 
					CCCommandParameterUID(pClient->GetChannelUID()), 
					CCCmdParamStr(g_szDummyChattingMsg[nChatIndex]));
			}

			static unsigned long int st_nChatCount = 0;
			st_nChatCount++;
			char szTemp[512];

			if ((st_nChatCount % 100) == 0) {
				sprintf(szTemp, "ChatFlood(%u)", st_nChatCount);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}
		}
		break;
	}
}

// 죽어라 Echo 하기
void OnBTDummyEchoFloodOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_NET_ONCONNECT:
		{
			ZBIRDPOSTCMD1(pClient, MC_NET_ECHO, CCCmdParamStr("무궁화 꽃이 피었습니다.무궁화 꽃이 피었습니다.무궁화 꽃이 피었습니다."));
		}
		break;
	case MC_NET_ONDISCONNECT:
		{

		}
		break;
	case MC_NET_ONERROR:
		{

		}
		break;
	case MC_NET_ECHO:
		{
			static unsigned long int st_nEchoCount = 0;
			st_nEchoCount++;
			char szTemp[512];
			if ((st_nEchoCount % 100) == 0)
			{
				sprintf(szTemp, "EchoFlood(%u)", st_nEchoCount);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}

			ZBIRDPOSTCMD1(pClient, MC_NET_ECHO, CCCmdParamStr("무궁화 꽃이 피었습니다.무궁화 꽃이 피었습니다.무궁화 꽃이 피었습니다."));
		}
		break;
	}
}

// 채널변경 테스트
void OnBTDummyChannelChangeOnCommand(ZBirdDummyClient* pClient, CCCommand* pCmd)
{
	if (OnCommonLogin(pClient, pCmd)) return;


	switch (pCmd->GetID())
	{
	case MC_MATCH_CHANNEL_RESPONSE_JOIN:
		{
			static unsigned long int st_nCount = 0;
			st_nCount++;
			char szTemp[128];
			if ((st_nCount % 100) == 0)
			{
				sprintf(szTemp, "Change Channel Flood(%u)", st_nCount);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}


			ZBIRDPOSTCMD2(pClient, MC_MATCH_CHANNEL_LIST_START, CCCommandParameterUID(pClient->GetPlayerUID()), CCCommandParameterInt(0));
		}
		break;
	case MC_MATCH_CHANNEL_LIST:
		{
			CCCommandParameter* pParam = pCmd->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();
			int nCount = CCGetBlobArrayCount(pBlob);
		
			CCUID uidChannels[100];

			for(int i=0; i<nCount; i++)
			{
				CCCHANNELLISTNODE* pNode = (CCCHANNELLISTNODE*)CCGetBlobArrayElement(pBlob, i);

				if (i<100) uidChannels[i] = pNode->uidChannel;
			}

			int nIndex = rand() % 2;

			ZBIRDPOSTCMD1(pClient, MC_MATCH_CHANNEL_LIST_STOP, CCCommandParameterUID(pClient->GetPlayerUID()));

			ZBIRDPOSTCMD2(pClient, MC_MATCH_CHANNEL_REQUEST_JOIN, 
				CCCommandParameterUID(pClient->GetPlayerUID()), CCCommandParameterUID(uidChannels[nIndex]));

		}
		break;
	}

}

// 여기까지 테스트를 위한 코드 - Bird //////////////////////////////////////////////////

#endif		// #ifdef _BIRDTEST
