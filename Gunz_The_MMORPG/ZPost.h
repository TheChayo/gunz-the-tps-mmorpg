#ifndef _ZPOST_H
#define _ZPOST_H

#include "ZPrerequisites.h"
#include "ZGameClient.h"
#include "CCBlobArray.h"
#include "CCMatchTransDataType.h"
#include "CCMath.h"
#include "CheckReturnCallStack.h"

#pragma pack(1)

inline bool IsNaN(float x)  { return x!=x; }
inline bool IsNaN(const rvector& v) { return (v.x!=v.x || v.y!=v.y || v.z!=v.z); }


// ���� �ð��������� ĳ���͵鳢�� ��� �ְ�޴� ������
struct ZPACKEDBASICINFO {
	float	fTime;
	short	posx, posy, posz;
	short	velx, vely, velz;
	short	dirx, diry, dirz;
	BYTE	upperstate;
	BYTE	lowerstate;
	BYTE	selweapon;
};

struct ZPACKEDSHOTINFO {
	float	fTime;
	short	posx, posy, posz;
	short	tox, toy, toz;
	BYTE	sel_type;
};

struct ZPACKEDDASHINFO {
	short	posx, posy, posz;
	short	dirx, diry, dirz;
	BYTE	seltype;
};

#pragma pack()

// ��Ŀ�� �𽺾������ Ŀ�ǵ�ID�� �˻��� ZPost���� �Լ��� ã�� ���ϵ��� �ϴ� ��ũ�� �Լ�
// Ŭ���̾�Ʈ �ڵ忡���� Ŀ�ǵ�ID�� ���� ������� ���� �� Ŀ�ǵ�ID ���� �Լ��� ���ļ� ����ϵ��� �Ѵ�.
// <�̷� ���� �ؾ߸��ϴ� ����>
// ���� ��� ZPostShot()�Լ� ��ġ�� ��Ŀ�� ã�� �ʹٸ�, MC_PEER_SHOT�� 0x2732�̹Ƿ� �𽺾���ؼ� PUSH 2732�� ã����
// ZPostShot()�� �ݹ� ã�Ƴ� �� �ִ�. �׷��� �츮�� 0x2732�� �ڵ忡 ��������� ����ϸ� �ʹ��� ��Ŀģȭ���� ȯ���̵ȴ�..
// �ζ��� ���� ���� �Լ� ������ 0x2732�� ������ ������ ������ ����ؼ� ID�� ��������� �巯���� �ʰ� �Ѵ�.

// �� ��ũ�δ� �����Ϸ� ����ȭ�� ��ġ�Ƿ� ������ �̹� �� ������� ����ǹǷ� ������ (FACTOR���� �ƹ� ������)
#define CLOAK_CMD_ID(CMD_ID, FACTOR)	UncloakCmdId((CMD_ID) + (FACTOR), FACTOR)
// �̰� �ٽ� �����ؼ� ���� id�� �ǵ����� �Լ� (�̰� �ζ����� �ƴϾ�� ��)
int UncloakCmdId(int cloakedCmdId, int cloakFactor);



inline void ZPostHPInfo(float fHP)
{
	ZPOSTCMD1(MC_PEER_HPINFO, MCmdParamFloat(fHP));
}

inline void ZPostHPAPInfo(float fHP, float fAP)
{
	ZPOSTCMD2(MC_PEER_HPAPINFO, MCmdParamFloat(fHP), MCmdParamFloat(fAP));
}

inline void ZPostDuelTournamentHPAPInfo(UCHAR MaxHP, UCHAR MaxAP, UCHAR HP, UCHAR AP)
{
	ZPOSTCMD4(MC_PEER_DUELTOURNAMENT_HPAPINFO, MCmdParamUChar(MaxHP), MCmdParamUChar(MaxAP), MCmdParamUChar(HP), MCmdParamUChar(AP));
}

inline void ZPostMove(rvector& vPos, rvector& vDir, rvector& vVelocity, 
					  ZC_STATE_UPPER upper, ZC_STATE_LOWER lower)
{
	ZPOSTCMD5(MC_PEER_MOVE, CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z), CCCommandParameterVector(vVelocity.x, vVelocity.y, vVelocity.z), CCCommandParameterInt(int(upper)), CCCommandParameterInt(int(lower)));
}

//DLL Injection �ٿ��� fShotTime�� ������ ������ ������ Ÿ���� �����ϰ� �̳� ���� �ӵ��� shot�� �����ϰ� �����. 
//�������� ��ƾ���� �ش� �Լ��� �Ҹ��� ��츦 ���� fShotTime = g_pGame->GetTime()�̴�. ���� �Ķ���ͷ� fShotTime �� ���� �ִ°ź���
//�ش� �Լ��� �ҷ��� �� ��Ŷ�� g_pGame->GetTime()�� ������ ������ ��ü�ߴ�. 
//inline void ZPostSkill(/*float fShotTime,*/int nSkill,int sel_type)	// ��ų�ߵ�~!
//{
////	ZPOSTCMD3(MC_PEER_SKILL, MCmdParamFloat(fShotTime), MCmdParamInt(nSkill),CCCommandParameterInt(sel_type));
//	ZPOSTCMD3(MC_PEER_SKILL, MCmdParamFloat(ZGetGame()->GetTime()), MCmdParamInt(nSkill),CCCommandParameterInt(sel_type));
//	CHECK_RETURN_CALLSTACK(ZPostSkill);
//}
//dll-injection���� ȣ�� ���ϰ� ���� �ζ��̴�
#define ZPostSkill(/*int*/ nSkill,/*int*/ sel_type)	{ \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_PEER_SKILL, 7636), MCmdParamFloat(ZGetGame()->GetTime()), MCmdParamInt(nSkill),CCCommandParameterInt(sel_type)); \
}

inline void ZPostChangeWeapon(int WeaponID)
{
	ZPOSTCMD1(MC_PEER_CHANGE_WEAPON, MCmdParamInt(WeaponID));
}

//dll-injection���� ȣ�� ���ϰ� ���� �ζ��̴�
#define ZPostSpMotion(/*int*/ type) { ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_SPMOTION, 123),MCmdParamInt(type)); }

inline void ZPostChangeParts(int PartsType,int PartsID)
{
	ZPOSTCMD2(MC_PEER_CHANGE_PARTS, MCmdParamInt(PartsType),MCmdParamInt(PartsID));
}

inline void ZPostChangeCharacter()
{
	ZPOSTCMD0(MC_PEER_CHANGECHARACTER);
}

inline void ZPostAttack(int type, rvector& vPos)
{
	ZPOSTCMD2(MC_PEER_ATTACK, MCmdParamInt(type),CCCommandParameterVector(vPos.x, vPos.y, vPos.z));
}

inline void ZPostDamage(CCUID ChrUID,int damage)
{
	ZPOSTCMD2(MC_PEER_DAMAGE, MCmdParamUID(ChrUID),MCmdParamInt(damage));
}

#define ZPostDie(/*CCUID*/ uidAttacker) { ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_DIE, 3421), MCmdParamUID(uidAttacker)); }

#ifdef _DEBUG
inline void ZPostSpawn(rvector& vPos, rvector& vDir)	// For Local Test Only
{
	ZPOSTCMD2(MC_PEER_SPAWN, CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterDir(vDir.x, vDir.y, vDir.z));
}
#endif

