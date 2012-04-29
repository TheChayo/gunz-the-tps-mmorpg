#include "CCObjectTypes.h"

const unsigned long int	g_nHPRegenTickInteval = 1000;		// (1 sec)
const unsigned long int g_nENRegenTickInteval = 1000;		// (1 sec)


int CCGetHP_LCM(int nLevel, CCCharacterClass nClass);
float CCGetEN_LCM(int nLevel, CCCharacterClass nClass);

int CCGetChangingModeTickCount(CCCharacterMode mode);		///< ĳ���� ��� ���Խð�(msec����)

int CCCalculateMaxHP(int nLevel, CCCharacterClass nClass, int nCON, int nDCS);	///< �ִ� HP ���
int CCCalculateMaxEN(int nLevel, CCCharacterClass nClass, int nINT, int nDMS);	///< �ִ� EN ���
float CCCalculateRegenHPPerTick(int nLevel, CCCharacterRace nRace, int nDCS);		///< �ʴ� ���� HP ���
float CCCalculateRegenENPerTick(int nLevel, CCCharacterClass nClass, int nDMS);	///< �ʴ� ���� EN ���
float CCCalculateAtkDamage();	///< Attack Damage ���
