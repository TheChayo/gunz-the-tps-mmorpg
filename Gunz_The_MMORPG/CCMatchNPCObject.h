#pragma once

#include "CCVector3.h"
#include "CCBaseQuest.h"
#include "CCUID.h"
#include "CCQuestDropTable.h"
#include <map>
using namespace std;

class CCMatchStage;
struct CCQuestPlayerInfo;

/// NPC ������Ʈ�� �÷��� - �������� ���ٸ��� ������� �ʴ´�.
enum NPCOBJECT_FLAG
{
	NOF_NONE			= 0,

};


/// NPC ������Ʈ
class CCMatchNPCObject
{
private:
	CCUID				m_UID;						///< ID
	CCQUEST_NPC			m_nType;					///< NPCŸ��
	CCUID				m_uidController;			///< ������
	CCVector3			m_Pos;						///< ��ġ
	unsigned long int	m_nFlags;					///< �÷��� ����
	CCQuestDropItem		m_DropItem;					///< ������ �ִ� ������ - ���� ��� nDropItemType�� QDIT_NA�̴�.

public:
	/// ������
	/// @param uid		NPC UID
	/// @param nType	NPC ����
	/// @param nFlags	�÷���
	CCMatchNPCObject(CCUID& uid, CCQUEST_NPC nType, unsigned long int nFlags=0);
	/// �Ҹ���
	~CCMatchNPCObject() { }
	/// NPC ������ �÷��̾�� �Ҵ��Ѵ�.
	/// @param uidPlayer	�Ҵ��� �÷��̾� UID
	void AssignControl(CCUID& uidPlayer);		
	/// NPC ������ ����
	void ReleaseControl();
	/// Drop�� �������� �����Ѵ�.
	/// @param pDropItem	������ ����
	void SetDropItem(CCQuestDropItem* pDropItem);

	// gets
	CCUID GetUID()					{ return m_UID; }				///< NPC UID ��ȯ
	CCQUEST_NPC	GetType()			{ return m_nType; }				///< NPC ���� ��ȯ
	CCUID& GetController()			{ return m_uidController; }		///< NPC ������(�÷��̾�) UID ��ȯ
	CCQuestDropItem* GetDropItem()	{ return &m_DropItem; }			///< ��� ������ ���� ��ȯ

	inline void SetFlag(unsigned int nFlag, bool bValue);			///< �÷��� ����
	inline bool CheckFlag(unsigned int nFlag);						///< �÷��� üũ
	inline void SetFlags(unsigned int nFlags);						///< �÷��� ����
	inline unsigned long GetFlags();								///< �÷��� ��ȯ
	inline bool HasDropItem();										///< ��� �������� ������ �ִ��� ��ȯ

};

typedef map<CCUID, CCMatchNPCObject*>		CCMatchNPCObjectMap;


class CCQuestPlayerManager;

/// NPC ������Ʈ ������
class CCMatchNPCManager
{
private:
	// var
	CCMatchStage*					m_pStage;
	CCQuestPlayerManager*			m_pPlayerManager;
	CCMatchNPCObjectMap				m_NPCObjectMap;

	unsigned long int				m_nLastSpawnTime;		// for test

	CCUID							m_uidKeyNPC;				// keyNPC
	int								m_nNPCCount[MNST_END];		// ����Ÿ�Ժ� ����ִ� NPC��
	int								m_nBossCount;				// ����ִ� ���� ��
	bool							m_bBossDie;					// ������ ��� �׾����� Ȯ��
	bool							m_bKeyNPCDie;				// keyNPC�� �׾����� Ȯ��

	// func
	CCUID NewUID();
	bool AssignControl(CCUID& uidNPC, CCUID& uidPlayer);
	bool Spawn(CCUID& uidNPC, CCUID& uidController, unsigned char nSpawnPositionIndex);
	void Clear();
	bool FindSuitableController(CCUID& out, CCQuestPlayerInfo* pSender);

