#pragma once

//////////////////////////////////////////////////////////////////////////
// CCMatchObjectCommandHistory
// �� MatchObject �� ���� Ŀ�ǵ带 �����ð�, �������� �����ϰ� �ִ�.
// �ʿ��� ������ �����غ��� ���� �ڷ��̴�

const int MAX_COMMAND_HISTORY_COUNT = 50;			///< ���� ���� 30��
const int MAX_HISTORY_HOLD_TIME		= 10*60*1000;	///< �ִ� �����ð� 10��

struct MOBJECTCOMMANDHISTORY {
	string			m_strName;	///< ĳ���� �̸�
	unsigned int	m_nCID;		///< ĳ���� ID
	list < pair < int, DWORD > >	m_commands;	///< command id �� �߰��� �ð�
	DWORD			m_dwLastTime;	///< ������ Ŀ�ǵ尡 �߰��� �ð�
};

class CCMatchObjectCommandHistory {
public:
	~CCMatchObjectCommandHistory();

	void SetCharacterInfo(CCUID uid, const char* szName, unsigned int nCID );
	
	void PushCommand(CCUID uid, int nCommandID, DWORD dwCurrentTime, bool* pbFloodingSuspect = NULL);
	void Update( DWORD dwCurrentTime );
	unsigned int GetObjectCount();

	MOBJECTCOMMANDHISTORY* GetCommandHistory(CCUID uid);
	bool Dump(CCUID uid);

private:
	typedef map< CCUID, MOBJECTCOMMANDHISTORY* > UIDHISTORYMAP;

	UIDHISTORYMAP m_mapHistories;

	MOBJECTCOMMANDHISTORY* AddNew(CCUID uid); 
};

inline unsigned int CCMatchObjectCommandHistory::GetObjectCount() { return (unsigned int)m_mapHistories.size(); }
