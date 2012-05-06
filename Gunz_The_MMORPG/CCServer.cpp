#include "stdafx.h"
#include "CCServer.h"
#include "CCSharedCommandTable.h"
#include "CCCommandBuilder.h"
#include <stdarg.h>
#include <windowsx.h>
#include "CCErrorTable.h"
#include "CCCRC32.h"
// #include "CCPacketHShieldCrypter.h"
#include "CCMatchConfig.h"

static int g_LogCommObjectCreated = 0;
static int g_LogCommObjectDestroyed = 0;

void __cdecl RCPLog(const char *pFormat,...)
{
	char szBuf[256];

	va_list args;

	va_start(args,pFormat);
	vsprintf(szBuf, pFormat, args);
	va_end(args);

	int nEnd = (int)(strlen(szBuf)-1);
	if ((nEnd >= 0) && (szBuf[nEnd] == '\n')) {
		szBuf[nEnd] = NULL;
		strcat(szBuf, "\n");
	}
	OutputDebugString(szBuf);
}

MServer::MServer()
{
	SetName("NoName");	// For Debug
#ifdef _DEBUG
	SetupRCPLog(RCPLog);
#endif
}

MServer::~MServer()
{
}

bool MServer::Create(int nPort, const bool bReuse )
{
	InitializeCriticalSection(&m_csSafeCmdQueue);
	InitializeCriticalSection(&m_csCommList);
	InitializeCriticalSection(&m_csAcceptWaitQueue);
	
	bool bResult = true;

	if(CCCommandCommunicator::Create()==false) 
	{
		cclog( "MServer::Create - CCCommandCommunicator::Create()==false\n" );
		bResult = false;
	}
	if(m_RealCPNet.Create(nPort, bReuse)==false) 
	{
		cclog( "MServer::Create - m_RealCPNet.Create(%u)==false", nPort );
		bResult = false;
	}

	m_RealCPNet.SetCallback(MServer::RCPCallback, this);

	return bResult;
}

void MServer::Destroy()
{
	// Log 
	RCPLog("MServer::Destroy() CommObjectCreated=%d, CommObjectDestroyed=%d \n", 
		g_LogCommObjectCreated, g_LogCommObjectDestroyed);

	m_RealCPNet.Destroy();

	LockCommList();
		for(CCUIDRefCache::iterator i=m_CommRefCache.begin(); i!=m_CommRefCache.end(); i++){
			delete (CCCommObject*)(((*i).second));
		}
		m_CommRefCache.clear();
	UnlockCommList();

	CCCommandCommunicator::Destroy();

	DeleteCriticalSection(&m_csAcceptWaitQueue);
	DeleteCriticalSection(&m_csCommList);
	DeleteCriticalSection(&m_csSafeCmdQueue);
}

int MServer::GetCommObjCount()	
{ 
	int count = 0;
	LockCommList();
		count = (int)m_CommRefCache.size(); 
	UnlockCommList();

	return count;
}

void MServer::AddCommObject(const CCUID& uid, CCCommObject* pCommObj)
{
	CCCommObject* pOldCommObj = (CCCommObject*)m_CommRefCache.GetRef(uid);
	if (pOldCommObj != NULL)
	{
		return;
	}

	pCommObj->SetUID(uid);
	CCCommandBuilder* pCmdBuilder = pCommObj->GetCommandBuilder();
	pCmdBuilder->SetUID(GetUID(), uid);

	m_CommRefCache.insert(CCUIDRefCache::value_type(uid, pCommObj));
	g_LogCommObjectCreated++;
}

void MServer::RemoveCommObject(const CCUID& uid)
{
	CCCommObject* pNew = (CCCommObject*)m_CommRefCache.Remove(uid);
	if (pNew) delete pNew;
	g_LogCommObjectDestroyed++;
}

void MServer::InitCryptCommObject(CCCommObject* pCommObj, unsigned int nTimeStamp)
{
	CCPacketCrypterKey key;

	CCMakeSeedKey(&key, m_This, pCommObj->GetUID(), nTimeStamp);
	pCommObj->GetCrypter()->InitKey(&key);
	pCommObj->GetCommandBuilder()->InitCrypt(pCommObj->GetCrypter(), true);
}

