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


// 일정 시간간격으로 캐릭터들끼리 계속 주고받는 데이터
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

// 해커가 디스어셈으로 커맨드ID를 검색해 ZPost류의 함수를 찾지 못하도록 하는 매크로 함수
// 클라이언트 코드에서는 커맨드ID를 직접 사용하지 말고 이 커맨드ID 숨김 함수를 거쳐서 사용하도록 한다.
// <이런 짓을 해야만하는 이유>
// 예를 들어 ZPostShot()함수 위치를 해커가 찾고 싶다면, MC_PEER_SHOT이 0x2732이므로 디스어셈해서 PUSH 2732를 찾으면
// ZPostShot()을 금방 찾아낼 수 있다. 그래서 우리가 0x2732를 코드에 명시적으로 사용하면 너무나 해커친화적인 환경이된다..
// 인라인 되지 않은 함수 내에서 0x2732를 만들어내는 임의의 수식을 사용해서 ID가 명시적으로 드러나지 않게 한다.

// 이 매크로는 컴파일러 최적화를 거치므로 덧셈이 이미 된 결과값이 빌드되므로 숨겨짐 (FACTOR에는 아무 정수나)
#define CLOAK_CMD_ID(CMD_ID, FACTOR)	UncloakCmdId((CMD_ID) + (FACTOR), FACTOR)
// 이걸 다시 뺄셈해서 원래 id로 되돌리는 함수 (이건 인라인이 아니어야 함)
int UncloakCmdId(int cloakedCmdId, int cloakFactor);



inline void ZPostHPInfo(float fHP)
{
	ZPOSTCMD1(MC_PEER_HPINFO, CCCmdParamFloat(fHP));
}

inline void ZPostHPAPInfo(float fHP, float fAP)
{
	ZPOSTCMD2(MC_PEER_HPAPINFO, CCCmdParamFloat(fHP), CCCmdParamFloat(fAP));
}

inline void ZPostDuelTournamentHPAPInfo(UCHAR MaxHP, UCHAR MaxAP, UCHAR HP, UCHAR AP)
{
	ZPOSTCMD4(MC_PEER_DUELTOURNAMENT_HPAPINFO, CCCmdParamUChar(MaxHP), CCCmdParamUChar(MaxAP), CCCmdParamUChar(HP), CCCmdParamUChar(AP));
}

inline void ZPostMove(rvector& vPos, rvector& vDir, rvector& vVelocity, 
					  ZC_STATE_UPPER upper, ZC_STATE_LOWER lower)
{
	ZPOSTCMD5(MC_PEER_MOVE, CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z), CCCommandParameterVector(vVelocity.x, vVelocity.y, vVelocity.z), CCCommandParameterInt(int(upper)), CCCommandParameterInt(int(lower)));
}

//DLL Injection 핵에서 fShotTime을 조작해 무기의 딜레이 타임을 무시하고 겁나 빠른 속도로 shot이 가능하게 만든다. 
//정상적인 루틴에서 해당 함수가 불리는 경우를 보면 fShotTime = g_pGame->GetTime()이다. 따라서 파라미터로 fShotTime 을 갖고 있는거보다
//해당 함수가 불렸을 때 패킷에 g_pGame->GetTime()을 보내는 것으로 대체했다. 
//inline void ZPostSkill(/*float fShotTime,*/int nSkill,int sel_type)	// 스킬발동~!
//{
////	ZPOSTCMD3(MC_PEER_SKILL, CCCmdParamFloat(fShotTime), CCCmdParamInt(nSkill),CCCommandParameterInt(sel_type));
//	ZPOSTCMD3(MC_PEER_SKILL, CCCmdParamFloat(ZGetGame()->GetTime()), CCCmdParamInt(nSkill),CCCommandParameterInt(sel_type));
//	CHECK_RETURN_CALLSTACK(ZPostSkill);
//}
//dll-injection으로 호출 못하게 강제 인라이닝
#define ZPostSkill(/*int*/ nSkill,/*int*/ sel_type)	{ \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_PEER_SKILL, 7636), CCCmdParamFloat(ZGetGame()->GetTime()), CCCmdParamInt(nSkill),CCCommandParameterInt(sel_type)); \
}

inline void ZPostChangeWeapon(int WeaponID)
{
	ZPOSTCMD1(MC_PEER_CHANGE_WEAPON, CCCmdParamInt(WeaponID));
}

//dll-injection으로 호출 못하게 강제 인라이닝
#define ZPostSpMotion(/*int*/ type) { ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_SPMOTION, 123),CCCmdParamInt(type)); }

inline void ZPostChangeParts(int PartsType,int PartsID)
{
	ZPOSTCMD2(MC_PEER_CHANGE_PARTS, CCCmdParamInt(PartsType),CCCmdParamInt(PartsID));
}

inline void ZPostChangeCharacter()
{
	ZPOSTCMD0(MC_PEER_CHANGECHARACTER);
}

