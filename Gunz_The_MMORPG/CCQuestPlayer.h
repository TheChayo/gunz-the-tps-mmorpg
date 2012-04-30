#pragma once

#include "CCMatchNPCObject.h"

#include <map>
using std::map;

struct RewardZItemInfo
{
	unsigned int		nItemID;
	int					nRentPeriodHour;
	int					nItemCnt;
};

class CCQuestRewardZItemList : public list<RewardZItemInfo>
{
};

/// ����Ʈ �꿡�� ���̴� �÷��� ����
struct CCQuestPlayerInfo
{
	// NPC Control ���� /////////
	CCMatchObject*		pObject;					///< Object ����
	unsigned long int	nNPCControlCheckSum;		///< NPC ���� üũ��
	CCMatchNPCObjectMap	NPCObjects;					///< �������� NPC
	bool				bEnableNPCControl;			///< NPC Control�� �������� ����

	/// NPC ���� ����
	/// - ���ھ �������� �켱������ ������
	int GetNPCControlScore()						
	{
		// ������ �׳� �����ϴ� NPC ����
		return (int)(NPCObjects.size());
	}


	// ����Ʈ �� ���� ///////////
	bool				bMovedtoNewSector;			///< ���� ���ͷ� �̵��ߴ��� ����


	// �����̹� �� ���� /////////



	// ���� ���� ////////////////
	int						nQL;						///< QL
	int						nDeathCount;				///< ���� ȸ��
	int						nUsedPageSacriItemCount;	///< �⺻ ��� ������ ��� ����(������)
	int						nUsedExtraSacriItemCount;	///< �߰� ��� ������ ��� ����
	int						nXP;						///< ���� XP
	int						nBP;						///< ���� BP
	int						nKilledNpcHpApAccum;		///< �÷��̾ ���� NPC AP,HP����

	CCQuestItemMap			RewardQuestItemMap;			///< ���� ����Ʈ ������
	CCQuestRewardZItemList	RewardZItemList;


	// Log���� ////////////////// - by �߱���.
	// char				szName[ 24 ];


	/// �ʱ�ȭ
	/// @param pObj		�÷��̾� ������Ʈ ����
	/// @param a_nQL	�÷��̾� ����Ʈ ����
	void Init(CCMatchObject* pObj, int a_nQL)
	{
		pObject = pObj;
		bEnableNPCControl = true;
		nNPCControlCheckSum = 0;
		NPCObjects.clear();
		bMovedtoNewSector = true;

		nQL = a_nQL;
		nDeathCount = 0;
		nUsedPageSacriItemCount = 0;
		nUsedExtraSacriItemCount = 0;
		nXP = 0;
		nBP = 0;
		nKilledNpcHpApAccum = 0;

		RewardQuestItemMap.Clear();
		RewardZItemList.clear();
	}

	/// ������
	CCQuestPlayerInfo() : nXP(0), nBP(0), nKilledNpcHpApAccum(0)
	{
		
	}
};

/// ����Ʈ���� �÷��̾� ������Ʈ ������
class CCQuestPlayerManager : public map<CCUID, CCQuestPlayerInfo*>
{
private:
	CCMatchStage* m_pStage;
	void AddPlayer(CCUID& uidPlayer);
public:
	CCQuestPlayerManager();										///< ������
	~CCQuestPlayerManager();										///< �Ҹ���
	void Create(CCMatchStage* pStage);							///< �ʱ�ȭ
	void Destroy();												///< ����
	void DelPlayer(CCUID& uidPlayer);							///< �÷��̾� ����
	void Clear();												///< �ʱ�ȭ
	CCQuestPlayerInfo* GetPlayerInfo(const CCUID& uidPlayer);		///< �÷��̾� ���� ��ȯ
};