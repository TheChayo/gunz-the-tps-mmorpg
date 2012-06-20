#ifndef _SACRIFICE_QUEST_ITEM_TABLE
#define _SACRIFICE_QUEST_ITEM_TABLE


#include "CCQuestConst.h"

class CCQuestSacrificeSlot;


#define SACRIFICE_TABLE_XML "SacrificeTable.xml"

/* CCQuestConst.h�� �̵�.
#define MSQITRES_NOR  1	// Ư���ó������� �ش��ϴ� ��������۸� ����, �Ϲݽó������� ���� ��� �����۸� �ִ� ��Ȳ.
#define MSQITRES_SPC  2	// �߳� �ó����� �����۰� Ư���ó������� �ش��ϴ� ����������� ����.
#define MSQITRES_INV  3	// �ش� QL������ ��������� ���� ���̺��� ����. �̰��� ���� �ʴ� ��� �������� �÷��� �������.
#define MSQITRES_DUP  4 // ���� ���Կ� Ư�� �ó������� ��� �������� �÷��� ����.
#define MSQITRES_EMP  5 // ���� ������ ��� ��� ����. �� ���´� QL����1�� ����� ��.
#define MSQITRES_ERR -1	// ����... ���̺��� �ش� QL�� ã���� ����. QL = 0 or QL���� ���� ������ MAX QL���� Ŭ���.
*/


#define CCSQITC_ITEM		"ITEM"
#define CCSQITC_MAP		"map"
#define CCSQITC_QL		"ql"
#define CCSQITC_DIID		"default_item_id"
#define CCSQITC_SIID1	"special_item_id1"
#define CCSQITC_SIID2	"special_item_id2"
#define CCSQITC_SIGNPC	"significant_npc"
#define CCSQITC_SDC		"sdc"
#define CCSQITC_SID		"ScenarioID"



class CCSacrificeQItemInfo
{
	friend class CCSacrificeQItemTable;

public :
	unsigned long	GetDefQItemID()		{ return m_nDefaultQItemID; }
	unsigned long	GetSpeQItemID1()	{ return m_nSpecialQItemID1; }
	unsigned long	GetSpeQItemID2()	{ return m_nSpecialQItemID2; }
	const char*		GetMap()			{ return m_szMap; }
	int				GetSigNPCID()		{ return m_nSignificantNPCID; }
	float			GetSDC()			{ return m_fSDC; }
	int				GetQL()				{ return m_nQL; }
	int				GetScenarioID()		{ return m_nScenarioID; }
	
private :
	CCSacrificeQItemInfo() : m_nDefaultQItemID( 0 ), m_nSpecialQItemID1( 0 ), m_nSpecialQItemID2( 0 ), m_nSignificantNPCID( 0 ), m_fSDC( 0.0f ) {}
	CCSacrificeQItemInfo( unsigned long nDfQItemID, unsigned long nSpecIID1, unsigned long nSpecIID2, const int nSigNPCID, const float fSdc ) :
		m_nDefaultQItemID( nDfQItemID ), m_nSpecialQItemID1( nSpecIID1 ), m_nSpecialQItemID2( nSpecIID2 ), 
		m_nSignificantNPCID( nSigNPCID ), m_fSDC( fSdc ) {}

	unsigned long	m_nDefaultQItemID;
	unsigned long	m_nSpecialQItemID1;
	unsigned long	m_nSpecialQItemID2;
	char			m_szMap[ 24 ];
	int				m_nSignificantNPCID;
	float			m_fSDC;
	int				m_nQL;
	int				m_nScenarioID;
};


// �������� �ʿ��� ��� ������ ���̺�.
class CCSacrificeQItemTable : private multimap< int, CCSacrificeQItemInfo >
{
public :
	CCSacrificeQItemTable() {}
	~CCSacrificeQItemTable() {}

	static CCSacrificeQItemTable& GetInst()
	{
		static CCSacrificeQItemTable SacrificeQItemTable;
		return SacrificeQItemTable;
	}

	int	 FindSacriQItemInfo( const int nQL, CCQuestSacrificeSlot* pSacrificeSlot, int& outResultQL );
	bool ReadXML( const char* pszFileName );
	bool ReadXml( CCZFileSystem* pFileSystem, const char* szFileName );

	CCSacrificeQItemInfo* GetResultTable() { return m_pResultTable; }

	bool TestInitTable();

private :
	void ParseTable( ::CCXmlElement& element );

	CCSacrificeQItemInfo* m_pResultTable;	// CheckInvalidSlot������ �ش��ϴ� ���̺��� �˻��Ǹ� ���⿡ ���õ�.
};


inline CCSacrificeQItemTable* GetSacriQItemTable()
{
	return &(CCSacrificeQItemTable::GetInst());
}



#endif ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////