inline void ZPostAttack(int type, rvector& vPos)
{
	ZPOSTCMD2(MC_PEER_ATTACK, CCCmdParamInt(type),CCCommandParameterVector(vPos.x, vPos.y, vPos.z));
}

inline void ZPostDamage(CCUID ChrUID,int damage)
{
	ZPOSTCMD2(MC_PEER_DAMAGE, CCCmdParaCCUID(ChrUID),CCCmdParamInt(damage));
}

#define ZPostDie(/*CCUID*/ uidAttacker) { ZPOSTCMD1(CLOAK_CMD_ID(MC_PEER_DIE, 3421), CCCmdParaCCUID(uidAttacker)); }

#ifdef _DEBUG
inline void ZPostSpawn(rvector& vPos, rvector& vDir)	// For Local Test Only
{
	ZPOSTCMD2(MC_PEER_SPAWN, CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterDir(vDir.x, vDir.y, vDir.z));
}
#endif

//dll-injection으로 호출 못하게 강제 인라이닝
// C/S Sync Spawn
#define ZPostRequestSpawn(/*CCUID*/ uidChar, /*rvector&*/ vPos, /*rvector&*/ vDir) { \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_MATCH_GAME_REQUEST_SPAWN, 8876), \
		CCCmdParaCCUID(uidChar), CCCmdParamPos(vPos.x, vPos.y, vPos.z), \
		CCCmdParamDir(vDir.x, vDir.y, vDir.z)); \
}

//강제 인라이닝
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
		ZPOSTCMD2(MC_PEER_CHAT, CCCmdParamInt(nTeam), CCCommandParameterString(szMsg))	
	}
	else
	{
		strncpy( szSendMsg, szMsg, 255 );
		ZPOSTCMD2(MC_PEER_CHAT, CCCmdParamInt(nTeam), CCCommandParameterString(szSendMsg))
	}

	CHECK_RETURN_CALLSTACK(ZPostPeerChat);
}

inline void ZPostPeerChatIcon(bool bShow)
{
	ZPOSTCMD1(MC_PEER_CHAT_ICON, CCCmdParamBool(bShow));
}

inline void ZPostReload()
{
	ZPOSTCMD0(MC_PEER_RELOAD);
}

inline void ZPostPeerEnchantDamage(CCUID ownerUID, CCUID targetUID)
{
	ZPOSTCMD2(MC_PEER_ENCHANT_DAMAGE, CCCmdParaCCUID(ownerUID), CCCmdParaCCUID(targetUID));
}

inline void ZPostConnect(const char* szAddr, unsigned int nPort)
{
	char szCmd[256];
	sprintf(szCmd, "%s:%u", szAddr, nPort);
	ZPOSTCMD1(MC_NET_CONNECT, CCCmdParamStr(szCmd));
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
	
	ZPOSTCMD5(MC_MATCH_LOGIN, CCCmdParamStr(szUserID)
		, CCCmdParamStr(szPassword)
		, CCCmdParamInt(MCOMMAND_VERSION)
		, CCCmdParamUInt(nChecksumPack)
		, CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}

inline void ZPostNetmarbleLogin(const char* szAuthCookie, const char* szDataCookie, const char* szCPCookie, char* szSpareParam, unsigned int nChecksumPack)
{
	ZPOSTCMD6(MC_MATCH_LOGIN_NETMARBLE, CCCmdParamStr(szAuthCookie), CCCmdParamStr(szDataCookie), CCCmdParamStr(szCPCookie), CCCmdParamStr(szSpareParam), CCCmdParamInt(MCOMMAND_VERSION), CCCmdParamUInt(nChecksumPack));
}

inline void ZPostNetmarbleJPLogin(char* szLoginID, char* szLoginPW, unsigned int nChecksumPack)
{
	ZPOSTCMD4(MC_MATCH_LOGIN_NETMARBLE_JP, CCCmdParamStr(szLoginID), CCCmdParamStr(szLoginPW), CCCmdParamInt(MCOMMAND_VERSION), CCCmdParamUInt(nChecksumPack));
}

inline void ZPostNHNUSALogin( char* szLoginID, char* szAuthStr, unsigned int nChecksumPack, char *szEncryptMD5Value )
{
	void *pBlob = CCMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );

	ZPOSTCMD5( MC_MATCH_LOGIN_NHNUSA, CCCmdParamStr(szLoginID), CCCmdParamStr(szAuthStr), CCCmdParamInt(MCOMMAND_VERSION), CCCmdParamUInt(nChecksumPack), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)) );
	CCEraseBlobArray(pBlob);
}

