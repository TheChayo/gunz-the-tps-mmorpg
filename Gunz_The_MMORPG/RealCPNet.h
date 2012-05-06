#ifndef REALCPNET_H
#define REALCPNET_H

//////////////////////////////////////////////////////////////////
// Class:	CCRealCPNet class (2001/10/25)
// File:	RealCPNet.cpp
// Author:	Kim young ho (moanus@maiet.net)
//
// Implements Scalable Network Module with I/O Competion Port.
// Code based on MS iocpserver/iocpclient example
////////////////////////////////////////////////////////////////// 

#pragma warning(disable:4786)
#include <list>
#include <algorithm>
using namespace std;

#include <Winsock2.h>
#include <mswsock.h>

#include "CCPacket.h"


// Constants
#define DEFAULT_PORT		5000
#define MAX_BUFF_SIZE		8192
#define MAX_WORKER_THREAD	16


typedef list<CCPacketHeader*>		CCPacketList;
typedef CCPacketList::iterator		CCPacketListItor;
class CCRealCPNet;


typedef enum RCP_IO_OPERATION {
	RCP_IO_NONE,
	RCP_IO_ACCEPT,
	RCP_IO_CONNECT,
	RCP_IO_DISCONNECT,
	RCP_IO_READ,
	RCP_IO_WRITE,
};

class RCPOverlapped : public WSAOVERLAPPED {
protected:
	RCP_IO_OPERATION	m_IOOperation;
public:
	RCPOverlapped(RCP_IO_OPERATION nIO) {	
		ZeroMemory(static_cast<WSAOVERLAPPED*>(this), sizeof(WSAOVERLAPPED));	
		m_IOOperation = nIO;
	}
	virtual ~RCPOverlapped() {}

	RCP_IO_OPERATION GetIOOperation()			{ return m_IOOperation; }
};

class RCPOverlappedSend : public RCPOverlapped {
protected:
	int		m_nTotalBytes;
	int		m_nTransBytes;
	char*	m_pData;
public:
	RCPOverlappedSend() : RCPOverlapped(RCP_IO_WRITE) {
		m_nTotalBytes = 0;
		m_nTransBytes = 0;
		m_pData = NULL;
	}
	virtual ~RCPOverlappedSend() {
		free(m_pData);	m_pData = NULL;
	}
	void SetData(char* pData, int nDataLen) {
		m_pData = pData;
		m_nTotalBytes = nDataLen;
	}
	int GetTotalBytes()				{ return m_nTotalBytes; }
	int GetTransBytes()				{ return m_nTransBytes; }
	void AddTransBytes(int nBytes)	{ m_nTransBytes += nBytes; }
	char* GetData()					{ return m_pData; }
};

class RCPOverlappedRecv : public RCPOverlapped { 
protected:
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	RCPOverlappedRecv() : RCPOverlapped(RCP_IO_READ) {
		m_pBuffer = 0;
		m_nBufferSize = 0;
	}
	void SetBuffer(char* pBuffer, int nBufferSize) {
		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
	char* GetBuffer()				{ return m_pBuffer; }
	int GetBufferSize()				{ return m_nBufferSize; }
};

class RCPOverlappedAccept : public RCPOverlapped {
protected:
	SOCKET	m_Socket;
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	RCPOverlappedAccept() : RCPOverlapped(RCP_IO_ACCEPT) {
		m_Socket = INVALID_SOCKET;
		m_pBuffer = 0;
		m_nBufferSize = 0;
	}
	void SetSocket(SOCKET sd)		{ m_Socket = sd; }
	SOCKET GetSocket()				{ return m_Socket; }
	void SetBuffer(char* pBuffer, int nBufferSize) {
		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
	char* GetBuffer()				{ return m_pBuffer; }
	int GetBufferSize()				{ return m_nBufferSize; }
};

// For AcceptEx, the IOCP key is the CCRealSession for the listening socket,
// so we need to another field SocketAccept in PER_IO_CONTEXT. When the outstanding
// AcceptEx completes, this field is our connection socket handle.
class CCRealSession {
public:
	enum SESSIONSTATE { SESSIONSTATE_IDLE, SESSIONSTATE_ACTIVE, SESSIONSTATE_DEAD };

private:
	SOCKET						m_sdSocket;
	SOCKADDR_IN					m_SockAddr;
	SESSIONSTATE				m_nSessionState;
	void*						m_pUserContext;

public:
	CHAR						m_RecvBuffer[MAX_BUFF_SIZE];	// RCPOverlappedRecv �뵵

public:
	CCRealSession() { 
		m_sdSocket = INVALID_SOCKET;
		ZeroMemory(&m_SockAddr, sizeof(SOCKADDR_IN));
		SetSessionState(SESSIONSTATE_IDLE); 
		SetUserContext(NULL);

		ZeroMemory(m_RecvBuffer, sizeof(CHAR)*MAX_BUFF_SIZE);
	}

