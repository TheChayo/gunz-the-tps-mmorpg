//#define _INDEPTH_DEBUG_

#include "stdafx.h"

#include "ZGameInterface.h"
#include "ZApplication.h"
#include "UPnP.h"
#include "ZConfiguration.h"
#include "FileInfo.h"
#include "ZInterfaceItem.h"
#include "CCPicture.h"
#include "ZInterfaceListener.h"
#include "CCProfiler.h"
#include "ZActionDef.h"
#include "CCSlider.h"
#include "ZMsgBox.h"
#include "CCDebug.h"
#include "CCBlobArray.h"
#include "CCListBox.h"
#include "CCTextArea.h"
#include "CCTabCtrl.h"
#include "CCComboBox.h"
#include "ZInterfaceBackground.h"
#include "RShaderMgr.h"
#include "RealSoundEffect.h"
#include "ZInitialLoading.h"
#include "RShaderMgr.h"
#include "CCToolTip.h"
#include "ZToolTip.h"
#include "ZCanvas.h"
#include "ZCrossHair.h"
#include "CCPanel.h"
#include "ZStencilLight.h"
#include "CCUtil.h"
#include "ZBmNumLabel.h"
#include "CCStringTable.h"

#include "CCFileDialog.h"
#include "ZLocale.h"

#include "ZLocatorList.h"
#include "ZInput.h"
#include "ZActionKey.h"
#include "ZGameInput.h"
#include "ZOptionInterface.h"

#include "ZStringResManager.h"

#ifdef _XTRAP
#include "./XTrap/Xtrap_C_Interface.h"				// Update sgk 0702
#endif

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#endif

#ifdef _GAMEGUARD
#include "ZGameguard.h"
#endif

//extern CCCommandLogFrame* m_pLogFrame;

static int g_debug_tex_update_cnt;
bool ZGameInterface::m_bSkipGlobalEvent = false;

bool ZGameInterface::m_sbRemainClientConnectionForResetApp = false;
ZGameClient* ZGameInterface::m_spGameClient = NULL;

void ZChangeGameState(GunzState state)
{
	PostMessage(g_hWnd, WM_CHANGE_GAMESTATE, int(state), 0);
}


void ZEmptyBitmap()
{
	CCBitmapManager::Destroy();
	CCBitmapManager::DestroyAniBitmap();
}

void ZGameInterface::LoadBitmaps(const char* szDir, const char* szSubDir, ZLoadingProgress *pLoadingProgress)
{
	cclog("ZGameInterface::LoadBitmaps()\n");

	const char *loadExts[] = { ".png", ".bmp", ".tga" };

#define EXT_LEN 4

	CCZFileSystem *pfs=ZGetFileSystem();

	int nDirLen = (int)strlen(szDir);

	int nTotalCount = 0;
	for(int i=0; i<pfs->GetFileCount(); i++){
		const char* szFileName = pfs->GetFileName(i);
		const CCZFILEDESC* desc = pfs->GetFileDesc(i);
		int nLen = (int)strlen(szFileName);
		for(int j=0;j<sizeof(loadExts)/sizeof(loadExts[0]);j++) {
			if( nLen>EXT_LEN && stricmp(szFileName+nLen-EXT_LEN, loadExts[j])==0 )
			{
				bool bAddDirToAliasName = false;
				bool bMatch = false;
				if(nDirLen==0 || strnicmp(desc->m_szFileName,szDir,nDirLen)==0)
					nTotalCount++;
				if(strnicmp(desc->m_szFileName,PATH_CUSTOM_CROSSHAIR,strlen(PATH_CUSTOM_CROSSHAIR))==0)
					nTotalCount++;
			}
		}
	}

	int nCount = 0;
	for(int i=0; i<pfs->GetFileCount(); i++){

		const char* szFileName = pfs->GetFileName(i);
		const CCZFILEDESC* desc = pfs->GetFileDesc(i);
		const CCZFILEDESC* subDesc = NULL;
		int nLen = (int)strlen(szFileName);
		const char* szTargetFile = NULL;
		for(int j=0;j<sizeof(loadExts)/sizeof(loadExts[0]);j++) {
			if( nLen>EXT_LEN && stricmp(szFileName+nLen-EXT_LEN, loadExts[j])==0 )
			{
				bool bAddDirToAliasName = false;
				bool bMatch = false;
				if(nDirLen==0 || strnicmp(desc->m_szFileName,szDir,nDirLen)==0)
					bMatch = true;
				if(strnicmp(desc->m_szFileName,PATH_CUSTOM_CROSSHAIR,strlen(PATH_CUSTOM_CROSSHAIR))==0)
				{
					bMatch = true;
					bAddDirToAliasName = true;
				}

				if(bMatch) {
					nCount++;
					if(pLoadingProgress && nCount%10==0)
						pLoadingProgress->UpdateAndDraw((float)nCount/(float)nTotalCount);


					char aliasname[256];
					char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
					_splitpath(szFileName,drive,dir,fname,ext);

					if (!bAddDirToAliasName) sprintf(aliasname,"%s%s",fname,ext);
					else sprintf(aliasname, "%s%s%s", dir, fname,ext);

					std::string strSubFile = desc->m_szFileName;
					strSubFile.replace(0, nDirLen, szSubDir);
					subDesc = pfs->GetFileDesc(strSubFile.c_str());
					if (subDesc)
						szTargetFile = subDesc->m_szFileName;
					else
						szTargetFile = desc->m_szFileName;

					//szFileName = desc->m_szFileName;
					CCBitmapR2* pBitmap = new CCBitmapR2;
					if(pBitmap->Create(aliasname, RGetDevice(), szTargetFile)==true){
						CCBitmapManager::Add(pBitmap);
					}else
						delete pBitmap;
				}
			}
		}
	}

	cclog("EXIT ZGameInterface::LoadBitmaps()\n");

}

void AddListItem(CCListBox* pList, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
	class CCDragableListItem : public CCDefaultListItem{
		char m_szDragItemString[256];
	public:
		CCDragableListItem(CCBitmap* pBitmap, const char* szText, const char* szItemString)
			: CCDefaultListItem(pBitmap, szText){
				if(szItemString!=NULL) strcpy(m_szDragItemString, szItemString);
				else m_szDragItemString[0] = 0;
			}
			virtual bool GetDragItem(CCBitmap** ppDragBitmap, char* szDragString, char* szDragItemString){
				*ppDragBitmap = GetBitmap(0);
				if(GetString(1)!=NULL) strcpy(szDragString, GetString(1));
				else szDragString[0] = 0;
				strcpy(szDragItemString, m_szDragItemString);
				return true;
			};
	};
	CCDefaultListItem* pNew = new CCDragableListItem(pBitmap, szString, szItemString);
	pList->Add(pNew);
}

bool InitSkillList(CCWidget* pWidget)
{
	if(pWidget==NULL) return false;

	if(strcmp(pWidget->GetClassName(), CORE_CCLISTBOX)!=0) return false;
	CCListBox* pList = (CCListBox*)pWidget;

	pList->SetItemHeight(32);
	pList->SetVisibleHeader(false);

	pList->AddField("Icon", 32);
	pList->AddField("Name", 600);
	AddListItem(pList, CCBitmapManager::Get("skill000.png"), "Fire-Ball", "Object.Skill $player $target 0");
	AddListItem(pList, CCBitmapManager::Get("skill001.png"), "Bull-Shit", "Object.Skill $player $target 1");
	return true;
}

bool InitItemList(CCWidget* pWidget)
{
	if(pWidget==NULL) return false;

	if(strcmp(pWidget->GetClassName(), CORE_CCLISTBOX)!=0) return false;
	CCListBox* pList = (CCListBox*)pWidget;

	//	pList->SetViewStyle(MVS_ICON);
	pList->SetVisibleHeader(false);
	pList->SetItemHeight(40);

	pList->AddField("Icon", 42);
	pList->AddField("Name", 600);
	// �׽�Ʈ�� 30�� �־����
	for (int i = 0; i < 30; i++)
	{
		char szName[256], szItem[256];
		int d = i % 6;
		sprintf(szItem, "item%03d.png", d);
		sprintf(szName, "�������̵�%d", i);
		AddListItem(pList, CCBitmapManager::Get(szItem), szName, "Command Something");
	}

	return true;
}

#define DEFAULT_SLIDER_MAX			10000

ZGameInterface::ZGameInterface(const char* szName, CCWidget* pParent, CCListener* pListener) : ZInterface(szName,pParent,pListener)
{
	CCSetString( 1, ZMsg(MSG_MENUITEM_OK));
	CCSetString( 2, ZMsg(MSG_MENUITEM_CANCEL));
	CCSetString( 3, ZMsg(MSG_MENUITEM_YES));
	CCSetString( 4, ZMsg(MSG_MENUITEM_NO));
	CCSetString( 5, ZMsg(MSG_MENUITEM_MESSAGE));

//	m_pShopEquipInterface = new ZShopEquipInterface;

	m_bShowInterface = true;
	m_bViewUI = true;
	m_bWaitingArrangedGame = false;

//	m_pMyCharacter = NULL;

	SetBounds(0, 0, CCGetWorkspaceWidth(), CCGetWorkspaceHeight());

	m_pGame = NULL;
	m_pCombatInterface = NULL;
	m_pLoadingInterface = NULL;

	m_dwFrameMoveClock = 0;

	m_nInitialState = GUNZ_LOGIN;
	m_nPreviousState = GUNZ_LOGIN;

	m_nState = GUNZ_NA;

	m_bCursor = true;
	CCCursorSystem::Show(m_bCursor);

	//m_bCursor = false;
	m_bLogin = false;
	m_bLoading = false;

	m_pRoomListFrame = NULL;
	m_pBottomFrame = NULL;
	m_pMsgBox = new ZMsgBox("", Core::GetInstance()->GetMainFrame(), this, CCT_OK);
	m_pConfirmMsgBox = new ZMsgBox("", Core::GetInstance()->GetMainFrame(), this, CCT_YESNO);

	m_pBackground = new ZInterfaceBackground();

	m_pCursorSurface=NULL;
	
	m_nDrawCount = 0;

	m_pEffectManager = NULL;
	m_pGameInput = NULL;
	
	Core::GetInstance()->SetGlobalEvent(ZGameInterface::OnGlobalEvent);
	ZGetInput()->SetEventListener(ZGameInterface::OnGlobalEvent);

    m_bOnEndOfReplay = false;
	m_nLevelPercentCache = 0;

	m_pLoginBG = NULL;
	m_pLoginPanel = NULL;

	m_nLoginState = LOGINSTATE_STANDBY;
	m_dwLoginTimer = 0;

	m_nLocServ = 0;

	m_dwHourCount = 0;
	m_dwTimeCount = timeGetTime() + 3600000;

	// Lobby Bitmap
	if ( m_pRoomListFrame != NULL)
	{
        delete m_pRoomListFrame;
		m_pRoomListFrame = NULL;
	}

	if ( m_pBottomFrame != NULL)
	{
		delete m_pBottomFrame;
		m_pBottomFrame = NULL;
	}

	// Login Bitmap
	if ( m_pLoginBG != NULL)
	{
		delete m_pLoginBG;
		m_pLoginBG = NULL;
	}

	if ( m_pLoginPanel != NULL)
	{
		delete m_pLoginPanel;
		m_pLoginPanel = NULL;
	}

	m_dwRefreshTime = 0;

	m_pLocatorList = ZGetConfiguration()->GetLocatorList();
	m_pTLocatorList = ZGetConfiguration()->GetTLocatorList();

	// ���̽� ���� �ʱ�ȭ
	m_dwVoiceTime = 0;
	m_szCurrVoice[ 0] = 0;
	m_szNextVoice[ 0] = 0;
	m_dwNextVoiceTime = 0;

	m_bReservedQuit = false;
	m_bReserveResetApp = false;

	m_dErrMaxPalyerDelayTime = 0;
	m_bErrMaxPalyer = false;

	m_bGameFinishLeaveBattle = true;

	m_MyPort = 0;
}

ZGameInterface::~ZGameInterface()
{
	ZEmptyBitmap();

	OnDestroy();

	SAFE_RELEASE(m_pCursorSurface);
	SAFE_DELETE(m_pBackground);
	SAFE_DELETE(m_pMsgBox);
	SAFE_DELETE(m_pConfirmMsgBox);
	SAFE_DELETE(m_pRoomListFrame);
	SAFE_DELETE(m_pBottomFrame);

	SAFE_DELETE(m_pLoginPanel);
}


bool ZGameInterface::InitInterface(const char* szSkinName, ZLoadingProgress *pLoadingProgress)
{
	DWORD _begin_time,_end_time;
#define BEGIN_ { _begin_time = timeGetTime(); }
#define END_(x) { _end_time = timeGetTime(); float f_time = (_end_time - _begin_time) / 1000.f; cclog("%s : %f \n", x,f_time ); }


	SetObjectTextureLevel(ZGetConfiguration()->GetVideo()->nCharTexLevel);
	SetMapTextureLevel(ZGetConfiguration()->GetVideo()->nMapTexLevel);
	SetTextureFormat(ZGetConfiguration()->GetVideo()->nTextureFormat);

	bool bRet = true;
	char szPath[256];
	char szSubPath[256] = "interface/grm/default/";	// ������ ���� ��ü ���
	char szFileName[256];
	char a_szSkinName[256];
	strcpy(a_szSkinName, szSkinName);

	ZGetInterfaceSkinPath(szPath, a_szSkinName);
	ZGetInterfaceSkinPathSubLanguage(szSubPath, a_szSkinName);
	sprintf(szFileName, "%s%s", szPath, FILENAME_INTERFACE_MAIN);

	ZEmptyBitmap();

	ZLoadingProgress pictureProgress("pictures",pLoadingProgress,.7f);
	BEGIN_;
	LoadBitmaps(szPath, szSubPath, &pictureProgress);
	END_("\n------------------------------------> Loaded Bitmaps in ");
	BEGIN_;
	if (!m_IDLResource.LoadFromFile(szFileName, this, ZGetFileSystem()))
	{
		strcpy(a_szSkinName, DEFAULT_INTERFACE_SKIN);

		ZGetInterfaceSkinPath(szPath, a_szSkinName);
		sprintf(szFileName, "%s%s", szPath, FILENAME_INTERFACE_MAIN);
		LoadBitmaps(szPath, szSubPath, &pictureProgress);
		if (m_IDLResource.LoadFromFile(szFileName, this, ZGetFileSystem()))
		{
			cclog("IDLResource Loading Success!!\n");
		}
		else
		{
			cclog("IDLResource Loading Failed!!\n");
		}
		bRet = false;
	}
	else
	{
		cclog("IDLResource Loading Success!!\n");
	}
	END_("IDL resources");

	CCBFrameLook* pFrameLook = (CCBFrameLook*)m_IDLResource.FindFrameLook("DefaultFrameLook"); 
	if (pFrameLook != NULL)
	{
		m_pMsgBox->ChangeCustomLook((CCFrameLook*)pFrameLook);
		m_pConfirmMsgBox->ChangeCustomLook((CCFrameLook*)pFrameLook);
	}
	else
	{
		_ASSERT(0);
	}

	m_textAreaLookItemDesc.SetBgColor(sColor(10, 10, 10, 220));

	InitInterfaceListener();

#define CENTERMESSAGE	"CenterMessage"
	BEGIN_WIDGETLIST(CENTERMESSAGE, &m_IDLResource, CCLabel*, pWidget);
	pWidget->SetAlignment(CCD_HCENTER);
	END_WIDGETLIST();

	ZGetOptionInterface()->InitInterfaceOption();

	return true;
}