void MServer::PostSafeQueue(CCCommand* pNew)
{
	LockSafeCmdQueue();
	{
		m_SafeCmdQueue.push_back(pNew);
	}
	UnlockSafeCmdQueue();
}

void MServer::PostSafeQueue(CCCommandBuilder* pCommandBuilder)
{
	LockSafeCmdQueue();
	{
		CCCommand *pNewCmd = NULL;
		while(pNewCmd = pCommandBuilder->GetCommand()) {
			m_SafeCmdQueue.push_back(pNewCmd);
		}
	}		
	UnlockSafeCmdQueue();
}

void MServer::SendCommand(CCCommand* pCommand)
{
	_ASSERT(pCommand->GetReceiverUID().High || pCommand->GetReceiverUID().Low);

	DWORD nClientKey = NULL;
	bool bGetComm = false;

	CCPacketCrypterKey CrypterKey;

	LockCommList();
		CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(pCommand->m_Receiver);
		if(pCommObj){
			nClientKey = pCommObj->GetUserContext();
			memcpy(&CrypterKey, pCommObj->GetCrypter()->GetKey(), sizeof(CCPacketCrypterKey));
			bGetComm = true;
		}
	UnlockCommList();

	if (bGetComm == false) return;

	int nSize = pCommand->GetSize();
	if ((nSize <= 0) || (nSize >= MAX_PACKET_SIZE)) return;

	char CmdData[MAX_PACKET_SIZE];
	nSize = pCommand->GetData(CmdData, nSize);

	if(pCommand->m_pCommandDesc->IsFlag(MCCT_NON_ENCRYPTED))
	{
		SendMsgCommand(nClientKey, CmdData, nSize, MSGID_RAWCOMMAND, NULL);
	}
	else 
	{
		// Ŀ�ǵ� ��ȣȭ
		SendMsgCommand(nClientKey, CmdData, nSize, MSGID_COMMAND, &CrypterKey);
	}
}

void MServer::OnPrepareRun()
{
	LockSafeCmdQueue();
		CCCommandList::iterator itorCmd;
		while ( (itorCmd = m_SafeCmdQueue.begin()) != m_SafeCmdQueue.end()) {
			CCCommand* pCmd = (*itorCmd);
			m_SafeCmdQueue.pop_front();
			GetCommandManager()->Post(pCmd);
		}
	UnlockSafeCmdQueue();
}

void MServer::OnRun()
{

}

bool MServer::OnCommand(CCCommand* pCommand)
{
	switch(pCommand->GetID()){
	case MC_LOCAL_LOGIN:
		{
			CCUID uidComm, uidPlayer;
			pCommand->GetParameter(&uidComm, 0, MPT_UID);
			pCommand->GetParameter(&uidPlayer, 1, MPT_UID);
			OnLocalLogin(uidComm, uidPlayer);
			return true;
		}
		break;

	case MC_NET_ECHO:
		{
			char szMessage[256];
			if (pCommand->GetParameter(szMessage, 0, MPT_STR, sizeof(szMessage) )==false) break;
			CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_NET_ECHO), pCommand->m_Sender, m_This);
			pNew->AddParameter(new CCCommandParameterString(szMessage));
			Post(pNew);
			return true;
		}
		break;

	case MC_NET_CLEAR:
		{
			CCUID uid;
			if (pCommand->GetParameter(&uid, 0, MPT_UID)==false) break;
			OnNetClear(uid);
			return true;
		}
		break;

	case MC_NET_CHECKPING:
		{
			CCUID uid;
			if (pCommand->GetParameter(&uid, 0, MPT_UID)==false) break;
			CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_NET_PING), uid, m_This);
			pNew->AddParameter(new CCCommandParameterUInt(timeGetTime()));
			Post(pNew);
			return true;
		}
		break;

	case MC_NET_PING:
		{
			unsigned int nTimeStamp;
			if (pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT)==false) break;
			CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_NET_PONG), pCommand->m_Sender, m_This);
			pNew->AddParameter(new CCCommandParameterUInt(nTimeStamp));
			Post(pNew);
			return true;
		}
		break;

	case MC_NET_PONG:
		{
			unsigned int nTimeStamp;
			pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);

			OnNetPong(pCommand->GetSenderUID(), nTimeStamp);

			LOG(LOG_DEBUG, "Ping from (%u:%u) = %d", 
				pCommand->GetSenderUID().High, pCommand->GetSenderUID().Low, timeGetTime()-nTimeStamp);
			return true;
		}
		break;
	};
	return false;
}