//dll-injection���� ȣ�� ���ϰ� ���� �ζ��̴�
// C/S Sync Spawn
#define ZPostRequestSpawn(/*CCUID*/ uidChar, /*rvector&*/ vPos, /*rvector&*/ vDir) { \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_MATCH_GAME_REQUEST_SPAWN, 8876), \
		MCmdParamUID(uidChar), MCmdParamPos(vPos.x, vPos.y, vPos.z), \
		MCmdParamDir(vDir.x, vDir.y, vDir.z)); \
}

//���� �ζ��̴�
#define ZPostDash(/*rvector&*/ vPos, /*rvector&*/ vDir, /*unsigned char*/ sel_type) { 	\
	ZPACKEDDASHINFO pdi;		\
	pdi.posx = Roundf(vPos.x);	\
	pdi.posy = Roundf(vPos.y);	\
	pdi.posz = Roundf(vPos.z);	\
								\
	pdi.dirx = vDir.x*32000;	\
	pdi.diry = vDir.y*32000;	\
	pdi.dirz = vDir.z*32000;	\
								\
	pdi.seltype = sel_type;		\
								\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_DASH, 9964),CCCommandParameterBlob(&pdi,sizeof(ZPACKEDDASHINFO)));	\
}

inline void ZPostPeerChat(char* szMsg, int nTeam=0)
{
	char szSendMsg[ 256 ] = {0,};
	
	const size_t nInputLen = strlen( szMsg );
	if( 256 > nInputLen )
	{
		ZPOSTCMD2(MC_PEER_CHAT, MCmdParamInt(nTeam), CCCommandParameterString(szMsg))	
	}
	else
	{
		strncpy( szSendMsg, szMsg, 255 );
		ZPOSTCMD2(MC_PEER_CHAT, MCmdParamInt(nTeam), CCCommandParameterString(szSendMsg))
	}

	CHECK_RETURN_CALLSTACK(ZPostPeerChat);
}

inline void ZPostPeerChatIcon(bool bShow)
{
	ZPOSTCMD1(MC_PEER_CHAT_ICON, MCmdParamBool(bShow));
}

inline void ZPostReload()
{
	ZPOSTCMD0(MC_PEER_RELOAD);
}

inline void ZPostPeerEnchantDamage(CCUID ownerUID, CCUID targetUID)
{
	ZPOSTCMD2(MC_PEER_ENCHANT_DAMAGE, MCmdParamUID(ownerUID), MCmdParamUID(targetUID));
}

inline void ZPostConnect(const char* szAddr, unsigned int nPort)
{
	char szCmd[256];
	sprintf(szCmd, "%s:%u", szAddr, nPort);
	ZPOSTCMD1(MC_NET_CONNECT, MCmdParamStr(szCmd));
}

inline void ZPostDisconnect()
{
	ZPOSTCMD0(MC_NET_DISCONNECT);
}

inline void ZPostLogin(char* szUserID, char* szPassword, unsigned int nChecksumPack, char *szEncryptMD5Value)
{
	void *pBlob = CCMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );
	
	ZPOSTCMD5(MC_MATCH_LOGIN, MCmdParamStr(szUserID)
		, MCmdParamStr(szPassword)
		, MCmdParamInt(MCOMMAND_VERSION)
		, MCmdParamUInt(nChecksumPack)
		, CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}

inline void ZPostNetmarbleLogin(const char* szAuthCookie, const char* szDataCookie, const char* szCPCookie, char* szSpareParam, unsigned int nChecksumPack)
{
	ZPOSTCMD6(MC_MATCH_LOGIN_NETMARBLE, MCmdParamStr(szAuthCookie), MCmdParamStr(szDataCookie), MCmdParamStr(szCPCookie), MCmdParamStr(szSpareParam), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack));
}

inline void ZPostNetmarbleJPLogin(char* szLoginID, char* szLoginPW, unsigned int nChecksumPack)
{
	ZPOSTCMD4(MC_MATCH_LOGIN_NETMARBLE_JP, MCmdParamStr(szLoginID), MCmdParamStr(szLoginPW), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack));
}

inline void ZPostNHNUSALogin( char* szLoginID, char* szAuthStr, unsigned int nChecksumPack, char *szEncryptMD5Value )
{
	void *pBlob = CCMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );

	ZPOSTCMD5( MC_MATCH_LOGIN_NHNUSA, MCmdParamStr(szLoginID), MCmdParamStr(szAuthStr), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)) );
	CCEraseBlobArray(pBlob);
}

inline void ZPostGameOnJPLogin( const char* szString, const char* szStatIndex, unsigned int nChecksumPack, char *szEncryptMD5Value )
{
	void *pBlob = CCMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );

	ZPOSTCMD5( MC_MATCH_LOGIN_GAMEON_JP, MCmdParamStr(szString), MCmdParamStr(szStatIndex), MCmdParamInt(MCOMMAND_VERSION), MCmdParamUInt(nChecksumPack), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}

inline void ZPostChannelRequestJoin(const CCUID& uidChar, const CCUID& uidChannel)
{
	ZPOSTCMD2(MC_MATCH_CHANNEL_REQUEST_JOIN, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidChannel));
}

inline void ZPostChannelRequestJoinFromChannelName(const CCUID& uidChar, int nChannelType, const char* szChannelName)
{
	if (strlen(szChannelName) >= CHANNELNAME_LEN) return;
	ZPOSTCMD3(MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME, MCmdParamUID(uidChar), MCmdParamInt(nChannelType), MCmdParamStr(szChannelName));
}

//���� �ζ��̴�
#define ZPostChannelChat(/*const CCUID&*/ uidChar, /*const CCUID&*/ uidChannel, /*char**/ szChat) { \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_MATCH_CHANNEL_REQUEST_CHAT, 87252), CCCommandParameterUID(uidChar), CCCommandParameterUID(uidChannel), MCmdParamStr(szChat)); \
}

inline void ZPostStartChannelList(const CCUID& uidChar, const int nChannelType)
{
	ZPOSTCMD2(MC_MATCH_CHANNEL_LIST_START, CCCommandParameterUID(uidChar), CCCommandParameterInt(nChannelType));
}

inline void ZPostStopChannelList(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CHANNEL_LIST_STOP, CCCommandParameterUID(uidChar));

}

inline void ZPostStageCreate(const CCUID& uidChar, char* szStageName, bool bPrivate, char* szPassword)
{
	ZPOSTCMD4(MC_MATCH_STAGE_CREATE, CCCommandParameterUID(uidChar), MCmdParamStr(szStageName),
		MCmdParamBool(bPrivate), MCmdParamStr(szPassword));
}

inline void ZPostRequestStageJoin(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_STAGE_JOIN, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage));
}

inline void ZPostStageGo(int nRoomNo)
{
	ZPOSTCMD1(MC_MATCH_STAGE_GO, MCmdParamUInt(nRoomNo));
}

inline void ZPostRequestPrivateStageJoin(const CCUID& uidChar, const CCUID& uidStage, char* szPassword)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage),
		MCmdParamStr(szPassword));
}

inline void ZPostStageLeave(const CCUID& uidChar)//, const CCUID& uidStage)
{
	ZPOSTCMD1(MC_MATCH_STAGE_LEAVE, CCCommandParameterUID(uidChar));//, CCCommandParameterUID(uidStage));
}