bool ZGameInterface::InitInterfaceListener()
{
	// ���̾˷α�
	m_pMsgBox->SetListener(ZGetMsgBoxListener());
	m_pConfirmMsgBox->SetListener(ZGetConfirmMsgBoxListener());

	// �α���ȭ��
	SetListenerWidget("Exit", ZGetExitListener());
	SetListenerWidget("LoginOK", ZGetLoginListener());

//	SetListenerWidget("ParentClose", ZGetParentCloseListener());

	CCTabCtrl *pTab = (CCTabCtrl*)m_IDLResource.FindWidget("PlayerListControl");
	if( pTab ) pTab->UpdateListeners();

	return true;
}

void ZGameInterface::FinalInterface()
{
	// Player Menu
//	SAFE_DELETE(m_pPlayerMenu);

	m_IDLResource.Clear();

	cclog("clear IDL resource end.\n");

	SetCursor(NULL);

	cclog("Final interface end.\n");
}

bool ZGameInterface::ChangeInterfaceSkin(const char* szNewSkinName)
{
	char szPath[256];
	char szFileName[256];
	ZGetInterfaceSkinPath(szPath, szNewSkinName);
	sprintf(szFileName, "%s%s", szPath, FILENAME_INTERFACE_MAIN);

	FinalInterface();
	bool bSuccess=InitInterface(szNewSkinName);

	if(bSuccess)
	{
		switch(m_nState)
		{
		case GUNZ_LOGIN:	ShowWidget("Login", true); break;
		case GUNZ_LOBBY:	ShowWidget("Lobby", true); 	break;
		case GUNZ_STAGE: 	ShowWidget("Stage", true); 	break;
		case GUNZ_CHARSELECTION:
			if (m_bShowInterface)
			{
				ShowWidget("CharSelection", true);
			}break;
		case GUNZ_CHARCREATION : ShowWidget("CharCreation", true); break;
		}
		ZGetOptionInterface()->Resize(CCGetWorkspaceWidth(),CCGetWorkspaceHeight());
	}

	return bSuccess;
}


// ������ȯ�� ��������� �ӽ� ������ ��....

static bool g_parts[10];	// ĳ���� ��� ���� �׽�Ʈ�� �ӽ� ����
static bool g_parts_change;

//static int	g_select_weapon=0;
//static bool g_weapon[10];
//static bool g_weapon_change;



/// OnCommand�� ZGameInterface_OnCommand.cpp�� �ȱ�. 

bool ZGameInterface::ShowWidget(const char* szName, bool bVisible, bool bModal)
{
	CCWidget* pWidget = m_IDLResource.FindWidget(szName);

	if ( pWidget == NULL)
		return false;

	if ( strcmp( szName, "Lobby") == 0)
	{
		pWidget->Show(bVisible, bModal);

		pWidget = m_IDLResource.FindWidget( "Shop");
		pWidget->Show( false);
		pWidget = m_IDLResource.FindWidget( "Equipment");
		pWidget->Show( false);
	}
	else
		pWidget->Show(bVisible, bModal);

	return true;
}

void ZGameInterface::SetListenerWidget(const char* szName, CCListener* pListener)
{
	BEGIN_WIDGETLIST(szName, &m_IDLResource, CCWidget*, pWidget);
	pWidget->SetListener(pListener);
	END_WIDGETLIST();
}

void ZGameInterface::EnableWidget(const char* szName, bool bEnable)
{
	CCWidget* pWidget = m_IDLResource.FindWidget(szName);
	if (pWidget) pWidget->Enable(bEnable);
}

void ZGameInterface::SetTextWidget(const char* szName, const char* szText)
{
	BEGIN_WIDGETLIST(szName, &m_IDLResource, CCWidget*, pWidget);
	pWidget->SetText(szText);
	END_WIDGETLIST();
}

bool ZGameInterface::OnGameCreate()
{
	// ��Ʋ ���Խ� �ٸ� ������ ��� false ó��
	HideAllWidgets();

	// ����� �̹��� ��ε��ؼ� �޸� Ȯ��
	GetItemThumbnailMgr()->UnloadTextureTemporarily();

	// WPE hacking protect
	HMODULE hMod = GetModuleHandle( "ws2_32.dll"); 
	FARPROC RetVal = GetProcAddress( hMod, "recv"); 
	if ( (BYTE)RetVal == 0xE9)
	{
		cclog( "Hacking detected");

//		MessageBox(NULL, ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);

//		ZPostDisconnect();
	}


//	m_Camera.Init();
	ClearMapThumbnail();

	g_parts[6] = true;//Į�� ��� ���� �ӽ�..

	//DrawLoadingScreen("Now Loading...", 0.0f);
	ZApplication::GetSoundEngine()->CloseMusic();

	m_bLoading = true;
	//m_pLoadingInterface = new ZLoading("loading", this, this);

	ZLoadingProgress gameLoading("Game");

	ZGetInitialLoading()->Initialize( 1, 0,0, RGetScreenWidth(), RGetScreenHeight(), 0,0, 1024, 768, true );

	// �ε� �̹��� �ε�
	char szFileName[256];
	int nBitmap = rand() % 9;
	switch ( nBitmap)
	{
		case ( 0) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_dash.jpg");
			break;
		case ( 1) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_gaurd.jpg");
			break;
		case ( 2) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_ksa.jpg");
			break;
		case ( 3) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_safefall.jpg");
			break;
		case ( 4) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_tumbling.jpg");
			break;
		case ( 5) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_wallhang.jpg");
			break;
		case ( 6) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_walljump.jpg");
			break;
		case ( 7) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_wallrun01.jpg");
			break;
		case ( 8) :
			strcpy( szFileName, "Interface/Default/LOADING/loading_wallrun02.jpg");
			break;
		default :
			strcpy( szFileName, "");
			break;
	}

#if defined(LOCALE_NHNUSA) || defined(LOCALE_BRAZIL) || defined(LOCALE_JAPAN)
	switch ( rand() % 3)
	{
	case ( 0) :
		strcpy( szFileName, "Interface/Default/LOADING/loading_1.jpg");
		break;
	case ( 1) :
		strcpy( szFileName, "Interface/Default/LOADING/loading_2.jpg");
		break;
	case ( 2) :
		strcpy( szFileName, "Interface/Default/LOADING/loading_3.jpg");
		break;
	}
#endif

	if ( !ZGetInitialLoading()->AddBitmap( 0, szFileName))
		ZGetInitialLoading()->AddBitmap( 0, "Interface/Default/LOADING/loading_teen.jpg");
	ZGetInitialLoading()->AddBitmapBar( "Interface/Default/LOADING/loading.bmp" );
	ZGetInitialLoading()->SetTipNum( nBitmap);

	ZGetInitialLoading()->SetPercentage( 0.0f );
	ZGetInitialLoading()->Draw( MODE_FADEIN, 0 , true  );


	//m_pLoadingInterface->OnCreate();

	//m_pLoadingInterface->Progress( LOADING_BEGIN );
	//Redraw();	// Loading Screen���� �ٽ� �׸���.

//	m_pGame=new ZGame;
//	if(!m_pGame->Create(ZApplication::GetFileSystem(), &gameLoading ))
//	{
//		cclog("ZGame ���� ����\n");
//		SAFE_DELETE(m_pGame);
//		m_bLoading = false;
//		//m_pLoadingInterface->OnDestroy();
//		//delete m_pLoadingInterface; m_pLoadingInterface = NULL;
//
//		// ���ҽ��ε� ���е��� ��Ʋ ���� �����ҽ� ������ �������� �����ٴ� ��Ŷ�� ������ �κ�� ���ư���.
//		ZPostStageLeave(ZGetGameClient()->GetPlayerUID());
//		ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);
//
//		ZGetInitialLoading()->Release();
//		
//		return false;
//	}
//
	/*
	if(m_spGameClient->IsForcedEntry())
		g_pGame->SetForcedEntry();
	*/

//	m_pMyCharacter=(ZMyCharacter*)ZGetGame()->m_pMyCharacter;

	

	SetFocus();

	//ZGetInitialLoading()->SetPercentage( 100.f );

	m_pGameInput = new ZGameInput();

//	m_pCombatInterface = new ZCombatInterface("combatinterface", this, this);
//	m_pCombatInterface->SetBounds(GetRect());
//	m_pCombatInterface->OnCreate();

	
	CCWidget *pWidget = m_IDLResource.FindWidget("SkillFrame");
	if(pWidget!=NULL) pWidget->Show(true);

	// Skill List
	InitSkillList(m_IDLResource.FindWidget("SkillList"));

	pWidget = m_IDLResource.FindWidget("InventoryFrame");
	if(pWidget!=NULL) pWidget->Show(true);

	// Item List
	InitItemList(m_IDLResource.FindWidget("ItemList"));

	// Ŀ�� ������� ����
	SetCursorEnable(false);

	m_bLoading = false;

#ifndef _FASTDEBUG
	ZGetInitialLoading()->SetPercentage( 100.0f) ;
	ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 , true );
#endif
	ZGetInitialLoading()->Release();


//	if( (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_REPLAY) ||
//		(ZGetGameClient()->IsLadderGame()) || 
//		ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()) ||
//		ZGetGame()->GetMatch()->GetMatchType() == CCMATCH_GAMETYPE_DUELTOURNAMENT)
//	{
//		m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, false);
//	}
//	else
//	{
//		m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, true);
//	}


#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportCreateGameScreen();
#endif

	//// ������ ĸ��...2008.10.02
//	m_Capture = new ZBandiCapturer;
//	m_Capture->Init(g_hWnd, (LPDIRECT3DDEVICE9)RGetDevice());


	return true;
}

void ZGameInterface::OnGameDestroy()
{
	cclog( "game interface destory begin.\n" );
	CCPicture* pPicture;
	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "ClanResult_ClanBitmap1");
	if(pPicture) pPicture->SetBitmap(NULL);
	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "ClanResult_ClanBitmap2");
	if(pPicture) pPicture->SetBitmap(NULL);

	CCWidget *pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "GameResult");
	if ( pWidget) {
		CCFrame *pFrame = (CCFrame*)pWidget;
		pFrame->CCFrame::Show( false);
	}

//	ZGetGameClient()->RequestOnGameDestroyed();

//	SAFE_DELETE(m_pMiniMap);

	if (m_pGameInput)
	{
		delete m_pGameInput; m_pGameInput = NULL;
//		Core::GetInstance()->SetGlobalEvent(NULL);
	}

	if (m_pCombatInterface)
	{
//		m_pCombatInterface->OnDestroy();
//		delete m_pCombatInterface;
//		m_pCombatInterface = NULL;
	}

	ShowWidget(CENTERMESSAGE, false);

	if(ZGetGame()!=NULL){
//		ZGetGame()->Destroy();
//		SAFE_DELETE(m_pGame);
	}

	SetCursorEnable(true);
//	m_bLeaveBattleReserved = false;
//	m_bLeaveStageReserved = false;

	// ������ ĸ��...2008.10.02
//	SAFE_DELETE(m_Capture);

	cclog("game interface destroy finished\n");
}

void ZGameInterface::OnGreeterCreate()
{
	ShowWidget("Greeter", true);

	if ( m_pBackground)
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());
}

void ZGameInterface::OnGreeterDestroy()
{
	ShowWidget("Greeter", false);

	if ( m_pBackground)
		m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
}

void ZGameInterface::OnLoginCreate()
{
	// WPE hacking protect
	HMODULE hMod = GetModuleHandle( "ws2_32.dll"); 
	FARPROC RetVal = GetProcAddress( hMod, "recv"); 
	if ( (BYTE)RetVal == 0xE9)
	{
		cclog( "Hacking detected");

//		MessageBox(NULL, ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);

//		ZPostDisconnect();
	}


	m_bLoginTimeout = false;
	m_nLoginState = LOGINSTATE_FADEIN;
	m_dwLoginTimer = timeGetTime();

	// ��� �̹��� �ε�
	if ( m_pLoginBG != NULL)
	{
		delete m_pLoginBG;
		m_pLoginBG = NULL;
	}
	m_pLoginBG = new CCBitmapR2;
	bool bRead = false;
	
	// �ܺ� ������ �д´�.
#ifdef _DEBUG
	bRead = m_pLoginBG->Create( "loginbg.png", RGetDevice(), "wallpaper.jpg", false);
#else
	CCZFile::SetReadMode( CCZIPREADFLAG_ZIP | CCZIPREADFLAG_MRS | CCZIPREADFLAG_MRS2 | CCZIPREADFLAG_FILE );
	bRead = m_pLoginBG->Create( "loginbg.png", RGetDevice(), "wallpaper.jpg", false);
	CCZFile::SetReadMode( CCZIPREADFLAG_MRS2 );
#endif

	if ( bRead)
	{
		if ( (m_pLoginBG->GetWidth() > 1024) || (m_pLoginBG->GetHeight() > 768))
			bRead = false;
	}

	// ������ ���� ������ �д´�.
	if ( !bRead)
		bRead = m_pLoginBG->Create( "loginbg.png", RGetDevice(), "Interface/loadable/loginbg.jpg");

	// �о�� ��Ʈ�� �̹��� �����͸� �ش� ������ �Ѱ��༭ ǥ���Ѵ�
	if ( bRead && m_pLoginBG)
	{
		CCPicture* pPicture = (CCPicture*)m_IDLResource.FindWidget( "Login_BackgrdImg");
		if ( pPicture)
			pPicture->SetBitmap( m_pLoginBG->GetSourceBitmap());
	}


    // �г� �̹��� �ε�
	if ( m_pLoginPanel != NULL)
	{
		delete m_pLoginPanel;
		m_pLoginPanel = NULL;
	}

	m_pLoginPanel = new CCBitmapR2;
	((CCBitmapR2*)m_pLoginPanel)->Create( "loginpanel.png", RGetDevice(), "Interface/loadable/loginpanel.tga");
	if ( m_pLoginPanel)
	{
		// �о�� ��Ʈ�� �̹��� �����͸� �ش� ������ �Ѱ��༭ ǥ���Ѵ�
		CCPicture* pPicture = (CCPicture*)m_IDLResource.FindWidget( "Login_Panel");
		if ( pPicture)
			pPicture->SetBitmap( m_pLoginPanel->GetSourceBitmap());
	}

	CCButton* pLoginOk = (CCButton*)m_IDLResource.FindWidget( "LoginOK");
	if (pLoginOk)
		pLoginOk->Enable(true);

	CCWidget* pLoginFrame	= m_IDLResource.FindWidget( "LoginFrame");
	CCWidget* pLoginBG		= m_IDLResource.FindWidget( "Login_BackgrdImg");
	if (pLoginFrame)
	{
		if ( pLoginBG)
			pLoginFrame->Show(false);
		else
			pLoginFrame->Show(true);
	}

	pLoginFrame = m_IDLResource.FindWidget( "Login_ConnectingMsg");
	if ( pLoginFrame)
		pLoginFrame->Show( false);

	CCLabel* pErrorLabel = (CCLabel*)m_IDLResource.FindWidget( "LoginError");
	if ( pErrorLabel)
		pErrorLabel->SetText( "");

	CCLabel* pPasswd = (CCLabel*)m_IDLResource.FindWidget( "LoginPassword");
	if ( pPasswd)
		pPasswd->SetText( "");

	// �ݸ��� ���� ��������Ʈ�� �����ֱ� ������ �ּ�ó��
	// Netmarble ���� �α��� ��쿡 Standalone Login�� �䱸�ϸ� ��������
	//if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE) {
	//	cclog("Netmarble Logout \n");
	//	ZApplication::Exit();
	//	return;
	//}
	
	HideAllWidgets();

	ShowWidget("Login", true);

	CCWidget* pWidget = m_IDLResource.FindWidget("LoginID");
	if(pWidget)
	{
		char buffer[256];
		if (ZGetApplication()->GetSystemValue("LoginID", buffer))
			pWidget->SetText(buffer);
	}

	// ���� IP

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

#if defined(_DEBUG)	|| defined(_RELEASE)
		ShowWidget("Login_Logo", false);
		ShowWidget("Label_ID", true);
		ShowWidget("LoginID", true);
		ShowWidget("Label_Password", true);
		ShowWidget("LoginPassword", true);
#endif
//#endif
}

