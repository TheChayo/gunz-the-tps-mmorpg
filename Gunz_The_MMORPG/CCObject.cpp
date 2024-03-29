#include "stdafx.h"
#include "winsock2.h"
#include "CCXml.h"
#include "CCObject.h"
#include "CCMap.h"
#include "CCFormulaTable.h"
#include "CCCharacterDesc.h"

IMPLEMENT_RTTI(CCObject)



CCObjectCacheNode* CCObjectCache::FindCacheNode(CCObject* pObj)
{
	for(iterator i=begin(); i!=end(); i++){
		CCObjectCacheNode* pNode = *i;
		if (pNode->m_pObject == pObj)
			return pNode;
	}
	return NULL;
}

void CCObjectCache::Invalidate()
{
	m_nUpdateCount = 0;
	for(iterator i=begin(); i!=end(); i++){
		CCObjectCacheNode* pNode = *i;
		pNode->m_CacheState = OBJECTCACHESTATE_EXPIRE;
		++m_nUpdateCount;
	}
}

void CCObjectCache::Update(CCObject* pObject)
{
	CCObjectCacheNode* pFoundNode = NULL;
	for(iterator i=begin(); i!=end(); i++){
		CCObjectCacheNode* pNode = *i;
		if (pNode->m_pObject == pObject)
			pFoundNode = pNode;
	}

	if (pFoundNode) {
		pFoundNode->m_CacheState = OBJECTCACHESTATE_KEEP;
		--m_nUpdateCount;
	} else {
		CCObjectCacheNode* pNewNode = new CCObjectCacheNode;
		pNewNode->m_pObject = pObject;
		pNewNode->m_CacheState = OBJECTCACHESTATE_NEW;
		++m_nUpdateCount;
		push_back(pNewNode);
	}
}

void CCObjectCache::RemoveExpired()
{
	for(iterator i=begin(); i!=end();){
		CCObjectCacheNode* pNode = *i;
		if (pNode->m_CacheState == OBJECTCACHESTATE_EXPIRE)
			i = erase(i);
		else
			++i;
	}
}
/*
rvector CCObject::GetPos(unsigned long int nTime)
{
	MOBJECTPATH* pBegin = NULL;
	MOBJECTPATH* pEnd = NULL;
	for(vector<MOBJECTPATH>::iterator i=m_Path.begin(); i!=m_Path.end(); i++){
		MOBJECTPATH* pPath = &(*i);
		if(pPath->nTime<=nTime) pBegin = pPath;
		else if(pBegin!=NULL){
			pEnd = pPath;
			break;
		}
	}

	if(pBegin==NULL){
		if(m_Path.empty()==false) return m_Path[0].Pos;
		else return m_Pos;
	}
	else{
		if(pEnd==NULL){
			return pBegin->Pos;
		}
		else{
			_ASSERT(pBegin!=pEnd && pBegin->nTime!=pEnd->nTime);
			rvector Pos = pBegin->Pos + (pEnd->Pos - pBegin->Pos) * (float)(nTime - pBegin->nTime) / (float)(pEnd->nTime - pBegin->nTime);
			return Pos;
		}
	}
}
*/
CCObject::CCObject()
{
	m_UID = CCUID::Invalid();
//	m_pMap = NULL;
//	m_Pos = rvector(0,0,0);
}
/*
CCObject::CCObject(CCUID& uid, MMap* pMap, rvector& Pos)
{
	m_UID = uid;
	m_pMap = pMap;
	//m_StartPos = m_EndPos = m_Pos = Pos;
	//m_nStartTime = m_nEndTime = 0;
	m_Pos = Pos;
	if (m_pMap)
		m_hRef = m_pMap->Add(Pos.x, Pos.y, Pos.z, this);

	m_ObjectType = CCOT_NONE;
}
*/
CCObject::CCObject(const CCUID& uid)
{
	m_UID = uid;
//	m_pMap = NULL;
//	m_Pos = rvector(0,0,0);

//	m_ObjectType = CCOT_NONE;
}
/*
CCObject::~CCObject()
{
	if (m_pMap)
		m_pMap->Del(m_hRef);
}
*/

/*
void CCObject::SetPos(unsigned long int nStartTime, unsigned long int nEndTime, rvector& StartPos, rvector& EndPos)
{
	_ASSERT(nStartTime<=nEndTime);

	m_Path.clear();
	//m_Path.reserve(2);
	m_Path.resize(2);
	m_Path[0].nTime = nStartTime;
	m_Path[0].Pos = StartPos;
	m_Path[1].nTime = nEndTime;
	m_Path[1].Pos = EndPos;
}*/
/*
void CCObject::SetPath(MOBJECTPATH* p, int nCount)
{
	m_Path.clear();
	//m_Path.reserve(nCount);
	m_Path.resize(nCount);
	for(int i=0; i<nCount; i++){
		memcpy(&m_Path[i], &(p[i]), sizeof(MOBJECTPATH));
	}
}
*/
/*
void CCObject::Tick(unsigned long int nTime)
{
	// 새로운 위치 계산
	m_Pos = GetPos(nTime);

	if (m_pMap)
		m_hRef = m_pMap->Move(m_Pos.x, m_Pos.y, m_Pos.z, this, m_hRef);
}
*/
void CCObject::AddCommListener(CCUID ListenerUID)
{
	if (IsCommListener(ListenerUID)) return;
	m_CommListener.push_back(ListenerUID);
}

void CCObject::RemoveCommListener(CCUID ListenerUID)
{
	for (list<CCUID>::iterator i=m_CommListener.begin(); i!=m_CommListener.end(); i++) {
		CCUID uid = *i;
		if (uid == ListenerUID) {
			m_CommListener.erase(i);
			return;
		}
	}
}

bool CCObject::IsCommListener(CCUID ListenerUID)
{
	for (list<CCUID>::iterator i=m_CommListener.begin(); i!=m_CommListener.end(); i++) {
		CCUID uid = *i;
		if (uid == ListenerUID)
			return true;
	}
	return false;
}


/*

#define ANGLE_SPEED			12.0f
#define MOVE_SPEED			400.0f

unsigned long int CCGetTimeToWalk(rvector& Distance)
{
	float fTime = D3DXVec3Length(&Distance) / MOVE_SPEED;
	unsigned long int nTime = (unsigned long int)(fTime*1000);
	if(nTime==0) nTime = 1;	// Divide By Zero를 막기 위해 최소 1msec는 걸린다고 가정한다.
	return nTime;
}

unsigned long int CCGetTimeToTurn(rvector& Dir1, rvector& Dir2)
{
	float fDot = max(min(D3DXVec3Dot(&Dir1, &Dir2), 1.f), -1.f);
	float fAngle = (float)acos(fDot);
	_ASSERT(!_isnan(fAngle));

	float fTime = (float)fabs(fAngle) / ANGLE_SPEED;
	unsigned long int nTime = (unsigned long int)(fTime*1000);
	if(nTime==0) nTime = 1;	// Divide By Zero를 막기 위해 최소 1msec는 걸린다고 가정한다.
	return nTime;
}
*/