inline void ZPostRequestStagePlayerList(const CCUID& uidStage)
{
	ZPOSTCMD1(MC_MATCH_STAGE_REQUEST_PLAYERLIST, CCCommandParameterUID(uidStage));
}

inline void ZPostStageFollow(const char* pszTargetName)
{
	ZPOSTCMD1(MC_MATCH_STAGE_FOLLOW, CCCommandParameterString( const_cast<char*>(pszTargetName) ));
}

inline void ZPostStageStart(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD3(MC_MATCH_STAGE_START, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage), CCCommandParameterInt(3));
}

inline void ZPostStageMap(const CCUID& uidStage, char* szMap)
{
	ZPOSTCMD2(MC_MATCH_STAGE_MAP, CCCommandParameterUID(uidStage), CCCommandParameterString(szMap));
}

inline void ZPostStageRelayMapElementUpdate(const CCUID& uidStage, int nType, int nRoundCount)
{
	ZPOSTCMD3(MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE, CCCommandParameterUID(uidStage), CCCommandParameterInt(nType), CCCommandParameterInt(nRoundCount));
}

inline void ZPostStageRelayMapInfoUpdate(const CCUID& uidStage, int nType, int nRoundCount, void* pBlob)
{
	ZPOSTCMD4(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE, CCCommandParameterUID(uidStage), CCCommandParameterInt(nType), CCCommandParameterInt(nRoundCount), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
}

inline void ZPostStageChat(const CCUID& uidChar, const CCUID& uidStage, char* szChat)
{
	ZPOSTCMD3(MC_MATCH_STAGE_CHAT, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage), MCmdParamStr(szChat));
}

inline void ZPostRequestStageSetting(const CCUID& uidStage)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_STAGESETTING, CCCommandParameterUID(uidStage));
}

inline void ZPostStageSetting(const CCUID& uidChar, const CCUID& uidStage, MSTAGE_SETTING_NODE* pSetting)
{
	void* pBlob = CCMakeBlobArray(sizeof(MSTAGE_SETTING_NODE), 1);
	MSTAGE_SETTING_NODE* pBlobNode = (MSTAGE_SETTING_NODE*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pBlobNode, pSetting, sizeof(MSTAGE_SETTING_NODE));
	ZPOSTCMD3(MC_MATCH_STAGESETTING, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}

inline void ZPostStageTeam(const CCUID& uidChar, const CCUID& uidStage, int nTeam)
{
	ZPOSTCMD3(MC_MATCH_STAGE_TEAM, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage), CCCommandParameterUInt(nTeam));
	CHECK_RETURN_CALLSTACK(ZPostStageTeam);
}

inline void ZPostStageState(const CCUID& uidChar, const CCUID& uidStage, CCMatchObjectStageState nStageState)
{ 	
	ZPOSTCMD3(MC_MATCH_STAGE_PLAYER_STATE, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage), CCCommandParameterInt(int(nStageState)));
	CHECK_RETURN_CALLSTACK(ZPostStageState);
}

//DLL Injection �ٿ��� fShotTime�� ������ ������ ������ Ÿ���� �����ϰ� �̳� ���� �ӵ��� shot�� �����ϰ� �����. 
//�������� ��ƾ���� �ش� �Լ��� �Ҹ��� ��츦 ���� fShotTime = g_pGame->GetTime()�̴�. ���� �Ķ���ͷ� fShotTime �� ���� �ִ°ź���
//�ش� �Լ��� �ҷ��� �� ��Ŷ�� g_pGame->GetTime()�� ������ ������ ��ü�ߴ�. 
//inline void ZPostShot(/*float fShotTime,*/ rvector &pos, rvector &to,int sel_type)
//{	
//	ZPACKEDSHOTINFO info;
////	info.fTime=fShotTime;
//	info.fTime=ZGetGame()->GetTime();
//	info.posx = pos.x;
//	info.posy = pos.y;
//	info.posz = pos.z;
//	info.tox = to.x;
//	info.toy = to.y;
//	info.toz = to.z;
//	info.sel_type = sel_type;
//
//	ZPOSTCMD1(MC_PEER_SHOT, CCCommandParameterBlob(&info,sizeof(ZPACKEDSHOTINFO)));
////	ZPOSTCMD4(MC_PEER_SHOT, CCCommandParameterFloat(fShotTime),CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z),CCCommandParameterInt(sel_type));
//	CHECK_RETURN_CALLSTACK(ZPostShot);
//}
//dll-injection���� ȣ�� ���ϵ��� �����ζ��̴�
#define ZPostShot(/*rvector&*/ pos, /*rvector&*/ to, /*int*/ sel_type) {	\
	ZPACKEDSHOTINFO info;					\
	info.fTime=ZGetGame()->GetTime();		\
	info.posx = pos.x;						\
	info.posy = pos.y;						\
	info.posz = pos.z;						\
	info.tox = to.x;						\
	info.toy = to.y;						\
	info.toz = to.z;						\
	info.sel_type = sel_type;				\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_SHOT, 1621), CCCommandParameterBlob(&info,sizeof(ZPACKEDSHOTINFO)));	\
}

//DLL Injection �ٿ��� fShotTime�� ������ ������ ������ Ÿ���� �����ϰ� �̳� ���� �ӵ��� shot�� �����ϰ� �����. 
//�������� ��ƾ���� �ش� �Լ��� �Ҹ��� ��츦 ���� fShotTime = g_pGame->GetTime()�̴�. ���� �Ķ���ͷ� fShotTime �� ���� �ִ°ź���
//�ش� �Լ��� �ҷ��� �� ��Ŷ�� g_pGame->GetTime()�� ������ ������ ��ü�ߴ�. 
//inline void ZPostShotMelee(/*float fShotTime,*/rvector &pos, int nShot)
//{
//	//ZPOSTCMD3(MC_PEER_SHOT_MELEE, CCCommandParameterFloat(fShotTime),CCCommandParameterPos(pos.x, pos.y, pos.z),CCCommandParameterInt(nShot));
//	ZPOSTCMD3(MC_PEER_SHOT_MELEE, CCCommandParameterFloat(ZGetGame()->GetTime()),CCCommandParameterPos(pos.x, pos.y, pos.z),CCCommandParameterInt(nShot));
//	CHECK_RETURN_CALLSTACK(ZPostShotMelee);
//}
//dll-injection���� ȣ�� ���ϵ��� �����ζ��̴�
#define ZPostShotMelee(/*rvector&*/ pos, /*int*/ nShot) {	\
	ZPOSTCMD3(CLOAK_CMD_ID(MC_PEER_SHOT_MELEE, 38274), CCCommandParameterFloat(ZGetGame()->GetTime()),CCCommandParameterPos(pos.x, pos.y, pos.z),CCCommandParameterInt(nShot));	\
}

inline void ZPostNPCRangeShot(CCUID uidOwner, float fShotTime,rvector &pos, rvector &to,int sel_type)
{
	ZPACKEDSHOTINFO info;
	info.fTime=fShotTime;
	info.posx = pos.x;
	info.posy = pos.y;
	info.posz = pos.z;
	info.tox = to.x;
	info.toy = to.y;
	info.toz = to.z;
	info.sel_type = sel_type;

	ZPOSTCMD2(MC_QUEST_PEER_NPC_ATTACK_RANGE, CCCommandParameterUID(uidOwner), CCCommandParameterBlob(&info,sizeof(ZPACKEDSHOTINFO)));
}

