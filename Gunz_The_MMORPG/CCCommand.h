#ifndef MCOMMAND_H
#define MCOMMAND_H

#include "CCUID.h"

#include <vector>
#include <list>
#include <set>
#include <deque>

using namespace std;

#include "CCCommandParameter.h"
#include "mempool.h"

class CCCommandManager;

// Command Description Flag
#define MCDT_NOTINITIALIZED		0	///< �ʱ�ȭ�� �ȵ� ����
#define MCDT_MACHINE2MACHINE	1	///< �ӽſ��� �ӽ����� ���޵Ǵ� Ŀ�ǵ�
#define MCDT_LOCAL				2	///< ���÷� ���޵Ǵ� Ŀ�ǵ�
#define MCDT_TICKSYNC			4	///< ƽ��ũ�� �ʿ��� Ŀ�ǵ�, ����ƽ�� ����ȴ�.
#define MCDT_TICKASYNC			8	///< ƽ��ũ�� �ʿ� ���� Ŀ�ǵ�, ��ٷ� ����ȴ�.
#define MCDT_USER				16	///< ���� ���� Ŀ�ǵ�
#define MCDT_ADMIN				32	///< ��� ���� Ŀ�ǵ�
#define MCDT_PEER2PEER			64	///< Peer���� Peer�� ���޵Ǵ� Ŀ�ǵ�, MACHINE2MACHINE�ʹ� ���� �����Ѵ�.

#define MCCT_NON_ENCRYPTED		128 ///< ��ȣȭ���� �ʴ� Ŀ�ǵ�
#define MCCT_HSHIELD_ENCRYPTED	256 ///< �ٽǵ� ��ȣȭ�ϴ� Ŀ�ǵ�(���� �̱���) MCCT_NON_ENCRYPTED�� ������ �� ����.

#define MAX_COMMAND_PARAMS		255	///< Ŀ�ǵ�� �ְ� 255������ �Ķ��Ÿ�� ���� �� �ִ�.

/// Ŀ�ǵ��� �Ӽ��� ����
/// - CCCommandDesc::m_nID �� ���� ID�� ���� CCCommand�� CCCommandDesc::m_ParamDescs�� ���ǵ� �Ķ���͸� ������ �ȴ�.
class CCCommandDesc{
protected:
	int			m_nID;					///< Command ID
	char		m_szName[256];			///< Name for Parsing
	char		m_szDescription[256];	///< Description of Command
	int			m_nFlag;				///< Command Description Flag

	vector<CCCommandParameterDesc*>	m_ParamDescs;	///< Parameters
public:
	/// @param nID				Ŀ�ǵ� ID
	/// @param szName			Ŀ�ǵ� �̸�
	/// @param szDescription	Ŀ�ǵ� ����
	/// @param nFlag			Ŀ�ǵ� �÷���, MCDT_NOTINITIALIZED
	CCCommandDesc(int nID, const char* szName, const char* szDescription, int nFlag);
	virtual ~CCCommandDesc();

	/// CCCommandParameterDesc �߰�
	void AddParamDesc(CCCommandParameterDesc* pParamDesc);

	/// �÷��� �˻�
	bool IsFlag(int nFlag) const;
	/// ���̵� ���
	int GetID() const { return m_nID; }
	/// �̸� ���
	const char* GetName() const { return m_szName; }
	/// ���� ���
	const char* GetDescription() const { return m_szDescription; }
	/// Parameter Description ���
	CCCommandParameterDesc* GetParameterDesc(int i) const {
		if(i<0 || i>=(int)m_ParamDescs.size()) return NULL;
		return m_ParamDescs[i];
	}
	/// Parameter Description ���� ���
	int GetParameterDescCount() const {
		return (int)m_ParamDescs.size();
	}
	CCCommandParameterType GetParameterType(int i) const
	{
		if(i<0 || i>=(int)m_ParamDescs.size()) return MPT_END;
		return m_ParamDescs[i]->GetType();
	}
	/// �ڱ⺹�� �Լ� - ���� �׽�Ʈ������ ����Ѵ�.
	CCCommandDesc* Clone();
};



