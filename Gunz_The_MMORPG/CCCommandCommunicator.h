#ifndef MCOMMANDCOMMUNICATOR_H
#define MCOMMANDCOMMUNICATOR_H

#include "CCCommandManager.h"
#include "CCUID.h"
#include "CCTCPSocket.h"
#include "CCPacketCrypter.h"

//#define _CMD_PROFILE

#ifdef _CMD_PROFILE
#include "CCCommandProfiler.h"
#endif


class CCCommandCommunicator;
class CCCommandBuilder;


/// Ŀ�´������Ϳ��� ������ ���� ��ü. CCCommandCommunicator::Connect()�� �Ķ���ͷ� ����.
class CCCommObject {
protected:
	CCUID					m_uid;

	CCCommandBuilder*		m_pCommandBuilder;
	CCPacketCrypter			m_PacketCrypter;

	CCCommandCommunicator*	m_pDirectConnection;	// ������ Ŀ�ؼ� Direction Connection
	DWORD					m_dwUserContext;

	char					m_szIP[128];
	int						m_nPort;
	DWORD					m_dwIP;
	bool					m_bAllowed;

	bool					m_bPassiveSocket;
public:
	CCCommObject(CCCommandCommunicator* pCommunicator);
	virtual ~CCCommObject();

	CCUID GetUID()			{ return m_uid; }
	void SetUID(CCUID uid)	{ m_uid = uid; }

	CCCommandBuilder*	GetCommandBuilder()				{ return m_pCommandBuilder; }
	CCPacketCrypter*		GetCrypter()					{ return &m_PacketCrypter; }

	CCCommandCommunicator* GetDirectConnection()			{ return m_pDirectConnection; }
	void SetDirectConnection(CCCommandCommunicator* pDC)	{ m_pDirectConnection = pDC; }
	DWORD GetUserContext()								{ return m_dwUserContext; }
	void SetUserContext(DWORD dwContext)				{ m_dwUserContext = dwContext; }

	char* GetIPString()		{ return m_szIP; }
	DWORD GetIP()			{ return m_dwIP; }
	int GetPort()			{ return m_nPort; }
	void SetAddress(const char* pszIP, int nPort) {
		strcpy(m_szIP, pszIP);
		m_dwIP = inet_addr(m_szIP);
		m_nPort = nPort;
	}
	void SetAllowed(bool bAllowed) { m_bAllowed = bAllowed; }
	bool IsAllowed() { return m_bAllowed; }

	// ���� �����ص� ������, ���� IOCP�� ������� ���Ͽ� ���� �κ�
	// Passive��, ����ڿ� ���� ������ CommObject�� �ƴ�, IOCP�󿡼� �ڵ����� �����Ǵ� CommObject�� �ǹ�.
	// (RCP_IO_ACCEPT���� ������, ���������� �����Ǵ� ����) 
	//																2010. 04. 06 - ȫ����
	void SetPassiveSocket(bool bActive) { m_bPassiveSocket = bActive; }
	bool IsPassiveSocket() { return m_bPassiveSocket; }

};


class CCPacketInfo {
public:
	CCCommObject*		m_pCommObj;
	CCPacketHeader*		m_pPacket;

	CCPacketInfo(CCCommObject* pCommObj, CCPacketHeader* pPacket) { m_pCommObj = pCommObj, m_pPacket = pPacket; }
};
typedef list<CCPacketInfo*>			CCPacketInfoList;
typedef CCPacketInfoList::iterator	CCPacketInfoListItor;


/// Ŀ�ǵ� ����� ����� ���������� Ŀ�´�������
class CCCommandCommunicator{
protected:
	CCCommandManager	m_CommandManager;		///< Ŀ�ǵ� �Ŵ���

	CCUID			m_This;					///< �ڱ� Ŀ�´������� UID
	CCUID			m_DefaultReceiver;		///< Ŀ�ǵ带 �Ľ��Ҷ� �⺻�� �Ǵ� Ÿ�� Ŀ�´������� UID

protected:
	/// Low-Level Command Transfer Function. ���߿� ��Ƶξ��ٰ� �� ���۵��� �����ϰ� ���� �� �ִ�.
	virtual void SendCommand(CCCommand* pCommand)=0;
	/// Low-Level Command Transfer Function. ���߿� ��Ƶξ��ٰ� �� ���۵��� �����ϰ� ���� �� �ִ�.
	virtual void ReceiveCommand(CCCommand* pCommand);

	/// �ʱ� Ŀ�ǵ� ����Ҷ� �Ҹ��� �Լ�
	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	/// Ŀ�´������Ͱ� Ŀ�ǵ带 ó���ϱ� ���� �Ҹ��� �Լ�
	virtual bool OnCommand(CCCommand* pCommand);
	/// Ŀ�´������� ���� �� �غ�
	virtual void OnPrepareRun(void);
	/// Ŀ�ǵ带 ó���ϱ� ����
	virtual void OnPrepareCommand(CCCommand* pCommand);
	/// Ŀ�´������� ����
	virtual void OnRun(void);

	/// Post()�Ǵ� �⺻ ���ù� Ŀ�´������� ����
	void SetDefaultReceiver(CCUID Receiver);
public:
	CCCommandCommunicator(void);
	virtual ~CCCommandCommunicator(void);

	/// �ʱ�ȭ
	bool Create(void);
	/// ����
	void Destroy(void);

	/// �ٸ� Ŀ��Ƽ�����ͷ� ���� ����
	/// @param	pAllocUID	�ڱ� Communicator�� �������� UID
	/// @return				���� �ڵ� (MErrorTable.h ����)
	virtual int Connect(CCCommObject* pCommObj)=0;
	/// Ŀ�ؼ��� �̷���� ���
	virtual int OnConnected(CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp, CCCommObject* pCommObj);
	/// ���� ����
	virtual void Disconnect( const CCUID& uid)=0;

	/// Ŀ�ǵ� �Է�
	virtual bool Post(CCCommand* pCommand);
	/// ��Ʈ������ Ŀ�ǵ� �Է�
	virtual bool Post(char* szErrMsg, int nErrMsgCount, const char* szCommand);

	virtual CCCommand* GetCommandSafe();

	/// ���� ( Ŀ�ǵ� ���۹� ó�� )
	void Run(void);

	/// Ŀ�ǵ� �Ŵ��� ���
	CCCommandManager* GetCommandManager(void){
		return &m_CommandManager;
	}
	CCCommand* CreateCommand(int nCmdID, const CCUID& TargetUID);

	// �ǹ̰� �����ǷΤ� LOG_PROG ���ֹ�����

	enum _LogLevel	{ LOG_DEBUG = 1, LOG_FILE = 3, LOG_PROG = 7, }; //, LOG_PROG = 7,  };
	/// ����Ʈ �α� ��� ( �α� ������ �����̷����� �� �ְ� ���ش�. )
	virtual void Log(unsigned int nLogLevel, const char* szLog){}
	void LOG(unsigned int nLogLevel, const char *pFormat,...);

	/// �ڽ��� UID�� ����.
	CCUID GetUID(void){ return m_This; }

#ifdef _CMD_PROFILE
	CCCommandProfiler		m_CommandProfiler;
#endif
};

int CalcPacketSize(CCCommand* pCmd);

#endif
