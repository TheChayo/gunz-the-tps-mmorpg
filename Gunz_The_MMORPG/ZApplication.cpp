#include "stdafx.h"

#include "ZApplication.h"
#include "ZGameInterface.h"
#include "ZInterface.h"
#include "Config.h"
#include "RShadermgr.h"
#include "ZConfiguration.h"
#include "ZInitialLoading.h"
//#include "CCMatchQuestMonsterGroup.h"
//#include "ZGameClient.h"
#include "CCRegistry.h"
#include "ZLocale.h"
#include "ZUtil.h"
#include "ZStringResManager.h"
#include "ZFile.h"
#include "ZActionKey.h"
#include "ZInput.h"
#include "ZOptionInterface.h"

ZApplication*	ZApplication::m_pInstance = NULL;
CCZFileSystem	ZApplication::m_FileSystem;    
ZSoundEngine	ZApplication::m_SoundEngine;
ZTimer			ZApplication::m_Timer;

ZApplication::ZApplication()
{
	_ASSERT(m_pInstance==NULL);

	m_nTimerRes = 0;
	m_pInstance = this;


	m_pGameInterface=NULL;
	m_pStageInterface = NULL;

	m_nInitialState = GUNZ_LOGIN;

	m_bLaunchDevelop = false;
	m_bLaunchTest = false;

	SetLaunchMode(ZLAUNCH_MODE_DEBUG);
//	SetLaunchMode(ZLAUNCH_MODE_STANDALONE_AI);

#ifdef _ZPROFILER
	m_pProfiler = new ZProfiler;
#endif
}

ZApplication::~ZApplication()
{
#ifdef _ZPROFILER
	SAFE_DELETE(m_pProfiler);
#endif

//	OnDestroy();
	m_pInstance = NULL;


}

// szBuffer �� �ִ� ���� �ܾ ��´�. �� ����ǥ�� ������ ����ǥ ���� �ܾ ��´�
bool GetNextName(char *szBuffer,int nBufferCount,char *szSource)
{
	while(*szSource==' ' || *szSource=='\t') szSource++;

	char *end=NULL;
	if(szSource[0]=='"') 
		end=strchr(szSource+1,'"');
	else
	{
		end=strchr(szSource,' ');
		if(NULL==end) end=strchr(szSource,'\t');
	}

	if(end)
	{
		int nCount=end-szSource-1;
		if(nCount==0 || nCount>=nBufferCount) return false;

		strncpy(szBuffer,szSource+1,nCount);
		szBuffer[nCount]=0;
	}
	else
	{
		int nCount=(int)strlen(szSource);
		if(nCount==0 || nCount>=nBufferCount) return false;

		strcpy(szBuffer,szSource);
	}

	return true;
}

bool ZApplication::ParseArguments(const char* pszArgs)
{
	strcpy(m_szCmdLine, pszArgs);
	{
		size_t nLength;

		// ����ǥ ������ �����Ѵ�
		if(pszArgs[0]=='"') 
		{
			strcpy(m_szFileName,pszArgs+1);

			nLength = strlen(m_szFileName);
			if(m_szFileName[nLength-1]=='"')
			{
				m_szFileName[nLength-1]=0;
				nLength--;
			}
		}
		else
		{
			strcpy(m_szFileName,pszArgs);
			nLength = strlen(m_szFileName);
		}
	}
	if ( pszArgs[0] == '/')
	{
	}
	return false;
}

void ZApplication::CheckSound()
{
	// ������ ������ �� mtrl ����� ����ɼ��� �����Ƿ� ������ ���� �� ����..

	int size = 0;//m_MeshMgr.m_id_last;
	int ani_size = 0;

	RAnimationMgr* pAniMgr = NULL;
	RAnimation* pAni = NULL;

	for(int i=0;i<size;i++) {
	}
}