/// �ӽŰ� �ӽ� Ȥ�� ���ÿ� ���޵Ǵ� Ŀ���
class CCCommand : public MemPool<CCCommand> 
{
public:
	CCUID						m_Sender;				///< �޼����� ������ �ӽ�(Ȥ�� ������Ʈ) UID
	CCUID						m_Receiver;				///< �޼����� �޴� �ӽ�(Ȥ�� ������Ʈ) UID
	const CCCommandDesc*			m_pCommandDesc;			///< �ش� Ŀ�ǵ��� Description
	vector<CCCommandParameter*>	m_Params;				///< �Ķ����
	unsigned char				m_nSerialNumber;		///< Ŀ�ǵ��� ���Ἲ�� üũ�ϱ� ���� �Ϸù�ȣ
	void ClearParam(int i);

protected:
	/// �ʱ�ȭ
	void Reset();
	/// �Ķ���� �ʱ�ȭ
	void ClearParam();

public:
	CCCommand();
	CCCommand(const CCCommandDesc* pCommandDesc, CCUID Receiver, CCUID Sender);
	CCCommand::CCCommand(int nID, CCUID Sender, CCUID Receiver, CCCommandManager* pCommandManager);
	virtual ~CCCommand();

	/// CCCommandDesc���� ID ����
	void SetID(const CCCommandDesc* pCommandDesc);
	/// ID ����
	void CCCommand::SetID(int nID, CCCommandManager* pCommandManager);
	/// ID ���
	int GetID() const { return m_pCommandDesc->GetID(); }
	/// ���� ���
	const char* GetDescription(){ return m_pCommandDesc->GetDescription(); }

	/// �Ķ���� �߰�
	bool AddParameter(CCCommandParameter* pParam);
	/// �Ķ���� ���� ���
	int GetParameterCount() const;
	/// �Ķ���� ���
	CCCommandParameter* GetParameter(int i) const;

	/// �ε����� �Ķ���͸� ��´�. �Ķ���� Ÿ���� ��Ȯ�ϰ� ������� ������ false�� �����Ѵ�.
	/// @brief �Ķ���� ���
	/// @param pValue	[out] �Ķ���� ��
	/// @param i		[in] �Ķ���� �ε���
	/// @param t		[in] �Ķ���� Ÿ��, ��Ȯ�� Ÿ���� �������� �Ѵ�.
	bool GetParameter(void* pValue, int i, CCCommandParameterType t, int nBufferSize=-1) const;

	CCUID GetSenderUID(){ return m_Sender; }
	void SetSenderUID(const CCUID &uid) { m_Sender = uid; }
	CCUID GetReceiverUID(){ return m_Receiver; }

	bool IsLocalCommand(){ return (m_Sender==m_Receiver); }

	/// ���� ������ ���� Ŀ�ǵ� ����
	CCCommand* Clone() const;

	/// Description�� �°Բ� �����Ǿ��°�?
	bool CheckRule();	

	/// Ŀ�ǵ� �޸� �� �����ͷ� ����
	/// @param pData	[out] Ŀ�ǵ� ������ ��
	/// @param nSize	[in] Ŀ�ǵ� ������ �� �ִ� ������
	/// @return			�� ������
	int GetData(char* pData, int nSize);
	/// Ŀ�ǵ� �޸� �� �����ͷκ��� ����
	/// @param pData	[in] Ŀ�ǵ� ������ ��
	/// @param pPM		[in] Ŀ�ǵ� �Ŵ���(CCCommandDesc�� enum�� �� �ִ�.)
	/// @return			���� ����
	bool SetData(char* pData, CCCommandManager* pCM, unsigned short nDataLen=USHRT_MAX);

	int GetSize();
	int GetSerial() { return m_nSerialNumber; }
};


class CCCommandSNChecker
{
private:
	int				m_nCapacity;
	deque<int>		m_SNQueue;
	set<int>		m_SNSet;
public:
	CCCommandSNChecker();
	~CCCommandSNChecker();
	void InitCapacity(int nCapacity);
	bool CheckValidate(int nSerialNumber);
};



// ���μ� ���̱� ���� ��ũ��
#define NEWCMD(_ID)		(new CCCommand(_ID))
#define AP(_P)			AddParameter(new _P)
#define MKCMD(_C, _ID)									{ _C = NEWCMD(_ID); }
#define MKCMD1(_C, _ID, _P0)							{ _C = NEWCMD(_ID); _C->AP(_P0); }
#define MKCMD2(_C, _ID, _P0, _P1)						{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); }
#define MKCMD3(_C, _ID, _P0, _P1, _P2)					{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); }
#define MKCMD4(_C, _ID, _P0, _P1, _P2, _P3)				{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); }
#define MKCMD5(_C, _ID, _P0, _P1, _P2, _P3, _P4)		{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); }
#define MKCMD6(_C, _ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); _C->AP(_P5); }


// Short Name
typedef CCCommand				CCCmd;
typedef CCCommandDesc			CCCmdDesc;

#endif