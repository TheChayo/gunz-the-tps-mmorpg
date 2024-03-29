#include "stdafx.h"

#include "ZBirdDummyClient.h"
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
#include "CCCommandBuilder.h"

#include "CCCommandLogFrame.h"
#include "CCListBox.h"
extern CCCommandLogFrame* m_pLogFrame;
#include "ZStringResManager.h"
#include "ZLanguageConf.h"



// 여기서부터 테스트를 위한 코드 - Bird ////////////////////////////////////////////////
//#ifdef _BIRDTEST

bool ZBirdPostCommand(ZBirdDummyClient* pDummyClient, CCCommand* pCmd)
{
	return pDummyClient->Post(pCmd);
}

ZBirdDummyClient::ZBirdDummyClient() : CCCommandCommunicator()
{
	m_nDummyID = 0;
	m_fnOnCommandCallBack = NULL;
	m_szChannel[0] = 0;
	m_szStageName[0] = 0;
	m_szPlayerName[0] = 0;

	m_pCommandBuilder = new CCCommandBuilder(CCUID(0,0), CCUID(0,0), GetCommandManager());

	m_nPBufferTop = 0;
	m_Server.SetInvalid();

	InitializeCriticalSection(&m_csRecvLock);

	// 소켓 이벤트 연결
	m_ClientSocket.SetCallbackContext(this);
	m_ClientSocket.SetConnectCallback(SocketConnectEvent);
	m_ClientSocket.SetDisconnectCallback(SocketDisconnectEvent);
	m_ClientSocket.SetRecvCallback(SocketRecvEvent);
	m_ClientSocket.SetSocketErrorCallback(SocketErrorEvent);

	
}

ZBirdDummyClient::~ZBirdDummyClient()
{
	delete m_pCommandBuilder;
	m_pCommandBuilder = NULL;

	DeleteCriticalSection(&m_csRecvLock);
}

void ZBirdDummyClient::Create(int nID, ZBT_DummyONCommand pCallBack)
{
	CCCommandCommunicator::Create();

	m_nDummyID = nID;
	m_fnOnCommandCallBack = pCallBack;
	m_pLogFrame->Show(true);
}

bool ZBirdDummyClient::Post(CCCommand* pCommand)
{
	LockRecv();
	bool bRet = CCCommandCommunicator::Post(pCommand);
	UnlockRecv();
	return bRet;
}

int ZBirdDummyClient::Connect(SOCKET* pSocket, char* szIP, int nPort)
{
	if (m_ClientSocket.Connect(pSocket, szIP, nPort))
		return MOK;
	else 
		return MERR_UNKNOWN;
}

void ZBirdDummyClient::Disconnect(CCUID uid)
{
	m_ClientSocket.Disconnect();
}

void ZBirdDummyClient::OutputLocalInfo()
{

}
void ZBirdDummyClient::OutputMessage(const char* szMessage, CCZMOMType nType)
{

}

void ZBirdDummyClient::OnRegisterCommand(CCCommandManager* pCommandManager)
{

}

CCUID ZBirdDummyClient::GetSenderUIDBySocket(SOCKET socket)
{ 
	if (m_ClientSocket.GetSocket() == socket)
		return m_Server;
	else
		return CCUID(0,0);
}


void ZBirdDummyClient::SendCommand(CCCommand* pCommand)
{
	static unsigned char nSerial = 0;
	nSerial++;
	pCommand->m_nSerialNumber = nSerial;


	int nPacketSize = CalcPacketSize(pCommand);
	char* pSendBuf = new char[nPacketSize];

	int size = MakeCmdPacket(pSendBuf, nPacketSize, pCommand);

	m_ClientSocket.Send(pSendBuf, size);		
}

CCCommand* ZBirdDummyClient::GetCommandSafe()
{
	LockRecv();
	CCCommand* pCmd = CCCommandCommunicator::GetCommandSafe();
	UnlockRecv();

	return pCmd;
}