	void SetNPCObjectToControllerInfo(CCUID& uidChar, CCMatchNPCObject* pNPCObject);
	void DelNPCObjectToControllerInfo(CCUID& uidChar, CCMatchNPCObject* pNPCObject);
public:
	/// ������
	CCMatchNPCManager();
	/// �Ҹ���
	~CCMatchNPCManager();
	/// �ʱ�ȭ
	/// @param pStage				�������� Ŭ����
	/// @param pPlayerManager		����Ʈ�꿡���� PlayerManager
	void Create(CCMatchStage* pStage, CCQuestPlayerManager* pPlayerManager);
	/// ����
	void Destroy();
	/// ��� NPC�� ���ش�.
	void ClearNPC();
	/// NPC ������Ʈ ����
	/// @param nType					NPC ����
	/// @param nSpawnPositionIndex		���� ��ġ
	/// @param bKeyNPC					�� NPC�� keyNPC�ΰ�
	CCMatchNPCObject* CreateNPCObject(CCQUEST_NPC nType, unsigned char nSpawnPositionIndex, bool bKeyNPC);
	/// NPC ������Ʈ ����
	/// @param uidNPC					NPC UID
	/// @param outItem					NPC�� ����ϴ� ������ ��ȯ��
	bool DestroyNPCObject(CCUID& uidNPC, CCQuestDropItem& outItem);
	/// NPC ������Ʈ ��ȯ
	/// @param uidNPC					NPC UID
	CCMatchNPCObject* GetNPCObject(CCUID& uidNPC);
	/// �÷��̾ ������������ ������ ȣ��ȴ�.
	/// @param uidPlayer				�÷��̾� UID
	void OnDelPlayer(const CCUID& uidPlayer);
	/// �ش� �÷��̾ �ش� NPC�� �����ϰ� �ִ��� üũ
	/// @param uidChar					�÷��̾� UID
	/// @param uidNPC					NPC UID
	bool IsControllersNPC(CCUID& uidChar, CCUID& uidNPC);
	/// NPC ������Ʈ�� ��ȯ
	int GetNPCObjectCount();
	/// �ش� ����Ÿ���� NPC ������Ʈ�� ��ȯ
	/// @param nSpawnType				NPC ���� Ÿ��
	int GetNPCObjectCount(CCQuestNPCSpawnType nSpawnType);
	int GetBossCount() { return m_nBossCount; }
	/// ������ �׾����� Ȯ��
	bool IsBossDie()		{ return m_bBossDie;	}
	/// keyNPC�� �׾����� Ȯ��(���� �����̹� ����..)
	bool IsKeyNPCDie()		{ return m_bKeyNPCDie; }
	/// keyNPC���� ������ �ʱ�ȭ
	void ClearKeyNPCState();

	void RemovePlayerControl(const CCUID& uidPlayer);
};



// inlines //////////////////////////////////////////////////////////////////////////////////
inline void CCMatchNPCObject::SetFlags(unsigned int nFlags)
{
	if (m_nFlags != nFlags)
	{
		m_nFlags = nFlags;
	}
}

inline void CCMatchNPCObject::SetFlag(unsigned int nFlag, bool bValue)
{
	if (bValue) m_nFlags |= nFlag;
	else m_nFlags &= ~nFlag;
}

inline bool CCMatchNPCObject::CheckFlag(unsigned int nFlag)
{
	return ((m_nFlags & nFlag) != 0);
}

inline unsigned long CCMatchNPCObject::GetFlags() 
{ 
	return m_nFlags; 
}

inline int CCMatchNPCManager::GetNPCObjectCount()
{
	return (int)m_NPCObjectMap.size();
}

inline bool CCMatchNPCObject::HasDropItem()
{
	return (m_DropItem.nDropItemType != QDIT_NA);
}


inline int CCMatchNPCManager::GetNPCObjectCount(CCQuestNPCSpawnType nSpawnType)
{
	return m_nNPCCount[nSpawnType];
}
