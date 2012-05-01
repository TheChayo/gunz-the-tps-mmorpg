#pragma once
#include "CCCommandCommunicator.h"
#include "CCPacketCrypter.h"

class CCClientCommandProcessor;
class CCCommand;

#define RINGBUFSIZE 16384

// ���߿� ��Ŷ ���� ��ü�� ����� �������� �켱 �����д�.
class CCRingBuffer {
private:
	int			m_iBufSize;
	char*		m_Buf;
	char*		m_cpBegin, m_cpEnd;
protected:
public:
	CCRingBuffer();
	CCRingBuffer(int iBufSize);
	virtual ~CCRingBuffer();
	void Reserve(int iBufSize); 
	bool Enqueue(char* cp, int iDataSize);
	bool Dequeue(char* cpOut, int iDataSize);
	int GetSize()		{ return m_iBufSize; }
};

/// Message Type
enum CCZMOMType{
	CCZMDM_GENERAL,			///< �Ϲ� �޼���
	CCZMOM_USERCOMMAND,		///< ����� �Է� Ŀ�ǵ�
	CCZMOM_ERROR,			///< ����
	CCZMOM_LOCALREPLY,		///< ���� ���� �޼���
	CCZMOM_SERVERREPLY,		///< ���� ���� �޼���
};


/// Ŭ���̾�Ʈ
class CCClient : public CCCommandCommunicator{
private:
	static CCClient*	m_pInstance;				///< ���� �ν��Ͻ�
protected:
	CCUID				m_Server;				///< ����� Ŀ�´�������

	CCClientSocket		m_ClientSocket;			///< Ŭ���̾�Ʈ ���Ͽ� Ŭ����
	CRITICAL_SECTION	m_csRecvLock;			///< CommandQueue critical section

	CCCommandBuilder*	m_pCommandBuilder;
	CCPacketCrypter		m_ServerPacketCrypter;	///< MatchServer���� ��ȣȭ Ŭ����

protected:
	void LockRecv() { EnterCriticalSection(&m_csRecvLock); }
	void UnlockRecv() { LeaveCriticalSection(&m_csRecvLock); }

	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	virtual bool OnCommand(CCCommand* pCommand);

	/// ���� ������ ����Ѵ�.
	virtual void OutputLocalInfo(void) = 0;
	/// �⺻ ���
	virtual void OutputMessage(const char* szMessage, CCZMOMType nType=CCZMDM_GENERAL) = 0;

	CCCommandBuilder* GetCommandBuilder()	{ return m_pCommandBuilder; }
	virtual void SendCommand(CCCommand* pCommand);
	virtual CCCommand* GetCommandSafe();

	virtual int OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);
	virtual int OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp);
	int MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, CCPacketCrypter* pPacketCrypter, CCCommand* pCommand);
	

	// Socket Event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

public:
	CCClient();
	virtual ~CCClient();

	/// ���� �ν��Ͻ� ���
	static CCClient* GetInstance(void);

	CCUID GetServerUID(void){ return m_Server; }
	virtual CCUID GetSenderUIDBySocket(SOCKET socket);
	CCClientSocket* GetClientSocket()						{ return &m_ClientSocket; }
	void GetTraffic(int* nSendTraffic, int* nRecvTraffic)	{ return m_ClientSocket.GetTraffic(nSendTraffic, nRecvTraffic); }

	virtual bool Post(CCCommand* pCommand);
	virtual bool Post(char* szErrMsg, int nErrMsgCount, const char* szCommand);

	virtual int Connect(CCCommObject* pCommObj);
	virtual int Connect(SOCKET* pSocket, char* szIP, int nPort);
	virtual void Disconnect( const CCUID&  uid );
	virtual void Log(const char* szLog){}

	/// �Ķ���� ���
	void OutputMessage(CCZMOMType nType, const char *pFormat,...);

	static bool SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize);
	static bool SocketConnectEvent(void* pCallbackContext, SOCKET sock);
	static bool SocketDisconnectEvent(void* pCallbackContext, SOCKET sock);
	static void SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	bool IsConnected() { return m_ClientSocket.IsActive(); }
};

void SplitIAddress(char* szIP, int* pPort, const char* szAddress);