void RegisterForbidKey()
{
	ZActionKey::RegisterForbidKey(0x35);// /
	ZActionKey::RegisterForbidKey(0x1c);// enter
	ZActionKey::RegisterForbidKey(0x01);// esc
}

void ZProgressCallBack(void *pUserParam,float fProgress)
{
	ZLoadingProgress *pLoadingProgress = (ZLoadingProgress*)pUserParam;
	pLoadingProgress->UpdateAndDraw(fProgress);
}

bool ZApplication::OnCreate(ZLoadingProgress *pLoadingProgress)
{	
	CCInitProfile();

	TIMECAPS tc;

	cclog("ZApplication::OnCreate : begin\n");
	__BP(2000,"ZApplication::OnCreate");

#define MMTIMER_RESOLUTION	1
	if (TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)))
	{
		m_nTimerRes = min(max(tc.wPeriodMin,MMTIMER_RESOLUTION),tc.wPeriodMax);
		timeBeginPeriod(m_nTimerRes);
	}


	DWORD _begin_time,_end_time;
#define BEGIN_ { _begin_time = timeGetTime(); }
#define END_(x) { _end_time = timeGetTime(); float f_time = (_end_time - _begin_time) / 1000.f; cclog("\n-------------------> %s : %f \n\n", x,f_time ); }

	__BP(2001,"m_SoundEngine.Create");

	ZLoadingProgress soundLoading("Sound",pLoadingProgress,.12f);
	BEGIN_;
	m_SoundEngine.Create(RealSpace2::g_hWnd, Z_AUDIO_HWMIXING, &soundLoading );

	END_("Sound Engine Create");
	soundLoading.UpdateAndDraw(1.f);

	__EP(2001);

	cclog( "sound engine create.\n" );

	RegisterForbidKey();

	__BP(2002,"m_pInterface->OnCreate()");

	ZLoadingProgress giLoading("GameInterface",pLoadingProgress,.35f);

	BEGIN_;
	m_pGameInterface=new ZGameInterface("GameInterface",Core::GetInstance()->GetMainFrame(),Core::GetInstance()->GetMainFrame());
	m_pGameInterface->m_nInitialState = m_nInitialState;
	if(!m_pGameInterface->OnCreate(&giLoading))
	{
		cclog("Failed: ZGameInterface OnCreate\n");
		SAFE_DELETE(m_pGameInterface);
		return false;
	}
	m_pGameInterface->SetBounds(0,0,CCGetWorkspaceWidth(),CCGetWorkspaceHeight());
	END_("GameInterface Create");

	giLoading.UpdateAndDraw(1.f);

	m_pStageInterface = new ZStageInterface();
	m_pOptionInterface = new ZOptionInterface;

	__EP(2002);
	__BP(2003,"Character Loading");

	ZLoadingProgress meshLoading("Mesh",pLoadingProgress,.41f);
	meshLoading.UpdateAndDraw(1.f);

	__EP(2003);
	CheckSound();
	
	BEGIN_;
	__EP(2005);
	__BP(2006,"ETC .. XML");

	m_pGameInterface->SetFocusEnable(true);
	m_pGameInterface->SetFocus();
	m_pGameInterface->Show(true);


	END_("ETC ..");

	__EP(2006);

	__EP(2000);

	__SAVEPROFILE("profile_loading.txt");

	return true;
}

void ZApplication::OnDestroy()
{
	m_SoundEngine.Destroy();
	cclog("Destroy console.\n");
;
	SAFE_DELETE(m_pGameInterface);
	SAFE_DELETE(m_pStageInterface);
	SAFE_DELETE(m_pOptionInterface);

	if (m_nTimerRes != 0)
	{
		timeEndPeriod(m_nTimerRes);
		m_nTimerRes = 0;
	}

	//CoUninitialize();

	RGetParticleSystem()->Destroy();		

	cclog("destroy game application done.\n");
}

void ZApplication::ResetTimer()
{
	m_Timer.ResetFrame();
}