inline void ZPostGameOnJPLogin( const char* szString, const char* szStatIndex, unsigned int nChecksumPack, char *szEncryptMD5Value )
{
	void *pBlob = CCMakeBlobArray( MAX_MD5LENGH, 1 );
	unsigned char *pCmdBlock = (unsigned char*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory( pCmdBlock, szEncryptMD5Value, MAX_MD5LENGH );

	ZPOSTCMD5( MC_MATCH_LOGIN_GAMEON_JP, CCCmdParamStr(szString), CCCmdParamStr(szStatIndex), CCCmdParamInt(MCOMMAND_VERSION), CCCmdParamUInt(nChecksumPack), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}

inline void ZPostChannelRequestJoin(const CCUID& uidChar, const CCUID& uidChannel)
{
	ZPOSTCMD2(MC_MATCH_CHANNEL_REQUEST_JOIN, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidChannel));
}

inline void ZPostChannelRequestJoinFromChannelName(const CCUID& uidChar, int nChannelType, const char* szChannelName)
{
	if (strlen(szChannelName) >= CHANNELNAME_LEN) return;
	ZPOSTCMD3(MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME, CCCmdParaCCUID(uidChar), CCCmdParamInt(nChannelType), CCCmdParamStr(szChannelName));
}

//강제 인라이닝
#define ZPostChannelChat(/*const CCUID&*/ uidChar, /*const CCUID&*/ uidChannel, /*char**/ szChat) { \
	ZPOSTCMD3(CLOAK_CMD_ID(MC_MATCH_CHANNEL_REQUEST_CHAT, 87252), CCCommandParameterUID(uidChar), CCCommandParameterUID(uidChannel), CCCmdParamStr(szChat)); \
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
	ZPOSTCMD4(MC_MATCH_STAGE_CREATE, CCCommandParameterUID(uidChar), CCCmdParamStr(szStageName),
		CCCmdParamBool(bPrivate), CCCmdParamStr(szPassword));
}

inline void ZPostRequestStageJoin(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_STAGE_JOIN, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage));
}

inline void ZPostStageGo(int nRoomNo)
{
	ZPOSTCMD1(MC_MATCH_STAGE_GO, CCCmdParamUInt(nRoomNo));
}

inline void ZPostRequestPrivateStageJoin(const CCUID& uidChar, const CCUID& uidStage, char* szPassword)
{
	ZPOSTCMD3(MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage),
		CCCmdParamStr(szPassword));
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
	ZPOSTCMD3(MC_MATCH_STAGE_CHAT, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage), CCCmdParamStr(szChat));
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

//DLL Injection 핵에서 fShotTime을 조작해 무기의 딜레이 타임을 무시하고 겁나 빠른 속도로 shot이 가능하게 만든다. 
//정상적인 루틴에서 해당 함수가 불리는 경우를 보면 fShotTime = g_pGame->GetTime()이다. 따라서 파라미터로 fShotTime 을 갖고 있는거보다
//해당 함수가 불렸을 때 패킷에 g_pGame->GetTime()을 보내는 것으로 대체했다. 
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
//dll-injection으로 호출 못하도록 강제인라이닝
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

//DLL Injection 핵에서 fShotTime을 조작해 무기의 딜레이 타임을 무시하고 겁나 빠른 속도로 shot이 가능하게 만든다. 
//정상적인 루틴에서 해당 함수가 불리는 경우를 보면 fShotTime = g_pGame->GetTime()이다. 따라서 파라미터로 fShotTime 을 갖고 있는거보다
//해당 함수가 불렸을 때 패킷에 g_pGame->GetTime()을 보내는 것으로 대체했다. 
//inline void ZPostShotMelee(/*float fShotTime,*/rvector &pos, int nShot)
//{
//	//ZPOSTCMD3(MC_PEER_SHOT_MELEE, CCCommandParameterFloat(fShotTime),CCCommandParameterPos(pos.x, pos.y, pos.z),CCCommandParameterInt(nShot));
//	ZPOSTCMD3(MC_PEER_SHOT_MELEE, CCCommandParameterFloat(ZGetGame()->GetTime()),CCCommandParameterPos(pos.x, pos.y, pos.z),CCCommandParameterInt(nShot));
//	CHECK_RETURN_CALLSTACK(ZPostShotMelee);
//}
//dll-injection으로 호출 못하도록 강제인라이닝
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

//DLL Injection 핵에서 fShotTime을 조작해 무기의 딜레이 타임을 무시하고 겁나 빠른 속도로 shot이 가능하게 만든다. 
//정상적인 루틴에서 해당 함수가 불리는 경우를 보면 fShotTime = g_pGame->GetTime()이다. 따라서 파라미터로 fShotTime 을 갖고 있는거보다
//해당 함수가 불렸을 때 패킷에 g_pGame->GetTime()을 보내는 것으로 대체했다.
//inline void ZPostShotSp(/*float fShotTime,*/rvector& vPos, rvector& vDir,int type,int sel_type)
//{
////	ZPOSTCMD5(MC_PEER_SHOT_SP, CCCommandParameterFloat(fShotTime),CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z),CCCommandParameterInt(type),CCCommandParameterInt(sel_type));
//	ZPOSTCMD5(MC_PEER_SHOT_SP, CCCommandParameterFloat(ZGetGame()->GetTime()),CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z),CCCommandParameterInt(type),CCCommandParameterInt(sel_type));
//}
//dll-injection으로 호출 못하도록 강제인라이닝
#define ZPostShotSp(/*rvector&*/ vPos, /*rvector&*/ vDir, /*int*/ type, /*int*/ sel_type) {	\
	if (!IsNaN(vPos) && !IsNaN(vDir)) { \
		ZPOSTCMD5(CLOAK_CMD_ID(MC_PEER_SHOT_SP, 12783), CCCommandParameterFloat(ZGetGame()->GetTime()), CCCommandParameterPos(vPos.x, vPos.y, vPos.z), CCCommandParameterVector(vDir.x, vDir.y, vDir.z), CCCommandParameterInt(type), CCCommandParameterInt(sel_type));	\
	} \
}