void ZGameInterface::OnLoginDestroy()
{
	ShowWidget("Login", false);

	CCWidget* pWidget = m_IDLResource.FindWidget("LoginID");
	if(pWidget)
	{
		// �α��� �����ϸ� write �ؾ� �ϳ�.. ���� check out ����� ����� -_-;
		ZGetApplication()->SetSystemValue("LoginID", pWidget->GetText());

		if ( m_pBackground)
			m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
	}

	// ��� �̹����� �޸𸮷κ��� �����Ѵ�
	if ( m_pLoginBG != NULL)
	{
		// ��� �̹����� �����ִ� ������ ��Ʈ�� �̹��� �����͸� �����Ѵ�
		CCPicture* pPicture = (CCPicture*)m_IDLResource.FindWidget( "Login_BackgrdImg");
		if ( pPicture)
			pPicture->SetBitmap( NULL);
	
		delete m_pLoginBG;
		m_pLoginBG = NULL;
	}

	// �г� �̹����� �޸𸮷κ��� �����Ѵ�
	if ( m_pLoginPanel != NULL)
	{
		// �г� �̹����� �����ִ� ������ ��Ʈ�� �̹��� �����͸� �����Ѵ�
		CCPicture* pPicture = (CCPicture*)m_IDLResource.FindWidget( "Login_Panel");
		if ( pPicture)
			pPicture->SetBitmap( NULL);
	
		delete m_pLoginPanel;
		m_pLoginPanel = NULL;
	}

//	ZGetShop()->Destroy();
}

void ZGameInterface::OnDirectLoginCreate()
{
#ifdef LOCALE_KOREA

	OnNetmarbleLoginCreate();

#endif
}

void ZGameInterface::OnDirectLoginDestroy()
{
#ifdef LOCALE_KOREA

	OnNetmarbleLoginDestroy();

#endif
}

//#include "ZNetmarble.h"
void ZGameInterface::OnNetmarbleLoginCreate()
{
	if ( m_pBackground)
	{
		m_pBackground->LoadMesh();
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
	}

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

//	if (m_spGameClient->IsConnected())
	{
//		ZPostDisconnect();
	}

	HideAllWidgets();
	ShowWidget("NetmarbleLogin", true);

//	ZBaseAuthInfo* pAuthInfo = ZGetLocale()->GetAuthInfo();
//	if (pAuthInfo)
//	{
#ifdef _DEBUG
//		cclog("Connect to Netmarble GunzServer(IP:%s , Port:%d) \n", pAuthInfo->GetServerIP(), pAuthInfo->GetServerPort());
#endif
//		ZPostConnect(pAuthInfo->GetServerIP(), pAuthInfo->GetServerPort());
//	}
//	else _ASSERT(0);
}

void ZGameInterface::OnNetmarbleLoginDestroy()
{
		if ( m_pBackground)
			m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
}


void ZGameInterface::OnGameOnLoginCreate()
{
	if ( m_pBackground)
	{
		m_pBackground->LoadMesh();
		m_pBackground->SetScene(LOGIN_SCENE_FIXEDSKY);
	}

	ZApplication::GetSoundEngine()->StopMusic();
	ZApplication::GetSoundEngine()->OpenMusic( BGMID_INTRO, ZApplication::GetFileSystem());

//	if (m_spGameClient->IsConnected())
	{
//		ZPostDisconnect();
	}

	HideAllWidgets();
	ShowWidget("NetmarbleLogin", true);

//	ZBaseAuthInfo* pAuthInfo = ZGetLocale()->GetAuthInfo();
//	if (pAuthInfo)
//		ZPostConnect( pAuthInfo->GetServerIP(), pAuthInfo->GetServerPort());
//	else
//		_ASSERT(0);
}

void ZGameInterface::OnGameOnLoginDestroy()
{
		if ( m_pBackground)
			m_pBackground->SetScene(LOGIN_SCENE_FALLDOWN);
}


void ZGameInterface::OnLobbyCreate()
{
	/*if( m_pAmbSound != NULL )
	{
		m_pAmbSound->Stop();
		m_pAmbSound		= NULL;
	}
	//*/
	
	// ���÷��� �Ŀ� �ٲ� LevelPercent���� ������� �����Ѵ�
	if ( m_bOnEndOfReplay)
	{
		m_bOnEndOfReplay = false;
//		ZGetMyInfo()->SetLevelPercent( m_nLevelPercentCache);
	}

	// ���÷��� �Ŀ� �ٲ� LevelPercent���� ������� �����Ѵ�
	if ( m_bOnEndOfReplay)
	{
		m_bOnEndOfReplay = false;
//		ZGetMyInfo()->SetLevelPercent( m_nLevelPercentCache);
	}

	if( m_pBackground != 0 )
		m_pBackground->Free();	// Free Memory...

	if (m_spGameClient)
	{
//		m_spGameClient->ClearPeers();
//		m_spGameClient->ClearStageSetting();
	}

	SetRoomNoLight(1);
//	ZGetGameClient()->RequestOnLobbyCreated();
	

	ShowWidget("CombatMenuFrame", false);
	ShowWidget("Lobby", true);
	EnableLobbyInterface(true);

	CCWidget* pWidget = m_IDLResource.FindWidget("StageName");
	if(pWidget){
		char buffer[256];
		if (ZGetApplication()->GetSystemValue("StageName", buffer))
			pWidget->SetText(buffer);
	}

// 	ZRoomListBox* pRoomList = (ZRoomListBox*)m_IDLResource.FindWidget("Lobby_StageList");
//	if (pRoomList) pRoomList->Clear();

	ShowWidget("Lobby_StageList", true);
	/*
	ShowWidget("ChannelFrame", true);
	ShowWidget("StageListFrame", true);
	ShowWidget("StageFrame", true);
	*/

	CCPicture* pPicture = 0;
	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StripBottom");
 	if(pPicture != NULL)	pPicture->SetAnimation( 0, 1000.0f);
	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StripTop");
	if(pPicture != NULL)	pPicture->SetAnimation( 1, 1000.0f);

    pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_RoomListBG");
	if ( pPicture)
	{
		m_pRoomListFrame = new CCBitmapR2;
		((CCBitmapR2*)m_pRoomListFrame)->Create( "gamelist_panel.png", RGetDevice(), "interface/loadable/gamelist_panel.png");

		if ( m_pRoomListFrame != NULL)
			pPicture->SetBitmap( m_pRoomListFrame->GetSourceBitmap());

//		m_pDuelTournamentLobbyFrame = new CCBitmapR2;
//		((CCBitmapR2*)m_pDuelTournamentLobbyFrame)->Create( "dueltournament_lobby_panel.png", RGetDevice(), "interface/loadable/dueltournament_lobby_panel.png");
	}
    pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_BottomBG");
	if ( pPicture)
	{
		m_pBottomFrame = new CCBitmapR2;
		((CCBitmapR2*)m_pBottomFrame)->Create( "bottom_panel.png", RGetDevice(), "interface/loadable/bottom_panel.png");

		if ( m_pBottomFrame != NULL)
			pPicture->SetBitmap( m_pBottomFrame->GetSourceBitmap());
	}

//	m_pClanInfoBg = new CCBitmapR2;
//	((CCBitmapR2*)m_pClanInfoBg)->Create( "claninfo_panel.tga", RGetDevice(), "interface/loadable/claninfo_panel.tga");
//	if ( m_pClanInfoBg != NULL)
	{
//		pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_ClanInfoBG");
//		if ( pPicture)	pPicture->SetBitmap( m_pClanInfoBg->GetSourceBitmap());
	}

//	m_pDuelTournamentInfoBg = new CCBitmapR2;
//	((CCBitmapR2*)m_pDuelTournamentInfoBg)->Create( "dueltournamentinfo_panel.tga", RGetDevice(), "interface/loadable/dueltournamentinfo_panel.tga");
//	if ( m_pDuelTournamentInfoBg != NULL)
	{
//		pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentInfoBG");
//		if ( pPicture)	pPicture->SetBitmap( m_pDuelTournamentInfoBg->GetSourceBitmap());
	}

//	m_pDuelTournamentRankingLabel = new CCBitmapR2;
//	((CCBitmapR2*)m_pDuelTournamentRankingLabel)->Create( "DuelTournamentRankingLabel.png", RGetDevice(), "interface/loadable/DuelTournamentRankingLabel.png");
//	if ( m_pDuelTournamentRankingLabel != NULL)
	{
//		pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentRankingListLabel");
//		if ( pPicture)	pPicture->SetBitmap( m_pDuelTournamentRankingLabel->GetSourceBitmap());
	}

//	ZDuelTournamentRankingListBox* pDTRankingListBox = (ZDuelTournamentRankingListBox*)m_IDLResource.FindWidget("Lobby_DuelTournamentRankingList");
//	if (pDTRankingListBox) pDTRankingListBox->LoadInterfaceImgs();

	// music
#ifdef _BIRDSOUND
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY);
	ZApplication::GetSoundEngine()->PlayMusic( true);
#else
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY, ZApplication::GetFileSystem());
	ZApplication::GetSoundEngine()->PlayMusic( true);
#endif

//	m_pBackground->SetScene(SCENE_TOWN);

	//ZCharacterViewList* pCharacterViewList = NULL;
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_STAGE);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->RemoveAll();
	//}
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_LOBBY);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->Assign(ZGetGameClient()->GetObjCacheMap());
	//}

//	SetupPlayerListTab();

//	ZPlayerListBox *pPlayerListBox = (ZPlayerListBox*)m_IDLResource.FindWidget( "LobbyChannelPlayerList" );
//	if(pPlayerListBox)
//		pPlayerListBox->SetMode(ZPlayerListBox::PLAYERLISTMODE_CHANNEL);

//	InitLadderUI();

	// ä���Է¿� ��Ŀ���� �����ش�.
	pWidget= m_IDLResource.FindWidget( "ChannelChattingInput" );
	if(pWidget) pWidget->SetFocus();

	if ( m_pBackground)
		m_pBackground->SetScene( LOGIN_SCENE_FIXEDCHAR);


	// UI ������Ʈ
//	bool bEnable = ZGetGameClient()->GetEnableInterface();

//	pWidget = m_IDLResource.FindWidget( "StageJoin");
//	if ( pWidget)		pWidget->Enable( bEnable);

//	pWidget = m_IDLResource.FindWidget( "StageCreateFrameCaller");
//	if ( pWidget)		pWidget->Enable( bEnable);
	
//	pWidget = m_IDLResource.FindWidget( "QuickJoin");
//	if ( pWidget)		pWidget->Enable( bEnable);
	
//	pWidget = m_IDLResource.FindWidget( "QuickJoin2");
//	if ( pWidget)		pWidget->Enable( bEnable);
	
//	pWidget = m_IDLResource.FindWidget( "ArrangedTeamGame");
//	if ( pWidget)		pWidget->Enable( bEnable);

//	pWidget = m_IDLResource.FindWidget( "ChannelChattingInput");
//	if ( pWidget)		pWidget->Enable( bEnable);

//	pWidget = m_IDLResource.FindWidget( "Lobby_StageList");
//	if ( pWidget)		pWidget->Enable( bEnable);

//	InitLobbyUIByChannelType();
}

void ZGameInterface::InitLobbyUIByChannelType()
{
//	bool bClanBattleUI =  ((ZGetGameClient()->GetServerMode() == CSM_CLAN) && (ZGetGameClient()->GetChannelType()==CCCHANNEL_TYPE_CLAN));
//	bool bDuelTournamentUI = (ZGetGameClient()->GetChannelType() == CCCHANNEL_TYPE_DUELTOURNAMENT);

	// ������ �߿��ϴ�.. true������ ���߿� ȣ������� �Ѵ� (�������� �����ͼ� �Լ� �ѱ����� �����ֱ⵵ �� �η���...)
//	if (bClanBattleUI)
//	{
//		ZGetGameInterface()->InitDuelTournamentLobbyUI(false);
//		ZGetGameInterface()->InitClanLobbyUI(true);
//	}
//	else if (bDuelTournamentUI)
//	{
//		ZGetGameInterface()->InitClanLobbyUI(false);
//		ZGetGameInterface()->InitDuelTournamentLobbyUI(true);
//
//		ZPostDuelTournamentRequestSideRankingInfo( ZGetMyUID());
///	}
///	else
//	{
//		ZGetGameInterface()->InitClanLobbyUI(false);
//		ZGetGameInterface()->InitDuelTournamentLobbyUI(false);
//	}
//
//	if (bDuelTournamentUI)
//		ZPostDuelTournamentRequestSideRankingInfo( ZGetMyUID());
}

void ZGameInterface::OnLobbyDestroy()
{
	ShowWidget("Lobby", false);

	CCPicture* pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_RoomListBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_BottomBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);
    
	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_ClanInfoBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentInfoBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Lobby_DuelTournamentRankingListLabel");
	if ( pPicture)
		pPicture->SetBitmap( NULL);


	SAFE_DELETE(m_pRoomListFrame);
//	SAFE_DELETE(m_pDuelTournamentLobbyFrame);
	SAFE_DELETE(m_pBottomFrame);