inline void ZPostNPCSkillStart(CCUID uidOwner, int nSkill, CCUID uidTarget, rvector& targetPos)
{
	ZPOSTCMD4(MC_QUEST_PEER_NPC_SKILL_START, CCCommandParameterUID(uidOwner), CCCommandParameterInt(nSkill), CCCommandParameterUID(uidTarget), CCCommandParameterPos(targetPos.x,targetPos.y,targetPos.z) );
}

inline void ZPostNPCSkillExecute(CCUID uidOwner, int nSkill, CCUID uidTarget, rvector& targetPos)
{
	ZPOSTCMD4(MC_QUEST_PEER_NPC_SKILL_EXECUTE, CCCommandParameterUID(uidOwner), CCCommandParameterInt(nSkill), CCCommandParameterUID(uidTarget), CCCommandParameterPos(targetPos.x,targetPos.y,targetPos.z) );
}

//DLL Injection �ٿ��� fShotTime�� ������ ������ ������ Ÿ���� �����ϰ� �̳� ���� �ӵ��� shot�� �����ϰ� �����. 
//�������� ��ƾ���� �ش� �Լ��� �Ҹ��� ��츦 ���� fShotTime = g_pGame->GetTime()�̴�. ���� �Ķ���ͷ� fShotTime �� ���� �ִ°ź���
//�ش� �Լ��� �ҷ��� �� ��Ŷ�� g_pGame->GetTime()�� ������ ������ ��ü�ߴ�.
//inline void ZPostShotSp(/*float fShotTime,*/rvector& vPos, rvector& vDir,int type,int sel_type)
//{
////	ZPOSTCMD5(MC_PEER_SHOT_SP, CCCommandParameterFloat(fShotTime),CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z),CCCommandParameterInt(type),CCCommandParameterInt(sel_type));
//	ZPOSTCMD5(MC_PEER_SHOT_SP, CCCommandParameterFloat(ZGetGame()->GetTime()),CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z),CCCommandParameterInt(type),CCCommandParameterInt(sel_type));
//}
//dll-injection���� ȣ�� ���ϵ��� �����ζ��̴�
#define ZPostShotSp(/*rvector&*/ vPos, /*rvector&*/ vDir, /*int*/ type, /*int*/ sel_type) {	\
	if (!IsNaN(vPos) && !IsNaN(vDir)) { \
		ZPOSTCMD5(CLOAK_CMD_ID(MC_PEER_SHOT_SP, 12783), CCCommandParameterFloat(ZGetGame()->GetTime()), CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z), CCCommandParameterInt(type), CCCommandParameterInt(sel_type));	\
	} \
}

#define ZPostNotifyThrowedTrap(/*short*/ itemId) { \
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_NOTIFY_THROW_TRAPITEM, 19191), CCCommandParameterShort(itemId)); \
}

#define ZPostNotifyActivatedTrap(/*short*/ itemId, /*rvector*/ pos) { \
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_NOTIFY_ACTIVATED_TRAPITEM, 95737), CCCommandParameterShort(itemId), MCmdParamShortVector(pos.x, pos.y, pos.z)); \
}

#define ZPostBuffInfo(/*void**/ pBlob) { \
	ZPOSTCMD1(MC_PEER_BUFF_INFO, CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob))); \
}

inline void ZPostReaction(float fTime, int id)
{
	ZPOSTCMD2(MC_PEER_REACTION, CCCommandParameterFloat(fTime), CCCommandParameterInt(id));
}

inline void ZPostLoadingComplete(const CCUID& uidChar, int nPercent)
{
	ZPOSTCMD2(MC_MATCH_LOADING_COMPLETE, CCCommandParameterUID(uidChar), MCmdParamInt(nPercent));
}

inline void ZPostStageEnterBattle(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_ENTERBATTLE, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage));
}

// �����ζ��̴�
#define ZPostStageLeaveBattle(/*const CCUID&*/ uidChar, /*bool*/bGameFinishLeaveBattle) { \
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 19472), CCCommandParameterUID(uidChar), MCmdParamBool(bGameFinishLeaveBattle));	\
}

inline void ZPostRequestPeerList(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_PEERLIST, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage));
}
/*
void ZPostGameRoundState(const CCUID& uidStage, int nState, int nRound)
{
ZPOSTCMD3(MC_MATCH_GAME_ROUNDSTATE, CCCommandParameterUID(uidStage), CCCommandParameterInt(nState), CCCommandParameterInt(nRound));
}
*/

// �����ζ��̴�
#define ZPostGameKill(/*const CCUID&*/ uidAttacker) {	\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_GAME_KILL, 27348), CCCommandParameterUID(uidAttacker));	\
}

inline void ZPostRequestTimeSync(unsigned long nTimeStamp)
{
	ZPOSTCMD1(MC_MATCH_GAME_REQUEST_TIMESYNC, MCmdParamUInt(nTimeStamp));
}

// �����ζ��̴�
#define ZPostAccountCharList(/*const unsigned char**/ pbyGuidAckMsg) {			\
	void* pBlob = CCMakeBlobArray(sizeof(unsigned char), SIZEOF_GUIDACKMSG);		\
	unsigned char* pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);	\
	CopyMemory(pCmdBlock, pbyGuidAckMsg, SIZEOF_GUIDACKMSG);					\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_REQUEST_ACCOUNT_CHARLIST, 57821), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));	\
	CCEraseBlobArray(pBlob);	\
}

inline void ZPostAccountCharInfo(int nCharNum)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_ACCOUNT_CHARINFO, CCCommandParameterChar((char)nCharNum));
}

// �����ζ��̴�
#define ZPostSelectMyChar(/*const CCUID&*/ uidChar, /*const int*/ nCharIndex) {	\
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_REQUEST_SELECT_CHAR, 33333), CCCommandParameterUID(uidChar), CCCommandParameterUInt(nCharIndex));	\
}

inline void ZPostDeleteMyChar(const CCUID& uidChar, const int nCharIndex, char* szCharName)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_DELETE_CHAR, CCCommandParameterUID(uidChar), CCCommandParameterUInt(nCharIndex),
		CCCommandParameterString(szCharName));
}

inline void ZPostCreateMyChar(const CCUID& uidChar, const int nCharIndex, char* szCharName,
							  const int nSex, const int nHair, const int nFace, const int nCostume)
{	
	ZPOSTCMD7(MC_MATCH_REQUEST_CREATE_CHAR, CCCommandParameterUID(uidChar), CCCommandParameterUInt(nCharIndex),
		CCCommandParameterString(szCharName), CCCommandParameterUInt(nSex), CCCommandParameterUInt(nHair),
		CCCommandParameterUInt(nFace), CCCommandParameterUInt(nCostume));

	CHECK_RETURN_CALLSTACK(ZPostCreateMyChar);
}

inline void ZPostSimpleCharInfo(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_SIMPLE_CHARINFO, CCCommandParameterUID(uidChar));
}

