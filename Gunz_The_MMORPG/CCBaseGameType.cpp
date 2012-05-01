#include "stdafx.h"
#include "CCBaseGameType.h"


#define CCMATCH_GAMETYPE_DEATHMATCH_SOLO_STR		"Death Match(Solo)"
#define CCMATCH_GAMETYPE_DEATHMATCH_TEAM_STR		"Death Match(Team)"
#define CCMATCH_GAMETYPE_GLADIATOR_SOLO_STR		"Gladiator(Solo)"
#define CCMATCH_GAMETYPE_GLADIATOR_TEAM_STR		"Gladiator(Team)"
#define CCMATCH_GAMETYPE_ASSASSINATE_STR			"Assassinate"
#define CCMATCH_GAMETYPE_TRAINING_STR			"Training"
#define CCMATCH_GAMETYPE_CLASSIC_SOLO_STR		"Classic(Solo)"
#define CCMATCH_GAMETYPE_CLASSIC_TEAM_STR		"Class(Team)"
#define CCMATCH_GAMETYPE_SURVIVAL_STR			"Survival"
#define CCMATCH_GAMETYPE_QUEST_STR				"Quest"
#define CCMATCH_GAMETYPE_BERSERKER_STR			"Berserker"
#define CCMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR	"Death Match(Solo Unlimit)"
#define CCMATCH_GAMETYPE_DUEL_STR				"Duel"
#define CCMATCH_GAMETYPE_DUELTOURNAMENT_STR		"Duel Tournament"

// ���÷��� ���ϸ�� ��� ��� (enum CCMATCH_GAMETYPE �� ������ ���ƾ���)
const char* CCMatchGameTypeAcronym[CCMATCH_GAMETYPE_MAX] = {
	"DM",		// DEATHMATCH_SOLO	
	"DMT",		// DEATHMATCH_TEAM	
	"GL",		// GLADIATOR_SOLO	
	"GLT",		// GLADIATOR_TEAM	
	"ASSA",		// ASSASSINATE		
	"TR",		// TRAINING		
	"SURV",		// SURVIVAL		
	"QST",		// QUEST			
	"DMBSK",	// BERSERKER (DeathMatch Berserker)		
	"DMTE",		// DEATHMATCH_TEAM2 (DeathMatch Team Extream)
	"DUEL",		// DUEL			
	"DUELT",	// DUELTOURNAMENT	
};

void CCMatchGameTypeInfo::Set(const CCMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		    const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio)
{
	CCMatchGameTypeInfo::nGameTypeID = a_nGameTypeID;
	strcpy(CCMatchGameTypeInfo::szGameTypeStr, a_szGameTypeStr);
	CCMatchGameTypeInfo::fGameExpRatio = a_fGameExpRatio;
	CCMatchGameTypeInfo::fTeamMyExpRatio = a_fTeamMyExpRatio;
	CCMatchGameTypeInfo::fTeamBonusExpRatio = a_fTeamBonusExpRatio;
}

void CCMatchGameTypeInfo::AddMap(int nMapID)
{
	MapSet.insert(set<int>::value_type(nMapID));
}

void CCMatchGameTypeInfo::AddAllMap()
{
	for (int i = 0; i < CCMATCH_MAP_MAX; i++)
	{
		AddMap(i);
	}
}

CCBaseGameTypeCatalogue::CCBaseGameTypeCatalogue()
{
#define _InitGameType(index, id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio)		\
m_GameTypeInfo[index].Set(id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio);

//				index,							id,									����Ÿ���̸�,					����ġ��� ����, ���� ���� ����ġ �����, ���� �� ����ġ �����
_InitGameType(CCMATCH_GAMETYPE_DEATHMATCH_SOLO,	CCMATCH_GAMETYPE_DEATHMATCH_SOLO,	CCMATCH_GAMETYPE_DEATHMATCH_SOLO_STR,  1.0f,			1.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_DEATHMATCH_TEAM,	CCMATCH_GAMETYPE_DEATHMATCH_TEAM,	CCMATCH_GAMETYPE_DEATHMATCH_TEAM_STR,  1.0f,			0.8f,					0.3f);
_InitGameType(CCMATCH_GAMETYPE_GLADIATOR_SOLO,	CCMATCH_GAMETYPE_GLADIATOR_SOLO,		CCMATCH_GAMETYPE_GLADIATOR_SOLO_STR,   0.5f,			1.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_GLADIATOR_TEAM,	CCMATCH_GAMETYPE_GLADIATOR_TEAM,		CCMATCH_GAMETYPE_GLADIATOR_TEAM_STR,   0.5f,			0.8f,					0.3f);
_InitGameType(CCMATCH_GAMETYPE_ASSASSINATE,		CCMATCH_GAMETYPE_ASSASSINATE,		CCMATCH_GAMETYPE_ASSASSINATE_STR,	  1.0f,			0.8f,					0.3f);
_InitGameType(CCMATCH_GAMETYPE_TRAINING,			CCMATCH_GAMETYPE_TRAINING,			CCMATCH_GAMETYPE_TRAINING_STR,		  0.0f,			0.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_SURVIVAL,			CCMATCH_GAMETYPE_SURVIVAL,			CCMATCH_GAMETYPE_SURVIVAL_STR,		  0.0f,			0.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_QUEST,			CCMATCH_GAMETYPE_QUEST,				CCMATCH_GAMETYPE_QUEST_STR,			  0.0f,			0.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_BERSERKER,		CCMATCH_GAMETYPE_BERSERKER,			CCMATCH_GAMETYPE_BERSERKER_STR,		  1.0f,			1.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_DEATHMATCH_TEAM2,	CCMATCH_GAMETYPE_DEATHMATCH_TEAM2,	CCMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR, 1.0f,			0.6f,					0.5f);
_InitGameType(CCMATCH_GAMETYPE_DUEL,				CCMATCH_GAMETYPE_DUEL,				CCMATCH_GAMETYPE_DUEL_STR,			  1.0f,			1.0f,					0.0f);
_InitGameType(CCMATCH_GAMETYPE_DUELTOURNAMENT,	CCMATCH_GAMETYPE_DUELTOURNAMENT,		CCMATCH_GAMETYPE_DUELTOURNAMENT_STR,	  0.0f,			0.0f,					0.0f);

/*
#ifdef _CLASSIC
_InitGameType(CCMATCH_GAMETYPE_CLASSIC_SOLO, CCMATCH_GAMETYPE_CLASSIC_SOLO,
			  CCMATCH_GAMETYPE_CLASSIC_SOLO_STR,		 1.0f,	1.0f,	0.0f);
_InitGameType(CCMATCH_GAMETYPE_CLASSIC_TEAM, CCMATCH_GAMETYPE_CLASSIC_TEAM,
			  CCMATCH_GAMETYPE_CLASSIC_TEAM_STR,		 1.0f,	0.8f,	0.3f);
#endif
*/

	// �� ����Ÿ�Կ��� �÷��� ������ �� - ����� ��� ���� �� ����
	for (int i = 0; i < CCMATCH_GAMETYPE_MAX; i++)
	{
		m_GameTypeInfo[i].AddAllMap();
	}
}

CCBaseGameTypeCatalogue::~CCBaseGameTypeCatalogue()
{

}