void MServer::OnNetClear(const CCUID& ComCCUID)
{
	LockCommList();
		RemoveCommObject(ComCCUID);
	UnlockCommList();
}

void MServer::OnNetPong(const CCUID& ComCCUID, unsigned int nTimeStamp)
{
}

int MServer::Connect(CCCommObject* pCommObj)
{
	/// Connecting Operation
	/// ...
	DWORD nKey = 0;
	if (m_RealCPNet.Connect((SOCKET*)&nKey, pCommObj->GetIPString(), pCommObj->GetPort()) == false)
	{
		delete pCommObj;
		return MERR_UNKNOWN;
	}

	// UID Caching
	LockCommList();
		AddCommObject(pCommObj->GetUID(), pCommObj);
		pCommObj->SetUserContext(nKey);
		m_RealCPNet.SetUserContext(nKey, pCommObj);	
	UnlockCommList();

	return MOK;
}

int MServer::ReplyConnect(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj)
{
	if (SendMsgReplyConnect(pTargetUID, pAllocUID, nTimeStamp, pCommObj) == true)
		return MOK;
	else
		return MERR_UNKNOWN;	
}

int MServer::OnAccept(CCCommObject* pCommObj)
{
	// �Ҵ��� �� �ִ� UID ������ ����.
	CCUID AllocUID = UseUID();
	if(AllocUID.IsInvalid()){
		Log(LOG_DEBUG, "Communicator has not UID space to allocate your UID.");
		return MERR_COMMUNICATOR_HAS_NOT_UID_SPACE;
	}

	pCommObj->SetUID(AllocUID);

	LockAcceptWaitQueue();
		m_AcceptWaitQueue.push_back(pCommObj);
	UnlockAcceptWaitQueue();

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_LOCAL_LOGIN), m_This, m_This);
	pNew->AddParameter(new CCCommandParameterUID(pCommObj->GetUID()));
	pNew->AddParameter(new CCCommandParameterUID(CCUID(0,0)));
	PostSafeQueue(pNew);

	return MOK;
}

// Login������ �ϼ��Ǹ� Player������ PlayerUID���. �ӽ÷� ������ AllocUID�����
void MServer::OnLocalLogin(CCUID ComCCUID, CCUID PlayerUID)
{
	CCCommObject* pCommObj = NULL;

	LockAcceptWaitQueue();
	for (list<CCCommObject*>::iterator i = m_AcceptWaitQueue.begin(); i!= m_AcceptWaitQueue.end(); i++) {
		CCCommObject* pTmpObj = (*i);
		if (pTmpObj->GetUID() == ComCCUID) {
			pCommObj = pTmpObj;
			m_AcceptWaitQueue.erase(i);
			break;
		}
	}
	UnlockAcceptWaitQueue();

	if (pCommObj == NULL) 
		return;

	unsigned int nTimeStamp = (timeGetTime() * 103) - 234723;

	LockCommList();
		AddCommObject(pCommObj->GetUID(), pCommObj);
		InitCryptCommObject(pCommObj, nTimeStamp);
	UnlockCommList();

	int nResult = ReplyConnect(&m_This, &ComCCUID, nTimeStamp, pCommObj);

/*	char szMsg[128];
	sprintf(szMsg, "Accept from %s:%d \n", pCommObj->GetIP(), pCommObj->GetPort());
	LOG(LOG_DEBUG, szMsg); */
}