inline void ZPostFinishedRoundInfo(const CCUID& uidStage, CCUID& uidChar, CCTD_RoundPeerInfo* pPeerInfo, 
								   int nPeerInfoCount, CCTD_RoundKillInfo* pKillInfo, int nKillInfoCount)
{
	void* pBlobPeerInfo = CCMakeBlobArray(sizeof(CCTD_RoundPeerInfo), nPeerInfoCount);
	for (int i = 0; i < nPeerInfoCount; i++)
	{
		CCTD_RoundPeerInfo* pNodePeerInfo = (CCTD_RoundPeerInfo*)CCGetBlobArrayElement(pBlobPeerInfo, i);
		CopyMemory(pNodePeerInfo, &pPeerInfo[i], sizeof(CCTD_RoundPeerInfo));
	}

	void* pBlobKillInfo = CCMakeBlobArray(sizeof(CCTD_RoundKillInfo), nKillInfoCount);
	for (int i = 0; i < nKillInfoCount; i++)
	{
		CCTD_RoundKillInfo* pNodeKillInfo = (CCTD_RoundKillInfo*)CCGetBlobArrayElement(pBlobKillInfo, i);
		CopyMemory(pNodeKillInfo, &pKillInfo[i], sizeof(CCTD_RoundKillInfo));
	}

	ZPOSTCMD4(MC_MATCH_ROUND_FINISHINFO, CCCommandParameterUID(uidStage), CCCommandParameterUID(uidChar), 
		CCCommandParameterBlob(pBlobPeerInfo, CCGetBlobArraySize(pBlobPeerInfo)),
		CCCommandParameterBlob(pBlobKillInfo, CCGetBlobArraySize(pBlobKillInfo)));

	CCEraseBlobArray(pBlobPeerInfo);
	CCEraseBlobArray(pBlobKillInfo);
}

inline void ZPostRequestBuyItem(const CCUID& uidChar, unsigned int nItemID, unsigned int nItemCnt = 1)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_BUY_ITEM, CCCommandParameterUID(uidChar), CCCommandParameterUInt(nItemID), CCCommandParameterUInt(nItemCnt));
}

inline void ZPostRequestSellItem(const CCUID& uidChar, const CCUID& uidItem, unsigned int nItemCnt = 1)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_SELL_ITEM, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidItem), CCCommandParameterUInt(nItemCnt));
}

inline void ZPostRequestForcedEntry(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_FORCED_ENTRY, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage));
}

inline void ZPostRequestShopItemList(const CCUID& uidChar, const int nFirstItemIndex, const int nItemCount)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_SHOP_ITEMLIST, CCCommandParameterUID(uidChar), 
		CCCommandParameterInt(nFirstItemIndex), CCCommandParameterInt(nItemCount));
}

inline void ZPostRequestCharacterItemList(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_CHARACTER_ITEMLIST, CCCommandParameterUID(uidChar));
}

inline void ZPostRequestCharacterItemListForce(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_CHARACTER_ITEMLIST_FORCE, CCCommandParameterUID(uidChar));
}

inline void ZPostRequestAccountItemList(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_ACCOUNT_ITEMLIST, CCCommandParameterUID(uidChar));
}

inline void ZPostRequestBringAccountItem(const CCUID& uidChar, const int nAIID, const int nItemID, const int nItemCnt)
{
	ZPOSTCMD4(MC_MATCH_REQUEST_BRING_ACCOUNTITEM, CCCommandParameterUID(uidChar), CCCommandParameterInt(nAIID), CCCommandParameterInt(nItemID), CCCommandParameterInt(nItemCnt));
}
inline void ZPostRequestSendAccountItem(const CCUID& uidChar, const CCUID& uidItem, const int nItemCnt=1)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_BRING_BACK_ACCOUNTITEM, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidItem), CCCommandParameterUInt(nItemCnt));
}

inline void ZPostRequestEquipItem(const CCUID& uidChar, const CCUID& uidItem, const CCMatchCharItemParts parts)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_EQUIP_ITEM, CCCommandParameterUID(uidChar), 
		CCCommandParameterUID(uidItem), CCCommandParameterUInt(unsigned long int(parts))); 
}

inline void ZPostRequestTakeoffItem(const CCUID& uidChar, const CCMatchCharItemParts parts)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_TAKEOFF_ITEM, CCCommandParameterUID(uidChar), 
		CCCommandParameterUInt(unsigned long int(parts))); 
}

// �ٸ� ����� �÷��̾� ���� ��û
inline void ZPostRequestCharInfoDetail(const CCUID& uidChar, const char* pszCharName)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_CHARINFO_DETAIL, CCCommandParameterUID(uidChar), MCmdParamStr(const_cast<char*>(pszCharName)));
}


inline void ZPostFriendAdd(const char* pszName)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_ADD, MCmdParamStr(const_cast<char*>(pszName)));
}

inline void ZPostFriendRemove(const char* pszName)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_REMOVE, MCmdParamStr(const_cast<char*>(pszName)));
}

inline void ZPostFriendList()
{
	ZPOSTCMD0(MC_MATCH_FRIEND_LIST);
}

inline void ZPostFriendMsg(const char* pszMsg)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_MSG, MCmdParamStr(const_cast<char*>(pszMsg)));
}


// �ڻ��û - ����ȣ���������� ZGameClient::RequestGameSuicide() �� ȣ���ϵ��� ����.
inline void ZPostRequestSuicide(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_REQUEST_SUICIDE, CCCommandParameterUID(uidChar));
}

inline void ZPostAdminTerminal(const CCUID& uidChar, char* szMsg)
{
	ZPOSTCMD2(MC_ADMIN_TERMINAL, CCCommandParameterUID(uidChar), CCCommandParameterString(szMsg));
}

inline void ZPostRequestGameInfo(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_GAME_INFO, CCCommandParameterUID(uidChar), 
		CCCommandParameterUID(uidStage));
}

inline void ZPostRequestRecommendChannel()
{
	ZPOSTCMD0(MC_MATCH_REQUEST_RECOMMANDED_CHANNEL);
}

inline void ZPostRequestStageList(const CCUID& uidChar, const CCUID& uidChannel, int nStageCursor)
{
	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - st_nLastTime) > 500)
	{
		ZPOSTCMD3(MC_MATCH_REQUEST_STAGE_LIST, CCCommandParameterUID(uidChar),
			CCCommandParameterUID(uidChannel), CCCommandParameterInt(nStageCursor));

		st_nLastTime = nNowTime;
	}
}

inline void ZPostRequestChannelPlayerList(const CCUID& uidChar, const CCUID& uidChannel, const int nPage)
{
	ZPOSTCMD3(MC_MATCH_CHANNEL_REQUEST_PLAYER_LIST, CCCommandParameterUID(uidChar),
		CCCommandParameterUID(uidChannel), CCCommandParameterInt(nPage));
}

inline void ZPostRequestObtainWorldItem(const CCUID& uidChar, const int nItemUID)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_OBTAIN_WORLDITEM, CCCommandParameterUID(uidChar),
		CCCommandParameterInt(nItemUID));
}

inline void ZPostRequestSpawnWorldItem(const CCUID& uidChar, const int nItemID, const rvector& pos, float fDropDelayTime)
{
	ZPOSTCMD4(MC_MATCH_REQUEST_SPAWN_WORLDITEM, CCCommandParameterUID(uidChar),
		CCCommandParameterInt(nItemID), CCCommandParameterPos(pos.x, pos.y, pos.z), CCCommandParameterFloat(fDropDelayTime));
}

inline void ZPostLadderInvite(const CCUID& uidChar, const int nRequestID, char* szTeamName,
							char** ppMemberCharNames, int nMemberCharNamesCount)
{
}

inline void ZPostWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage)
{
	ZPOSTCMD3(MC_MATCH_USER_WHISPER, MCmdParamStr(pszSenderName), 
			MCmdParamStr(pszTargetName), MCmdParamStr(pszMessage));
}

inline void ZPostWhere(char* pszTargetName)
{
	ZPOSTCMD1(MC_MATCH_USER_WHERE, MCmdParamStr(pszTargetName));
}

void ZPostUserOption();

inline void ZPostChatRoomCreate(const CCUID& uidPlayer, char* pszChatRoomName)
{
	ZPOSTCMD2(MC_MATCH_CHATROOM_CREATE, MCmdParamUID(uidPlayer),
			MCmdParamStr(pszChatRoomName));
}

inline void ZPostChatRoomJoin(char* pszChatRoomName)
{
	ZPOSTCMD2( MC_MATCH_CHATROOM_JOIN, MCmdParamStr(""),
			MCmdParamStr(pszChatRoomName) );
}

inline void ZPostChatRoomLeave(char* pszChatRoomName)
{
	ZPOSTCMD2( MC_MATCH_CHATROOM_LEAVE, MCmdParamStr(""),
			MCmdParamStr(pszChatRoomName) );
}

inline void ZPostSelectChatRoom(char* pszChatRoomName)
{
	ZPOSTCMD1( MC_MATCH_CHATROOM_SELECT_WRITE, MCmdParamStr(pszChatRoomName) );
}

inline void ZPostInviteChatRoom(char* pszPlayerName)
{
	ZPOSTCMD3( MC_MATCH_CHATROOM_INVITE, MCmdParamStr(""), MCmdParamStr(pszPlayerName),
			MCmdParamStr("") );
}

inline void ZPostChatRoomChat(char* pszChat)
{
	ZPOSTCMD3( MC_MATCH_CHATROOM_CHAT, MCmdParamStr(""), MCmdParamStr(""), MCmdParamStr(pszChat) );
}

inline void ZPostRequestMySimpleCharInfo(const CCUID& uidChar)
{
	ZPOSTCMD1( MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO, MCmdParamUID(uidChar) );
}


inline void ZPostRequestCopyToTestServer(const CCUID& uidChar)
{
	return;	// ������� ����
	ZPOSTCMD1(MC_MATCH_REQUEST_COPY_TO_TESTSERVER, MCmdParamUID(uidChar) );

}


// ������ ��û
inline void ZPostRequestQuickJoin(const CCUID& uidChar, CCTD_QuickJoinParam* pParam)
{
	void* pBlob = CCMakeBlobArray(sizeof(CCTD_QuickJoinParam), 1);
	CCTD_QuickJoinParam* pBlobNode = (CCTD_QuickJoinParam*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pBlobNode, pParam, sizeof(CCTD_QuickJoinParam));

	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_QUICKJOIN, CCCommandParameterUID(uidChar), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}


// Ŭ������ //////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostRequestCreateClan(const CCUID& uidChar, const int nRequestID, char* szClanName,
								   char** ppMemberCharNames, int nMemberCharNamesCount)
{
	if (nMemberCharNamesCount != CLAN_SPONSORS_COUNT) return;
	ZPOSTCMD7(MC_MATCH_CLAN_REQUEST_CREATE_CLAN, MCmdParamUID(uidChar), MCmdParamInt(nRequestID), MCmdParamStr(szClanName), 
				MCmdParamStr(ppMemberCharNames[0]), MCmdParamStr(ppMemberCharNames[1]),
				MCmdParamStr(ppMemberCharNames[2]), MCmdParamStr(ppMemberCharNames[3]));

}

inline void ZPostAnswerSponsorAgreement(const int nRequestID, const CCUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer)
{
	ZPOSTCMD4(MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT, MCmdParamInt(nRequestID), MCmdParamUID(uidClanMaster), 
		MCmdParamStr(szSponsorCharName), MCmdParamBool(bAnswer));
}

inline void ZPostRequestAgreedCreateClan(const CCUID& uidChar, char* szClanName, char** ppMemberCharNames, int nMemberCharNamesCount)
{
	if (nMemberCharNamesCount != CLAN_SPONSORS_COUNT) return;
	ZPOSTCMD6(MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN, MCmdParamUID(uidChar), MCmdParamStr(szClanName), 
				MCmdParamStr(ppMemberCharNames[0]), MCmdParamStr(ppMemberCharNames[1]),
				MCmdParamStr(ppMemberCharNames[2]), MCmdParamStr(ppMemberCharNames[3]));
}

// Ŭ�� ��� ��û
inline void ZPostRequestCloseClan(const CCUID& uidChar, char* szClanName)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_CLOSE_CLAN, MCmdParamUID(uidChar), MCmdParamStr(szClanName));
}

// Ŭ�� ���� ó��
inline void ZPostRequestJoinClan(const CCUID& uidChar, const char* szClanName, const char* szJoiner)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_JOIN_CLAN, MCmdParamUID(uidChar), MCmdParamStr(szClanName), MCmdParamStr(szJoiner));
}

// Ŭ�� ���� ���� ����
inline void ZPostAnswerJoinAgreement(const CCUID& uidClanAdmin, const char* szJoiner, const bool bAnswer)
{
	ZPOSTCMD3(MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT, MCmdParamUID(uidClanAdmin), MCmdParamStr(szJoiner), MCmdParamBool(bAnswer));
}

// Ŭ�� ������ ����ó�� ��û
inline void ZPostRequestAgreedJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN, MCmdParamUID(uidClanAdmin), MCmdParamStr(szClanName), MCmdParamStr(szJoiner));
}

// Ŭ�� Ż�� ��û
inline void ZPostRequestLeaveClan(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_LEAVE_CLAN, MCmdParamUID(uidChar));
}

// Ŭ�� ��� ���� ���� ��û
inline void ZPostRequestChangeClanGrade(const CCUID& uidClanAdmin, const char* szMember, int nClanGrade)
{
	ZPOSTCMD3(MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE, MCmdParamUID(uidClanAdmin), MCmdParamStr(szMember), MCmdParamInt(nClanGrade));

}

// Ŭ�� ��� Ż�� ��û
inline void ZPostRequestExpelClanMember(const CCUID& uidClanAdmin, const char* szMember)
{
	ZPOSTCMD2(MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER, MCmdParamUID(uidClanAdmin), MCmdParamStr(szMember));
}


// ä���� ��� �÷��̾� ����Ʈ ��û
inline void ZPostRequestChannelAllPlayerList(const CCUID& uidChar, const CCUID& uidChannel, const unsigned long int nPlaceFilter,
											 const unsigned long int nOptions)
{
	ZPOSTCMD4(MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST, CCCommandParameterUID(uidChar),
		CCCommandParameterUID(uidChannel), CCCommandParameterUInt(nPlaceFilter), CCCommandParameterUInt(nOptions));
}

// Ŭ�� �޼���
inline void ZPostClanMsg(const CCUID& uidSender, const char* pszMsg)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_MSG, MCmdParamUID(uidSender), MCmdParamStr(const_cast<char*>(pszMsg)));
}

inline void ZPostRequestClanMemberList(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_MEMBER_LIST, MCmdParamUID(uidChar));
}

// Ŭ�� ���� ��û - Ŭ�������������� ����Ѵ�.
inline void ZPostRequestClanInfo(const CCUID& uidChar, const char* szClanName)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_CLAN_INFO, MCmdParamUID(uidChar), MCmdParamStr(szClanName));
}


// �ٸ������ ���� ��û - ���Լ��� ��������������� ZGameClient::RequestProposal�� �̿��ؾ��Ѵ�.
inline void ZPostRequestProposal(const CCUID& uidChar, const int nProposalMode, const int nRequestID,
								 char** ppReplierCharNames, const int nReplierCount)
{
	void* pBlobRepliersName = CCMakeBlobArray(sizeof(CCTD_ReplierNode), nReplierCount);
	for (int i = 0; i < nReplierCount; i++)
	{
		CCTD_ReplierNode* pReplierNode = (CCTD_ReplierNode*)CCGetBlobArrayElement(pBlobRepliersName, i);
		strcpy(pReplierNode->szName, ppReplierCharNames[i]);
	}

	ZPOSTCMD5(MC_MATCH_REQUEST_PROPOSAL, MCmdParamUID(uidChar), MCmdParamInt(nProposalMode),
		MCmdParamInt(nRequestID), MCmdParamInt(nReplierCount), 
		MCmdParamBlob(pBlobRepliersName, CCGetBlobArraySize(pBlobRepliersName)));

	CCEraseBlobArray(pBlobRepliersName);
}


// ���� ���� - ���Լ��� ��������������� ZGameClient::ReplyAgreement�� �̿��ؾ��Ѵ�.
inline void ZPostReplyAgreement(const CCUID& uidProposer, const CCUID& uidChar, char* szReplierName, int nProposalMode, 
								int nRequestID, bool bAgreement)
{
	ZPOSTCMD6(MC_MATCH_REPLY_AGREEMENT, MCmdParamUID(uidProposer), MCmdParamUID(uidChar),
		MCmdParamStr(szReplierName), MCmdParamInt(nProposalMode), MCmdParamInt(nRequestID), MCmdParamBool(bAgreement));
}


// �������� ��û
inline void ZPostLadderRequestChallenge(char** ppMemberCharNames, const int nMemberCount, unsigned long int nOptions)	// �ڽű��� ����
{
	void* pBlobMembersName = CCMakeBlobArray(sizeof(CCTD_ReplierNode), nMemberCount);
	for (int i = 0; i < nMemberCount; i++)
	{
		CCTD_LadderTeamMemberNode* pMemberNode = (CCTD_LadderTeamMemberNode*)CCGetBlobArrayElement(pBlobMembersName, i);
		strcpy(pMemberNode->szName, ppMemberCharNames[i]);
	}

	ZPOSTCMD3( MC_MATCH_LADDER_REQUEST_CHALLENGE, MCmdParamInt(nMemberCount), MCmdParamUInt(nOptions),
			   MCmdParamBlob(pBlobMembersName, CCGetBlobArraySize(pBlobMembersName)) );
		

	CCEraseBlobArray(pBlobMembersName);
}

inline void ZPostLadderCancel()
{
	ZPOSTCMD0(MC_MATCH_LADDER_REQUEST_CANCEL_CHALLENGE)
}



// Duel Tournament ///////////////////////////////////////////////////////////////////////////////////
#ifdef _DUELTOURNAMENT
inline void ZPostDuelTournamentRequestJoinGame(const CCUID& uidChar, CCDUELTOURNAMENTTYPE nType)
{
	ZPOSTCMD2( MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME, MCmdParamUID(uidChar), MCmdParamInt((int)nType));
}

inline void ZPostDuelTournamentRequestCancelGame(const CCUID& uidChar, CCDUELTOURNAMENTTYPE nType)
{
	ZPOSTCMD2( MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME, MCmdParamUID(uidChar), MCmdParamInt((int)nType));
}

inline void ZPostDuelTournamentRequestSideRankingInfo(const CCUID& uidChar)
{
	static char szCharName[MATCHOBJECT_NAME_LENGTH] = { -1, 0, };	// ZGetMyInfo()->GetCharName()�� ""�� ������ �� �����Ƿ� �̺����� ""�� �ʱ�ȭ�ϸ� ���۵� �Ͼ
	static DWORD timeLastReqeust = 0;
	DWORD currTime = timeGetTime();

	// �ʹ� ���� ��û���� �ʵ��� ����
	// ĳ���� ������ �� ���� �׳� ��û
	if (timeLastReqeust==0 || currTime-timeLastReqeust > 10*1000 || stricmp(ZGetMyInfo()->GetCharName(), szCharName)!=0)
	{
		timeLastReqeust = currTime;
		strcpy(szCharName, ZGetMyInfo()->GetCharName());
		ZPOSTCMD1( MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO, MCmdParamUID(uidChar));
	}
}

inline void ZPostDuelTournamentGamePlayerStatus(const CCUID& uidChar, float fAccumulationDamage, float fHP, float fAP)
{
	ZPOSTCMD4( MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS, MCmdParamUID(uidChar), MCmdParamFloat(fAccumulationDamage), MCmdParamFloat(fHP), MCmdParamFloat(fAP));
}
#endif //_DUELTOURNAMENT


// Admin /////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostAdminPingToAll()
{
	ZPOSTCMD0(MC_ADMIN_PING_TO_ALL);
}


inline void ZPostAdminRequestKickPlayer(char* pszTargetPlayerName)
{
	ZPOSTCMD1(MC_ADMIN_REQUEST_KICK_PLAYER, MCmdParamStr(pszTargetPlayerName));
}

inline void ZPostAdminRequestMutePlayer(char* pszTargetPlayerName, int nPunishHour)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_MUTE_PLAYER, MCmdParamStr(pszTargetPlayerName), MCmdParamInt(nPunishHour));
}

inline void ZPostAdminRequestBlockPlayer(char* pszTargetPlayerName, int nPunishHour)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_BLOCK_PLAYER, MCmdParamStr(pszTargetPlayerName), MCmdParamInt(nPunishHour));
}

inline void ZPostAdminRequestSwitchLadderGame(const CCUID& uidChar, bool bEnabled)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_SWITCH_LADDER_GAME, MCmdParamUID(uidChar), MCmdParamBool(bEnabled));
}


inline void ZPostAdminAnnounce(const CCUID& uidChar, char* szMsg, ZAdminAnnounceType nType)
{
	ZPOSTCMD3(MC_ADMIN_ANNOUNCE, CCCommandParameterUID(uidChar), 
		CCCommandParameterString(szMsg), CCCommandParameterUInt(nType));
}

inline void ZPostAdminHalt(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_ADMIN_SERVER_HALT, CCCommandParameterUID(uidChar) );
}

inline void ZPostAdminReloadClientHash()
{
	ZPOSTCMD0(MC_ADMIN_RELOAD_CLIENT_HASH);
}

inline void ZPostAdminResetAllHackingBlock()
{
	ZPOSTCMD0(MC_ADMIN_RESET_ALL_HACKING_BLOCK);
}

inline void ZPostAdminReloadGambleItem()
{
	ZPOSTCMD0(MC_ADMIN_RELOAD_GAMBLEITEM);
}

inline void ZPostAdminDumpGambleItemLog()
{
	ZPOSTCMD0(MC_ADMIN_DUMP_GAMBLEITEM_LOG);
}

inline void ZPostAdminAssasin()
{
	ZPOSTCMD0(MC_ADMIN_ASSASIN);
}


// Event /////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostChangeMaster()
{
	ZPOSTCMD0(MC_EVENT_CHANGE_MASTER);
}

inline void ZPostChangePassword(char* pszPassword)
{
	ZPOSTCMD1(MC_EVENT_CHANGE_PASSWORD, MCmdParamStr(pszPassword));
}