	virtual ~CCRealSession() {}

	void SetSocket(SOCKET sd)	{ m_sdSocket = sd; }
	SOCKET	GetSocket()			{ return m_sdSocket; }

	void SetSockAddr(SOCKADDR_IN* pAddr, int nAddrLen) { CopyMemory(&m_SockAddr, pAddr, min(sizeof(SOCKADDR_IN), nAddrLen)); }
	SOCKADDR_IN* GetSockAddr()	{ return &m_SockAddr; }
	char* GetIPString()			{ return inet_ntoa(m_SockAddr.sin_addr); }
	DWORD GetIP()				{ return m_SockAddr.sin_addr.S_un.S_addr; }
	int GetPort()				{ return ntohs(m_SockAddr.sin_port); }
	WORD GetRawPort()			{ return m_SockAddr.sin_port; }

	void SetSessionState(SESSIONSTATE nState)	{ m_nSessionState = nState; }
	SESSIONSTATE GetSessionState()				{ return m_nSessionState; }

	void SetUserContext(void* pContext) { m_pUserContext = pContext; }
	void* GetUserContext()		{ return m_pUserContext; }
};


class CCSessionMap : protected map<SOCKET, CCRealSession*> {
protected:
	CRITICAL_SECTION	m_csLock;

public:
	CCSessionMap()			{ InitializeCriticalSection(&m_csLock); }
	virtual ~CCSessionMap()	{ DeleteCriticalSection(&m_csLock); }

	// Safe Methods ////////////////////////////////////////////////////////
	void Add(CCRealSession* pSession) {
		Lock();
			_ASSERT(pSession->GetSocket() != INVALID_SOCKET);
			insert(CCSessionMap::value_type(pSession->GetSocket(), pSession));
		Unlock();
	}
	bool Remove(SOCKET sd, CCSessionMap::iterator* pNextItor=NULL) {
		bool bResult = false;
		Lock();
			CCSessionMap::iterator i = find(sd);
			if (i!=end()) {
				CCRealSession* pSession = (*i).second;
				delete pSession;
				CCSessionMap::iterator itorTmp = erase(i);
				if (pNextItor)
					*pNextItor = itorTmp;
				bResult = true;
			}
		Unlock();
		return bResult;
	}
	void RemoveAll() {
#ifdef _DEBUG
		OutputDebugString("CCSessionMap::RemoveAll() Proceeding \n");
#endif
		Lock();
			CCSessionMap::iterator itor = begin();
			while( itor != end()) {
				CCRealSession* pSession = (*itor).second;
				delete pSession;
				itor = erase(itor);
			}
		Unlock();
#ifdef _DEBUG
		OutputDebugString("CCSessionMap::RemoveAll() Finished \n");
#endif
	}
	bool IsExist(SOCKET sd) {
		bool bResult = false;
		Lock();
			CCSessionMap::iterator i = find(sd);
			if(i!=end()) bResult = true;				
		Unlock();
		return bResult;
	}
	CCRealSession* GetSession(SOCKET sd) {
		CCRealSession* pSession = NULL;
		Lock();
			CCSessionMap::iterator i = find(sd);
			if(i!=end()) 
				pSession = (*i).second;
		Unlock();
		return pSession;
	}

	// Unsafe Methods /////////////////////////////////////////////////////////////////////
	// ~Unsafe() �뵵 �̿ܿ� ������
	void Lock()		{ EnterCriticalSection(&m_csLock); }
	void Unlock()	{ LeaveCriticalSection(&m_csLock); }

