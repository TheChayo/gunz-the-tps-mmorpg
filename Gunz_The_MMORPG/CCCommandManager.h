#ifndef MCOMMANDMANAGER_H
#define MCOMMANDMANAGER_H

#include "CCCommand.h"
#include "CCCommandParameter.h"
#include <map>
#include <string>
using namespace std;

//typedef vector<CCCommandDesc*>	CCCommandDescVector;	///< Command Description Vector(ID�� �ε����� �˻��� ���� �ϹǷ�, ���ĵ� ���͸� �̿��Ѵ�.)
typedef map<int, CCCommandDesc*>	CCCommandDescMap;	///< Command Description Map
typedef list<CCCommand*>			CCCommandList;		///< Command List
typedef map<string, string>		CCCommandAliasMap;		///< Alias


/// Ŀ�ǵ� �Ŵ���
/// - Ŀ�ǵ� ��ũ���� ����
/// - Ŀ�ǵ� ť ����
class CCCommandManager{
protected:
	//CCCommandDescVector	m_CommandDescs;			///< Command Description List
	CCCommandDescMap		m_CommandDescs;
	CCCommandList		m_CommandQueue;			///< Posted Command List
	CCCommandAliasMap	m_CommandAlias;			///< Aliases
protected:
	void InitializeCommandDesc(void);
	void InitializeCommandMemPool();
	void FinalizeCommandMemPool();
public:
	CCCommandManager(void);
	virtual ~CCCommandManager(void);

	void Initialize(void);

	int GetCommandDescCount(void) const;
	int GetCommandQueueCount(void) const;
	CCCommandDesc* GetCommandDesc(int i);
	CCCommandDesc* GetCommandDescByID(int nID);
	void AssignDescs(CCCommandManager* pTarCM);

	/// Command Description �߰�
	void AddCommandDesc(CCCommandDesc* pCD);

	/// Ŀ�ǵ� Posting
	bool Post(CCCommand* pNew);

	/// ���� ���� �߰��� Ŀ�ǵ带 �о����
	CCCommand* GetCommand(void);
	/// ���� ���� �߰��� Ŀ�ǵ带 CommandManager���� ������ �ʰ� �о����
	CCCommand* PeekCommand(void);

	/// Command Description�� ���� ���� ��Ʈ�� ����
	void GetSyntax(char* szSyntax, const CCCommandDesc* pCD);
	/// �޼����� �Ľ��ؼ� Ŀ�ǵ�� �����
	/// @param pCmd				[out] ������� Ŀ�ǵ�
	/// @param szErrMsg			[out] ���� �޼���
	/// @param nErrMsgMaxLength	[out] ���� �޼��� ũ��
	/// @param szMsg			[in]  �Ľ��� ���� �޼���
	bool ParseMessage(CCCommand* pCmd, char* szErrMsg, int nErrMsgMaxLength, const char* szMsg);

	/// Alias�� �߰��Ѵ�.
	/// @param szName			[in] Alias �̸�
	/// @param szText			[in] Alias ����
	void AddAlias(string szName, string szText);
};

// Post Command Macro For Convenience
#define POSTCMD0(_ID)									{ CCCommand* pC=NEWCMD(_ID); ASPostCommand(pC); }
#define POSTCMD1(_ID, _P0)								{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); ASPostCommand(pC); }
#define POSTCMD2(_ID, _P0, _P1)							{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); ASPostCommand(pC); }
#define POSTCMD3(_ID, _P0, _P1, _P2)					{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ASPostCommand(pC); }
#define POSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ASPostCommand(pC); }
#define POSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)			{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ASPostCommand(pC); }
#define POSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ASPostCommand(pC); }

#define ENPOSTCMD0(_ID)									{ CCCommand* pC=NEWCMD(_ID); ASEnPostCommand(pC); }
#define ENPOSTCMD1(_ID, _P0)							{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); ASEnPostCommand(pC); }
#define ENPOSTCMD2(_ID, _P0, _P1)						{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); ASEnPostCommand(pC); }
#define ENPOSTCMD3(_ID, _P0, _P1, _P2)					{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ASEnPostCommand(pC); }
#define ENPOSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ASPostCommand(pC); }
#define ENPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)		{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ASEnPostCommand(pC); }
#define ENPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ CCCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ASEnPostCommand(pC); }



// Command Description Add Macro For Convenience
#ifdef _DEBUG

#define BEGIN_CMD_DESC(_pCommandManager)	{ CCCommandDesc* pCD4m; CCCommandManager* pCM4m = _pCommandManager;
#define END_CMD_DESC()						}
#define C(_a, _b, _c, _d)					{ pCD4m = new CCCommandDesc(_a, _b, _c, _d); pCM4m->AddCommandDesc(pCD4m); }
#define P(_a, _b)							{ pCD4m->AddParamDesc(new CCCommandParameterDesc(_a, _b)); }
#define CA(_a, _b)							{ pCM4m->AddAlias(_a, _b); }
#define P_MINMAX(_a, _b, _min, _max)		{ CCCommandParameterDesc* pNewDesc = new CCCommandParameterDesc(_a, _b);  \
	pNewDesc->AddCondition(new CCCommandParamConditionMinMax(_min, _max)); \
	pCD4m->AddParamDesc(pNewDesc); }

#else	// ��ũ���ǿ� ���ڿ��� ��ŷ �������� �ǹǷ� ��������� �����Ѵ�

#define BEGIN_CMD_DESC(_pCommandManager)	{ CCCommandDesc* pCD4m; CCCommandManager* pCM4m = _pCommandManager;
#define END_CMD_DESC()						}
#define C(_a, _b, _c, _d)					{ pCD4m = new CCCommandDesc(_a, "", "", _d); pCM4m->AddCommandDesc(pCD4m); }
#define P(_a, _b)							{ pCD4m->AddParamDesc(new CCCommandParameterDesc(_a, "")); }
#define CA(_a, _b)							{ pCM4m->AddAlias(_a, _b); }
#define P_MINMAX(_a, _b, _min, _max)		{ CCCommandParameterDesc* pNewDesc = new CCCommandParameterDesc(_a, "");  \
	pNewDesc->AddCondition(new CCCommandParamConditionMinMax(_min, _max)); \
	pCD4m->AddParamDesc(pNewDesc); }

#endif


#endif