//	SAFE_DELETE(m_pClanInfoBg);
//	SAFE_DELETE(m_pDuelTournamentInfoBg);
//	SAFE_DELETE(m_pDuelTournamentRankingLabel);

//	ZDuelTournamentRankingListBox* pDTRankingListBox = (ZDuelTournamentRankingListBox*)m_IDLResource.FindWidget("Lobby_DuelTournamentRankingList");
//	if (pDTRankingListBox) {
//		pDTRankingListBox->UnloadInterfaceImgs();
//		pDTRankingListBox->SetVisible(false);
//	}

	CCWidget* pWidget = m_IDLResource.FindWidget("StageName");
	if(pWidget) ZGetApplication()->SetSystemValue("StageName", pWidget->GetText());
}

void ZGameInterface::OnStageCreate()
{
	// WPE hacking protect
	HMODULE hMod = GetModuleHandle( "ws2_32.dll"); 
	FARPROC RetVal = GetProcAddress( hMod, "recv"); 
//	if ( ZCheckHackProcess() || (BYTE)RetVal == 0xE9)
//	{
//		cclog( "Hacking detected");

//		MessageBox(NULL, ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
//		ZApplication::GetGameInterface()->ShowWidget("HackWarnings", true, true);

//		ZPostDisconnect();
//	}


	cclog("StageCreated\n");

//	if (m_spGameClient)
	{
//		m_spGameClient->ClearPeers();
	}

	ShowWidget("Shop", false);
	ShowWidget("Equipment", false);
	ShowWidget("Stage", true);
	EnableStageInterface(true);
	CCButton* pObserverBtn = (CCButton*)m_IDLResource.FindWidget("StageObserverBtn");
	if ( pObserverBtn)
		pObserverBtn->SetCheck( false);

	/*
	CCListBox* pListBox  = (CCListBox*)m_IDLResource.FindWidget("StageChattingOutput");
	if (pListBox != NULL)
	{
		pListBox->RemoveAll();		
	}
	*/

	//ZCharacterViewList* pCharacterViewList = NULL;
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_LOBBY);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->RemoveAll();
	//}
	//pCharacterViewList = ZGetCharacterViewList(GUNZ_STAGE);
	//if (pCharacterViewList != NULL)
	//{
	//	pCharacterViewList->Assign(ZGetGameClient()->GetObjCacheMap());
	//}

	ZCharacterView* pCharView = (ZCharacterView*)m_IDLResource.FindWidget("Stage_Charviewer");
	
	if( pCharView != NULL )
	{
		//CCMatchObjCacheMap* pObjCacheMap = ZGetGameClient()->GetObjCacheMap();
		//for(CCMatchObjCacheMap::iterator itor = pObjCacheMap->begin(); itor != pObjCacheMap->end(); ++itor)
		//{
		//	CCMatchObjCache* pObj = (*itor).second;
		//	if( pObj->GetUID() == ZGetMyUID() )
		//	{
		//		pCharView->SetCharacter( pObj->GetUID() );
		//	}
		//}
//		pCharView->SetCharacter( ZGetMyUID());
	}

	CCPicture* pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripBottom");
	if(pPicture != NULL)	pPicture->SetBitmapColor(0xFFFFFFFF);
	pPicture = (CCPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripTop");
	if(pPicture != NULL)	pPicture->SetBitmapColor(0xFFFFFFFF);

//	ZPostRequestStageSetting(ZGetGameClient()->GetStageUID());
	SerializeStageInterface();

#ifdef _BIRDSOUND
		ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY);
		ZApplication::GetSoundEngine()->PlayMusic(true);
#else
		ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY, ZApplication::GetFileSystem());
		ZApplication::GetSoundEngine()->PlayMusic(true);
#endif
/*
#ifdef _BIRDSOUND
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY);
	ZApplication::GetSoundEngine()->PlayMusic();
#else
#ifndef _FASTDEBUG
	ZApplication::GetSoundEngine()->OpenMusic(BGMID_LOBBY, ZApplication::GetFileSystem());
	ZApplication::GetSoundEngine()->PlayMusic();
#endif
#endif
*/
/*
	// �����̸� ���� ���� ������ �ٽ� �����Ѵ�.
	if ( ZGetGameClient() && ZGetGameClient()->AmIStageMaster())
	{
		CCChannelRule* pRule = ZGetChannelRuleMgr()->GetRule( ZGetGameClient()->GetChannelRuleName());
		if ( pRule)
		{
			CCComboBox* pCB = (CCComboBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageType");				
			if ( pCB)
			{
				pCB->SetSelIndex( pRule->GetDefault());

				ZStageSetting::InitStageSettingGameFromGameType();
				ZStageSetting::PostDataToServer();
			}
		}
	}
*/
	ZApplication::GetStageInterface()->OnCreate();
}

void ZGameInterface::OnStageDestroy()
{
	ZApplication::GetStageInterface()->OnDestroy();
}

bool ZGameInterface::OnCreate(ZLoadingProgress *pLoadingProgress)
{	
	ZLoadingProgress interfaceProgress("interfaceSkin",pLoadingProgress,.7f);
	if(!InitInterface(ZGetConfiguration()->GetInterfaceSkinName(),&interfaceProgress))
	{
		cclog("ZGameInterface::OnCreate: Failed InitInterface\n");
		return false;
	}

	interfaceProgress.UpdateAndDraw(1.f);


	// ���ȭ�� ����Ʈ�ڽ� �ʱ�ȭ
	int nLineHeightTextArea = int(18/600.f * CCGetWorkspaceHeight());	// 800*600 �ػ� �������� 18�ȼ�
	SetCursorEnable(true);

	cclog( "game interface create success.\n" );

	return true;
}

void ZGameInterface::OnDestroy()
{
	cclog("Destroy interface begin \n");
	SetCursorEnable(false);

	SetState(GUNZ_NA);	// ���� GunzState ����

//	SAFE_DELETE(m_pMapThumbnail);
	SAFE_DELETE(m_pLoginBG);			// ���� �ȵǴ� ��찡 �ֱ淡 -_-;

	FinalInterface();

	cclog("Destroy game interface done.\n");
}

void ZGameInterface::OnShutdownState()
{
	cclog("ZGameInterface::OnShutdown() : begin \n");
	
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			CCLabel* pLabel = (CCLabel*)pResource->FindWidget("NetmarbleLogiiMessage");
//			pLabel->SetText(CCGetErrorString(MERR_CLIENT_DISCONNECTED));
			pLabel->SetText( ZErrStr(MERR_CLIENT_DISCONNECTED) );
			ZApplication::GetGameInterface()->ShowWidget("NetmarbleLogin", true);

	cclog("ZGameInterface::OnShutdown() : done() \n");
}



bool ZGameInterface::SetState(GunzState nState)
{
#ifdef _BIRDTEST
	if ((nState != GUNZ_LOGIN) && (m_nState==GUNZ_BIRDTEST)) return false;
#endif

	// ���� ��ġ�� ���� ��ġ�� ������ ����
	if ( m_nState == nState)
		return true;

	if ( nState == GUNZ_PREVIOUS)
		nState = m_nPreviousState;


	// ������ ����(GUNZ_GAME)�ϱ� ���� ����Ʈ ��� ���� ���� ������ �ʿ䰡 �ִ��� �˻�
	if ( nState == GUNZ_GAME)
	{
		if ( ZApplication::GetStageInterface()->IsShowStartMovieOfQuest())
		{
			ZApplication::GetStageInterface()->ChangeStageEnableReady( true);
			CCWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageReady");
			if ( pWidget)
				pWidget->Enable( false);
			ZApplication::GetStageInterface()->StartMovieOfQuest();
			return true;
		}
	}

	m_nPreviousState = m_nState;

	if(m_nState==GUNZ_GAME) OnGameDestroy();
	else if(m_nState==GUNZ_LOGIN)
	{
		CCWidget* pWidget = m_IDLResource.FindWidget( "Login_BackgrdImg");
		if ( !pWidget)
			OnLoginDestroy();
	}
	else if(m_nState==GUNZ_DIRECTLOGIN) OnDirectLoginDestroy();
	else if(m_nState==GUNZ_LOBBY) OnLobbyDestroy();
	else if(m_nState==GUNZ_STAGE) OnStageDestroy();

	bool bStateChanged = true;
	if(nState==GUNZ_GAME) bStateChanged = OnGameCreate();
	else if(nState==GUNZ_LOGIN) OnLoginCreate();
	else if(nState==GUNZ_DIRECTLOGIN) OnDirectLoginCreate();
	else if(nState==GUNZ_LOBBY)	OnLobbyCreate();
	else if(nState==GUNZ_STAGE) OnStageCreate();
	else if(nState==GUNZ_GREETER) OnGreeterCreate();
	else if(nState==GUNZ_CHARSELECTION)
	{
		if ( m_nPreviousState == GUNZ_LOGIN)
		{
			CCWidget* pWidget = m_IDLResource.FindWidget( "Login_BackgrdImg");
			if ( !pWidget)
				OnCharSelectionCreate();
			else
			{
				m_nLoginState = LOGINSTATE_LOGINCOMPLETE;
				m_dwLoginTimer = timeGetTime() + 1000;
				return true;
			}
		}
		else
			OnCharSelectionCreate();
	}
	else if(nState==GUNZ_CHARCREATION) OnCharCreationCreate();
	else if(nState==GUNZ_SHUTDOWN) OnShutdownState();
#ifdef _BIRDTEST
	else if(nState==GUNZ_BIRDTEST) OnBirdTestCreate();
#endif


	if(bStateChanged==false){
		m_pMsgBox->SetText("Error: Can't Create a Game!");
		m_pMsgBox->Show(true, true);
		SetState(GUNZ_PREVIOUS);
	}
	else{
		m_nState = nState;
	}

	m_nDrawCount = 0;
	return true;
}

_NAMESPACE_REALSPACE2_BEGIN
//extern int g_nCheckWallPolygons,g_nRealCheckWallPolygons;
//extern int g_nCheckFloorPolygons,g_nRealCheckFloorPolygons;
extern int g_nPoly,g_nCall;
extern int g_nPickCheckPolygon,g_nRealPickCheckPolygon;
_NAMESPACE_REALSPACE2_END

#ifndef _PUBLISH
#include "fmod.h"
#endif


/*
void ZGameInterface::OnUpdateGameMessage()
{
switch (ZGetGame()->GetMatch()->GetMatchState())
{
case ZMS_ROUND_READY:
{
int nRoundReadyCount = ZGetGame()->GetMatch()->GetRoundReadyCount();
if(nRoundReadyCount<-1){
ShowWidget(CENTERMESSAGE, false);
return;
}

char szReadyMessage[256] = "";
if(nRoundReadyCount>0){
sprintf(szReadyMessage, "Round %d : Start in %d", 
ZGetGame()->GetMatch()->GetNowRound()+1, nRoundReadyCount);
}
else{
strcpy(szReadyMessage, "Fight");
}

ShowWidget(CENTERMESSAGE, true);
SetTextWidget(CENTERMESSAGE, szReadyMessage);
}
break;
case ZMS_ROUND_PLAYING:
{
ShowWidget(CENTERMESSAGE, false);
}
break;
case ZMS_ROUND_FINISH:
{
char szReadyMessage[256] = "";
sprintf(szReadyMessage, "Finish!");

ShowWidget(CENTERMESSAGE, true);
SetTextWidget(CENTERMESSAGE, szReadyMessage);
}
break;
case ZMS_GAME_FINISH:
{
ShowWidget(CENTERMESSAGE, false);
}
break;
}

}
*/

void ZGameInterface::OnDrawStateGame(CCDrawContext* pDC)
{
	if(m_pGame!=NULL) 
	{
//		if(!IsMiniMapEnable())
//			m_pGame->Draw();

		if (m_bViewUI) {

//			if(m_bLeaveBattleReserved)
//			{
//				int nSeconds = (m_dwLeaveBattleTime - timeGetTime() + 999 ) / 1000;
//				m_pCombatInterface->SetDrawLeaveBattle(m_bLeaveBattleReserved,nSeconds);
//			}else
//				m_pCombatInterface->SetDrawLeaveBattle(false,0);
//			if(GetCamera()->GetLookMode()==ZCAMERA_MINIMAP) {
//				_ASSERT(m_pMiniMap);
//				m_pMiniMap->OnDraw(pDC);
//			}

			// �׸��� ���������� ���� ���
//			m_pCombatInterface->OnDrawCustom(pDC);
		}

	}
//	m_ScreenDebugger.DrawDebugInfo(pDC);
}