void MServer::Disconnect( const CCUID& uid )
{
	DWORD nClientKey = NULL;
	bool bGetComm = false;

	LockCommList();
		CCCommObject* pCommObj = (CCCommObject*)m_CommRefCache.GetRef(uid);
		if(pCommObj){
			nClientKey = pCommObj->GetUserContext();
			bGetComm = true;
		}
	UnlockCommList();

	if (bGetComm == false) 
		return;

//	OnDisconnect(uid);
	m_RealCPNet.Disconnect(nClientKey);
}

int MServer::OnDisconnect(const CCUID& uid)
{
	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_NET_CLEAR), m_This, m_This);
	pNew->AddParameter(new CCCommandParameterUID(uid));
	PostSafeQueue(pNew);
	
	return MOK;
}

bool MServer::SendMsgReplyConnect(CCUID* pHostUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj)
{
	DWORD nKey = pCommObj->GetUserContext();
	
	MReplyConnectMsg* pMsg = (MReplyConnectMsg*)malloc(sizeof(MReplyConnectMsg));
	pMsg->nMsg = MSGID_REPLYCONNECT; 
	pMsg->nSize = sizeof(MReplyConnectMsg);
	pMsg->nHostHigh = pHostUID->High;
	pMsg->nHostLow = pHostUID->Low;
	pMsg->nAllocHigh = pAllocUID->High;
	pMsg->nAllocLow = pAllocUID->Low;
	pMsg->nTimeStamp = nTimeStamp;

	return m_RealCPNet.Send(nKey, pMsg, pMsg->nSize);
}

bool MServer::SendMsgCommand(DWORD nClientKey, char* pBuf, int nSize, unsigned short nMsgHeaderID, CCPacketCrypterKey* pCrypterKey)
{
	int nBlockSize = nSize+sizeof(CCPacketHeader);
	CCCommandMsg* pMsg = (CCCommandMsg*)malloc(nBlockSize);
	pMsg->Buffer[0] = 0;
	pMsg->nCheckSum = 0;	// CheckSum�� ���ԵǱ� ������ ���� �ʱ�ȭ 
	pMsg->nMsg = nMsgHeaderID;;
	pMsg->nSize = nBlockSize;
	int nPacketSize = nBlockSize;

	if (nMsgHeaderID == MSGID_RAWCOMMAND)
	{
		CopyMemory(pMsg->Buffer, pBuf, nSize);
	}
	else if (nMsgHeaderID == MSGID_COMMAND)
	{
		if ((nSize > MAX_PACKET_SIZE) || (pCrypterKey == NULL)) return false;

//#ifdef _HSHIELD
//		// �ٽǵ� ��ȣȭ
//		if (CCPacketHShieldCrypter::Encrypt((PBYTE)&pMsg->nSize, sizeof(unsigned short)) != ERROR_SUCCESS)
//			return false;   
//#else
		// size ��ȣȭ
		if (!CCPacketCrypter::Encrypt((char*)&pMsg->nSize, sizeof(unsigned short), pCrypterKey))
			return false;
//#endif
		char SendBuf[MAX_PACKET_SIZE];

		// Ŀ�ǵ� ��ȣȭ
		if (!CCPacketCrypter::Encrypt(pBuf, nSize, SendBuf, nSize, pCrypterKey))
			return false;

		CopyMemory(pMsg->Buffer, SendBuf, nSize);			
	}
	else
	{
		_ASSERT(0);
		return false;
	}

	pMsg->nCheckSum = MBuildCheckSum(pMsg, nPacketSize);

	return m_RealCPNet.Send(nClientKey, pMsg, nPacketSize);
}

struct SOCKADDR_EXT : public SOCKADDR_IN {
	char sin_ext[16];
};