void ZApplication::OnUpdate()
{
	__BP(0,"ZApplication::OnUpdate");

	float fElapsed;

	fElapsed = ZApplication::m_Timer.UpdateFrame();
	__BP(1,"ZApplication::OnUpdate::m_pInterface->Update");
	if (m_pGameInterface) m_pGameInterface->Update(fElapsed);
	__EP(1);
	__BP(2,"ZApplication::OnUpdate::SoundEngineRun");

#ifdef _BIRDSOUND
	m_SoundEngine.Update();
#else
	m_SoundEngine.Run();
#endif

	__EP(2);

	//// ANTIHACK ////
	{
		static DWORD dwLastAntiHackTick = 0;
		if (timeGetTime() - dwLastAntiHackTick > 10000) {
			dwLastAntiHackTick = timeGetTime();
			if (m_GlobalDataChecker.UpdateChecksum() == false) {
				Exit();
			}
		}
	}

	// �ƹ��������� ��� ���ؼ�..

//	if(Core::GetInstance()) {
		if(ZIsActionKeyPressed(ZACTION_SCREENSHOT)) {
			if(m_pGameInterface)
				m_pGameInterface->SaveScreenShot();
		}
//	}

	// ������ �޸� �������� item �Ӽ����� �����ϴ� ��ŷ����
//	CCGetMatchItemDescMgr()->ShiftMemoryGradually();


	__EP(0);
}

bool g_bProfile=false;

#define PROFILE_FILENAME	"profile.txt"

bool ZApplication::OnDraw()
{
	static bool currentprofile=false;
	if(g_bProfile && !currentprofile)
	{
		/*
		ENABLEONELOOPPROFILE(true);
		*/
		currentprofile=true;
        CCInitProfile();
	}

	if(!g_bProfile && currentprofile)
	{
		/*
		ENABLEONELOOPPROFILE(false);
		FINALANALYSIS(PROFILE_FILENAME);
		*/
		currentprofile=false;
		CCSaveProfile(PROFILE_FILENAME);
	}


	__BP(3,"ZApplication::Draw");

		__BP(4,"ZApplication::Draw::Core::Run");
			if(ZGetGameInterface()->GetState()!= GUNZ_GAME)	// ���Ӿȿ����� ���´�
			{
				Core::GetInstance()->Run();
			}
		__EP(4);

		__BP(5,"ZApplication::Draw::Core::Draw");

			Core::GetInstance()->Draw();

		__EP(5);

	__EP(3);

#ifdef _ZPROFILER
	// profiler
	m_pProfiler->Update();
	m_pProfiler->Render();
#endif

	return m_pGameInterface->IsDone();
}

ZSoundEngine* ZApplication::GetSoundEngine()
{
	return &m_SoundEngine;
}

void ZApplication::OnInvalidate()
{
	RGetShaderMgr()->Release();
	if(m_pGameInterface)
		m_pGameInterface->OnInvalidate();
}

void ZApplication::OnRestore()
{
	if(m_pGameInterface)
		m_pGameInterface->OnRestore();
	if( ZGetConfiguration()->GetVideo()->bShader )
	{
		RMesh::mHardwareAccellated		= true;
		if( !RGetShaderMgr()->SetEnable() )
		{
			RGetShaderMgr()->SetDisable();
		}
	}
}

void ZApplication::Exit()
{
	PostMessage(g_hWnd,WM_CLOSE,0,0);
}

#define ZTOKEN_GAME				"game"
#define ZTOKEN_REPLAY			"replay"
#define ZTOKEN_GAME_CHARDUMMY	"dummy"
#define ZTOKEN_GAME_AI			"ai"
#define ZTOKEN_QUEST			"quest"
#define ZTOKEN_FAST_LOADING		"fast"

void ZApplication::PreCheckArguments()
{
	char *str;

	str=strstr(m_szCmdLine,ZTOKEN_FAST_LOADING);

	if(str != NULL) {
		RMesh::SetPartsMeshLoadingSkip(1);
	}
}

