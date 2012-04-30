#pragma once

#include "MSingleton.h"
#include "MUtil.h"

// ���⿡ �ϳ� �߰� �Ͻø� �Ʒ� ��¡ ���̺� �ϳ� �߰��ϼž� �մϴ�
enum CCMatchHackingType
{
	CCMHT_NO = 0,	
	CCMHT_XTRAP_HACKER,
	CCMHT_HSHIELD_HACKER,
	CCMHT_BADFILECRC,
	CCMHT_BADUSER,
	CCMHT_GAMEGUARD_HACKER,
	CCMHT_GIVE_ONESELF_UP_DLLINJECTION,
	CCMHT_INVALIDSTAGESETTING,
	CCMHT_COMMAND_FLOODING,
	CCMHT_COMMAND_BLOCK_BY_ADMIN,
	CCMHT_SLEEP_ACCOUNT = 10,					///< �ϴ��� NHN ��û�� ���� �޸� ���� ó���̴�. (����� 10�̾�� �Ѵ�. �ٲ� �ȵȴ�)
	CCMHT_END,
};

struct PUNISH_TABLE_ITEM {
	DWORD				dwMessageID;
	const char*			szComment;

	u_short				nDay;
	u_short				nHour;
	u_short				nMin;
	
	CCMatchBlockLevel	eLevel;
};

class MPunishTable {
	static const PUNISH_TABLE_ITEM PUNISH_TABLE[CCMHT_END];
public:
	static const PUNISH_TABLE_ITEM& GetPunish( CCMatchHackingType eType );
};