int ZBirdDummyClient::MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, CCCommand* pCommand)
{
	CCCommandMsg* pMsg = (CCCommandMsg*)pOutPacket;

	pMsg->Buffer[0] = 0;
	pMsg->nCheckSum = 0;
	pMsg->nMsg = MSGID_COMMAND;
	pMsg->nSize = (unsigned short)( sizeof(CCPacketHeader) + pCommand->GetData(pMsg->Buffer, iMaxPacketSize-sizeof(CCPacketHeader)) );

	pMsg->nCheckSum = MBuildCheckSum(pMsg, pMsg->nSize);

	return pMsg->nSize;
}

bool ZBirdDummyClient::OnSockConnect(SOCKET sock)
{
	ZBIRDPOSTCMD0(this, MC_NET_ONCONNECT);
	return true;
}

bool ZBirdDummyClient::OnSockDisconnect(SOCKET sock)
{
	ZBIRDPOSTCMD0(this, MC_NET_ONDISCONNECT);
	return true;
}

bool ZBirdDummyClient::OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize)
{
	if (m_pCommandBuilder==NULL) return false;
	// New Cmd Buffer ////////////////
	m_pCommandBuilder->SetUID(m_This, GetSenderUIDBySocket(sock));
	m_pCommandBuilder->Read((char*)pPacket, dwSize);

	LockRecv();
	while(CCCommand* pCmd = m_pCommandBuilder->GetCommand()) 
	{
		Post(pCmd);
	}
	UnlockRecv();

	while(CCPacketHeader* pNetCmd = m_pCommandBuilder->GetNetCommand()) 
	{
		if (pNetCmd->nMsg == MSGID_REPLYCONNECT) 
		{
			MReplyConnectMsg* pMsg = (MReplyConnectMsg*)pNetCmd;
			CCUID HostUID, AllocUID;
			HostUID.High = pMsg->nHostHigh;
			HostUID.Low = pMsg->nHostLow;
			AllocUID.High = pMsg->nAllocHigh;
			AllocUID.Low = pMsg->nAllocLow;
			free(pNetCmd);

			LockRecv();
			OnConnected(sock, &HostUID, &AllocUID);
			UnlockRecv();
		}
	}

	return true;
}

int ZBirdDummyClient::OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID)
{
	if (sock == m_ClientSocket.GetSocket()) {
		int ret = CCCommandCommunicator::OnConnected(pTargetUID, pAllocUID, 0, NULL);
		m_Server = *pTargetUID;

		if (m_pCommandBuilder)
			m_pCommandBuilder->SetUID(*pAllocUID, *pTargetUID);

		return ret;
	} else {
		return MERR_UNKNOWN;
	}
}


void ZBirdDummyClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{

}


bool ZBirdDummyClient::SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize)
{
	ZBirdDummyClient* pClient = (ZBirdDummyClient*)pCallbackContext;
	return pClient->OnSockRecv(sock, pPacket, dwSize);
}

bool ZBirdDummyClient::SocketConnectEvent(void* pCallbackContext, SOCKET sock)
{
	ZBirdDummyClient* pClient = (ZBirdDummyClient*)pCallbackContext;
	return pClient->OnSockConnect(sock);
}

bool ZBirdDummyClient::SocketDisconnectEvent(void* pCallbackContext, SOCKET sock)
{
	ZBirdDummyClient* pClient = (ZBirdDummyClient*)pCallbackContext;
	return pClient->OnSockDisconnect(sock);
}

void ZBirdDummyClient::SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	ZBirdDummyClient* pClient = (ZBirdDummyClient*)pCallbackContext;
	pClient->OnSockError(sock, ErrorEvent, ErrorCode);
}

int ZBirdDummyClient::OnResponseMatchLogin(const CCUID& uidServer, int nResult, const CCUID& uidPlayer)
{
	m_uidServer = uidServer;
	m_uidPlayer = uidPlayer;


	return MOK;
}