void ZGameInterface::OnDrawStateLogin(CCDrawContext* pDC)
{/*
	if( m_pBackground!=0)
	{
		m_pBackground->LoadMesh();
		m_pBackground->Draw();
	}
*/
#ifndef _FASTDEBUG
	if (m_nDrawCount == 1)
	{
		/*static RealSoundEffectSource* pSES = ZApplication::GetSoundEngine()->GetSES("fx_amb_wind");
		
		if( pSES != NULL )
		{
		
			m_pAmbSound	= ZGetSoundEngine()->PlaySE( pSES, true );
		}
		//*/
	}
#endif

	CCLabel* pConnectingLabel = (CCLabel*)m_IDLResource.FindWidget( "Login_ConnectingMsg");
	if ( pConnectingLabel)
	{
		char szMsg[ 128];
		memset( szMsg, 0, sizeof( szMsg));
        int nCount = ( timeGetTime() / 800) % 4;
		for ( int i = 0;  i < nCount;  i++)
			szMsg[ i] = '<';
		sprintf( szMsg, "%s %s ", szMsg, "Connecting");
		for (int i = 0;  i < nCount;  i++)
			strcat( szMsg, ">");

		pConnectingLabel->SetText( szMsg);
		pConnectingLabel->SetAlignment( CCD_HCENTER | CCD_VCENTER);
	}


	CCWidget* pWidget = m_IDLResource.FindWidget( "LoginFrame");
	CCPicture* pPicture = (CCPicture*)m_IDLResource.FindWidget( "Login_BackgrdImg");
	if ( !pWidget || !pPicture)
		return;


	ZServerView* pServerList = (ZServerView*)m_IDLResource.FindWidget( "SelectedServer");
	CCEdit* pPassword = (CCEdit*)m_IDLResource.FindWidget( "LoginPassword");
	CCWidget* pLogin = m_IDLResource.FindWidget( "LoginOK");
//	if ( pServerList && pPassword && pLogin)
//	{
//		if ( pServerList->IsSelected() && (int)strlen( pPassword->GetText()))
//			pLogin->Enable( true);
//		else
//			pLogin->Enable( false);
//	}


	DWORD dwCurrTime = timeGetTime();

	// Check timeout
	if ( m_bLoginTimeout && (m_dwLoginTimeout <= dwCurrTime))
	{
		m_bLoginTimeout = false;

		CCLabel* pLabel = (CCLabel*)m_IDLResource.FindWidget( "LoginError");
		if (pLabel)
			pLabel->SetText( ZErrStr( MERR_CLIENT_CONNECT_FAILED));

		CCButton* pLoginOK = (CCButton*)m_IDLResource.FindWidget( "LoginOK");
		if (pLoginOK)
			pLoginOK->Enable(true);

		pWidget->Show(true);

		if ( pConnectingLabel)
			pConnectingLabel->Show( false);
	}

	// Fade in
	if ( m_nLoginState == LOGINSTATE_FADEIN)
	{
		m_bLoginTimeout = false;

		if ( dwCurrTime >= m_dwLoginTimer)
		{
			int nOpacity = pPicture->GetOpacity() + 3;
			if ( nOpacity > 255)
				nOpacity = 255;

			pPicture->SetOpacity( nOpacity);

			m_dwLoginTimer = dwCurrTime + 9;
		}

		if ( pPicture->GetOpacity() == 255)
		{
			m_dwLoginTimer = dwCurrTime + 1000;
			m_nLoginState = LOGINSTATE_SHOWLOGINFRAME;
		}
		else
			pWidget->Show( false);
	}
	// Show login frame
	else if ( m_nLoginState == LOGINSTATE_SHOWLOGINFRAME)
	{
		m_bLoginTimeout = false;

		if ( timeGetTime() > m_dwLoginTimer)
		{
			m_nLoginState = LOGINSTATE_STANDBY;
			pWidget->Show( true);
		}
	}
	// Standby
	else if ( m_nLoginState == LOGINSTATE_STANDBY)
	{
#ifdef _LOCATOR
		// Refresh server status info
		if ( timeGetTime() > m_dwRefreshTime)
			RequestServerStatusListInfo();
#endif
	}
	// Login Complete
	else if ( m_nLoginState == LOGINSTATE_LOGINCOMPLETE)
	{
		m_bLoginTimeout = false;

		if ( timeGetTime() > m_dwLoginTimer)
			m_nLoginState = LOGINSTATE_FADEOUT;

		if ( pConnectingLabel)
			pConnectingLabel->Show( false);
	}
	// Fade out
	else if ( m_nLoginState == LOGINSTATE_FADEOUT)
	{
		m_bLoginTimeout = false;
		pWidget->Show(false);

		if ( dwCurrTime >= m_dwLoginTimer)
		{
			int nOpacity = pPicture->GetOpacity() - 3;
			if ( nOpacity < 0)
				nOpacity = 0;

			pPicture->SetOpacity( nOpacity);

			m_dwLoginTimer = dwCurrTime + 9;
		}

		if ( pPicture->GetOpacity() == 0)
		{
			OnLoginDestroy();
			
			m_nLoginState = LOGINSTATE_STANDBY;
			m_nState = GUNZ_CHARSELECTION;
			OnCharSelectionCreate();
		}
	}

	if(IsErrMaxPlayer())
	{
		if(m_dErrMaxPalyerDelayTime <= dwCurrTime)
		{
//			ZPostDisconnect();
			SetErrMaxPlayer(false);
			ZApplication::GetGameInterface()->ShowErrorMessage( 10003 );
		}
	}
	
//#ifdef LOCALE_NHNUSA
#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
#define CHECK_PING_TIME	(1 * 3 * 1000)	// 3 sec
/*	static unsigned long tmAgentPingTestCheckTimer = dwCurrTime;
	if (dwCurrTime - tmAgentPingTestCheckTimer > CHECK_PING_TIME)
	{
		tmAgentPingTestCheckTimer = dwCurrTime;

		if ( pServerList)
		{
			SERVERLIST cServerList = pServerList->GetServerList();
		
			for ( SERVERLIST::iterator itr = cServerList.begin(); itr != cServerList.end();  itr++)
			{
				ServerInfo *serverInfo = (*itr);

//				CCCommand* pCmd = GetGameClient()->CreateCommand(MC_UDP_PING, CCUID(0,0));
//				unsigned int nIP = (unsigned int)inet_addr(m_spGameClient->GetUDPInfo()->GetAddress());
//				pCmd->AddParameter(new CCCmdParamUInt(dwCurrTime));
//				GetGameClient()->SendCommandByUDP(pCmd, serverInfo->szAgentIP, 7778);
//				delete pCmd;
			}
		}
	}*/
#endif
}

void ZGameInterface::OnDrawStateLobbyNStage(CCDrawContext* pDC)
{/*
	ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();

	DWORD dwClock = timeGetTime();
	if ( (dwClock - m_dwFrameMoveClock) < 30)
		return;
	m_dwFrameMoveClock = dwClock;


	if( GetState() == GUNZ_LOBBY )
	{
		// TODO : �̵��� draw Ÿ�ӿ� ����ؼ� �� �ʿ�� ����δ�. ������ ������ �ű���

		// Lobby
		char buf[512];
		// �̸�
		CCLabel* pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerName");
		if (pLabel)
		{
//			sprintf( buf, "%s", ZGetMyInfo()->GetCharName() );
			pLabel->SetText(buf);
		}
		// ������ (��ȯ�̰� �߰�)
		// Clan
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecClan");
//		sprintf( buf, "%s : %s", ZMsg( MSG_CHARINFO_CLAN), ZGetMyInfo()->GetClanName());
		if (pLabel) pLabel->SetText(buf);
		// LV
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecLevel");
//		sprintf( buf, "%s : %d %s", ZMsg( MSG_CHARINFO_LEVEL), ZGetMyInfo()->GetLevel(), ZMsg(MSG_CHARINFO_LEVELMARKER));
		if (pLabel) pLabel->SetText(buf);
		// XP
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecXP");
//		sprintf( buf, "%s : %d%%", ZMsg( MSG_CHARINFO_XP), ZGetMyInfo()->GetLevelPercent());
		if (pLabel) pLabel->SetText(buf);
		// BP
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecBP");
//		sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_BOUNTY), ZGetMyInfo()->GetBP());
		if (pLabel) pLabel->SetText(buf);
		// HP
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecHP");
//		sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_HP), ZGetMyInfo()->GetHP());
		if (pLabel) pLabel->SetText(buf);
		// AP
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecAP");
//		sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_AP), ZGetMyInfo()->GetAP());
		if (pLabel) pLabel->SetText(buf);
		// WT
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_PlayerSpecWT");
//		ZMyItemList* pItems= ZGetMyInfo()->GetItemList();
//		sprintf( buf, "%s : %d/%d", ZMsg( MSG_CHARINFO_WEIGHT), pItems->GetEquipedTotalWeight(), pItems->GetMaxWeight());
		if (pLabel) pLabel->SetText(buf);

		// ä�� ����
		pLabel = (CCLabel*)pRes->FindWidget("Lobby_ChannelName");
//		sprintf( buf, "%s > %s > %s", ZGetGameClient()->GetServerName(), ZMsg( MSG_WORD_LOBBY), ZGetGameClient()->GetChannelName());
	//	if (pLabel) 
	//		pLabel->SetText(buf);
	}

	// Stage
	else if( GetState() == GUNZ_STAGE)
	{
		// �÷��̾� ���� ǥ��
		char buf[512];
		CCListBox* pListBox = (CCListBox*)pRes->FindWidget( "StagePlayerList_");
		bool bShowMe = true;
		if ( pListBox)
		{
			ZStagePlayerListItem* pItem = NULL;
			if ( pListBox->GetSelIndex() < pListBox->GetCount())
			{
				if ( pListBox->GetSelIndex() >= 0)
					pItem = (ZStagePlayerListItem*)pListBox->Get( pListBox->GetSelIndex());

				if ( (pListBox->GetSelIndex() != -1) && (strcmp(ZGetMyInfo()->GetCharName(), pItem->GetString( 3)) != 0))
					bShowMe = false;
			}

			ZPlayerInfo* pInfo = NULL;
			if ( bShowMe){}
//				pInfo = ZGetPlayerManager()->Find( ZGetMyUID());
			else if ( pItem != NULL)
				pInfo = ZGetPlayerManager()->Find( pItem->GetUID());


			// �̸�
			CCLabel* pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerName");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s", ZGetMyInfo()->GetCharName() );
				else
					sprintf( buf, "%s", pItem->GetString( 3));
				pLabel->SetText(buf);
			}

			// Ŭ��
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecClan");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %s", ZMsg( MSG_CHARINFO_CLAN), ZGetMyInfo()->GetClanName());
				else
				{
					if ( strcmp( pItem->GetString( 5), "") == 0)
						sprintf( buf, "%s :", ZMsg( MSG_CHARINFO_CLAN));
					else
						sprintf( buf, "%s : %s", ZMsg( MSG_CHARINFO_CLAN), pItem->GetString( 5));
				}
				pLabel->SetText(buf);
			}

			// ����
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecLevel");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d %s", ZMsg( MSG_CHARINFO_LEVEL), ZGetMyInfo()->GetLevel(), ZMsg(MSG_CHARINFO_LEVELMARKER));
				else
					sprintf( buf, "%s : %s %s", ZMsg( MSG_CHARINFO_LEVEL), pItem->GetString( 1), ZMsg(MSG_CHARINFO_LEVELMARKER));
				pLabel->SetText(buf);
			}

			// XP
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecXP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d%%", ZMsg( MSG_CHARINFO_XP), ZGetMyInfo()->GetLevelPercent());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_XP));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// BP
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecBP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_BOUNTY), ZGetMyInfo()->GetBP());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_BOUNTY));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// HP
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecHP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_HP), ZGetMyInfo()->GetHP());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_HP));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// AP
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecAP");
			if ( pLabel)
			{
				if ( bShowMe)
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_AP), ZGetMyInfo()->GetAP());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_AP));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}

			// WT
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecWT");
			if ( pLabel)
			{
				ZMyItemList* pItems= ZGetMyInfo()->GetItemList();
				if ( bShowMe)
					sprintf( buf, "%s : %d/%d", ZMsg( MSG_CHARINFO_WEIGHT), pItems->GetEquipedTotalWeight(), pItems->GetMaxWeight());
				else
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_WEIGHT));
				pLabel->SetText(buf);
				pLabel->Enable( bShowMe);
			}


			

			// Ranking
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecRanking");
			if ( pLabel && pInfo)
			{
				if ( pInfo->GetRank() == 0)
					sprintf( buf, "%s : -", ZMsg( MSG_CHARINFO_RANKING));
				else
					sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_RANKING), pInfo->GetRank());
				pLabel->SetText(buf);
			}

			// Kill
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecKill");
			if ( pLabel && pInfo)
			{
				sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_KILL), pInfo->GetKill());
				pLabel->SetText(buf);
			}

			// Death
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecDeath");
			if ( pLabel && pInfo)
			{
				sprintf( buf, "%s : %d", ZMsg( MSG_CHARINFO_DEATH), pInfo->GetDeath());
				pLabel->SetText(buf);
			}

			// Winning percent
			pLabel = (CCLabel*)pRes->FindWidget("Stage_PlayerSpecWinning");
			if ( pLabel && pInfo)
			{
				sprintf( buf, "%s : %.1f%%", ZMsg( MSG_CHARINFO_WINNING), pInfo->GetWinningRatio());
				pLabel->SetText(buf);
			}


			// Character View
			if ( bShowMe)
			{
				ZCharacterView* pCharView = (ZCharacterView*)pRes->FindWidget( "Stage_Charviewer");
//				if ( pCharView)
//					pCharView->SetCharacter( ZGetMyUID());
			}
		}


		// ����Ʈ ���� �����Ҷ� ���� ������
		ZApplication::GetStageInterface()->OnDrawStartMovieOfQuest();


		// ���� �̹��� Opacity ����
		int nOpacity = 90.0f * ( sin( timeGetTime() * 0.003f) + 1) + 75;

		CCLabel* pLabel = (CCLabel*)pRes->FindWidget( "Stage_SenarioName");
		CCPicture* pPicture = (CCPicture*)pRes->FindWidget( "Stage_Lights0");
		if ( pPicture)
		{
			pPicture->SetOpacity( nOpacity);
		}
		pPicture = (CCPicture*)pRes->FindWidget( "Stage_Lights1");
		if ( pPicture)
		{
			pPicture->SetOpacity( nOpacity);
		}



		// ��� ������ ����Ʈ ������ ������
		CCWidget* pWidget = pRes->FindWidget( "Stage_ItemListView");
		if ( !pWidget)
			return;

		int nEndPos = ZApplication::GetStageInterface()->m_nListFramePos;
		sRect rect = pWidget->GetRect();
		if ( rect.x != nEndPos)
		{
			int nNewPos = rect.x + ( nEndPos - rect.x) * 0.25;
			if ( nNewPos == rect.x)		// not changed
				rect.x = nEndPos;
			else						// changed
				rect.x = nNewPos;

			pWidget->SetBounds( rect);

			if ( rect.x == 0)
			{
				pWidget = pRes->FindWidget( "Stage_CharacterInfo");
				if ( pWidget)
					pWidget->Enable( false);
			}
		}

		// �����̸� ����Ʈ ������ ������
		pWidget = pRes->FindWidget( "Stage_RelayMapListView");
		if ( !pWidget)
			return;

		nEndPos = ZApplication::GetStageInterface()->m_nRelayMapListFramePos;
		rect = pWidget->GetRect();
		if ( rect.x != nEndPos)
		{
			int nNewPos = rect.x + ( nEndPos - rect.x) * 0.25;
			if ( nNewPos == rect.x)		// not changed
				rect.x = nEndPos;
			else						// changed
				rect.x = nNewPos;

			pWidget->SetBounds( rect);

			if ( rect.x == 0)
			{
				pWidget = pRes->FindWidget( "Stage_CharacterInfo");
				if ( pWidget)
					pWidget->Enable( false);
			}
		}
	}*/
}

void ZGameInterface::OnDrawStateCharSelection(CCDrawContext* pDC)
{
//	if ( m_pBackground && m_pCharacterSelectView)
	{
//		m_pBackground->LoadMesh();
//		m_pBackground->Draw();
//		m_pCharacterSelectView->Draw();

		// Draw effects(smoke, cloud)
//		ZGetEffectManager()->Draw( timeGetTime());

		// Draw maiet logo effect
//		ZGetScreenEffectManager()->DrawEffects();
	}
}

