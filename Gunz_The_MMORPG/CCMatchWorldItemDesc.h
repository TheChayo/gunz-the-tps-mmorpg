#pragma once
#include "CCXml.h"
#include <map>
using namespace std;

#define WORLDITEM_NAME_LENGTH		256

enum MMATCH_WORLD_ITEM_TYPE
{
	WIT_HP			= 0,
	WIT_AP			= 1,
	WIT_BULLET		= 2,
	WIT_HPAP		= 3,
	WIT_CLIENT		= 4,	// Ŭ���̾�Ʈ ���� ���������

	WIT_QUEST		= 5,	// ����Ʈ ������ �ڽ�
	WIT_BOUNTY		= 6,	// ����Ʈ���� ������ �ٿ�Ƽ ������

	WIT_END
};


struct CCMatchWorldItemDesc
{
	short					m_nID;
	MMATCH_WORLD_ITEM_TYPE	m_nItemType;
	float					m_fAmount;
	unsigned long int		m_nTime;
	char					m_szModelName[WORLDITEM_NAME_LENGTH];
	char					m_szDescName[WORLDITEM_NAME_LENGTH];
};

class CCMatchWorldItemDescMgr;
class CCZFileSystem;


/// ���� ������ Ÿ�� ���
class CCMatchWorldItemDescMgr : public map<short, CCMatchWorldItemDesc*>
{
private:
protected:
	void ParseWorldItem(CCXmlElement& element);
public:
	CCMatchWorldItemDescMgr();
	virtual ~CCMatchWorldItemDescMgr();
	bool ReadXml(const char* szFileName);
	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);
	void Clear();
	CCMatchWorldItemDesc* GetItemDesc(short nID);
	static CCMatchWorldItemDescMgr* GetInstance();
};

inline CCMatchWorldItemDescMgr* MGetMatchWorldItemDescMgr() 
{ 
	return CCMatchWorldItemDescMgr::GetInstance();
}


/////////////////////////////////////////////////////////////////////////////////////


#include "CCMatchWorldItem.h"
#include "CCMatchStage.h"

#define MAX_WORLDITEM_SPAWN		100

// �� �ʿ��� ������ �ִ� ��������
struct CCMatchMapsWorldItemSpawnInfoSet
{
	CCMatchWorldItemSpawnInfo	SoloSpawnInfo[MAX_WORLDITEM_SPAWN];
	CCMatchWorldItemSpawnInfo	TeamSpawnInfo[MAX_WORLDITEM_SPAWN];
	int							m_nSoloSpawnCount;
	int							m_nTeamSpawnCount;

	const DWORD					GetCRC32();
};

// ��ü ���� ��������
class CCMatchMapsWorldItemSpawnInfo
{
private:
	void ParseSpawnInfo(CCXmlElement& element, int nMapID);
	void SetMapsSpawnInfo(int nMapID, char* szGameTypeID, int nItemID, float x, float y, float z, 
						  unsigned long int nCoolTime);
	bool ReadXml(const char* szFileName, int nMapID);
protected:
public:
	CCMatchMapsWorldItemSpawnInfoSet		m_MapsSpawnInfo[MMATCH_MAP_COUNT];
	CCMatchMapsWorldItemSpawnInfo();
	virtual ~CCMatchMapsWorldItemSpawnInfo();
	
	bool Read();
	void Clear();
	static CCMatchMapsWorldItemSpawnInfo* GetInstance();
};



inline CCMatchMapsWorldItemSpawnInfo* MGetMapsWorldItemSpawnInfo() 
{ 
	return CCMatchMapsWorldItemSpawnInfo::GetInstance();
}
