#ifndef _CCQUESTDROPTABLE_H
#define _CCQUESTDROPTABLE_H

#include "CCQuestConst.h"

/// ��� ������ Ÿ��
enum CCQuestDropItemType
{
	QDIT_NA			= 0,	///< �������� ����
	QDIT_WORLDITEM	= 1,	///< HP, AP���� �Ϲ����� ���������
	QDIT_QUESTITEM	= 2,	///< ����Ʈ ������
	QDIT_ZITEM		= 3,	///< �Ϲ� ������
};

/// ��� ������ ����
struct CCQuestDropItem
{
	CCQuestDropItemType	nDropItemType;
	int					nID;
	int					nRentPeriodHour;
	// int					nMonsetBibleIndex;	// � ������ ���Ͱ� �������� ����߷ȴ��� �� �������� ������ �ִ� ������ ������ ����.
											// ���� ������ ���ؼ� ���.

	CCQuestDropItem() : nDropItemType(QDIT_NA), nID(0), nRentPeriodHour(0) {}
	void Assign(CCQuestDropItem* pSrc)		// ����
	{
		nDropItemType	= pSrc->nDropItemType;
		nID				= pSrc->nID;
		nRentPeriodHour = pSrc->nRentPeriodHour;
	}
};

#define MAX_DROPSET_RATE		1000		///< ��� ���� 0.001���� ���� ����

/// ��� ������ ��
class CCQuestDropSet
{
private:
	int						m_nID;
	char					m_szName[16];
	CCQuestDropItem			m_DropItemSet[MAX_QL+1][MAX_DROPSET_RATE];
	int						m_nTop[MAX_QL+1];
	set<int>				m_QuestItems;			// �� ��Ʈ�� ������ �ִ� ����Ʈ ������ ��Ʈ - Ŭ���̾�Ʈ�� ����Ϸ��� ����
public:
	/// ������
	CCQuestDropSet()
	{
		m_nID = 0;
		m_szName[0] = 0;
		memset(m_DropItemSet, 0, sizeof(m_DropItemSet));
		for (int i = 0; i <= MAX_QL; i++)
		{
			m_nTop[i] = 0;
		}
	}
	int GetID() { return m_nID; }									///< ID ��ȯ
	const char* GetName() { return m_szName; }						///< �̸� ��ȯ
	void SetID(int nID) { m_nID = nID; }							///< ID ����
	void SetName(const char* szName) { strcpy(m_szName, szName); }	///< �̸� ����
	/// ��ӵǴ� ������ �߰�
	/// @param pItem		��ӵ� ������ ����
	/// @param nQL			����Ʈ ����
	/// @param fRate		���� ����
	void AddItem(CCQuestDropItem* pItem, int nQL, float fRate);
	/// ��ӵ� �������� �����Ѵ�.
	/// @param outDropItem		��ӵ� ������ ��ȯ��
	/// @param nQL				����Ʈ ����
	bool Roll(CCQuestDropItem& outDropItem, int nQL);

	set<int>& GetQuestItems() { return m_QuestItems; }				///< ��ӵ� ������ ����
};


/// ��� ���̺� ������ Ŭ����
class CCQuestDropTable : public map<int, CCQuestDropSet*>
{
private:
	void ParseDropSet(CCXmlElement& element);
	void ParseDropItemID(CCQuestDropItem* pItem, const char* szAttrValue);
public:
	CCQuestDropTable();													///< ������
	~CCQuestDropTable();													///< �Ҹ���

	void Clear();
	
	bool ReadXml(const char* szFileName);								///< xml���� ������ �д´�. 
	bool ReadXml(CCZFileSystem* pFileSystem,const char* szFileName);		///< xml���� ������ �д´�. 
	/// ��� ���̺� ID�� QL�� �������� ��ӵ� �������� �����Ѵ�.
	/// @param outDropItem		��ӵ� ������ ��ȯ��
	/// @param nDropTableID		��� ���̺� ID
	/// @param nQL				����Ʈ ����
	bool Roll(CCQuestDropItem& outDropItem, int nDropTableID, int nQL);
	CCQuestDropSet* Find(int nDropTableID);								///< ��� ������ �� ���� ��ȯ
};

#endif