void ZBirdDummyClient::OnResponseRecommandedChannel(const CCUID& uidChannel, char* szChannel)
{
	m_uidChannel = uidChannel;
	strcpy(m_szChannel, szChannel);
}

void ZBirdDummyClient::OnStageJoin(const CCUID& uidChar, const CCUID& uidStage, char* szStageName)
{
	if (uidChar == GetPlayerUID()) 
	{
		m_uidStage = uidStage;
	}

	strcpy(m_szStageName, szStageName);
}

void ZBirdDummyClient::OnStageLeave(const CCUID& uidChar, const CCUID& uidStage)
{
	if (uidChar == GetPlayerUID()) 
	{
		m_uidStage = CCUID(0,0);
	}
}


bool ZBirdDummyClient::OnCommand(CCCommand* pCommand)
{
	switch(pCommand->GetID())
	{
	case MC_LOCAL_INFO:
		{

		}
		break;
	case MC_NET_CONNECT:
		{
			char szAddress[256];
			pCommand->GetParameter(szAddress, 0, MPT_STR, sizeof(szAddress) );

			char szIP[256];
			int nPort;
			SplitIAddress(szIP, &nPort, szAddress);

			SOCKET socket;
			int nReturn = Connect(&socket, szIP, nPort);
			if(nReturn!=MOK)
			{
				OutputMessage("Can't connect to communicator", CCZMOM_ERROR);
//				OutputMessage(CCGetErrorString(nReturn), CCZMOM_ERROR);
				OutputMessage(
					ZErrStr(nReturn), 
					CCZMOM_ERROR );
				break;
			}
		}
		break;
	case MC_NET_DISCONNECT:
		Disconnect(m_Server);
		break;
	case MC_MATCH_RESPONSE_LOGIN:
		{
			int nResult;
			CCUID uidPlayer;
			
			pCommand->GetParameter(&nResult, 0, MPT_INT);
			pCommand->GetParameter(&uidPlayer, 3, MPT_UID);

			OnResponseMatchLogin(pCommand->GetSenderUID(), nResult, uidPlayer);
		}
		break;
	case MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL:
		{
			CCUID uidChannel;
			char szChannelName[256];
			pCommand->GetParameter(&uidChannel, 0, MPT_UID);
			pCommand->GetParameter(szChannelName, 1, MPT_STR, sizeof(szChannelName) );

			OnResponseRecommandedChannel(uidChannel, szChannelName);
		}
		break;
	case MC_MATCH_STAGE_JOIN:
		{
			CCUID uidChar, uidStage;
			char szStageName[256];

			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(&uidStage, 1, MPT_UID);
			pCommand->GetParameter(szStageName, 2, MPT_STR, sizeof(szStageName) );

			OnStageJoin(uidChar, uidStage, szStageName);
		}
		break;
	case MC_MATCH_STAGE_LEAVE:
		{
			CCUID uidChar, uidStage;

			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(&uidStage, 1, MPT_UID);

			OnStageLeave(uidChar, uidStage);
		}
		break;

	}

	if (m_fnOnCommandCallBack)
		m_fnOnCommandCallBack(this, pCommand);


	return false;

}


void AddToLogFrame(int nDummyID, const char* szStr)
{
	static unsigned long st_nFirstTime = timeGetTime();
	if (!m_pLogFrame) return;
	
	char szTemp[1024];
	sprintf(szTemp, "%u(%d): %s", timeGetTime()-st_nFirstTime, nDummyID, szStr);

	m_pLogFrame->GetCommandList()->Add(szTemp);
	if (m_pLogFrame->GetCommandList()->GetCount() > 200)
	{
		m_pLogFrame->GetCommandList()->Remove(0);
	}

	int t = m_pLogFrame->GetCommandList()->GetCount() - m_pLogFrame->GetCommandList()->GetShowItemCount();
	m_pLogFrame->GetCommandList()->SetStartItem(t);

}


//#endif