void ZGameInterface::OnDraw(CCDrawContext *pDC)
{
	m_nDrawCount++;

	__BP(11,"ZGameInterface::OnDraw");

	if(m_bLoading) 
	{
		__EP(11);
		return;
	}

#ifdef _BIRDTEST
	if (GetState() == GUNZ_BIRDTEST)
	{
		OnBirdTestDraw();
		__EP(11);
		return;
	}
#endif		

	switch (GetState())
	{
	case GUNZ_GAME:
		{
			OnDrawStateGame(pDC);
		}
		break;
	case GUNZ_LOGIN:
	case GUNZ_DIRECTLOGIN:
		{
			OnDrawStateLogin(pDC);
		}
		break;
	case GUNZ_LOBBY:
	case GUNZ_STAGE:
		{
			OnDrawStateLobbyNStage(pDC);
		}
		break;
	case GUNZ_CHARSELECTION:
	case GUNZ_CHARCREATION:
		{
			OnDrawStateCharSelection(pDC);
		}
		break;
	}

	// û�ҳ� ���� ���� �����(������������...). 1�ð����� �޽��� ���� �����°Ŵ�...
#ifdef LOCALE_KOREA			// �ѱ������� �����Ÿ� �Ѵ�...
	if ( timeGetTime() >= m_dwTimeCount)
	{
		m_dwTimeCount += 3600000;
		m_dwHourCount++;

		char szText[ 256];
		if ( m_dwHourCount > 3)
			sprintf( szText, "%d �ð��� ����߽��ϴ�. ��� �޽��� ���Ͻñ� �ٶ��ϴ�.", m_dwHourCount);
		else
			sprintf( szText, "%d �ð��� ��� �Ͽ����ϴ�.", m_dwHourCount);
		ZChatOutput( sColor(ZCOLOR_CHAT_SYSTEM), szText);


		ZChatOutput( sColor(ZCOLOR_CHAT_SYSTEM), "�� ������ 15�� �̿밡�μ� �� 15�� �̸��� �̿��� �� �����ϴ�.");
	}
#endif

	__EP(11);
}



// �ӽ�

// ���ڴ� 2��° ��Ʈ ����..

void ZGameInterface::TestChangePartsAll() 
{
}

void ZGameInterface::TestChangeParts(int mode) {

#ifndef _PUBLISH
	// �����̳ʿ�... ȥ�� �ʰ��� �Դ� �׽�Ʈ �Ҷ� ����Ѵ�...

	RMeshPartsType ptype = eq_parts_etc;

 		 if(mode==0)	{ ptype = eq_parts_chest;  }
	else if(mode==1)	{ ptype = eq_parts_head	;  }
	else if(mode==2)	{ ptype = eq_parts_hands;  }
	else if(mode==3)	{ ptype = eq_parts_legs	;  }
	else if(mode==4)	{ ptype = eq_parts_feet	;  }
	else if(mode==5)	{ ptype = eq_parts_face	;  }

//	ZPostChangeParts(ptype,1);

#endif

}
// ĳ���� ���� �����찡 ����� ����..
void ZGameInterface::TestToggleCharacter()
{
//	ZPostChangeCharacter();
}

void ZGameInterface::TestChangeWeapon(RVisualMesh* pVMesh)
{
	static int nWeaponIndex = 0;

	int nItemID = 0;
	switch(nWeaponIndex)
	{
	case 0:
		nItemID = 1;		// katana
		break;
	case 1:
		nItemID = 5;		// dagger
		break;
	case 2:
		nItemID = 2;		// double pistol
		break;
	case 3:
		nItemID = 3;		// SMG
		break;
	case 4:
		nItemID = 6;		// shotgun
		break;
	case 5:
		nItemID = 7;		// Rocket
		break;
	case 6:
		nItemID = 4;		// grenade
		break;
	}


	if (GetState() == GUNZ_GAME)
	{
//		if (m_pMyCharacter == NULL) return;


		switch(nWeaponIndex)
		{
		case 0:
		case 1:
//			m_pMyCharacter->GetItems()->EquipItem(MMCIP_MELEE, nItemID);		// dagger
//			m_pMyCharacter->ChangeWeapon(MMCIP_MELEE);
			break;
		case 2:
		case 3:
		case 4:
		case 5:
//			m_pMyCharacter->GetItems()->EquipItem(MMCIP_PRIMARY, nItemID);		// Rocket
//			m_pMyCharacter->ChangeWeapon(MMCIP_PRIMARY);
			break;
		case 6:
//			m_pMyCharacter->GetItems()->EquipItem(MMCIP_CUSTOM1, nItemID);		// grenade
//			m_pMyCharacter->ChangeWeapon(MMCIP_CUSTOM1);
			break;
		}

	}
	else if (GetState() == GUNZ_CHARSELECTION)
	{
		if (pVMesh != NULL)
		{
//			ZChangeCharWeaponMesh(pVMesh, nItemID);
//			pVMesh->SetAnimation("login_intro");
//			pVMesh->GetFrameInfo(ani_mode_lower)->m_nFrame = 0;
//			pVMesh->m_nFrame[ani_mode_lower] = 0;
		}
	}
	else if (GetState() == GUNZ_LOBBY)
	{
		if (pVMesh != NULL)
		{
//			ZChangeCharWeaponMesh(pVMesh, nItemID);
		}
	}


	nWeaponIndex++;
	if (nWeaponIndex >= 7) nWeaponIndex = 0;
}

/*
bool ZGameInterface::ProcessLowLevelCommand(const char* szCommand)
{
if(stricmp(szCommand, "FORWARD")==0){
}
else if(stricmp(szCommand, "BACK")==0){
}
else if(stricmp(szCommand, "LEFT")==0){
}
else if(stricmp(szCommand, "RIGHT")==0){
}
return false;
}
*/

void ZGameInterface::RespawnMyCharacter()	// ȥ���׽�Ʈ�Ҷ� Ŭ���ϸ� �ǻ�Ƴ���.
{
	if (ZGetGame() == NULL) return;

//	m_pMyCharacter->Revival();
	rvector pos=rvector(0,0,0), dir=rvector(0,1,0);

//	ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetSoloRandomData();
//	if (pSpawnData != NULL)
//	{
//		pos = pSpawnData->m_Pos;
//		dir = pSpawnData->m_Dir;
//	}

//	m_pMyCharacter->SetPosition(pos);
//	m_pMyCharacter->SetDirection(dir);
}

bool ZGameInterface::OnGlobalEvent(CCEvent* pEvent)
{
	if (ZGameInterface::CheckSkipGlobalEvent() == true)
		return true;

	if ((ZGetGameInterface()->GetState() == GUNZ_GAME)) 
		return ZGameInput::OnEvent(pEvent);

#ifndef _PUBLISH
	switch(pEvent->iMessage){
		case CCWM_CHAR:
		{
			switch (pEvent->uKey) {
			case '`' :
				if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_DEVELOP)
				{
//					ZGetConsole()->Show(!ZGetConsole()->IsVisible());
//					ZGetConsole()->SetZOrder(CC_TOP);
					return true;
					//				m_pLogFrame->Show(ZGetConsole()->IsVisible());
				}
				break;
			}
		}break;

		case CCWM_KEYDOWN:
			{
				// Ctrl+R UI ���ҽ� ���ε� - ������ ����. �ٵ� �׳� UI �����ο� ���� ����̴ϱ� �׳� ���ô�..
				// (800*600�̿ܿ����� ȭ���� �̻�����, ����� �޸𸮸� ��û ��, �����߿� ���ε� �۵��ȵ�)
				if (pEvent->GetCtrlState() && pEvent->uKey == 'R') {
					GunzState state = ZGetGameInterface()->GetState();
					m_sbRemainClientConnectionForResetApp = true;
					ZLoadingProgress progress("");
					ZGetGameInterface()->OnDestroy();
					ZGetGameInterface()->OnCreate(&progress);
					//OnRestore();
					ZGetGameInterface()->SetState(state);
					m_sbRemainClientConnectionForResetApp = false;
				}
			}
	}
#endif
	return false;
}

bool ZGameInterface::OnDebugEvent(CCEvent* pEvent, CCListener* pListener)
{
	switch(pEvent->iMessage){
	case CCWM_KEYDOWN:
		{
			switch (pEvent->uKey)
			{
			case VK_F10:
			//	m_pLogFrame->Show(!m_pLogFrame->IsVisible());
				return true;
			case VK_NUMPAD8:
				{
					if (GetState() == GUNZ_CHARSELECTION)
					{
//						if (m_pCharacterSelectView != NULL)
						{
//							TestChangeWeapon(m_pCharacterSelectView->GetVisualMesh());
						}
					}
					else if (GetState() == GUNZ_LOBBY)
					{
//						if (ZGetCharacterViewList(GUNZ_LOBBY) != NULL)
						{
					//		RVisualMesh* pVMesh = 
//								ZGetCharacterViewList(GUNZ_LOBBY)->Get(ZGetGameClient()->GetPlayerUID())->GetVisualMesh();

//							TestChangeWeapon(pVMesh);
						}
					}

				}
				break;
			}
		}
		break;
	}
	return false;
}

bool ZGameInterface::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
#ifndef _PUBLISH
	if (OnDebugEvent(pEvent, pListener)) return true;
#endif

	return false;
}

bool ZGameInterface::OnCommand(CCWidget* pWidget, const char* szMessage)
{
//	if (pWidget==m_pPlayerMenu) {
//		CCMenuItem* pItem = (CCMenuItem*)pWidget;


		OutputDebugString("PLAYERMENU");
//	}
	return false;
}

void ZGameInterface::ChangeParts(int key)
{
	//1��~6��Ű���
	/*
	int mode = 0;
	int type = 0;		// ���߿� ���� ���̶�� ����� id ����...

	if(key=='1')	{ mode=0;type = 2;}
	else if(key=='2')	{ mode=1;type = 7;}
	else if(key=='3')	{ mode=2;type =12;}
	else if(key=='4')	{ mode=3;type =17;}
	else if(key=='5')	{ mode=4;type =22;}
	else if(key=='6')	{ mode=5;type =27;}
	else return;

	g_parts[mode] = !g_parts[mode];	// ĳ���� Ŭ������ �ű��
	//	g_parts_change = true;

	if(!g_parts[mode])	// �⺻��
	type = 0;	

	g_pGame->m_pMyCharacter->OnChangeParts(mode,type);

	ZPostChangeParts(mode,type);
	*/
}
/*
void ZGameInterface::UpdateReserveChangeWeapon() {

	if(!m_pMyCharacter) return;

	if(m_bisReserveChangeWeapon) {
		if( m_pMyCharacter->m_fNextShotTime < g_pGame->GetTime()) {
			ChangeWeapon(m_nReserveChangeWeapon);
			m_bisReserveChangeWeapon = false;
		}
	}
}
*/

void ZGameInterface::ChangeWeapon(ZChangeWeaponType nType)
{}


void ZGameInterface::OnGameUpdate(float fElapsed)
{
	__BP(12,"ZGameInterface::OnGameUpdate");
	if(m_pGame==NULL) return;
	if (m_pGameInput) m_pGameInput->Update(fElapsed);

//	UpdateReserveChangeWeapon();

//	m_pGame->Update(fElapsed);

//	if (m_pCombatInterface) m_pCombatInterface->Update(fElapsed);

//	if(m_bReservedWeapon)
//		ChangeWeapon(m_ReservedWeapon);

	__EP(12);
}


//extern bool g_bTestFromReplay;

void ZGameInterface::OnReplay()
{
	ShowWidget( "ReplayConfirm", false);

//	CreateReplayGame(NULL);
}

bool ZGameInterface::Update(float fElapsed)
{
	// ����� ���Ḧ Ȯ���Ѵ�.
	if ( m_bReservedQuit) {
		if ( timeGetTime() > m_dwReservedQuitTimer) {
			ZGetApplication()->Exit();
		}
	}

	if ( m_pBackground && ( ( GetState() == GUNZ_CHARSELECTION) || ( GetState() == GUNZ_CHARCREATION)) )
		m_pBackground->OnUpdate( fElapsed);


	// ���̽� ���� ������Ʈ
	OnVoiceSound();


	// �׽�Ʈ�� ���÷��̴� �ٽ� ó������ �����ϱ� ���� �̷��� �������.
	if (GetState() == GUNZ_LOBBY)
	{
//		if (g_bTestFromReplay == true) 
//		{
//			ShowWidget( "Lobby", false);
//			ShowWidget( "ReplayConfirm", true);
//			return false;
//		}
	}

	__BP(13,"ZGameInterface::Update");

	ZGetOptionInterface()->Update();

//	GetShopEquipInterface()->Update();

	__BP(14,"ZGameInterface::GameClient Run & update");
//	if (m_spGameClient != NULL) m_spGameClient->Run();
//	m_spGameClient->Tick();
	__EP(14);

 	if(!m_bLoading) {
		if(GetState()==GUNZ_GAME){
			OnGameUpdate(fElapsed);
		}
		else{
#ifdef _BIRDTEST 
			if (GetState()==GUNZ_BIRDTEST) OnBirdTestUpdate();
#endif
		}
	}

	if(GetState()==GUNZ_LOBBY && m_bWaitingArrangedGame) {
        // �ӽ÷�
		CCLabel *pLabel = (CCLabel*)m_IDLResource.FindWidget("LobbyWaitingArrangedGameLabel");
		if(pLabel) {
			int nCount = (timeGetTime()/500)%5;
			char dots[10];
			for(int i=0;i<nCount;i++) {
				dots[i]='.';
			}
			dots[nCount]=0;

			char szBuffer[256];
			sprintf(szBuffer,"%s%s", ZMsg( MSG_WORD_FINDTEAM), dots);
			pLabel->SetText(szBuffer);
		}
	}

	// �κ� �ƴѵ� ��ʸ�Ʈ ����â�� �������� ���� ���ܼ� �α��� ȭ������ ���ư� ��� ���̹Ƿ� ����â�� �ݾ��ش�
	CCWidget* pDTWaitMatchDlg = m_IDLResource.FindWidget("DuelTournamentWaitMatchDialog");
	if(pDTWaitMatchDlg && pDTWaitMatchDlg->IsVisible() && GetState()!=GUNZ_LOBBY )
		OnDuelTournamentGameUI(false);

	UpdateDuelTournamentWaitMsgDots();

	UpdateCursorEnable();

	// ���� ���ӿ��� ������ ����� �θ���
//	if(ZGetGame()!=NULL && m_bLeaveBattleReserved && (m_dwLeaveBattleTime < timeGetTime()))
//		LeaveBattle();

	__EP(13);

	return true;
}

void ZGameInterface::OnResetCursor()
{
	SetCursorEnable(m_bCursor);
}

void ZGameInterface::SetCursorEnable(bool bEnable)
{
	//	_RPT1(_CRT_WARN,"cursor %d\n",bEnable);
	
	if(m_bCursor==bEnable) return;

	m_bCursor = bEnable;
	CCCursorSystem::Show(bEnable);
}

void ZGameInterface::UpdateCursorEnable()
{}

void ZGameInterface::UpdateDuelTournamentWaitMsgDots()
{}

void ZGameInterface::SetMapThumbnail(const char* szMapName)
{
	SAFE_DELETE(m_pMapThumbnail);

	char szThumbnail[256];
	sprintf(szThumbnail, "maps/%s/%s.rs.bmp", szMapName,szMapName);

	m_pMapThumbnail=Core::GetInstance()->OpenBitmap(szThumbnail);
	if(!m_pMapThumbnail)
	{
		sprintf(szThumbnail, "maps/%s/%s.bmp", szMapName,szMapName);
		m_pMapThumbnail=Core::GetInstance()->OpenBitmap(szThumbnail);
	}
}

void ZGameInterface::ClearMapThumbnail()
{
	SAFE_DELETE(m_pMapThumbnail);
}


void ZGameInterface::Reload()
{}

void ZGameInterface::SaveScreenShot()
{}