#define ZPostNotifyThrowedTrap(/*short*/ itemId) { \
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_NOTIFY_THROW_TRAPITEM, 19191), CCCommandParameterShort(itemId)); \
}

#define ZPostNotifyActivatedTrap(/*short*/ itemId, /*rvector*/ pos) { \
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_NOTIFY_ACTIVATED_TRAPITEM, 95737), CCCommandParameterShort(itemId), CCCmdParamShortVector(pos.x, pos.y, pos.z)); \
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
	ZPOSTCMD2(MC_MATCH_LOADING_COMPLETE, CCCommandParameterUID(uidChar), CCCmdParamInt(nPercent));
}

inline void ZPostStageEnterBattle(const CCUID& uidChar, const CCUID& uidStage)
{
	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_ENTERBATTLE, CCCommandParameterUID(uidChar), CCCommandParameterUID(uidStage));
}

// 강제인라이닝
#define ZPostStageLeaveBattle(/*const CCUID&*/ uidChar, /*bool*/bGameFinishLeaveBattle) { \
	ZPOSTCMD2(CLOAK_CMD_ID(MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER, 19472), CCCommandParameterUID(uidChar), CCCmdParamBool(bGameFinishLeaveBattle));	\
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

// 강제인라이닝
#define ZPostGameKill(/*const CCUID&*/ uidAttacker) {	\
	ZPOSTCMD1(CLOAK_CMD_ID(MC_MATCH_GAME_KILL, 27348), CCCommandParameterUID(uidAttacker));	\
}

inline void ZPostRequestTimeSync(unsigned long nTimeStamp)
{
	ZPOSTCMD1(MC_MATCH_GAME_REQUEST_TIMESYNC, CCCmdParamUInt(nTimeStamp));
}

// 강제인라이닝
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

// 강제인라이닝
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

// 다른 사람의 플레이어 정보 요청
inline void ZPostRequestCharInfoDetail(const CCUID& uidChar, const char* pszCharName)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_CHARINFO_DETAIL, CCCommandParameterUID(uidChar), CCCmdParamStr(const_cast<char*>(pszCharName)));
}


inline void ZPostFriendAdd(const char* pszName)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_ADD, CCCmdParamStr(const_cast<char*>(pszName)));
}

inline void ZPostFriendRemove(const char* pszName)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_REMOVE, CCCmdParamStr(const_cast<char*>(pszName)));
}

inline void ZPostFriendList()
{
	ZPOSTCMD0(MC_MATCH_FRIEND_LIST);
}

inline void ZPostFriendMsg(const char* pszMsg)
{
	ZPOSTCMD1(MC_MATCH_FRIEND_MSG, CCCmdParamStr(const_cast<char*>(pszMsg)));
}


// 자살요청 - 직접호출하지말고 ZGameClient::RequestGameSuicide() 를 호출하도록 하자.
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

inline void ZPostRequestObtainWorldItem(const CCUID& uidChar, const int nIteCCUID)
{
	ZPOSTCMD2(MC_MATCH_REQUEST_OBTAIN_WORLDITEM, CCCommandParameterUID(uidChar),
		CCCommandParameterInt(nIteCCUID));
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
	ZPOSTCMD3(MC_MATCH_USER_WHISPER, CCCmdParamStr(pszSenderName), 
			CCCmdParamStr(pszTargetName), CCCmdParamStr(pszMessage));
}

inline void ZPostWhere(char* pszTargetName)
{
	ZPOSTCMD1(MC_MATCH_USER_WHERE, CCCmdParamStr(pszTargetName));
}

void ZPostUserOption();

inline void ZPostChatRoomCreate(const CCUID& uidPlayer, char* pszChatRoomName)
{
	ZPOSTCMD2(MC_MATCH_CHATROOM_CREATE, CCCmdParaCCUID(uidPlayer),
			CCCmdParamStr(pszChatRoomName));
}

inline void ZPostChatRoomJoin(char* pszChatRoomName)
{
	ZPOSTCMD2( MC_MATCH_CHATROOM_JOIN, CCCmdParamStr(""),
			CCCmdParamStr(pszChatRoomName) );
}

