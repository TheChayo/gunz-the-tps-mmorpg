#include "stdafx.h"
#include "CCHackingTypes.h"

const PUNISH_TABLE_ITEM MPunishTable::PUNISH_TABLE[CCMHT_END] =
{
	{ 0,	  "none",							0, 0, 0,	MMBL_NO },		// CCMHT_NO = 0,							
	{ 130001, "irregularity player",			0, 0, 10,	MMBL_LOGONLY },	// CCMHT_XTRAP_HACKER, 10���� ���� ����.
	{ 130001, "x-trap hacking detected.",		0, 1, 0,	MMBL_LOGONLY },	// CCMHT_HSHIELD_HACKER, 1�ð� �Ŀ� ���� ����.
	{ 130001, "hackshield hacking detected.",	0, 1, 0,	MMBL_LOGONLY },	// CCMHT_BADFILECRC,
	{ 130004, "bad filecrc.",					0, 0, 0,	MMBL_LOGONLY },	// CCMHT_BADUSER,
	{ 130001, "gameguard hacker",				0, 0, 0,	MMBL_LOGONLY },	// CCMHT_GAMEGUARD_HACKER,
	{ 130001, "dll injectoin",					0, 0, 0,	MMBL_LOGONLY },	// CCMHT_GIVE_ONESELF_UP_DLLINJECTION,
	{ 130001, "invalid stage setting.",			0, 0, 0,	MMBL_LOGONLY },	// CCMHT_INVALIDSTAGESETTING,
	{ 130005, "command flooding.",				0, 1, 0,	MMBL_ACCOUNT },	// CCMHT_COMMAND_FLOODING, 1�ð� �Ŀ� ���� ����.
	{ 130006, "block by admin",					0, 1, 0,	MMBL_ACCOUNT },	// CCMHT_COMMAND_BLOCK_BY_ADMIN, 1�ð� �Ŀ� ���� ����.
	{ 130007, "Sleep Account, Block by Admin",	0, 1, 0,	MMBL_ACCOUNT },	// CCMHT_SLEEP_ACCOUNT, ��ڿ� ���� �޸� �������� �з��� ȸ��. ������ ���� �Ұ���
};

const PUNISH_TABLE_ITEM& MPunishTable::GetPunish( CCMatchHackingType eType )
{
	if(eType<0 || eType >=CCMHT_END) return PUNISH_TABLE[0];

	return PUNISH_TABLE[ eType ];
}