void ZGameInterface::ShowMessage(const char* szText, CCListener* pCustomListenter, int iMessageID)
{
	if (pCustomListenter)
		m_pMsgBox->SetCustomListener(pCustomListenter);

	char text[1024] ="";

	// iMessageID�� 0�� �ƴϸ� �޼��� �ڿ� �޼��� ��ȣ�� �Բ� ������ش�.(�ٸ� ���� ���϶� Ȯ���ϱ� ����)
	if (iMessageID != 0)
	{
		sprintf(text, "%s (M%d)", szText, iMessageID);
	}
	else
	{
		strcpy(text, szText);
	}

	m_pMsgBox->SetText(text);
	m_pMsgBox->Show(true, true);
}

void ZGameInterface::ShowConfirmMessage(const char* szText, CCListener* pCustomListenter)
{
	if (pCustomListenter)
		m_pConfirmMsgBox->SetCustomListener(pCustomListenter);

	m_pConfirmMsgBox->SetText(szText);
	m_pConfirmMsgBox->Show(true, true);
}

void ZGameInterface::ShowMessage(int iMessageID)
{
	const char *str = ZMsg( iMessageID );
	if(str)
	{
		char text[1024];
		sprintf(text, "%s (M%d)", str, iMessageID);
		ShowMessage(text);
	}
}

void ZGameInterface::ShowErrorMessage(int nErrorID)
{
	const char *str = ZErrStr( nErrorID );
	if(str)
	{
		char text[1024];
		sprintf(text, "%s (E%d)", str, nErrorID);
		ShowMessage(text);
	}
}

void ZGameInterface::ShowErrorMessage(const char* szErrorMsg, int nErrorID)
{
	char szErrorID[64];
	sprintf(szErrorID, " (E%d)", nErrorID);

	char text[1024];
	strcpy(text, szErrorMsg);
	strcat(text, szErrorID);
	
	ShowMessage(text);
}

void ZGameInterface::ChangeSelectedChar(int nNum)
{}

void ZGameInterface::OnCharSelectionCreate()
{}

void ZGameInterface::OnCharSelect()
{}

void ZGameInterface::OnCharSelectionDestroy()
{}

void ZGameInterface::OnCharCreationCreate()
{}

void ZGameInterface::OnCharCreationDestroy()
{}

void ZGameInterface::ChangeToCharSelection()
{}

void ZGameInterface::OnInvalidate()
{
	if(m_pBackground)
		m_pBackground->OnInvalidate();

}

void ZGameInterface::OnRestore()
{
	if(m_pBackground)
		m_pBackground->OnRestore();

}


void ZGameInterface::UpdateBlueRedTeam( void)
{
}

void ZGameInterface::ShowInterface(bool bShowInterface)
{
	m_bShowInterface = bShowInterface;

	if (m_nState != GUNZ_GAME)
	{
		SetCursorEnable(bShowInterface);
	}

	// Login
	if (m_nState == GUNZ_LOGIN)
	{
		ShowWidget("Login", m_bShowInterface);
	}
	else if (m_nState == GUNZ_CHARSELECTION)
	{
		ShowWidget("CharSelection", m_bShowInterface);
	}
	else if (m_nState == GUNZ_GAME)
	{
//		bool bConsole=ZGetConsole()->IsVisible();
	//	bool bLogFrame=m_pLogFrame->IsVisible();

		//		m_pCombatInterface->Show(m_bShowInterface);
		//m_pLogFrame->Show(m_bShowInterface);
//		ZGetConsole()->Show(m_bShowInterface);
		ShowWidget("CombatInfo1",m_bShowInterface);
		ShowWidget("CombatInfo2",m_bShowInterface);
		ShowWidget("Time", m_bShowInterface);
///		ZGetConsole()->Show(bConsole);
		//m_pLogFrame->Show(bLogFrame);
	}
}

void ZGameInterface::FinishGame()
{
	m_bGameFinishLeaveBattle = true;
}

void ZGameInterface::SerializeStageInterface()
{
}


void ZGameInterface::HideAllWidgets()
{
	ShowWidget("Login", false);
	ShowWidget("Lobby", false);
	ShowWidget("Stage", false);
	ShowWidget("Game", false);
	ShowWidget("Option", false);
	ShowWidget("CharSelection", false);
	ShowWidget("CharCreation", false);
	ShowWidget("Shop", false);
//	ShowWidget("LobbyChannelPlayerList", false);

	// dialog
	ShowWidget("StageSettingFrame", false);
	ShowWidget("BuyConfirm", false);
	ShowWidget("Equipment", false);
	ShowWidget("StageCreateFrame", false);
	ShowWidget("PrivateStageJoinFrame", false);
}

bool SetWidgetToolTipText(char* szWidget,const char* szToolTipText, CCAlignmentMode mam) {

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if(pResource==NULL)		return false;
	if(!szToolTipText)		return false;


	CCWidget* pWidget = pResource->FindWidget(szWidget);

	if(pWidget) {

		
		if(!szToolTipText[0]) {
			pWidget->DetachToolTip();
		}
		else {
			pWidget->AttachToolTip(new ZToolTip(szToolTipText, pWidget, mam));
		}

	}
	return false;
}

// #define CheckLine(str) 
// �ִ� 200 ������� -14 ���� üũ ���ڰ� �߸��� �ʵ���..

 // zmaplistbox.cpp ���� ����Ǿ��ִ�..

bool GetItemDescName(string& str,DWORD nItemID)
{
	return false;
}

bool GetItemDescStr(string& str,DWORD nItemID) {
	return true;
}

void ZGameInterface::ShowEquipmentDialog(bool bShow)
{
}

void ZGameInterface::ShowShopDialog(bool bShow)
{
}








void ZGameInterface::EnableCharSelectionInterface(bool bEnable)
{
}

void ZGameInterface::EnableLobbyInterface(bool bEnable)
{

	EnableWidget("LobbyOptionFrame", bEnable);			// �ɼ� ��ư
	EnableWidget("Lobby_Charviewer_info", bEnable);		// �������� ��ư
	EnableWidget("StageJoin", bEnable);					// �������� ��ư
	EnableWidget("StageCreateFrameCaller", bEnable);	// ���ӻ��� ��ư
	EnableWidget("ShopCaller", bEnable);				// ���� ��ư
	EnableWidget("EquipmentCaller", bEnable);			// ��� ��ư
	EnableWidget("ReplayCaller", bEnable);				// ���÷��� ��ư
	EnableWidget("CharSelectionCaller", bEnable);		// ĳ���� ���� ��ư
	EnableWidget("Logout", bEnable);					// �α׾ƿ� ��ư
	EnableWidget("QuickJoin", bEnable);					// ������ ��ư
	EnableWidget("QuickJoin2", bEnable);				// ������ ��ư
	EnableWidget("ChannelListFrameCaller", bEnable);	// ä�κ��� ��ư
	EnableWidget("StageList", bEnable);					// �渮��Ʈ
	EnableWidget("Lobby_StageList",bEnable);
	EnableWidget("LobbyChannelPlayerList", bEnable);
	EnableWidget("ChannelChattingOutput", bEnable);
	EnableWidget("ChannelChattingInput", bEnable);

	if (bEnable)
	{
//		CCMatchServerMode nCurrentServerMode = ZGetGameClient()->GetServerMode();
//		CCCHANNEL_TYPE nCurrentChannelType = ZGetGameClient()->GetChannelType();
//		bool bClanBattleUI = (nCurrentServerMode== CSM_CLAN) && (nCurrentChannelType==CCCHANNEL_TYPE_CLAN);
//		ZGetGameInterface()->InitClanLobbyUI(bClanBattleUI);
	}

	// ���������� ������ ���� ���ð��� ���� ����(���ӽ��۹�ư ����)
	ZApplication::GetStageInterface()->SetEnableWidgetByRelayMap(!bEnable);
}

void ZGameInterface::EnableStageInterface(bool bEnable)
{
	EnableWidget("Stage_Charviewer_info", bEnable);		// �������� ��ư
	EnableWidget("StagePlayerNameInput_combo", bEnable);
	EnableWidget("GameStart", bEnable);					// ���ӽ��� ��ư
	CCButton* pButton = (CCButton*)m_IDLResource.FindWidget("StageReady");				// ���� ��ư
	if ( pButton)
	{
		pButton->Enable( bEnable);
		pButton->SetCheck( false);
	}
	EnableWidget("ForcedEntryToGame", bEnable);			// ���� ��ư
	EnableWidget("ForcedEntryToGame2", bEnable);		// ���� ��ư
	EnableWidget("StageTeamBlue",  bEnable);			// blue�� ���� ��ư
	EnableWidget("StageTeamBlue2", bEnable);			// blue�� ���� ��ư
	EnableWidget("StageTeamRed",  bEnable);				// red�� ���� ��ư
	EnableWidget("StageTeamRed2", bEnable);				// red�� ���� ��ư
	EnableWidget("ShopCaller", bEnable);				// ���� ��ư
	EnableWidget("EquipmentCaller", bEnable);			// ��� ��ư
	EnableWidget("StageSettingCaller", bEnable);		// �漳�� ��ư
	EnableWidget("StageObserverBtn", bEnable);			// ���� üũ ��ư
	EnableWidget("Lobby_StageExit", bEnable);			// ������ ��ư

	EnableWidget("MapSelection", bEnable);				// �ʼ��� �޺��ڽ�

//	if( ZGetGameClient()->AmIStageMaster() == true )
//		EnableWidget("StageType", true);					// ���ӹ�� �޺��ڽ�
//	else
//		EnableWidget("StageType", false);					// ���ӹ�� �޺��ڽ�

	EnableWidget("StageMaxPlayer", bEnable);			// �ִ��ο� �޺��ڽ�
	EnableWidget("StageRoundCount", bEnable);			// ���Ƚ�� �޺��ڽ�

	EnableWidget("StagePlayerList_", bEnable);
}

void ZGameInterface::SetRoomNoLight( int d )
{
	/*
	CCTabCtrl *pTab = (CCTabCtrl*)m_IDLResource.FindWidget("Lobby_RoomNoControl");
	if( pTab ) 
		pTab->SetSelIndex(d-1);
	*/
    
	/*
	for(int i=1;i<=6;i++)
	{
		char szBuffer[64];
		sprintf(szBuffer, "Lobby_RoomNo%d", i);
		CCButton* pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szBuffer);
		if(pButton)
		{
			bool bCheck = (i==d);
			pButton->SetCheck(bCheck);
		}
	}
	*/

	char szBuffer[64];
	sprintf(szBuffer, "Lobby_RoomNo%d", d);
	CCButton* pButton = (CCButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szBuffer);
	if(pButton) 
		pButton->SetCheck(true);

}

void ZGameInterface::ShowPrivateStageJoinFrame(const char* szStageName)
{
}

void ZGameInterface::LeaveBattle()
{
}

void ZGameInterface::ReserveLeaveStage()
{
//	m_bLeaveStageReserved = true;
//	ReserveLeaveBattle();
}

void ZGameInterface::ReserveLeaveBattle()
{
}

void ZGameInterface::ShowMenu(bool bEnable)
{
//	if(!GetCombatInterface()->IsShowResult())
//		m_CombatMenu.ShowModal(bEnable);
//	ZGetGameInterface()->SetCursorEnable(bEnable);
}

bool ZGameInterface::IsMenuVisible()
{
	return true;
//	return m_CombatMenu.IsVisible();
}

void ZGameInterface::Show112Dialog(bool bShow)
{
}


void ZGameInterface::RequestQuickJoin()
{
}


void ZGameInterface::InitClanLobbyUI(bool bClanBattleEnable)
{
}

void ZGameInterface::InitDuelTournamentLobbyUI(bool bEnableDuelTournamentUI)
{
}

/*void ZGameInterface::InitChannelFrame(CCCHANNEL_TYPE nChannelType)
{/*
	CCWidget* pWidget;

	pWidget = m_IDLResource.FindWidget("PrivateChannelInput");
	if(pWidget) pWidget->Show( nChannelType == CCCHANNEL_TYPE_USER );
	pWidget = m_IDLResource.FindWidget("PrivateChannelEnter");
	if(pWidget) pWidget->Show( nChannelType == CCCHANNEL_TYPE_USER );
	pWidget = m_IDLResource.FindWidget("MyClanChannel");
	if(pWidget) pWidget->Show( nChannelType == CCCHANNEL_TYPE_CLAN );

	CCListBox* pListBox = (CCListBox*)m_IDLResource.FindWidget("ChannelList");
	if (pListBox) pListBox->RemoveAll();*
}
*/
void ZGameInterface::InitLadderUI(bool bLadderEnable)
{/*
	OnArrangedTeamGameUI(false);

	CCWidget *pWidget;

	pWidget= m_IDLResource.FindWidget( "StageJoin" );
	if(pWidget) pWidget->Show(!bLadderEnable);
	pWidget= m_IDLResource.FindWidget( "StageCreateFrameCaller" );
	if(pWidget) pWidget->Show(!bLadderEnable);

	pWidget= m_IDLResource.FindWidget( "ArrangedTeamGame" );
	if(pWidget) pWidget->Show(bLadderEnable);

	m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, !bLadderEnable);
	//pWidget= m_IDLResource.FindWidget( "BattleExit" );
	//if(pWidget) pWidget->Enable(!bLadderEnable);

//	bool bLadderServer = 
//		ZGetGameClient()->GetServerMode()==CSM_CLAN ||
//		ZGetGameClient()->GetServerMode()==CSM_LADDER ||
//		ZGetGameClient()->GetServerMode()==CSM_EVENT;

//	pWidget= m_IDLResource.FindWidget( "PrivateChannelInput" );
//	if(pWidget) pWidget->Show(bLadderServer);
	
//	pWidget= m_IDLResource.FindWidget( "PrivateChannelEnter" );
//	if(pWidget) pWidget->Show(bLadderServer);*/

}

void ZGameInterface::OnArrangedTeamGameUI(bool bFinding)
{/*
	CCWidget *pWidget;

	pWidget= m_IDLResource.FindWidget( "ArrangedTeamGame" );
	if(pWidget) pWidget->Show(!bFinding);

	pWidget = m_IDLResource.FindWidget("LobbyFindClanTeam");
	if(pWidget!=NULL) pWidget->Show(bFinding);


	// ���� �������� enable/disable ���ش�
#define SAFE_ENABLE(x,b) { pWidget= m_IDLResource.FindWidget( x ); if(pWidget) pWidget->Enable(!b); }

	SAFE_ENABLE("LobbyChannelPlayerList", bFinding);
//	SAFE_ENABLE("LobbyPlayerListTabClanCreateButton");
	SAFE_ENABLE("ShopCaller", bFinding);
	SAFE_ENABLE("EquipmentCaller", bFinding);
	SAFE_ENABLE("ChannelListFrameCaller", bFinding);
	SAFE_ENABLE("LobbyOptionFrame", bFinding);
	SAFE_ENABLE("Logout", bFinding);
	SAFE_ENABLE("ReplayCaller", bFinding);
	SAFE_ENABLE("CharSelectionCaller", bFinding);
	SAFE_ENABLE("QuickJoin", bFinding);
	SAFE_ENABLE("QuickJoin2", bFinding);

	m_bWaitingArrangedGame = bFinding;*/
}

