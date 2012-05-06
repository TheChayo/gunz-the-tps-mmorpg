#ifndef MSERVER_H
#define MSERVER_H

#include "CCCommandCommunicator.h"
#include "RealCPNet.h"

#include <list>
using namespace std;

class CCCommand;

/// ����
class MServer : public CCCommandCommunicator {
protected:
	CCRealCPNet					m_RealCPNet;

	list<CCCommObject*>			m_AcceptWaitQueue;
	CRITICAL_SECTION			m_csAcceptWaitQueue;

	void LockAcceptWaitQueue()		{ EnterCriticalSection(&m_csAcceptWaitQueue); }
	void UnlockAcceptWaitQueue()		{ LeaveCriticalSection(&m_csAcceptWaitQueue); }

	CCUIDRefCache				m_CommRefCache;			///< ���� ������ ������ �ٸ� Ŀ�´������� ĳ��
	CRITICAL_SECTION			m_csCommList;

	void LockCommList()			{ EnterCriticalSection(&m_csCommList); }
	void UnlockCommList()		{ LeaveCriticalSection(&m_csCommList); }

	CCCommandList				m_SafeCmdQueue;
	CRITICAL_SECTION			m_csSafeCmdQueue;
	void LockSafeCmdQueue()		{ EnterCriticalSection(&m_csSafeCmdQueue); }
	void UnlockSafeCmdQueue()	{ LeaveCriticalSection(&m_csSafeCmdQueue); }

	/// ���ο� UID ����
	// virtual CCUID UseUID() = 0;
	virtual CCUID UseUID() { return CCUID(0, 0); }

	void AddCommObject(const CCUID& uid, CCCommObject* pCommObj);
	void RemoveCommObject(const CCUID& uid);
	void InitCryptCommObject(CCCommObject* pCommObj, unsigned int nTimeStamp);

	void PostSafeQueue(CCCommand* pNew);
	void PostSafeQueue(CCCommandBuilder* pCommandBuilder);

	/// Low-Level Command Transfer Function. ���߿� ��Ƶξ��ٰ� �� ���۵��� �����ϰ� ���� �� �ִ�.
	void SendCommand(CCCommand* pCommand);
	void ParsePacket(CCCommObject* pCommObj, CCPacketHeader* pPacket);

	/// Ŀ�´������� ���� �� �غ�
	virtual void  OnPrepareRun();
	/// Ŀ�´������� ����
	virtual void OnRun();
	/// ����� Ŀ�ǵ� ó��
	virtual bool OnCommand(CCCommand* pCommand);

	virtual void OnNetClear(const CCUID& ComCCUID);
	virtual void OnNetPong(const CCUID& ComCCUID, unsigned int nTimeStamp);
	virtual void OnHShieldPong(const CCUID& ComCCUID, unsigned int nTimeStamp) {};

	bool SendMsgReplyConnect(CCUID* pHostUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);
	bool SendMsgCommand(DWORD nClientKey, char* pBuf, int nSize, unsigned short nMsgHeaderID, CCPacketCrypterKey* pCrypterKey);

	static void RCPCallback(void* pCallbackContext, RCP_IO_OPERATION nIO, DWORD nKey, CCPacketHeader* pPacket, DWORD dwPacketLen);	// Thread not safe

	bool m_bFloodCheck;

public:	// For Debugging
	char m_szName[128];
	void SetName(char* pszName) { strcpy(m_szName, pszName); }
	void DebugLog(char* pszLog) {
		#ifdef _DEBUG
		char szLog[128];
		wsprintf(szLog, "[%s] %s \n", m_szName, pszLog);
		OutputDebugString(szLog);
		#endif
	}

public:
	MServer();
	~MServer();

	/// �ʱ�ȭ
	bool Create(int nPort, const bool bReuse = false );
	/// ����
	void Destroy();
	int GetCommObjCount();


	/// �ٸ� Ŀ��Ƽ�����ͷ� ���� ����
	/// @param	pAllocUID	�ڱ� Communicator�� �������� UID
	/// @return				���� �ڵ� (MErrorTable.h ����)
	virtual int Connect(CCCommObject* pCommObj);	// ������н� �ݵ�� Disconnect() ȣ���ؾ���
	int ReplyConnect(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);
	virtual int OnAccept(CCCommObject* pCommObj);
	/// �α��εǾ�����
	virtual void OnLocalLogin(CCUID ComCCUID, CCUID PlayerUID);
	/// ���� ����
	virtual void Disconnect( const CCUID& uid );	
	virtual int OnDisconnect(const CCUID& uid);	// Thread not safe

	virtual void Log(unsigned int nLogLevel, const char* szLog){}

	void SetFloodCheck(bool bVal)	{ m_bFloodCheck = bVal; }			///< (ũ�� ������ ���� �ʰ�����) �����ڿ����� ȣ�����ֵ��� �սô�.
	bool IsFloodCheck()				{ return m_bFloodCheck; }
};

#endif
