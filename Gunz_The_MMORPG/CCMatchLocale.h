#pragma once
#include "CCBaseLocale.h"
#include "CCNJ_DBAgentClient.h"
#include "CCUID.h"

class CCMatchLocale : public CCBaseLocale
{
protected:
	virtual bool			OnInit();
	CCNJ_DBAgentClient*		m_pDBAgentClient;		///< �Ϻ��ݸ������� ����ϴ� Ŭ���̾�Ʈ ����
	bool					m_bCheckAntiHackCrack;
public:
							CCMatchLocale();
	virtual					~CCMatchLocale();
	static CCMatchLocale*	GetInstance();

	bool					ConnectToDBAgent();
	bool					PostLoginInfoToDBAgent(const CCUID&		uidComm, 
												   const char*		szCN, 
												   const char*		szPW, 
												   bool				bFreeLoginIP, 
												   unsigned long	nChecksumPack, 
												   int				nTotalUserCount);
//	bool					SkipCheckAntiHackCrack();	///< XTrap Crack�� üũ���� ����
};

inline CCMatchLocale* CCGetLocale()
{
	return CCMatchLocale::GetInstance();
}