#ifndef _MCUSTOMCLIENT_H
#define _MCUSTOMCLIENT_H


/// Ŭ���̾�Ʈ
class CCCustomClient
{
private:

protected:
	CCClientSocket		m_ClientSocket;			///< Ŭ���̾�Ʈ ���Ͽ� Ŭ����
	CRITICAL_SECTION	m_csRecvLock;			///< CommandQueue critical section
protected:
	void LockRecv() { EnterCriticalSection(&m_csRecvLock); }
	void UnlockRecv() { LeaveCriticalSection(&m_csRecvLock); }

	// Socket Event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
public:
	CCCustomClient();
	virtual ~CCCustomClient();
	CCClientSocket* GetClientSocket()						{ return &m_ClientSocket; }

	virtual int Connect(char* szIP, int nPort);
	void Send(char* pBuf, DWORD nSize);

	static bool SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize);
	static bool SocketConnectEvent(void* pCallbackContext, SOCKET sock);
	static bool SocketDisconnectEvent(void* pCallbackContext, SOCKET sock);
	static void SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	bool IsConnected() { return m_ClientSocket.IsActive(); }
	CCClientSocket* GetSock() { return &m_ClientSocket; }
};









#endif