inline void ZPostChatRoomLeave(char* pszChatRoomName)
{
	ZPOSTCMD2( MC_MATCH_CHATROOM_LEAVE, CCCmdParamStr(""),
			CCCmdParamStr(pszChatRoomName) );
}

inline void ZPostSelectChatRoom(char* pszChatRoomName)
{
	ZPOSTCMD1( MC_MATCH_CHATROOM_SELECT_WRITE, CCCmdParamStr(pszChatRoomName) );
}

inline void ZPostInviteChatRoom(char* pszPlayerName)
{
	ZPOSTCMD3( MC_MATCH_CHATROOM_INVITE, CCCmdParamStr(""), CCCmdParamStr(pszPlayerName),
			CCCmdParamStr("") );
}

inline void ZPostChatRoomChat(char* pszChat)
{
	ZPOSTCMD3( MC_MATCH_CHATROOM_CHAT, CCCmdParamStr(""), CCCmdParamStr(""), CCCmdParamStr(pszChat) );
}

inline void ZPostRequestMySimpleCharInfo(const CCUID& uidChar)
{
	ZPOSTCMD1( MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO, CCCmdParaCCUID(uidChar) );
}


inline void ZPostRequestCopyToTestServer(const CCUID& uidChar)
{
	return;	// 사용하지 않음
	ZPOSTCMD1(MC_MATCH_REQUEST_COPY_TO_TESTSERVER, CCCmdParaCCUID(uidChar) );

}


// 퀵조인 요청
inline void ZPostRequestQuickJoin(const CCUID& uidChar, CCTD_QuickJoinParam* pParam)
{
	void* pBlob = CCMakeBlobArray(sizeof(CCTD_QuickJoinParam), 1);
	CCTD_QuickJoinParam* pBlobNode = (CCTD_QuickJoinParam*)CCGetBlobArrayElement(pBlob, 0);
	CopyMemory(pBlobNode, pParam, sizeof(CCTD_QuickJoinParam));

	ZPOSTCMD2(MC_MATCH_STAGE_REQUEST_QUICKJOIN, CCCommandParameterUID(uidChar), CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
	CCEraseBlobArray(pBlob);
}


// 클랜관련 //////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostRequestCreateClan(const CCUID& uidChar, const int nRequestID, char* szClanName,
								   char** ppMemberCharNames, int nMemberCharNamesCount)
{
	if (nMemberCharNamesCount != CLAN_SPONSORS_COUNT) return;
	ZPOSTCMD7(MC_MATCH_CLAN_REQUEST_CREATE_CLAN, CCCmdParaCCUID(uidChar), CCCmdParamInt(nRequestID), CCCmdParamStr(szClanName), 
				CCCmdParamStr(ppMemberCharNames[0]), CCCmdParamStr(ppMemberCharNames[1]),
				CCCmdParamStr(ppMemberCharNames[2]), CCCmdParamStr(ppMemberCharNames[3]));

}

inline void ZPostAnswerSponsorAgreement(const int nRequestID, const CCUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer)
{
	ZPOSTCMD4(MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT, CCCmdParamInt(nRequestID), CCCmdParaCCUID(uidClanMaster), 
		CCCmdParamStr(szSponsorCharName), CCCmdParamBool(bAnswer));
}

inline void ZPostRequestAgreedCreateClan(const CCUID& uidChar, char* szClanName, char** ppMemberCharNames, int nMemberCharNamesCount)
{
	if (nMemberCharNamesCount != CLAN_SPONSORS_COUNT) return;
	ZPOSTCMD6(MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN, CCCmdParaCCUID(uidChar), CCCmdParamStr(szClanName), 
				CCCmdParamStr(ppMemberCharNames[0]), CCCmdParamStr(ppMemberCharNames[1]),
				CCCmdParamStr(ppMemberCharNames[2]), CCCmdParamStr(ppMemberCharNames[3]));
}

// 클랜 폐쇄 요청
inline void ZPostRequestCloseClan(const CCUID& uidChar, char* szClanName)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_CLOSE_CLAN, CCCmdParaCCUID(uidChar), CCCmdParamStr(szClanName));
}

// 클랜 가입 처리
inline void ZPostRequestJoinClan(const CCUID& uidChar, const char* szClanName, const char* szJoiner)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_JOIN_CLAN, CCCmdParaCCUID(uidChar), CCCmdParamStr(szClanName), CCCmdParamStr(szJoiner));
}

// 클랜 가입 동의 응답
inline void ZPostAnswerJoinAgreement(const CCUID& uidClanAdmin, const char* szJoiner, const bool bAnswer)
{
	ZPOSTCMD3(MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT, CCCmdParaCCUID(uidClanAdmin), CCCmdParamStr(szJoiner), CCCmdParamBool(bAnswer));
}

