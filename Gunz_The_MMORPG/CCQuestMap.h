#pragma once

#include "CCQuestConst.h"

/// ���� ��ũ ����
struct CCQuestSectorLink
{
	char			szName[64];					///< �̸�
	vector<int>		vecTargetSectors;			///< ����� �� �ִ� ���� ����
	CCQuestSectorLink() 
	{
		szName[0] = 0;
	}
};


/// ���� ����ũ ����
struct CCQuestSectorBacklink
{
	int nSectorID;
	int nLinkIndex;
};


/// ����Ʈ �� ���� ����
struct CCQuestMapSectorInfo
{
	int								nID;								///< ID
	char							szTitle[64];						///< ���� �̸�
	bool							bBoss;								///< ������ ����
	int								nLinkCount;							///< ��ũ��
	CCQuestSectorLink				Links[MAX_SECTOR_LINK];				///< ��ũ ����
	vector<CCQuestSectorBacklink>	VecBacklinks;						///< ����ũ��
	int								nSpawnPointCount[MNST_END];			///< ��������Ʈ ����

	CCQuestMapSectorInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nLinkCount = 0;
		bBoss = false;
		memset(nSpawnPointCount, 0, sizeof(nSpawnPointCount));
	}
};


/// ����Ʈ �� ��Ʈ ����
struct CCQuestMapsetInfo
{
	int				nID;									///< ID
	char			szTitle[64];							///< ��Ʈ �̸�
	int				nLinkCount;								///< ��ũ��
	vector<int>		vecSectors;								///< ������ �ִ� ����

	CCQuestMapsetInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nLinkCount = 0;
	}
};

typedef	std::map<int, CCQuestMapsetInfo*>		CCQuestMapsetMap;
typedef	std::map<int, CCQuestMapSectorInfo*>		CCQuestMapSectorMap;

/// ����Ʈ �� ������
class CCQuestMapCatalogue
{
private:
	// ��� ����
	CCQuestMapsetMap			m_MapsetInfo;
	CCQuestMapSectorMap		m_SectorInfo;

	// �Լ�
	void InsertMapset(CCQuestMapsetInfo* pMapset);
	void InsertSector(CCQuestMapSectorInfo* pSector);
	void ParseMapset(CCXmlElement& element);
	void ParseMapsetSector1Pass(CCXmlElement& elementMapset, CCQuestMapsetInfo* pMapset);
	void ParseSector(CCXmlElement& element, CCQuestMapSectorInfo* pSector);
	void InitBackLinks();
public:
	CCQuestMapCatalogue();													///< ������
	~CCQuestMapCatalogue();													///< �Ҹ���

	void Clear();															///< �ʱ�ȭ
	bool ReadXml(const char* szFileName);									///< xml���� �������� �д´�. (������)
	bool ReadXml(CCZFileSystem* pFileSystem,const char* szFileName);			///< xml���� �������� �д´�. (Ŭ���̾�Ʈ��)
	void DebugReport();														///< �������� ����� �����Ǿ����� Ȯ���Ѵ�.

	CCQuestMapSectorInfo*	GetSectorInfo(int nSector);						///< ���� ���� ��ȯ
	CCQuestMapsetInfo*		GetMapsetInfo(int nMapset);						///< �ʼ� ���� ��ȯ
	CCQuestMapSectorInfo*	GetSectorInfoFromName(char* szSectorTitle);		///< ���� �̸����� ���� ���� ��ȯ

	inline CCQuestMapsetMap*		GetMapsetMap();

	bool IsHacked();											///< �ӽ���ŷ������..
};


inline CCQuestMapsetMap* CCQuestMapCatalogue::GetMapsetMap()
{
	return &m_MapsetInfo;
}
