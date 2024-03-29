#pragma once

bool					ZIsLaunchDevelop(); //This must be Before #include "ZApplication.h" or it cant find the define... ?.?

//#include "RMesh.h"
#include "ZApplication.h"
//#include "ZGameInterface.h"
// 전역적으로 사용하는 것은 이곳에다 넣도록 하자
#ifdef LOCALE_BRAZIL
#define APPLICATION_NAME		"Gunz The MMORPG"
#else
#define APPLICATION_NAME		"Gunz The MMORPG"
#endif

// 저장되는 폴더
#ifdef LOCALE_BRAZIL
#define GUNZ_FOLDER			"/The Duel"
#elif LOCALE_JAPAN
#define GUNZ_FOLDER			"/GUNZWEI"
#else
#define GUNZ_FOLDER			"/Gunz"
#endif

#define SCREENSHOT_FOLDER	"/Screenshots"
#define REPLAY_FOLDER		"/Replay"
#define EMBLEM_FOLDER		"/Emblem"

class CCZFileSystem;
class MMessenger;

class ZApplication;
class ZGameClient;
class ZSoundEngine;
class ZGameInterface;
class ZEffectManager;
//class ZScreenEffectManager;
class ZDirectInput;
class ZCombatInterface;
class ZCamera;
class ZGame;
class ZBaseQuest;
class ZQuest;
class ZSurvival;
class ZGameTypeManager;
class ZWorldManager;
class ZMessengerManager;
class ZEmblemInterface;
class ZInput;

extern ZDirectInput	g_DInput;
extern ZInput* g_pInput;

//

//RMeshMgr*				ZGetNpcMeshMgr();
//RMeshMgr*				ZGetMeshMgr();
//RMeshMgr*				ZGetWeaponMeshMgr();
//RAniEventMgr*			ZGetAniEventMgr();

ZSoundEngine*			ZGetSoundEngine();

ZEffectManager*			ZGetEffectManager();
//ZScreenEffectManager*	ZGetScreenEffectManager();

string						ZGetSVNRevision();


// dll-injection으로 호출하는 핵 때문에 매크로 인라이닝
#define ZGetApplication()		ZApplication::GetInstance()
#define ZGetGameClient()		(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGameClient() : NULL)
#define ZGetGame()				(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGame() : NULL)

#define ZGetGameInterface()		ZApplication::GetGameInterface()
#define ZGetCombatInterface()	(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCombatInterface() : NULL)

#define ZGetFileSystem()		ZApplication::GetFileSystem()
#define ZGetDirectInput()		(&g_DInput)

#define ZGetQuest()				((ZBaseQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuest() : NULL))
#define ZGetQuestExactly()		((ZQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuestExactly() : NULL))
#define ZGetSurvivalExactly()	((ZSurvival*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetSurvivalExactly() : NULL))

#define ZGetGameTypeManager()	((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetGameTypeManager() : NULL)

#define ZGetInput()				g_pInput
#define ZGetCamera()			(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCamera() : NULL)

#define ZGetWorldManager()		ZApplication::GetInstance()->GetWorldManager()
#define ZGetWorld()				(ZGetWorldManager()->GetCurrent())

//inline ZEmblemInterface*	ZGetEmblemInterface() { return ZApplication::GetInstance()->GetEmblemInterface(); }
inline ZOptionInterface*	ZGetOptionInterface() { return ZApplication::GetInstance()->GetOptionInterface(); }


#define ZIsActionKeyPressed(_ActionID)	ZGetInput()->IsActionKeyPressed(_ActionID)

//jintriple3 메크로..
#define PROTECT_DEBUG_REGISTER(b) if(GetTickCount() >0)if(GetTickCount() >0)if(GetTickCount() >0)if(b)
//jintriple3 디버그 레지스터 해킹 방어 위한 비교 숫자.
#define FOR_DEBUG_REGISTER 1000