// 클랜 동의한 가입처리 요청
inline void ZPostRequestAgreedJoinClan(const CCUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ZPOSTCMD3(MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN, CCCmdParaCCUID(uidClanAdmin), CCCmdParamStr(szClanName), CCCmdParamStr(szJoiner));
}

// 클랜 탈퇴 요청
inline void ZPostRequestLeaveClan(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_LEAVE_CLAN, CCCmdParaCCUID(uidChar));
}

// 클랜 멤버 권한 변경 요청
inline void ZPostRequestChangeClanGrade(const CCUID& uidClanAdmin, const char* szMember, int nClanGrade)
{
	ZPOSTCMD3(MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE, CCCmdParaCCUID(uidClanAdmin), CCCmdParamStr(szMember), CCCmdParamInt(nClanGrade));

}

// 클랜 멤버 탈퇴 요청
inline void ZPostRequestExpelClanMember(const CCUID& uidClanAdmin, const char* szMember)
{
	ZPOSTCMD2(MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER, CCCmdParaCCUID(uidClanAdmin), CCCmdParamStr(szMember));
}


// 채널의 모든 플레이어 리스트 요청
inline void ZPostRequestChannelAllPlayerList(const CCUID& uidChar, const CCUID& uidChannel, const unsigned long int nPlaceFilter,
											 const unsigned long int nOptions)
{
	ZPOSTCMD4(MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST, CCCommandParameterUID(uidChar),
		CCCommandParameterUID(uidChannel), CCCommandParameterUInt(nPlaceFilter), CCCommandParameterUInt(nOptions));
}

// 클랜 메세지
inline void ZPostClanMsg(const CCUID& uidSender, const char* pszMsg)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_MSG, CCCmdParaCCUID(uidSender), CCCmdParamStr(const_cast<char*>(pszMsg)));
}

inline void ZPostRequestClanMemberList(const CCUID& uidChar)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_MEMBER_LIST, CCCmdParaCCUID(uidChar));
}

// 클랜 정보 요청 - 클랜전서버에서만 사용한다.
inline void ZPostRequestClanInfo(const CCUID& uidChar, const char* szClanName)
{
	ZPOSTCMD2(MC_MATCH_CLAN_REQUEST_CLAN_INFO, CCCmdParaCCUID(uidChar), CCCmdParamStr(szClanName));
}


// 다른사람의 동의 요청 - 이함수는 직접사용하지말고 ZGameClient::RequestProposal를 이용해야한다.
inline void ZPostRequestProposal(const CCUID& uidChar, const int nProposalMode, const int nRequestID,
								 char** ppReplierCharNames, const int nReplierCount)
{
	void* pBlobRepliersName = CCMakeBlobArray(sizeof(CCTD_ReplierNode), nReplierCount);
	for (int i = 0; i < nReplierCount; i++)
	{
		CCTD_ReplierNode* pReplierNode = (CCTD_ReplierNode*)CCGetBlobArrayElement(pBlobRepliersName, i);
		strcpy(pReplierNode->szName, ppReplierCharNames[i]);
	}

	ZPOSTCMD5(MC_MATCH_REQUEST_PROPOSAL, CCCmdParaCCUID(uidChar), CCCmdParamInt(nProposalMode),
		CCCmdParamInt(nRequestID), CCCmdParamInt(nReplierCount), 
		CCCmdParamBlob(pBlobRepliersName, CCGetBlobArraySize(pBlobRepliersName)));

	CCEraseBlobArray(pBlobRepliersName);
}


// 동의 응답 - 이함수는 직접사용하지말고 ZGameClient::ReplyAgreement를 이용해야한다.
inline void ZPostReplyAgreement(const CCUID& uidProposer, const CCUID& uidChar, char* szReplierName, int nProposalMode, 
								int nRequestID, bool bAgreement)
{
	ZPOSTCMD6(MC_MATCH_REPLY_AGREEMENT, CCCmdParaCCUID(uidProposer), CCCmdParaCCUID(uidChar),
		CCCmdParamStr(szReplierName), CCCmdParamInt(nProposalMode), CCCmdParamInt(nRequestID), CCCmdParamBool(bAgreement));
}


// 래더게임 신청
inline void ZPostLadderRequestChallenge(char** ppMemberCharNames, const int nMemberCount, unsigned long int nOptions)	// 자신까지 포함
{
	void* pBlobMembersName = CCMakeBlobArray(sizeof(CCTD_ReplierNode), nMemberCount);
	for (int i = 0; i < nMemberCount; i++)
	{
		CCTD_LadderTeamMemberNode* pMemberNode = (CCTD_LadderTeamMemberNode*)CCGetBlobArrayElement(pBlobMembersName, i);
		strcpy(pMemberNode->szName, ppMemberCharNames[i]);
	}

	ZPOSTCMD3( MC_MATCH_LADDER_REQUEST_CHALLENGE, CCCmdParamInt(nMemberCount), CCCmdParamUInt(nOptions),
			   CCCmdParamBlob(pBlobMembersName, CCGetBlobArraySize(pBlobMembersName)) );
		

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
	ZPOSTCMD2( MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME, CCCmdParaCCUID(uidChar), CCCmdParamInt((int)nType));
}

