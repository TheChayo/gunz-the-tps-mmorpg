#pragma once

#include "CCQuestConst.h"

struct CCQuestScenarioInfoMapJaco
{
	CCQUEST_NPC	nNPCID;
	float		fRate;
};


struct CCQuestScenarioInfoMaps
{
	int										nKeySectorID;			///< 최종 섹터 ID
	int										nKeyNPCID;				///< 키 NPC ID (퀘스트모드에서 최종 섹터에 출현하는 보스형NPC)
	bool									bKeyNPCIsBoss;			///< 키 NPC가 보스인지 여부
	vector<int>								vecNPCSetArray;			///< NPC Set Array

	vector<int>								vecKeyNPCArray;			///< 각 섹터의 KeyNpc들 (서바이벌 전용)
	vector<int>								vecSectorXpArray;		///< 각 섹터의 XP (서바이벌 전용)
	vector<int>								vecSectorBpArray;		///< 각 섹터의 BP (서바이벌 전용)

	// 보스방에서 쓸 정보
	int										nJacoCount;				///< 1회 스폰시 스폰될 졸병수
	unsigned int							nJacoSpawnTickTime;		///< 졸병 스폰 틱 타임
	int										nJacoMinNPCCount;		///< 이값이하일때는 졸병이 스폰하지 않는다.
	int										nJacoMaxNPCCount;		///< 이값이하일때는 졸병이 스폰하지 않는다.
	vector<CCQuestScenarioInfoMapJaco>		vecJacoArray;			///< 보스방에서 나올 졸병들

	CCQuestScenarioInfoMaps()
	{
		nKeySectorID = 0;
		nKeyNPCID = 0;
		bKeyNPCIsBoss = false;
		nJacoCount = 0;
		nJacoSpawnTickTime = 9999999;
		nJacoMinNPCCount = 0;
		nJacoMaxNPCCount = 0;
	}
};

/// 퀘스트 시나리오 정보
struct CCQuestScenarioInfo
{
	int				nID;										///< 시나리오 ID
	char			szTitle[64];								///< 시나리오 이름
	int				nQL;										///< 요구 퀘스트 레벨
	float			fDC;										///< 난이도 계수(DC)
	int				nResSacriItemCount;							///< 시나리오를 위한 희생 아이템 개수
	unsigned int	nResSacriItemID[MAX_SCENARIO_SACRI_ITEM];	///< 시나리오를 위한 희생 아이템
	int				nMapSet;									///< 맵셋
	bool			bSpecialScenario;							///< 특별시나리오인지 여부

	int				nXPReward;									///< XP 보상치
	int				nBPReward;									///< BP 보상치
	int				nRewardItemCount;							///< 특별 아이템 개수
	int				nRewardItemID[MAX_SCENARIO_REWARD_ITEM];	///< 특별 아이템 보상
	float			fRewardItemRate[MAX_SCENARIO_REWARD_ITEM];	///< 특별 아이템 보상 확률
	int				nSectorXP;									///< 섹터별 보너스 XP 보상치
	int				nSectorBP;									///< 섹터별 보너스 BP 보상치
	int				nRepeat;									///< 시나리오 반복 횟수 (서바이벌용)
	int				nMaxSpawn;									///< 섹터의 최대 NPC 스폰 횟수 (서바이벌용)
	int				nMaxSpawnSameTime;							///< 섹터의 최대 동시 NPC 스폰량 (서바이벌용)

	CCQuestScenarioInfoMaps		Maps[SCENARIO_STANDARD_DICE_SIDES];

	/// 생성자
	CCQuestScenarioInfo()
	{
		nID = -1;
		szTitle[0] = 0;
		nQL = 0;
		fDC = 0.0f;
		nResSacriItemCount = 0;
		memset(nResSacriItemID, 0, sizeof(nResSacriItemID));
		nMapSet = 0;
		nXPReward = 0;
		nBPReward = 0;
		nRewardItemCount = 0;
		memset(fRewardItemRate, 0, sizeof(fRewardItemRate));
		bSpecialScenario = false;

		for (int i = 0; i < SCENARIO_STANDARD_DICE_SIDES; i++)
		{
			Maps[i].nKeySectorID = 0;
			Maps[i].nKeyNPCID = 0;
		}

		nSectorXP = -1;
		nSectorBP = -1;
		nRepeat = 0;
		nMaxSpawn = 1;
		nMaxSpawnSameTime = 1;
	}

	/// 섹터 수 반환
	int GetSectorCount(int nDice)
	{
		return (int)Maps[nDice].vecNPCSetArray.size();
	}
};

/// 시나리오 정보 관리자
class CCQuestScenarioCatalogue : public map<int, CCQuestScenarioInfo*>
{
private:
	// 멤버 변수
	int		m_nDefaultStandardScenarioID;
	// 함수
	void Clear();
	void Insert(CCQuestScenarioInfo* pScenarioInfo);
	void ParseSpecialScenario(::CCXmlElement& element);
	void ParseStandardScenario(::CCXmlElement& element);
	void ParseNPCSetArray(::CCXmlElement& element, vector<int>& vec);
	void ParseKeyNPCArray(::CCXmlElement& element, vector<int>& vec);
	void ParseSectorXpBpArray(::CCXmlElement& element, vector<int>& vec);
	void ParseJaco(::CCXmlElement& element, CCQuestScenarioInfoMaps* pMap);
	void ParseRewardItem(::CCXmlElement& element, CCQuestScenarioInfo* pScenarioInfo);
	void ParseSacriItem(::CCXmlElement& element, CCQuestScenarioInfo* pScenarioInfo);
	void ParseMap(::CCXmlElement& element, CCQuestScenarioInfo* pScenarioInfo);
	int CalcStandardScenarioID(int nMapsetID, int nQL);
public:
	CCQuestScenarioCatalogue();											///< 생성자
	~CCQuestScenarioCatalogue();											///< 소멸자

	bool ReadXml(const char* szFileName);								///< xml로부터 npc정보를 읽는다.
	bool ReadXml(CCZFileSystem* pFileSystem,const char* szFileName);		///< xml로부터 npc정보를 읽는다.
	

	CCQuestScenarioInfo* GetInfo(int nScenarioID);						///< 시나리오 정보 반환
	/// 정규 시나리오 반환
	/// @param nQL				퀘스트 레벨
	/// @param nDice			주사위 굴림
	int GetStandardScenarioID(int nMapsetID, int nQL);

	/// 특별 시나리오 검색
	/// @param nMapsetID		맵셋
	/// @param nQL				퀘스트 레벨
	bool FindSpecialScenarioID(int nMapsetID, int nPlayerQL, unsigned int* SacriQItemIDs, unsigned int* outScenarioID);

	unsigned int MakeScenarioID(int nMapsetID, int nPlayerQL, unsigned int* SacriQItemIDs);


	const int GetDefaultStandardScenarioID() { return m_nDefaultStandardScenarioID; }
};