void ZApplication::ParseStandAloneArguments(char* pszArgs)
{
	char buffer[256];

	char *str;
	str=strstr(pszArgs, ZTOKEN_GAME);
	if ( str != NULL) {
#ifndef LOCALE_NHNUSA
		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME; 

		if(GetNextName(buffer,sizeof(buffer),str+strlen(ZTOKEN_GAME)))
		{
			strcpy(m_szFileName,buffer);
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_GAME);
			return;
		}
#endif
	}

	str=strstr(pszArgs, ZTOKEN_GAME_CHARDUMMY);
	if ( str != NULL) {
		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME;
		char szTemp[256], szMap[256];
		int nDummyCount = 0, nShotEnable = 0;
		sscanf(str, "%s %s %d %d", szTemp, szMap, &nDummyCount, &nShotEnable);
		bool bShotEnable = false;
		if (nShotEnable != 0) bShotEnable = true;

		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_GAME);
		return;
	}

	str=strstr(pszArgs, ZTOKEN_QUEST);
	if ( str != NULL) {
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_QUEST);
		return;
	}

#ifndef LOCALE_NHNUSA
	#ifdef _QUEST
		str=strstr(pszArgs, ZTOKEN_GAME_AI);
		if ( str != NULL) {
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_AI);

			ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME;
			char szTemp[256], szMap[256];
			sscanf(str, "%s %s", szTemp, szMap);

//			ZGetGameClient()->GetMatchStageSetting()->SetGameType(CCMATCH_GAMETYPE_QUEST);
			
			return;
		}
	#endif
#endif

}

void ZApplication::SetInitialState()
{

	ParseStandAloneArguments(m_szCmdLine);

	ZGetGameInterface()->SetState(m_nInitialState);
}


bool ZApplication::InitLocale()
{
//	ZGetLocale()->Init( GetCountryID(ZGetConfiguration()->GetLocale()->strCountry.c_str()));
#ifdef _INDEPTH_DEBUG_
	cclog("ZApplication::InitLocale() Initialized ZGetLocale()->Init()\n");
#endif
	char szPath[MAX_PATH] = "system/";

	// ������ �ٸ� �� �����ߴ��� Ȯ��
	if (!ZGetConfiguration()->IsUsingDefaultLanguage())
	{
		const char* szSelectedLanguage = ZGetConfiguration()->GetSelectedLanguage();

		// ����Ʈ �� �ƴ϶�� �� ���� �����Ѵ�
//		ZGetLocale()->SetLanguage( GetLanguageID(szSelectedLanguage) );

		// ��Ʈ���� �ε��� ��θ� ���õ� �� ���߾� ����
		strcat(szPath, szSelectedLanguage);
		strcat(szPath, "/");
	}

//	ZGetStringResManager()->Init(szPath, ZGetLocale()->GetLanguage(), GetFileSystem());
#ifdef _INDEPTH_DEBUG_
	cclog("EXIT ZApplication::InitLocale() Initialized ZGetLocale()->Init()\n");
#endif
	return true;
}

bool ZApplication::GetSystemValue(const char* szField, char* szData)
{
	return CCRegistry::Read(HKEY_CURRENT_USER, szField, szData);
}

void ZApplication::SetSystemValue(const char* szField, const char* szData)
{
	CCRegistry::Write(HKEY_CURRENT_USER, szField, szData);
}



void ZApplication::InitFileSystem()
{
	m_FileSystem.Create("./","update");
//	m_FileSystem.SetPrivateKey( szPrivateKey, sizeof( szPrivateKey));

	string strFileNameFillist(FILENAME_FILELIST);
#ifndef _DEBUG
	strFileNameFillist += ".mef";

	m_fileCheckList.Open(strFileNameFillist.c_str(), &m_FileSystem);
	m_FileSystem.SetFileCheckList(&m_fileCheckList);
#endif

	RSetFileSystem(ZApplication::GetFileSystem());
}
