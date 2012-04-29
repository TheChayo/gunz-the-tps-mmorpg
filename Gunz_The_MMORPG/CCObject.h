#pragma once
#include "CCUID.h"
#include "CCGridMap.h"
#include "CCObjectTypes.h"

using namespace std;
#include <algorithm>

class CCMap;
class CCObject;

typedef CCGridMap<CCObject*>	CCObjectGridMap;


/// ������ �̿��� RTTI ����. Class Declaration
#define DECLARE_RTTI()								public: static const char* _m_RTTI_szClassName; virtual const char* GetClassName(void){ return _m_RTTI_szClassName; }
/// ������ �̿��� RTTI ����. Class Implementation
#define IMPLEMENT_RTTI(_Class)						const char* _Class::_m_RTTI_szClassName = #_Class;
/// ���� Ŭ����Ÿ���ΰ�?
#define ISEQUALCLASS(_Class, _InstancePointer)		(_Class::_m_RTTI_szClassName==(_InstancePointer)->GetClassName())	// ������ ��

//#define SAFECAST(_Class, _InstancePointer)			_InstancePointer-
/*
/// �̵��� ���� ����
struct MOBJECTPATH{
	unsigned long int	nTime;		///< �ش� �ð�
	rvector				Pos;		///< �ش� ��ġ
	int					nPathNode;	///< Path Node ID
};
#define MOBJECTMOVE	MOBJECTPATH	// �ӽ�, ���߿� �̸��� �� �ٲ��ش�.
*/
class CCObject;
enum OBJECTCACHESTATE {	OBJECTCACHESTATE_KEEP, OBJECTCACHESTATE_NEW, OBJECTCACHESTATE_EXPIRE };
class CCObjectCacheNode {
public:
	CCObject*			m_pObject;
	OBJECTCACHESTATE	m_CacheState;
};
class CCObjectCache : public list<CCObjectCacheNode*> {
	int		m_nUpdateCount;	// New, Expire�� ī��Ʈ
public:
	int GetUpdateCount() { return m_nUpdateCount; }
	CCObjectCacheNode* FindCacheNode(CCObject* pObj);
	void Invalidate();
	void Update(CCObject* pObject);
	void RemoveExpired();
};


/// �������� �����ϴ� ��� ������Ʈ�� Abstract Class
class CCObject{
protected:
	CCUID			m_UID;

	CCObjectType		m_ObjectType;		///< �켱 �̰��� ������ PC, NPC�� �Ǻ��Ѵ�.
private:
	//rvector			m_Pos;		///< ��ġ
	//rvector				m_StartPos, m_EndPos, m_Pos;
	//unsigned long int	m_nStartTime, m_nEndTime;

//	vector<MOBJECTPATH>	m_Path;		///< �н�
//	rvector				m_Pos;		///< ���� ��ġ
//	int					m_nSpawnID;

//protected:
//	CCMap*					m_pMap;		///< ���� �����ִ� ��
//	CCObjectGridMap::HREF	m_hRef;		///< �ʿ����� ��ġ �ڵ�


public:
	CCObjectCache	m_ObjectCache;
	list<CCUID>		m_CommListener;

	/*
protected:
	void UpdateObjectCache(void);
	*/

protected:
	/// �ð��� ���� ��ġ ����
//	rvector GetPos(unsigned long int nTime);

public:
	CCObject();
//	CCObject(CCUID& uid, CCMap* pMap, rvector& Pos);	// Map Navication Object
	CCObject(const CCUID& uid);	// No Map Object
	virtual ~CCObject(void)	{};

	inline const CCUID GetUID(void) const { return m_UID; }

	/// ���� ��ġ ����
//	void SetPos(rvector& Pos){ SetPos(0, 0, Pos, Pos); }
	/// �ð��� ���� ��ġ ����
//	void SetPos(unsigned long int nStartTime, unsigned long int nEndTime, rvector& StartPos, rvector& EndPos);
	/// �н��� ����
//	void SetPath(MOBJECTPATH* p, int nCount);
	/// ���� �ð��� ������Ʈ ��ġ ���
//	const rvector GetPos(void){ return m_Pos; }
	/// SpawnID ��� (Spawn Object �ƴϸ� 0)
//	int GetSpawnID() { return m_nSpawnID; }
//	void SetSpawnID(int nSpawnID) { m_nSpawnID = nSpawnID; }

	void SetObjectType(CCObjectType type) { m_ObjectType = type; }
	CCObjectType GetObjectType()	{ return m_ObjectType; }

	virtual bool CheckDestroy(int nTime) { return false; }

	/// ƽ ó��
	virtual void Tick(unsigned long int nTime)	{}

	/// ���� �� ����
//	void SetMap(CCMap* pMap){ m_pMap = pMap; }
	/// ���� �����ִ� �� ���
//	CCMap* GetMap(void){ return m_pMap; }
	

	void AddCommListener(CCUID ListenerUID);
	void RemoveCommListener(CCUID ListenerUID);
	bool IsCommListener(CCUID ListenerUID);
	bool HasCommListener() { if (m_CommListener.size() > 0) return true; else return false; }
	const CCUID GetCommListener() { 
		if (HasCommListener())
			return *m_CommListener.begin(); 
		else
			return CCUID(0,0);
	}

	void InvalidateObjectCache() { m_ObjectCache.Invalidate(); } 
	void UpdateObjectCache(CCObject* pObject) { m_ObjectCache.Update(pObject); }
	void ExpireObjectCache(CCObject* pObject) { 
		CCObjectCacheNode* pNode = m_ObjectCache.FindCacheNode(pObject);
		if (pNode == NULL) return;
		pNode->m_CacheState = OBJECTCACHESTATE_EXPIRE;
	}
	void RemoveObjectCacheExpired() { m_ObjectCache.RemoveExpired(); }
	int GetObjectCacheUpdateCount() { return m_ObjectCache.GetUpdateCount(); }

	DECLARE_RTTI()
};