inline void ZPostDuelTournamentRequestCancelGame(const CCUID& uidChar, CCDUELTOURNAMENTTYPE nType)
{
	ZPOSTCMD2( MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME, CCCmdParaCCUID(uidChar), CCCmdParamInt((int)nType));
}

inline void ZPostDuelTournamentRequestSideRankingInfo(const CCUID& uidChar)
{
	static char szCharName[MATCHOBJECT_NAME_LENGTH] = { -1, 0, };	// ZGetMyInfo()->GetCharName()가 ""를 리턴할 수 있으므로 이변수를 ""로 초기화하면 오작동 일어남
	static DWORD timeLastReqeust = 0;
	DWORD currTime = timeGetTime();

	// 너무 자주 요청하지 않도록 제한
	// 캐릭터 변경을 한 경우는 그냥 요청
	if (timeLastReqeust==0 || currTime-timeLastReqeust > 10*1000 || stricmp(ZGetMyInfo()->GetCharName(), szCharName)!=0)
	{
		timeLastReqeust = currTime;
		strcpy(szCharName, ZGetMyInfo()->GetCharName());
		ZPOSTCMD1( MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO, CCCmdParaCCUID(uidChar));
	}
}

inline void ZPostDuelTournamentGamePlayerStatus(const CCUID& uidChar, float fAccumulationDamage, float fHP, float fAP)
{
	ZPOSTCMD4( MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS, CCCmdParaCCUID(uidChar), CCCmdParamFloat(fAccumulationDamage), CCCmdParamFloat(fHP), CCCmdParamFloat(fAP));
}
#endif //_DUELTOURNAMENT


// Admin /////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostAdminPingToAll()
{
	ZPOSTCMD0(MC_ADMIN_PING_TO_ALL);
}


inline void ZPostAdminRequestKickPlayer(char* pszTargetPlayerName)
{
	ZPOSTCMD1(MC_ADMIN_REQUEST_KICK_PLAYER, CCCmdParamStr(pszTargetPlayerName));
}

inline void ZPostAdminRequestMutePlayer(char* pszTargetPlayerName, int nPunishHour)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_MUTE_PLAYER, CCCmdParamStr(pszTargetPlayerName), CCCmdParamInt(nPunishHour));
}

inline void ZPostAdminRequestBlockPlayer(char* pszTargetPlayerName, int nPunishHour)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_BLOCK_PLAYER, CCCmdParamStr(pszTargetPlayerName), CCCmdParamInt(nPunishHour));
}

inline void ZPostAdminRequestSwitchLadderGame(const CCUID& uidChar, bool bEnabled)
{
	ZPOSTCMD2(MC_ADMIN_REQUEST_SWITCH_LADDER_GAME, CCCmdParaCCUID(uidChar), CCCmdParamBool(bEnabled));
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
	ZPOSTCMD1(MC_EVENT_CHANGE_PASSWORD, CCCmdParamStr(pszPassword));
}

inline void ZPostAdminHide()
{
	ZPOSTCMD0(MC_ADMIN_HIDE);
}

inline void ZPostAdminRequestJjang(char* pszTargetName)
{
	ZPOSTCMD1(MC_EVENT_REQUEST_JJANG, CCCmdParamStr(pszTargetName));
}

inline void ZPostAdminRemoveJjang(char* pszTargetName)
{
	ZPOSTCMD1(MC_EVENT_REMOVE_JJANG, CCCmdParamStr(pszTargetName));
}


// Emblem ////////////////////////////////////////////////////////////////////////////////////////////
inline void ZPostRequestEmblemURL(void* pBlob)
{
	ZPOSTCMD1(MC_MATCH_CLAN_REQUEST_EMBLEMURL, CCCommandParameterBlob(pBlob, CCGetBlobArraySize(pBlob)));
}

inline void ZPostClanEmblemReady(unsigned int nCLID, char* pszEmblemURL)
{
	ZPOSTCMD2(MC_MATCH_CLAN_LOCAL_EMBLEMREADY, CCCmdParamInt(nCLID), CCCmdParamStr(pszEmblemURL));
}

// Quest /////////////////////////////////////////////////////////////////////////////////////////////
// 강제인라이닝
#define ZPostQuestRequestNPCDead(/*const CCUID&*/ uidKiller, /*const CCUID&*/ uidNPC, /*rvector&*/ vPos) {	\
	ZPOSTCMD3(CLOAK_CMD_ID(MC_QUEST_REQUEST_NPC_DEAD, 24781), CCCmdParaCCUID(uidKiller), CCCmdParaCCUID(uidNPC), CCCmdParamShortVector(vPos.x, vPos.y, vPos.z));	\
}

