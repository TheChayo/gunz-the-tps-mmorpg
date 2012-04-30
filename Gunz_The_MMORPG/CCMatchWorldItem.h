#pragma once

#include <vector>
#include <list>
#include <map>
using namespace std;

class CCMatchStage;
class CCMatchObject;
class CCZFileSystem;


#define WORLDITEM_EXTRAVALUE_NUM		2
#define WORLDITEM_MAX_NUM				30		// �÷ε� �ٿ����� ���׷� �������...(�޵�Ŷ,����Ŷ ����ʿ� ��������)

// �ʿ� ������ ������
struct MMatchWorldItem
{
	unsigned short		nUID;
	unsigned short		nItemID;
	short				nStaticSpawnIndex;
	float				x;
	float				y;
	float				z;
	int					nLifeTime;			// ������ Ȱ�� �ð�( -1�̸� ���� )

	union {
		struct {
		    int			nDropItemID;		// ���� ����Ʈ�� ��� QuestItem �Ǵ� �Ϲ� �������� ID
			int			nRentPeriodHour;	// ���� �Ϲ� �������� ��� Item ID
		} ;
		int				nExtraValue[WORLDITEM_EXTRAVALUE_NUM];
	};
};

struct UserDropWorldItem
{
	UserDropWorldItem( CCMatchObject* pObj, const int nItemID, const float x, const float y, const float z, unsigned long nDropDelayTime )
	{
		m_pObj			= pObj;
		m_nItemID		= nItemID;
		m_x				= x;
		m_y				= y;
		m_z				= z;
		m_nDropDelayTime = nDropDelayTime;
	}

	CCMatchObject*		m_pObj;
	int					m_nItemID;
	float				m_x;
	float				m_y;
	float				m_z;
	unsigned long		m_nDropDelayTime;
};


typedef map<unsigned short, MMatchWorldItem*> MMatchWorldItemMap;


// ���� ���� ����
struct MMatchWorldItemSpawnInfo
{
	unsigned short		nItemID;
	unsigned long int	nCoolTime;
	unsigned long int	nElapsedTime;
	float x;
	float y;
	float z;
	bool				bExist;
	bool				bUsed;
};


class MMatchWorldItemManager
{
private:
	CCMatchStage*						m_pMatchStage;
	MMatchWorldItemMap					m_ItemMap;				// �ʿ� �����ϰ� �ִ� ������ ����Ʈ

	vector<MMatchWorldItemSpawnInfo>	m_SpawnInfos;			// ���� ���� ������ ����
	vector< UserDropWorldItem >			m_UserDropWorldItem;	// ������ ���� ������ ����
	int									m_nSpawnItemCount;		// ���� ������ ���� ����
	unsigned long int					m_nLastTime;

	short								m_nUIDGenerate;
	bool								m_bStarted;

	void AddItem(const unsigned short nItemID, short nSpawnIndex, 
				 const float x, const float y, const float z);
	void AddItem(const unsigned short nItemID, short nSpawnIndex, 
				 const float x, const float y, const float z, int nLifeTime, int* pnExtraValues );
	void DelItem(short nUID);
	void Spawn(int nSpawnIndex);
	void Clear();
	void SpawnInfoInit();
	void ClearItems();

	void RouteSpawnWorldItem(MMatchWorldItem* pWorldItem);
	void RouteObtainWorldItem(const CCUID& uidPlayer, int nWorldItemUID);
	void RouteRemoveWorldItem(int nWorldItemUID);
public:
	MMatchWorldItemManager();
	virtual ~MMatchWorldItemManager();

	// CCMatchStage���� �����ϴ� �Լ�
	bool Create(CCMatchStage* pMatchStage);
	void Destroy();

	void OnRoundBegin();
	void OnStageBegin(CCMatchStageSetting* pStageSetting);
	void OnStageEnd();
	void Update();

	bool Obtain(CCMatchObject* pObj, short nItemUID, int* poutItemID, int* poutExtraValues);
	void SpawnDynamicItem(CCMatchObject* pObj, const int nItemID, const float x, const float y, const float z, float fDropDelayTime);
	void SpawnDynamicItem(CCMatchObject* pObj, const int nItemID, const float x, const float y, const float z, 
						  int nLifeTime, int* pnExtraValues );
	void RouteAllItems(CCMatchObject* pObj);

};
