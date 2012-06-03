#pragma once
#include "winsock2.h"
#include "CCXml.h"
#include <vector>
using namespace std;

#include "CCMatchItem.h"

#define FILENAME_MATCH_FORMULA		"formula.xml"
#define MAX_LEVEL					99		///< �ִ� ����

class CCMatchEquipedItem;
class CCMatchStageSetting;
class CCMatchObject;

class CCMatchFormula
{
private:
	static float				m_fNeedExpLMTable[MAX_LEVEL+1];
	static float				m_fGettingExpLMTable[MAX_LEVEL+1];
	static float				m_fGettingBountyLMTable[MAX_LEVEL+1];

	static unsigned long int	m_nNeedExp[MAX_LEVEL+1];
	static unsigned long int	m_nGettingExp[MAX_LEVEL+1];
	static unsigned long int	m_nGettingBounty[MAX_LEVEL+1];

	static bool ReadXml(const char* szXmlFileName);
	static void ParseNeedExpLM(::CCXmlElement& element);
	static void ParseGettingExpLM(::CCXmlElement& element);
	static void ParseGettingBountyLM(::CCXmlElement& element);

	static void PreCalcGettingBounty();
	static void PreCalcNeedExp();
	static void PreCalcGettingExp();
public:
	static bool Create();
	static unsigned long int CalcPanaltyEXP(int nAttackerLevel, int nVictimLevel);
	static unsigned long int GetSuicidePanaltyEXP(int nLevel);
	static unsigned long int GetGettingExp(int nAttackerLevel, int nVictimLevel);
	static unsigned long int GetGettingBounty(int nAttackerLevel, int nVictimLevel);
	static unsigned long int GetNeedExp(int nLevel) { if (nLevel<0) nLevel=0; if (nLevel>MAX_LEVEL) nLevel=MAX_LEVEL; return m_nNeedExp[nLevel]; }
	static int GetLevelFromExp(unsigned long int nExp);

	// ����ġ�� ���� ���� ��������� �ۼ�Ʈ�� ��ȯ
	static int GetLevelPercent(unsigned long int nExp, int nNowLevel);
	static int GetClanBattlePoint(int nWinnerClanPoint, int nLoserClanPoint, int nOneTeamMemberCount);

	// ����ġ ���ʽ� ���
	static float CalcXPBonusRatio(CCMatchObject* pCharObj, CCMatchItemBonusType nBonusType);
	// �ٿ�Ƽ ���ʽ� ���
	static float CalcBPBounsRatio(CCMatchObject* pCharObj, CCMatchItemBonusType nBonusType );
};
