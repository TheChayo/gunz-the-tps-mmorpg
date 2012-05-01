#pragma once
#include <crtdbg.h>
#include <map>
#include <set>
using namespace std;


/// ���� Ÿ��
enum CCMATCH_GAMETYPE {
	CCMATCH_GAMETYPE_DEATHMATCH_SOLO		=0,			///< ���� ������ġ
	CCMATCH_GAMETYPE_DEATHMATCH_TEAM		=1,			///< �� ������ġ
	CCMATCH_GAMETYPE_GLADIATOR_SOLO		=2,			///< ���� �۷�������
	CCMATCH_GAMETYPE_GLADIATOR_TEAM		=3,			///< �� �۷�������
	CCMATCH_GAMETYPE_ASSASSINATE			=4,			///< ������(�ϻ���)
	CCMATCH_GAMETYPE_TRAINING			=5,			///< ����

	CCMATCH_GAMETYPE_SURVIVAL			=6,			///< �����̹�
	CCMATCH_GAMETYPE_QUEST				=7,			///< ����Ʈ

	CCMATCH_GAMETYPE_BERSERKER			=8,			///< ������ġ ����Ŀ
	CCMATCH_GAMETYPE_DEATHMATCH_TEAM2	=9,			///< ��������ġ �ͽ�Ʈ��
	CCMATCH_GAMETYPE_DUEL				=10,		///< ��� ��ġ
	CCMATCH_GAMETYPE_DUELTOURNAMENT		=11,		///< ��� ��ʸ�Ʈ
/*
#ifndef _CLASSIC
	CCMATCH_GAMETYPE_CLASSIC_SOLO,
	CCMATCH_GAMETYPE_CLASSIC_TEAM,
#endif
*/
	CCMATCH_GAMETYPE_MAX,
};
// ��尡 �߰��Ǹ� ���÷��� ����� ��� �� �߰��ؾ� �Ѵ� (cpp)
extern const char* CCMatchGameTypeAcronym[CCMATCH_GAMETYPE_MAX];

#define		MAX_RELAYMAP_LIST_COUNT			20
struct RelayMap
{
	int				nMapID;
};

enum RELAY_MAP_TYPE
{
	RELAY_MAP_TURN			= 0,
	RELAY_MAP_RANDOM,

	RELAY_MAP_MAX_TYPE_COUNT
};

enum RELAY_MAP_REPEAT_COUNT
{
	RELAY_MAP_1REPEAT			= 0,
	RELAY_MAP_2REPEAT,
	RELAY_MAP_3REPEAT,
	RELAY_MAP_4REPEAT,
	RELAY_MAP_5REPEAT,

	RELAYMAP_MAX_REPEAT_COUNT,
};

// ����Ʈ ���� Ÿ��
const CCMATCH_GAMETYPE CCMATCH_GAMETYPE_DEFAULT = CCMATCH_GAMETYPE_DEATHMATCH_SOLO;
// const CCMATCH_GAMETYPE CCMATCH_GAMETYPE_DEFAULT = CCMATCH_GAMETYPE_DEATHMATCH_TEAM2;


struct CCMatchGameTypeInfo
{
	CCMATCH_GAMETYPE		nGameTypeID;			// ID
	char				szGameTypeStr[24];		// ���� Ÿ�� �̸�
	float				fGameExpRatio;			// ����ġ ��� ���� - ���� Gladiator�� 50%�̴�
	float				fTeamMyExpRatio;		// �������� ���ο��� ����Ǵ� ����ġ
	float				fTeamBonusExpRatio;		// �������� ������ �����Ǵ� ����ġ
	set<int>			MapSet;					// �� ����Ÿ�Կ��� �÷��� ������ ��
	void Set(const CCMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		     const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio);
	void AddMap(int nMapID);
	void AddAllMap();
};


class CCBaseGameTypeCatalogue
{
private:
	CCMatchGameTypeInfo			m_GameTypeInfo[CCMATCH_GAMETYPE_MAX];
public:
	CCBaseGameTypeCatalogue();
	virtual ~CCBaseGameTypeCatalogue();