// �����ʸ�Ʈ ��û���� �� ȭ����� ��ư���� ��Ȱ��ȭ�Ѵ�( Ȥ�� �׹ݴ�)
void ZGameInterface::OnDuelTournamentGameUI(bool bWaiting)
{/*
	CCWidget *pWidget;

	pWidget = m_IDLResource.FindWidget("DuelTournamentWaitMatchDialog");
	if(pWidget) pWidget->Show(bWaiting);

	// ���� �������� enable/disable ���ش�
	SAFE_ENABLE("LobbyChannelPlayerList", bWaiting);
	SAFE_ENABLE("ShopCaller", bWaiting);
	SAFE_ENABLE("EquipmentCaller", bWaiting);
	SAFE_ENABLE("ChannelListFrameCaller", bWaiting);
	SAFE_ENABLE("LobbyOptionFrame", bWaiting);
	SAFE_ENABLE("Logout", bWaiting);
	SAFE_ENABLE("ReplayCaller", bWaiting);
	SAFE_ENABLE("CharSelectionCaller", bWaiting);
	SAFE_ENABLE("DuelTournamentGame", bWaiting);
	SAFE_ENABLE("DuelTournamentGame_2Test", bWaiting);
	SAFE_ENABLE("DuelTournamentGame_4Test", bWaiting);*/
}

bool ZGameInterface::IsReadyToPropose()
{
	if(GetState() != GUNZ_LOBBY)
		return false;

	if(m_bWaitingArrangedGame)
		return false;

	if(GetLatestExclusive()!=NULL)
		return false;

	if(m_pMsgBox->IsVisible())
		return false;

	// TODO : �տ� modal â�� �������� return false ����..
	return true;
}

bool ZGameInterface::IsMiniMapEnable()
{
	return true;
	//return GetCamera()->GetLookMode()==ZCAMERA_MINIMAP;
}

bool ZGameInterface::OpenMiniMap()
{
//	if(!m_pMiniMap) {
//		m_pMiniMap = new ZMiniMap;
//		if(!m_pMiniMap->Create(ZGetGameClient()->GetMatchStageSetting()->GetMapName()))
//		{
//			SAFE_DELETE(m_pMiniMap);
//			return false;
//		}
//	}

	return true;
}









// ���÷��� ���� �Լ���(��ȯ�̰� �߰�)

/***********************************************************************
  ShowReplayDialog : public
  
  desc : ���÷��� ȭ�� ���̱�
  arg  : true(=show) or false(=hide)
  ret  : none
************************************************************************/
class ReplayListBoxItem : public CCListItem
{
protected:
	char		m_szName[ _MAX_PATH];
	char		m_szVersion[ 10];

public:
	ReplayListBoxItem( const char* szName, const char* szVersion)
	{
		strcpy(m_szName, szName);
		strcpy(m_szVersion, szVersion);
	}

	virtual const char* GetString( void)
	{
		return m_szName;
	}
	virtual const char* GetString( int i)
	{
		if ( i == 0)
			return m_szName;
		else if ( i == 1)
			return m_szVersion;

		return NULL;
	}
	virtual CCBitmap* GetBitmap( int i)
	{
		return NULL;
	}
};

void ZGameInterface::ShowReplayDialog( bool bShow)
{/*
	if ( bShow)			// ���̱��̸�...
	{
		// ���÷��� ȭ�� ���̱�
		CCWidget* pWidget;
		pWidget = (CCWidget*)m_IDLResource.FindWidget( "Replay");
		if ( pWidget)
			pWidget->Show( true, true);

		// '����'��ư ��Ȱ��ȭ
		pWidget = (CCWidget*)m_IDLResource.FindWidget( "Replay_View");
		if ( pWidget)
			pWidget->Enable( false);


		// ���� ����Ʈ �ʱ�ȭ
		CCListBox* pListBox = (CCListBox*)m_IDLResource.FindWidget( "Replay_FileList");
		if ( pListBox)
		{
			pListBox->RemoveAll();

			// Get path name
			TCHAR szPath[ MAX_PATH];
			if ( GetMyDocumentsPath( szPath))		// ������ ���� ���
			{
				strcat( szPath, GUNZ_FOLDER);		// Gunz ���� ���
				strcat( szPath, REPLAY_FOLDER);		// Replay ���� ���
				CreatePath( szPath );
			}
			TCHAR szFullPath[ MAX_PATH];
			strcpy( szFullPath, szPath);

			strcat( szPath, "/*.gzr");			// Ȯ��� ����

			// Get file list
			struct _finddata_t c_file;
			long hFile;
			char szName[ _MAX_PATH];
			char szFullPathName[ _MAX_PATH];
			if ( (hFile = _findfirst( szPath, &c_file)) != -1L)
			{
				do
				{
					strcpy( szName, c_file.name);

					strcpy( szFullPathName, szFullPath);
					strcat( szFullPathName, "/");
					strcat( szFullPathName, szName);

					DWORD dwFileVersion = 0;
					char szVersion[10];
					int nRead;
					DWORD header;
					ZFile *file = zfopen( szFullPathName);
					if ( file)
					{
						nRead = zfread( &header, sizeof( header), 1, file);
						if( (nRead != 0) && (header == GUNZ_REC_FILE_ID))
						{
							zfread( &dwFileVersion, sizeof( dwFileVersion), 1, file);
							sprintf( szVersion, "v%d.0", dwFileVersion);
						}
						else
							strcpy( szVersion, "--");

						zfclose( file);
					}
					else
						strcpy( szVersion, "--");
				
					pListBox->Add( new ReplayListBoxItem( szName, szVersion));			// Add to listbox
				} while ( _findnext( hFile, &c_file) == 0);

				_findclose( hFile);
			}

			pListBox->Sort();		// Sorting
		}
	}
	else				// ���߱��̸�...
	{
		// ���÷��� ȭ�� ���߱�
		ShowWidget( "Replay", false);
	}*/
}

/***********************************************************************
  ViewReplay : public
  
  desc : ���÷��� ����
  arg  : none
  ret  : none
************************************************************************/
void ZGameInterface::ViewReplay( void)
{/*
	// ���÷��� ���̾�α� �ݱ�
	ShowReplayDialog( false);


	// ���� ���-ȭ�ϸ� ����
	CCListBox* pListBox = (CCListBox*)m_IDLResource.FindWidget( "Replay_FileList");
	if ( !pListBox)
		return ;

	if ( pListBox->GetSelItemString() == NULL)
		return ;

	TCHAR szName[ MAX_PATH];
	if ( GetMyDocumentsPath( szName))				// ������ ���� ���
	{
		strcat( szName, GUNZ_FOLDER);				// Gunz ���� ���
		strcat( szName, REPLAY_FOLDER);				// Replay ���� ���
		strcat( szName, "/");
		strcat( szName, pListBox->GetSelItemString());
	}


	m_bOnEndOfReplay = true;
//	m_nLevelPercentCache = ZGetMyInfo()->GetLevelPercent();

	// ���÷��� ����
//	if ( !CreateReplayGame( szName))
	{
//		ZApplication::GetGameInterface()->ShowMessage( "Can't Open Replay File" );
	}

	// �ɼ� �޴��� �Ϻ� ��ư ��Ȱ��ȭ
	m_CombatMenu.EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, false);*/
}



////////////////////////////////////////////////////////////////
// ����Ʈ�� ������ ������ ȭ�ϸ�(�ϵ� �ڵ� ���~  -_-;)
// �������� ��� �ϴ� �����...  -_-;
CCBitmap* ZGameInterface::GetQuestItemIcon( int nItemID, bool bSmallIcon)
{
	char szFileName[ 64] = "";
	switch ( nItemID)
	{
		// Page
		case 200001 :		// ������ 13������
		case 200002 :		// ������ 25������
		case 200003 :		// ������ 41������
		case 200004 :		// ������ 65������
			strcpy( szFileName, "slot_icon_page");
			break;

		// Skull
		case 200005 :		// �����ΰ�
		case 200006 :		// ū�ΰ�
		case 200007 :		// ���������� �ΰ�
		case 200008 :		// ����� �ΰ�
		case 200009 :		// ��� ŷ�� �ΰ�
		case 200010 :		// �Ŵ��� ����
			strcpy( szFileName, "slot_icon_skull");
			break;

		// Fresh
		case 200011 :		// �����
		case 200012 :		// �Ұ��
		case 200013 :		// ������ũ
			strcpy( szFileName, "slot_icon_fresh");
			break;

		// Ring
		case 200014 :		// ö�Ͱ���
		case 200015 :		// ���Ͱ���
		case 200016 :		// �ݱͰ���
		case 200017 :		// �÷�Ƽ�� �Ͱ���
			strcpy( szFileName, "slot_icon_ring");
			break;

		// Necklace
		case 200018 :		// ũ������ �����
			strcpy( szFileName, "slot_icon_neck");
			break;

		// Doll
		case 200019 :		// ���̷��� ����
		case 200020 :		// �ں��� ����
		case 200021 :		// ��� ����
		case 200022 :		// ������ ����
		case 200023 :		// �䳢����
		case 200024 :		// ������
		case 200025 :		// ���� ���� ������
		case 200026 :		// ������ ���̵�
		case 200027 :		// ���� ���� ������ ���̵�
			strcpy( szFileName, "slot_icon_doll");
			break;

		// Book
		case 200028 :		// �Ǹ��� ����
		case 200029 :		// ��ũ���̴��� ��� ����
		case 200030 :		// ��ũ���̴��� ��� ����
			strcpy( szFileName, "slot_icon_book");
			break;

		// Object
		case 200031 :		// �ູ���� ���ڰ�
		case 200032 :		// ���ֹ��� ���ڰ�
		case 200033 :		// ���
		case 200034 :		// ���ϴ� ������
		case 200035 :		// ������ ����
		case 200036 :		// �μ��� ���
		case 200037 :		// ��� ���
			strcpy( szFileName, "slot_icon_object");
			break;

		// Sword
		case 200038 :		// ��� ŷ�� ����
		case 200039 :		// �ں��� ���� ����
		case 200040 :		// �Ŵ� ���̷����� Į
		case 200041 :		// ���ֹ��� �ý��� Į
		case 200042 :		// ��ġ�� ����
		case 200043 :		// �������� ���� Į
		case 200044 :		// ��ũ���̴��� Į
			strcpy( szFileName, "slot_icon_qsword");
			break;
	}

//	if ( bSmallIcon)
//		strcat( szFileName, "_s");

	strcat(szFileName, ".tga");


	return CCBitmapManager::Get( szFileName);
}


void ZGameInterface::OnResponseServerStatusInfoList( const int nListCount, void* pBlob )
{
}


void ZGameInterface::OnResponseBlockCountryCodeIP( const char* pszBlockCountryCode, const char* pszRoutingURL )
{/*
	if( 0 != pszBlockCountryCode )
	{
		// ���� IP�� ������ Ŭ���̾�Ʈ�� �뺸����� ��.
		// Message�� ��� ������ �߰��ؾ� ��.

		ShowMessage( pszRoutingURL );
	}*/
}


void ZGameInterface::RequestServerStatusListInfo()
{
}


void ZGameInterface::OnDisconnectMsg( const DWORD dwMsgID )
{
}

void ZGameInterface::ShowDisconnectMsg( DWORD errStrID, DWORD delayTime )
{
}


void ZGameInterface::OnAnnounceDeleteClan( const string& strAnnounce )
{
}



// ���̽� ���� ����
void ZGameInterface::OnVoiceSound()
{
	DWORD dwCurrTime = timeGetTime();

	// ������ �ð��� �ƴ��� �˻�
	if ( dwCurrTime < m_dwVoiceTime)
		return;

	m_szCurrVoice[ 0] = 0;

	// ����� ���� ���
	if ( m_szNextVoice[ 0] == 0)
		return;

	ZApplication::GetSoundEngine()->PlaySound( m_szNextVoice);
	m_dwVoiceTime = dwCurrTime + m_dwNextVoiceTime;

	strcpy( m_szCurrVoice, m_szNextVoice);
	m_szNextVoice[ 0] = 0;
	m_dwNextVoiceTime = 0;
}

void ZGameInterface::PlayVoiceSound( char* pszSoundName, DWORD time)
{
	if ( !Z_AUDIO_NARRATIONSOUND)
		return;


	if ( strcmp( pszSoundName, m_szCurrVoice) != 0)
	{
		sprintf( m_szNextVoice, pszSoundName);
		m_dwNextVoiceTime = time;
	}


	// �÷��� ���� ���尡 ������ �ٷ� �÷����Ѵ�.
	if ( timeGetTime() > m_dwVoiceTime)
		OnVoiceSound();
}


void ZGameInterface::OnRequestGameguardAuth( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 )
{
#ifdef _GAMEGUARD
//	GetZGameguard().SendToCallback( dwIndex, dwValue1, dwValue2, dwValue3 );
#endif
}


void ZGameInterface::OnSendGambleItemList( void* pGItemArray, const DWORD dwCount )
{/*
	ZGetGambleItemDefineMgr().Release();

	CCTD_DBGambleItmeNode*	pGItem;
	ZGambleItemDefine*		pZGItem;


	for( DWORD i = 0; i < dwCount; ++i )
	{
		pGItem = (CCTD_DBGambleItmeNode*)CCGetBlobArrayElement( pGItemArray, i );
		if( NULL ==  pGItem )
			return;

		pZGItem = new ZGambleItemDefine( pGItem->nItemID
			, pGItem->szName
			, pGItem->szDesc
			, pGItem->nBuyPrice
			, pGItem->bIsCash);
		if( NULL == pZGItem )
			return;

		if( !ZGetGambleItemDefineMgr().AddGambleItemDefine(pZGItem) )
		{
			_ASSERT( 0 );
		}
	}*/
}

void ZGameInterface::SetAgentPing(DWORD nIP, DWORD nTimeStamp)
{/*
	ZServerView* pServerList = (ZServerView*)m_IDLResource.FindWidget( "SelectedServer");
	pServerList->SetAgentPing(nIP, nTimeStamp);*/
}

void ZGameInterface::MultiplySize( float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight )
{
	CCWidget::MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);

	if (m_pMsgBox)
		m_pMsgBox->MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);
	if (m_pConfirmMsgBox)
		m_pConfirmMsgBox->MultiplySize(byIDLWidth, byIDLHeight, byCurrWidth, byCurrHeight);
}

/*void ZGameInterface::SetDuelTournamentCharacterList(CCDUELTOURNAMENTTYPE nType, const vector<DTPlayerInfo>& vecDTPlayerInfo)
{/*
	m_eDuelTournamentType = nType;

	m_vecDTPlayerInfo.clear();
	m_vecDTPlayerInfo = vecDTPlayerInfo;*

}
*/