	CCSessionMap::iterator GetBeginItorUnsafe()	{ return begin(); }
	CCSessionMap::iterator GetEndItorUnsafe()	{ return end(); }
	CCRealSession* GetSessionUnsafe(SOCKET sd) {		// �ݵ�� Lock�� Unlock�� �����ؼ� ���
		CCSessionMap::iterator i = find(sd);
		if(i==end()) 
			return NULL;
		else
			return (*i).second;
	}
	bool IsExistUnsafe(CCRealSession* pSession) {
		for (CCSessionMap::iterator i=begin(); i!=end(); i++) {
			CCRealSession* pItorSession = (*i).second;
			if (pItorSession == pSession)
				return true;
		}
		return false;
	}
	bool RemoveUnsafe(SOCKET sd, CCSessionMap::iterator* pNextItor=NULL) {
		bool bResult = false;
		CCSessionMap::iterator i = find(sd);
		if (i!=end()) {
			CCRealSession* pSession = (*i).second;
			delete pSession;
			CCSessionMap::iterator itorTmp = erase(i);
			if (pNextItor)
				*pNextItor = itorTmp;
			bResult = true;
		}
		return bResult;
	}
friend CCRealCPNet;
};


typedef void(RCPCALLBACK)(void* pCallbackContext, RCP_IO_OPERATION nIO, DWORD dwKey, CCPacketHeader* pPacket, DWORD dwPacketLen);


class CCRealCPNet {
private:
	unsigned short		m_nPort;
	BOOL				m_bEndServer;
	BOOL				m_bRestart;
	BOOL				m_bVerbose;
	HANDLE				m_hIOCP;
	SOCKET				m_sdListen;
	DWORD               m_dwThreadCount;
	HANDLE				m_ThreadHandles[MAX_WORKER_THREAD];
	HANDLE				m_hCleanupEvent;

	CCRealSession*		m_pListenSession;
	CCSessionMap			m_SessionMap;

	CRITICAL_SECTION	m_csCrashDump;

	RCPCALLBACK*		m_fnCallback;
	void*				m_pCallbackContext;

private:
	DWORD CrashDump(PEXCEPTION_POINTERS ExceptionInfo);

protected:
	SOCKET CreateSocket();
	BOOL CreateListenSocket( const bool bReuse );
	BOOL CreateAcceptSocket(BOOL fUpdateIOCP);

	CCRealSession* UpdateCompletionPort(SOCKET sd, RCP_IO_OPERATION nOperation, BOOL bAddToList);
	// bAddToList is FALSE for listening socket, and TRUE for connection sockets.
	// As we maintain the context for listening socket in a global structure, we
	// don't need to add it to the list.

	bool PostIOSend(SOCKET sd, char* pBuf, DWORD nBufLen);
	void PostIORecv(SOCKET sd);

	static bool MakeSockAddr(char* pszIP, int nPort, sockaddr_in* pSockAddr);
	bool CheckIPFloodAttack(sockaddr_in* pRemoteAddr, int* poutIPCount);

	CCRealSession* CreateSession(SOCKET sd, RCP_IO_OPERATION ClientIO);
	void DeleteAllSession();

	static DWORD WINAPI WorkerThread(LPVOID WorkContext);

public:
	CCRealCPNet();
	~CCRealCPNet();
	bool Create(int nPort, const bool bReuse = false );
	void Destroy();

	void SetLogLevel(int nLevel) { if (nLevel > 0) m_bVerbose = TRUE; else m_bVerbose = FALSE; }
	void SetCallback(RCPCALLBACK* pCallback, void* pCallbackContext) { m_fnCallback = pCallback; m_pCallbackContext = pCallbackContext; }

	bool Connect(SOCKET* pSocket, char* pszAddress, int nPort);
	void Disconnect(SOCKET sd, bool bIsInCallback = false);
	VOID CloseSession(CCRealSession* pSession, BOOL bGraceful);

	bool GetAddress(SOCKET sd, char* pszAddress, int* pPort);
	void* GetUserContext(SOCKET sd);
	void SetUserContext(SOCKET sd, void* pContext);

	bool Send(SOCKET sd, CCPacketHeader* pPacket, int nSize);	/// Packet�� malloc free ���

friend RCPCALLBACK;
};


typedef void(RCPLOGFUNC)(const char *pFormat,...);
void SetupRCPLog(RCPLOGFUNC* pFunc);

#ifdef REALCPNET_LINK_SOCKET_LIBS
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "mswsock.lib")
#endif


#endif