	inline CCMatchGameTypeInfo* GetInfo(CCMATCH_GAMETYPE nGameType);
	inline const char* GetGameTypeStr(CCMATCH_GAMETYPE nGameType);
	inline void SetGameTypeStr(CCMATCH_GAMETYPE nGameType, const char* szName);
	inline bool IsCorrectGameType(const int nGameTypeID);	///< �ùٸ� ���� Ÿ�� ID���� ����
	inline bool IsTeamGame(CCMATCH_GAMETYPE nGameType);		///< �ش� ����Ÿ���� �������� ����
	inline bool IsTeamLimitTime(CCMATCH_GAMETYPE nGameType);
	inline bool IsWaitForRoundEnd(CCMATCH_GAMETYPE nGameType);		///< ���� ���������� ����� �ؾ��ϴ��� ����
	inline bool IsQuestOnly(CCMATCH_GAMETYPE nGameType);	///< �ش� ����Ÿ���� ����Ʈ���� ����
	inline bool IsSurvivalOnly(CCMATCH_GAMETYPE nGameType); ///< �ش� ����Ÿ���� �����̹����� ����
	inline bool IsQuestDerived(CCMATCH_GAMETYPE nGameType);	///< �ش� ����Ÿ���� ����Ʈ ����(NPC�� ��������)���� ����
	inline bool IsWorldItemSpawnEnable(CCMATCH_GAMETYPE nGameType);	///< �ش� ����Ÿ�Կ��� ����������� �����Ǵ��� ����
};


//////////////////////////////////////////////////////////////////////////////////
inline bool CCBaseGameTypeCatalogue::IsTeamGame(CCMATCH_GAMETYPE nGameType)
{
	// ���� ���������� Description�� teamgame���� ���ΰ��� �ֵ��� ����.

	if ((nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||
		(nGameType == CCMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_ASSASSINATE) )
	{
		return true;
	}
	return false;
}

inline bool CCBaseGameTypeCatalogue::IsTeamLimitTime(CCMATCH_GAMETYPE nGameType)
{
	// ���� ���������� Description�� teamgame���� ���ΰ��� �ֵ��� ����.

	if ((nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_DUEL) ||
		(nGameType == CCMATCH_GAMETYPE_ASSASSINATE) )
	{
		return true;
	}
	return false;
}

inline bool CCBaseGameTypeCatalogue::IsWaitForRoundEnd(CCMATCH_GAMETYPE nGameType)
{
	// ���� ���������� ����� �ϴ°�?

	if ((nGameType == CCMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_DUEL) ||
		(nGameType == CCMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == CCMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == CCMATCH_GAMETYPE_DUELTOURNAMENT))
	{
		return true;
	}
	return false;
}


inline bool CCBaseGameTypeCatalogue::IsQuestDerived(CCMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == CCMATCH_GAMETYPE_SURVIVAL) ||(nGameType == CCMATCH_GAMETYPE_QUEST) )
	{
		return true;
	}

	return false;
}

inline bool CCBaseGameTypeCatalogue::IsQuestOnly(CCMATCH_GAMETYPE nGameType)
{
	return nGameType == CCMATCH_GAMETYPE_QUEST;
}

inline bool CCBaseGameTypeCatalogue::IsSurvivalOnly(CCMATCH_GAMETYPE nGameType)
{
	return nGameType == CCMATCH_GAMETYPE_SURVIVAL;
}

inline const char* CCBaseGameTypeCatalogue::GetGameTypeStr(CCMATCH_GAMETYPE nGameType)
{
	return m_GameTypeInfo[nGameType].szGameTypeStr;
}

inline void CCBaseGameTypeCatalogue::SetGameTypeStr(CCMATCH_GAMETYPE nGameType, const char* szName)
{
	strcpy( m_GameTypeInfo[nGameType].szGameTypeStr, szName) ;
}

bool CCBaseGameTypeCatalogue::IsCorrectGameType(const int nGameTypeID)
{
	if ((nGameTypeID < 0) || (nGameTypeID >= CCMATCH_GAMETYPE_MAX)) return false;
	return true;
}

inline CCMatchGameTypeInfo* CCBaseGameTypeCatalogue::GetInfo(CCMATCH_GAMETYPE nGameType)
{
	_ASSERT((nGameType >= 0) && (nGameType < CCMATCH_GAMETYPE_MAX));
	return &m_GameTypeInfo[nGameType];
}

inline bool CCBaseGameTypeCatalogue::IsWorldItemSpawnEnable(CCMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == CCMATCH_GAMETYPE_SURVIVAL) ||(nGameType == CCMATCH_GAMETYPE_QUEST) )
	{
		return false;
	}

	return true;

}