void MServer::RCPCallback(void* pCallbackContext, RCP_IO_OPERATION nIO, DWORD nKey, CCPacketHeader* pPacket, DWORD dwPacketLen)
{
	MServer* pServer = (MServer*)pCallbackContext;

	if (nIO == RCP_IO_ACCEPT) {
		//pServer->DebugLog("MServer::RCPCallback(RCP_IO_ACCEPT) \n");

		char szIP[64] = "";
		int nPort = 0;

		pServer->m_RealCPNet.GetAddress(nKey, szIP, &nPort);

		CCCommObject* pCommObj = new CCCommObject(pServer);
		pCommObj->SetAddress(szIP, nPort);
		pCommObj->SetUserContext(nKey);
		pCommObj->SetPassiveSocket(true);

		pServer->OnAccept(pCommObj);
			
		pServer->m_RealCPNet.SetUserContext(nKey, pCommObj);
	}
	else if (nIO == RCP_IO_CONNECT) {
		//pServer->DebugLog("MServer::RCPCallback(RCP_IO_CONNECT) \n");
	}
	else if (nIO == RCP_IO_DISCONNECT) {
		//pServer->DebugLog("MServer::RCPCallback(RCP_IO_DISCONNECT) \n");

		CCCommObject* pCommObj = (CCCommObject*)pServer->m_RealCPNet.GetUserContext(nKey);
		if (pCommObj) {
			pServer->OnDisconnect(pCommObj->GetUID());
		}
	}
	else if (nIO == RCP_IO_READ) {
		//pServer->DebugLog("MServer::RCPCallback(RCP_IO_READ) \n");

		bool bFloodCheck = pServer->IsFloodCheck();
		pServer->LockCommList();
		CCCommObject* pCommObj = (CCCommObject*)pServer->m_RealCPNet.GetUserContext(nKey);
		if ((pCommObj) && (pCommObj->IsAllowed()))
		{
			// New Cmd Buffer ////////////////
			
			// 080528 LockCommList �߰�
			CCCommandBuilder* pCmdBuilder = pCommObj->GetCommandBuilder();

			bool bIsFlooding = false;
			if (!pCmdBuilder->Read((char*)pPacket, dwPacketLen, bFloodCheck, &bIsFlooding))
			{
				// 080528 Read�� false �����ϸ� SessionMap Disconnect���� ������ɸ��� �ִ�.
				// ��Ŷ�� ����� �ȿ��� ���������.

				char szIP[64] = "";
				int nPort = 0;
				pServer->m_RealCPNet.GetAddress(nKey, szIP, &nPort);

				if( bIsFlooding ) {	
					// Flooding ����� ����! 1�ð����� Ban�ϴ� �κ��� ������!
					CCCommand* pNew = new CCCommand(pServer->m_CommandManager.GetCommandDescByID(MC_NET_BANPLAYER_FLOODING), pServer->m_This, pServer->m_This);
					pNew->AddParameter(new CCCommandParameterUID(pCommObj->GetUID()));
					pServer->PostSafeQueue(pNew);

					pCommObj->SetAllowed(false);				
					pServer->m_RealCPNet.Disconnect(pCommObj->GetUserContext(), true);

					pServer->LOG(LOG_FILE, "%s, %d is Disconnect(RCP_IO_READ Error) - Flood\n", szIP, nPort);
				}
				else {
					pCommObj->SetAllowed(false);				
					pServer->m_RealCPNet.Disconnect(pCommObj->GetUserContext(), true);

					pServer->LOG(LOG_FILE, "%s, %d is Disconnect(RCP_IO_READ Error)\n", szIP, nPort);
				}								
			} 
			else 
			{
				pServer->PostSafeQueue(pCmdBuilder);

				while(CCPacketHeader* pNetCmd = pCmdBuilder->GetNetCommand()) {
					if (pNetCmd->nMsg == MSGID_REPLYCONNECT) {
						MReplyConnectMsg* pMsg = (MReplyConnectMsg*)pNetCmd;
						CCUID HostUID, AllocUID;
						unsigned int nTimeStamp;

						HostUID.High = pMsg->nHostHigh;
						HostUID.Low = pMsg->nHostLow;
						AllocUID.High = pMsg->nAllocHigh;
						AllocUID.Low = pMsg->nAllocLow;
						nTimeStamp = pMsg->nTimeStamp;

						free(pNetCmd);

						pServer->OnConnected(&HostUID, &AllocUID, nTimeStamp, pCommObj);
					}
				}
			}
		}
		pServer->UnlockCommList();
	}
	else if (nIO == RCP_IO_WRITE) {
		//pServer->DebugLog("MServer::RCPCallback(RCP_IO_WRITE) \n");
	}

}