inline void ZPostAdminHide()
{
	ZPOSTCMD0(MC_ADMIN_HIDE);
}

inline void ZPostAdminRequestJjang(char* pszTargetName)
{
	ZPOSTCMD1(MC_EVENT_REQUEST_JJANG, MCmdParamStr(pszTargetName));
}

inline void ZPostAdminRemoveJjang(char* pszTargetName)
{
	ZPOSTCMD1(MC_EVENT_REMOVE_JJANG, MCmdParamStr(pszTargetName));
}


// Emblem ////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostRequestEmblemURL(void* pBlob)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_EMBLEMURL, CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
}

inline void ZPostClanEmblemReady(unsigned int nCLID, char* pszEmblemURL)
{
	ZPOSTCMD2(MC_MATCH_CLAN_LOCAL_EMBLEMREADY, MCmdParamInt(nCLID), MCmdParamStr(pszEmblemURL));
}

// Quest /////////////////////////////////////////////////////////////////////////////////////////////
// �����ζ��̴�
#define ZPostQuestRequestNPCDead(/*const CCUID&*/ uidKiller, /*const CCUID&*/ uidNPC, /*rvector&*/ vPos) {	\
	ZPOSTCMD3(CLOAK_CMD_ID(MC_QUEST_REQUEST_NPC_DEAD, 24781), MCmdParamUID(uidKiller), MCmdParamUID(uidNPC), MCmdParamShortVector(vPos.x, vPos.y, vPos.z));	\
}

inline void ZPostQuestPeerNPCDead(const CCUID& uidKiller, const CCUID& uidNPC)
{
	ZPOSTCMD2(MC_QUEST_PEER_NPC_DEAD, MCmdParamUID(uidKiller), MCmdParamUID(uidNPC));
}


inline void ZPostQuestGameKill()
{
	ZPOSTCMD0(MC_MATCH_QUEST_REQUEST_DEAD);
}


#ifdef _DEBUG
inline void ZPostQuestTestNPCSpawn(const int nNPCType, const int nNPCCount)
{
	ZPOSTCMD2(MC_QUEST_TEST_REQUEST_NPC_SPAWN, MCmdParamInt(nNPCType), MCmdParamInt(nNPCCount));
}
#endif


#ifdef _DEBUG
inline void ZPostQuestTestClearNPC()
{
	ZPOSTCMD0(MC_QUEST_TEST_REQUEST_CLEAR_NPC);
}
#endif


#ifdef _DEBUG
inline void ZPostQuestTestSectorClear()
{
	ZPOSTCMD0(MC_QUEST_TEST_REQUEST_SECTOR_CLEAR);
}
#endif


#ifdef _DEBUG
inline void ZPostQuestTestFinish()
{
	ZPOSTCMD0(MC_QUEST_TEST_REQUEST_FINISH);
}
#endif


inline void ZPostQuestRequestMovetoPortal(const char nCurrSectorIndex)
{
	ZPOSTCMD1(MC_QUEST_REQUEST_MOVETO_PORTAL, MCmdParamChar(nCurrSectorIndex));
}

inline void ZPostQuestReadyToNewSector(const CCUID& uidPlayer)
{
	ZPOSTCMD1(MC_QUEST_READYTO_NEWSECTOR, MCmdParamUID(uidPlayer));
}

inline void ZPostQuestPong(unsigned long int nTime)
{
	ZPOSTCMD1(MC_QUEST_PONG, MCmdParamUInt(nTime));
}


#ifdef _QUEST_ITEM
inline void ZPostRequestGetCharQuestItemInfo( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST, MCmdParamUID(uid) );
}

inline void ZPostRequestBuyQuestItem( const CCUID& uid, const unsigned long int nItemID, const int nCount = 1 )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_BUY_QUEST_ITEM, MCmdParamUID(uid), CCCommandParameterInt(nItemID), CCCommandParameterInt(nCount) );
}

inline void ZPostRequestSellQuestItem( const CCUID& uid, const unsigned long int nItemID, const int nCount = 1 )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_SELL_QUEST_ITEM, MCmdParamUID(uid), CCCommandParameterInt(nItemID), CCCommandParameterInt(nCount) );
}

inline void ZPostRequestDropSacrificeItem( const CCUID& uid, const int nSlotIndex, const unsigned long int nItemID )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM, MCmdParamUID(uid), CCCommandParameterInt(nSlotIndex), CCCommandParameterInt(nItemID) );
}

inline void ZPostRequestCallbackSacrificeItem( const CCUID& uid, const int nSlotIndex, const unsigned long int nItemID )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM, MCmdParamUID(uid), CCCommandParameterInt(nSlotIndex), CCCommandParameterInt(nItemID) );
}

inline void ZPostRequestQL( const CCUID& uid )
{
	ZPOSTCMD1( MC_QUEST_REQUEST_QL, MCmdParamUID(uid) );
}

inline void ZPostRequestSacrificeSlotInfo( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_SLOT_INFO, MCmdParamUID(uid) );
}

inline void ZPostQuestStageMapset(const CCUID& uidStage, int nMapsetID)
{
	ZPOSTCMD2(MC_QUEST_STAGE_MAPSET, CCCommandParameterUID(uidStage), CCCommandParameterChar((char)nMapsetID));
}

inline void ZPostRequestMonsterBibleInfo( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_MONSTER_BIBLE_INFO, MCmdParamUID(uid) );
}

inline void ZPostResponseXTrapSeedKey(unsigned char *szComBuf)			// Update sgk 0706
{
	void *pBlob = CCMakeBlobArray(sizeof(unsigned char), 128);
	unsigned char *pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pCmdBlock, szComBuf, 128);

	ZPOSTCMD1(MC_RESPONSE_XTRAP_SEEDKEY, CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));

	CCEraseBlobArray(pBlob);
}

inline void ZPsotResponseGameguardAuht( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	ZPOSTCMD4( MC_RESPONSE_GAMEGUARD_AUTH, MCmdParamUInt(dwIndex), MCmdParamUInt(dwValue1), MCmdParamUInt(dwValue2), MCmdParamUInt(dwValue3) );
}


inline void ZPostResponseFirstGameguardAuth( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	ZPOSTCMD4( MC_RESPONSE_FIRST_GAMEGUARD_AUTH, MCmdParamUInt(dwIndex), MCmdParamUInt(dwValue1), MCmdParamUInt(dwValue2), MCmdParamUInt(dwValue3) );
}

inline void ZPostRequestGamble( const CCUID& uid)
{
	ZPOSTCMD1( MC_MATCH_REQUEST_GAMBLE, MCmdParamUID(uid) );
}

inline void ZPostResponseResourceCRC32( DWORD dwCRC, DWORD dwXOR )
{
	if( 0 == dwCRC )
	{
		_ASSERT( 0 != dwCRC );
		return;
	}

	ZPOSTCMD2( MC_RESPONSE_RESOURCE_CRC32, MCmdParamUInt(dwCRC), MCmdParamUInt(dwXOR) );
}

inline void ZPostRequestUseSpendableNormalItem( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_USE_SPENDABLE_NORMAL_ITEM, MCmdParamUID(uid) );
}

inline void ZPostRequestUseSpendableBuffItem( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_USE_SPENDABLE_BUFF_ITEM, MCmdParamUID(uid) );
}

#endif




#endif