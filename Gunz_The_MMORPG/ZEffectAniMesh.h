#ifndef _ZEffectAniMesh_h
#define _ZEffectAniMesh_h

#include "ZEffectManager.h"
#include "RVisualMeshMgr.h"

#include "mempool.h"

class ZEffectAniMesh : public ZEffect{
protected:
	RealSpace2::RVisualMesh m_VMesh;

	rvector	m_Scale;
	rvector	m_Pos;
	rvector	m_Dir,m_DirOrg;
	rvector	m_Up;

	float	m_nStartTime;
	float	m_fRotateAngleZ;
	float	m_fRotateAngleY;
	int		m_nAlignType;

	float	m_nStartAddTime;

	int		m_nLifeTime;

	CCUID	m_uid;
	bool	m_bDelay;
	bool	m_isCheck;
	bool	m_bLoopType;
	
	rvector m_vBackupPos;

public:

	ZEffectAutoAddType	m_nAutoAddEffect;

public:
	ZEffectAniMesh(RMesh* pMesh, const rvector& Pos, rvector& Dir);
	virtual bool Draw(unsigned long int nTime);

	void CheckCross(rvector& Dir,rvector& Up);

	RVisualMesh* GetVMesh();

	virtual void SetUpVector(rvector& v);

	void SetUid(CCUID uid);
	CCUID GetUID() { return m_uid; }
	void SetDelayPos(CCUID id);
	void SetScale(rvector s);
	void SetRotationAngleZ(float a);
	void SetRotationAngleY(float a);
	void SetAlignType(int type);
	void SetStartTime(DWORD _time);
	void SetLifeTime(int nLifeTime) { m_nLifeTime = nLifeTime; }
	virtual rvector GetSortPos();

};


class ZEffectSlash : public ZEffectAniMesh , public MemPoolSm<ZEffectSlash> {
public:
	ZEffectSlash(RMesh* pMesh, const rvector& Pos, rvector& Dir);
};

class ZEffectDash : public ZEffectAniMesh , public MemPoolSm<ZEffectDash> {
public:
	ZEffectDash(RMesh* pMesh, rvector& Pos, rvector& Dir,CCUID uidTarget);
	virtual bool Draw(unsigned long int nTime);
};

class ZEffectLevelUp : public ZEffectAniMesh , public MemPoolSm<ZEffectLevelUp> {
public:
	ZEffectLevelUp(RMesh* pMesh, rvector& Pos, rvector& Dir,rvector& AddPos,ZObject* pObj);
	virtual bool Draw(unsigned long int nTime);

	RMeshPartsPosInfoType m_type;
	rvector m_vAddPos;
};

class ZEffectPartsTypePos : public ZEffectAniMesh , public MemPoolSm<ZEffectPartsTypePos> {
public:
	ZEffectPartsTypePos(RMesh* pMesh, rvector& Pos, rvector& Dir,rvector& AddPos,ZObject* pObj);
	virtual bool Draw(unsigned long int nTime);

	RMeshPartsPosInfoType m_type;
	rvector m_vAddPos;
};

class ZEffectWeaponEnchant : public ZEffectAniMesh , public MemPoolSm<ZEffectWeaponEnchant> {
public:
	ZEffectWeaponEnchant(RMesh* pMesh, rvector& Pos, rvector& Dir, ZObject* pObj);
	virtual bool Draw(unsigned long int nTime);

};

class ZEffectIcon : public ZEffectAniMesh {
public:
	ZEffectIcon(RMesh* pMesh, ZObject* pObj);
	virtual bool Draw(unsigned long int nTime);
	RMeshPartsPosInfoType m_type;
};

class ZEffectShot : public ZEffectAniMesh , public MemPoolSm<ZEffectShot> {
public:
	ZEffectShot(RMesh* pMesh, rvector& Pos, rvector& Dir,ZObject* pObj);
	virtual bool Draw(unsigned long int nTime);

	void SetStartTime(DWORD _time) {
		m_nStartAddTime = _time;
	}

	void SetIsLeftWeapon(bool b) {
		m_isLeftWeapon = b;
	}

public:
//	CCUID		m_uid;
//	ZCharacter* m_pCharacter;

	float		m_nStartAddTime;
	bool		m_isMovingPos;
	CCUID		m_uid;
	bool		m_isLeftWeapon;

};


class ZEffectBerserkerIconLoop : public ZEffectIcon, public MemPoolSm<ZEffectBerserkerIconLoop> {
private:
	unsigned int m_nElapsedTime;
public:
	ZEffectBerserkerIconLoop(RMesh* pMesh, ZObject* pObj);
	virtual bool Draw(unsigned long int nTime);
};

#endif//_ZEffectAniMesh_h