inline void ZPostQuestPeerNPCDead(const CCUID& uidKiller, const CCUID& uidNPC)
{
	ZPOSTCMD2(MC_QUEST_PEER_NPC_DEAD, CCCmdParaCCUID(uidKiller), CCCmdParaCCUID(uidNPC));
}


inline void ZPostQuestGameKill()
{
	ZPOSTCMD0(MC_MATCH_QUEST_REQUEST_DEAD);
}


#ifdef _DEBUG
inline void ZPostQuestTestNPCSpawn(const int nNPCType, const int nNPCCount)
{
	ZPOSTCMD2(MC_QUEST_TEST_REQUEST_NPC_SPAWN, CCCmdParamInt(nNPCType), CCCmdParamInt(nNPCCount));
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
	ZPOSTCMD1(MC_QUEST_REQUEST_MOVETO_PORTAL, CCCmdParamChar(nCurrSectorIndex));
}

inline void ZPostQuestReadyToNewSector(const CCUID& uidPlayer)
{
	ZPOSTCMD1(MC_QUEST_READYTO_NEWSECTOR, CCCmdParaCCUID(uidPlayer));
}

inline void ZPostQuestPong(unsigned long int nTime)
{
	ZPOSTCMD1(MC_QUEST_PONG, CCCmdParamUInt(nTime));
}


#ifdef _QUEST_ITEM
inline void ZPostRequestGetCharQuestItemInfo( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST, CCCmdParaCCUID(uid) );
}

inline void ZPostRequestBuyQuestItem( const CCUID& uid, const unsigned long int nItemID, const int nCount = 1 )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_BUY_QUEST_ITEM, CCCmdParaCCUID(uid), CCCommandParameterInt(nItemID), CCCommandParameterInt(nCount) );
}

inline void ZPostRequestSellQuestItem( const CCUID& uid, const unsigned long int nItemID, const int nCount = 1 )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_SELL_QUEST_ITEM, CCCmdParaCCUID(uid), CCCommandParameterInt(nItemID), CCCommandParameterInt(nCount) );
}

inline void ZPostRequestDropSacrificeItem( const CCUID& uid, const int nSlotIndex, const unsigned long int nItemID )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM, CCCmdParaCCUID(uid), CCCommandParameterInt(nSlotIndex), CCCommandParameterInt(nItemID) );
}

inline void ZPostRequestCallbackSacrificeItem( const CCUID& uid, const int nSlotIndex, const unsigned long int nItemID )
{
	ZPOSTCMD3( MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM, CCCmdParaCCUID(uid), CCCommandParameterInt(nSlotIndex), CCCommandParameterInt(nItemID) );
}

inline void ZPostRequestQL( const CCUID& uid )
{
	ZPOSTCMD1( MC_QUEST_REQUEST_QL, CCCmdParaCCUID(uid) );
}

inline void ZPostRequestSacrificeSlotInfo( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_SLOT_INFO, CCCmdParaCCUID(uid) );
}

inline void ZPostQuestStageMapset(const CCUID& uidStage, int nMapsetID)
{
	ZPOSTCMD2(MC_QUEST_STAGE_MAPSET, CCCommandParameterUID(uidStage), CCCommandParameterChar((char)nMapsetID));
}

inline void ZPostRequestMonsterBibleInfo( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_MONSTER_BIBLE_INFO, CCCmdParaCCUID(uid) );
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
	ZPOSTCMD4( MC_RESPONSE_GAMEGUARD_AUTH, CCCmdParamUInt(dwIndex), CCCmdParamUInt(dwValue1), CCCmdParamUInt(dwValue2), CCCmdParamUInt(dwValue3) );
}


inline void ZPostResponseFirstGameguardAuth( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
	ZPOSTCMD4( MC_RESPONSE_FIRST_GAMEGUARD_AUTH, CCCmdParamUInt(dwIndex), CCCmdParamUInt(dwValue1), CCCmdParamUInt(dwValue2), CCCmdParamUInt(dwValue3) );
}

inline void ZPostRequestGamble( const CCUID& uid)
{
	ZPOSTCMD1( MC_MATCH_REQUEST_GAMBLE, CCCmdParaCCUID(uid) );
}

inline void ZPostResponseResourceCRC32( DWORD dwCRC, DWORD dwXOR )
{
	if( 0 == dwCRC )
	{
		_ASSERT( 0 != dwCRC );
		return;
	}

	ZPOSTCMD2( MC_RESPONSE_RESOURCE_CRC32, CCCmdParamUInt(dwCRC), CCCmdParamUInt(dwXOR) );
}

inline void ZPostRequestUseSpendableNormalItem( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_USE_SPENDABLE_NORMAL_ITEM, CCCmdParaCCUID(uid) );
}

inline void ZPostRequestUseSpendableBuffItem( const CCUID& uid )
{
	ZPOSTCMD1( MC_MATCH_REQUEST_USE_SPENDABLE_BUFF_ITEM, CCCmdParaCCUID(uid) );
}

#endif




#endif