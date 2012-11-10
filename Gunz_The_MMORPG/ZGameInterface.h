#ifndef _ZGAMEINTERFACE_H
#define _ZGAMEINTERFACE_H

#include "ZPrerequisites.h"
#include "ZInterface.h"
#include "ZCamera.h"
#include "ZMsgBox.h"
#include "ZLoading.h"
#include "ZInterfaceBackground.h"
//#include "ZChat.h"
//#include "ZQuest.h"
//#include "ZSurvival.h"
//#include "ZGameType.h"
//#include "ZTips.h"
//#include "ZCombatMenu.h"
#include "ZBitmapManager.h"
#include "CCTextArea.h"
#include "CCListBox.h"


#define LOGINSTATE_FADEIN				0
#define LOGINSTATE_SHOWLOGINFRAME		1
#define LOGINSTATE_STANDBY				2
#define LOGINSTATE_LOGINCOMPLETE		3
#define LOGINSTATE_FADEOUT				4

#if defined(_DEBUG) || defined(_RELEASE)
#define _CHATOUTPUT_ENABLE_CHAR_DAMAGE_INFO_				// �� ĳ���� ������ �α�(�׽�Ʈ �۾���)
#endif

class ZLocatorList;
class ZGameInput;
class ZInterface;

enum ZChangeWeaponType;

class CCUserDataListItem : public CCDefaultListItem{
	int m_nUserData;
public:
	CCUserDataListItem(const char* szText, int nUserData)
		: CCDefaultListItem(szText){
			m_nUserData=nUserData;
		}

	int GetUserData() { return m_nUserData; }
};

class ZGameInterface : public ZInterface {
public:
	GunzState			m_nInitialState;
	bool				m_bTeenVersion;
	bool				m_bViewUI;
	bool				m_bTeamPlay;

	bool				m_bLoginTimeout;
	DWORD				m_dwLoginTimeout;

//	int					m_nSelectedCharacter;

	CCTextAreaLook		m_textAreaLookItemDesc;	// �������� ������ ����� TextArea�� Look
	
protected:
	ZEffectManager*		m_pEffectManager;

	GunzState			m_nPreviousState;

	ZCombatInterface*	m_pCombatInterface;
	ZGameInput*			m_pGameInput;
	ZLoading*			m_pLoadingInterface;

	static ZGameClient*	m_spGameClient;
	ZGame*				m_pGame;
//	ZGameTypeManager	m_GameTypeManager;	
//	ZCombatMenu			m_CombatMenu;

	
	bool				m_bShowInterface;

	bool				m_bCursor;					///< Ŀ���� ����� �� �ִ� ����
	LPDIRECT3DSURFACE9	m_pCursorSurface;

	DWORD				m_dwFrameMoveClock;

	ZIDLResource		m_IDLResource;

	GunzState			m_nState;			///< ���� ����
	bool				m_bLogin;			///< Login �Ǿ��°�?

	bool				m_bLoading;
	bool				m_bWaitingArrangedGame;

	CCBitmap				*m_pMapThumbnail;///< �� �����

	ZMsgBox*				m_pMsgBox;
	ZMsgBox*				m_pConfirmMsgBox;
	ZInterfaceBackground*	m_pBackground;

	bool				m_bOnEndOfReplay;		// ���÷��� �������� �÷��̾��� Level Percent�� �ٲ�� ������ ���÷��� ���� ����
	int					m_nLevelPercentCache;	// m_bOnEndOfReplay�� true�� ���� ���� m_nLevelPercentCache�� ���� LevelPercent
												// ���� ������ ������ ������ �ٽ� �����Ѵ�. �� ������ ����... ����� ����. -_-;

	unsigned long int	m_nDrawCount;

	bool			m_bReservedWeapon;
	int				m_nLoginState;
	DWORD			m_dwLoginTimer;
	DWORD			m_dwRefreshTime;
	int				m_nLocServ;

	CCBitmapR2*		m_pRoomListFrame;							// ���ӹ� ����Ʈ ������ �̹���
	CCBitmapR2*		m_pBottomFrame;								// �ϴ� ����â ������ �̹���
	CCBitmapR2*		m_pLoginBG;									// �α��� ��� �̹���
	CCBitmapR2*		m_pLoginPanel;								// �α��� �г� �̹���

	ZBitmapManager<int> m_ItemThumbnailMgr;							// ����/���â/�����ʸ�Ʈ �����߿� ������ ������ ����� �Ŵ���
	//�������� �ñ⸶�� ��� ��ε��ؼ� �޸𸮸� �������� �ʵ��� �ؾ� �մϴ�. (���� ������, ���â ������, �����ʸ�Ʈ ���� ������)

	ZLocatorList*	m_pLocatorList;
	ZLocatorList*	m_pTLocatorList;

	DWORD			m_dwTimeCount;								// ���� ����ð� ī��Ʈ. û�ҳ� �������� ����� ����...
	DWORD			m_dwHourCount;								// ���� ����ð�(hour) ī��Ʈ. û�ҳ� �������� ����� ����...

	DWORD			m_dwVoiceTime;								// ���� ������� ���̽� ���� �ð�
	char			m_szCurrVoice[ 256];						// ���� ����ϴ� ���̽� ���� �̸�
	char			m_szNextVoice[ 256];						// ������ ����� ���̽� ���� �̸�
	DWORD			m_dwNextVoiceTime;							// ������ ����� ���̽� ���� �ð�

	int				m_nRetryCount;

	bool			m_bReservedQuit;
	DWORD			m_dwReservedQuitTimer;

	bool			m_bReserveResetApp;							// for changing language

	static bool		m_bSkipGlobalEvent;

	DWORD			m_MyPort;

	DWORD			m_dErrMaxPalyerDelayTime;
	DWORD			m_bErrMaxPalyer;

	bool			m_bGameFinishLeaveBattle;		// ��Ʋ���� ������ �������� ���� ����

//	list<CCCommand*>	m_listDelayedGameCmd;

// _DUELTOURNAMENT
//	vector<DTPlayerInfo> m_vecDTPlayerInfo;
//	CCDUELTOURNAMENTTYPE m_eDuelTournamentType;

protected:
	static bool		OnGlobalEvent(CCEvent* pEvent);
	virtual bool	OnEvent(CCEvent* pEvent, CCListener* pListener);
	bool			OnDebugEvent(CCEvent* pEvent, CCListener* pListener);
	virtual bool	OnCommand(CCWidget* pWidget, const char* szMessage);
//	static bool		OnCommand(CCCommand* pCommand);

	bool ResizeWidget(const char* szName, int w, int h);
	bool ResizeWidgetRecursive( CCWidget* pWidget, int w, int h);
	void SetListenerWidget(const char* szName, CCListener* pListener);

	void UpdateCursorEnable();
	void UpdateDuelTournamentWaitMsgDots();

//	void LoadCustomBitmap();
	bool InitInterface(const char* szSkinName,ZLoadingProgress *pLoadingProgress = NULL);
	bool InitInterfaceListener();
	void FinalInterface();

	void LoadBitmaps(const char* szDir, const char* szSubDir, ZLoadingProgress *pLoadingProgress);

	void LeaveBattle();

	void OnGreeterCreate();
	void OnGreeterDestroy();

	void OnLoginCreate();
	void OnLoginDestroy();

	void OnDirectLoginCreate();
	void OnDirectLoginDestroy();

	void OnNetmarbleLoginCreate();
	void OnNetmarbleLoginDestroy();

	void OnGameOnLoginCreate();
	void OnGameOnLoginDestroy();

	void OnLobbyCreate();
	void OnLobbyDestroy();

	void OnStageCreate();
	void OnStageDestroy();

	void OnCharSelectionCreate();
	void OnCharSelectionDestroy();

	void OnCharCreationCreate();
	void OnCharCreationDestroy();

	void OnShutdownState();
	void OnUpdateGameMessage();

	void HideAllWidgets();

//	void OnResponseShopItemList( const vector< CCTD_ShopItemInfo*> &vShopItemList  , const vector<CCTD_GambleItemNode*>& vGItemList );
//	void OnResponseCharacterItemList(CCUID* puidEquipItem
//		, CCTD_ItemNode* pItemNodes
//		, int nItemCount
//		, CCTD_GambleItemNode* pGItemNodes
//		, int nGItemCount );

	void OnSendGambleItemList( void* pGItemArray, const DWORD dwCount );

	void OnDrawStateGame(CCDrawContext* pDC);
	void OnDrawStateLogin(CCDrawContext* pDC);
	void OnDrawStateLobbyNStage(CCDrawContext* pDC);
	void OnDrawStateCharSelection(CCDrawContext* pDC);

	void OnResponseServerStatusInfoList( const int nListCount, void* pBlob );
	void OnResponseBlockCountryCodeIP( const char* pszBlockCountryCode, const char* pszRoutingURL );

	// locator����.
	void RequestServerStatusListInfo();

public:
	ZGameInterface(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	~ZGameInterface();

	static bool m_sbRemainClientConnectionForResetApp;	// �� �ٲپ ���ҽ��� �ٽ� �ε��ؾ� �Ҷ� �̰� true�� ����� ��

	bool OnCreate(ZLoadingProgress *pLoadingProgress);
	void OnDestroy();

	void OnInvalidate();
	void OnRestore();

	bool Update(float fElapsed);
	void OnDraw(CCDrawContext *pDC);

	void SetCursorEnable(bool bEnable);
	void OnResetCursor();
	bool IsCursorEnable() { return m_bCursor; }

	bool SetState(GunzState nState);
	GunzState GetState(){ return m_nState; }
	
	void UpdateBlueRedTeam();		// ��ȯ�̰� �߰�

	void ChangeToCharSelection();	///< ĳ���� �������� �̵�

	bool ChangeInterfaceSkin(const char* szNewSkinName);

	/// �ش��ϴ� �̸��� ���������� ���� ������ Visible���¸� �ٲ۴�.
	bool ShowWidget(const char* szName, bool bVisible, bool bModal=false);
	void SetTextWidget(const char* szName, const char* szText);
	void EnableWidget(const char* szName, bool bEnable);

	// ���߿� �����..�켱 ���� �׽�Ʈ�� ����
	void TestChangeParts(int mode);
	void TestChangePartsAll();
	void TestChangeWeapon(RVisualMesh* pVMesh = NULL);
	void TestToggleCharacter();

	void ChangeParts(int mode);
	void ChangeWeapon(ZChangeWeaponType nType);
	
	void Reload();

	void RespawnMyCharacter();	// ȥ���׽�Ʈ�Ҷ� Ŭ���ϸ� �ǻ�Ƴ���.

	void ReserveLeaveStage();	// ������������ ������ �����ð� �帥�� ������
	void ReserveLeaveBattle();	// �������� ������ �����ð� �帥�� ������
	void FinishGame();

	void ReserveResetApp(bool b)	{ m_bReserveResetApp = b; }
	bool IsReservedResetApp()		{ return m_bReserveResetApp; }

	void SaveScreenShot();

	void ShowMessage(const char* szText, CCListener* pCustomListenter=NULL, int iMessageID=0);
	void ShowConfirmMessage(const char* szText, CCListener* pCustomListenter=NULL);
	void ShowMessage(int iMessageID);
	void ShowErrorMessage(int nErrorID);
	void ShowErrorMessage(const char* szErrorMsg, int nErrorID);
	
	void ShowInterface(bool bShowInterface);
	bool IsShowInterface() { return m_bShowInterface; }

	void SetTeenVersion(bool bt) { m_bTeenVersion = bt; }
	bool GetTeenVersion() { return m_bTeenVersion; }

	void OnCharSelect();


	// GunzState�� ���� Create/Destroy �ڵ鷯
	bool OnGameCreate();
	void OnGameDestroy();
	void OnGameUpdate(float fElapsed);


	// �κ� UI ����
	void OnArrangedTeamGameUI(bool bFinding);
	void OnDuelTournamentGameUI(bool bWaiting);

	void InitLobbyUIByChannelType();

	void InitLadderUI(bool bLadderEnable);
	void InitClanLobbyUI(bool bClanBattleEnable);
	void InitDuelTournamentLobbyUI(bool bEnableDuelTournamentUI);
//	void InitChannelFrame(CCCHANNEL_TYPE nChannelType);

//	bool InitLocatorList( CCZFileSystem* pFileSystem, const char* pszLocatorList );

	// �������� UI ����
	void SetMapThumbnail(const char* szMapName);
	void ClearMapThumbnail();
	void SerializeStageInterface();

	void EnableLobbyInterface(bool bEnable);
	void EnableStageInterface(bool bEnable);
	void ShowPrivateStageJoinFrame(const char* szStageName);

	void SetRoomNoLight( int d );


	// ������ ���
	void ShowEquipmentDialog(bool bShow=true);
	void ShowShopDialog(bool bShow=true);

	// ĳ���� ����
	void ChangeSelectedChar( int nNum);


	// ���÷���
	void ShowReplayDialog( bool bShow);
	void ViewReplay( void);


	void ShowMenu(bool bEnable);
	void Show112Dialog(bool bShow);
	bool IsMenuVisible();

	bool OpenMiniMap();
	bool IsMiniMapEnable();

	void RequestQuickJoin();

	void EnableCharSelectionInterface(bool bEnable);

public:


	// Ŭ�������̳� �����������ʴ븦 ������ �ִ� �����ΰ� ?
	bool IsReadyToPropose();

	// ���÷���
	void OnReplay();

	// XTrap
	void OnRequestXTrapSeedKey(unsigned char *pComBuf);			// add sgk 0402

	void OnDisconnectMsg( const DWORD dwMsgID );
	void ShowDisconnectMsg( DWORD errStrID, DWORD delayTime );

	void OnAnnounceDeleteClan( const string& strAnnounce );

	// ����Ʈ ������ ������ ��Ʈ�� ���(���µ� ������ ������ �ѵ��� ���... -_-;)
	CCBitmap* GetQuestItemIcon( int nItemID, bool bSmallIcon);

	// ZActionKey �Է��� GlobalEvent ����ȭ
	static bool CheckSkipGlobalEvent() { return m_bSkipGlobalEvent; }
	void SetSkipGlobalEvent(bool bSkip) { m_bSkipGlobalEvent = bSkip; }

	// ���̽� ���� ���
	void OnVoiceSound();
	void PlayVoiceSound( char* pszSoundName, DWORD time=0);

	void SetAgentPing(DWORD nIP, DWORD nTimeStamp);

	void OnRequestGameguardAuth( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 );

	void SetErrMaxPlayerDelayTime(DWORD dDelayTime) { m_dErrMaxPalyerDelayTime = dDelayTime; }
	DWORD GetErrMaxPlayerDelayTime() { return m_dErrMaxPalyerDelayTime; }
	void SetErrMaxPlayer(bool bErrMaxPalyer) { m_bErrMaxPalyer = bErrMaxPalyer; }
	bool IsErrMaxPlayer() { return m_bErrMaxPalyer == 0 ? false : true; }

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

// _DUELTOURNAMENT
//	void SetDuelTournamentCharacterList(CCDUELTOURNAMENTTYPE nType, const vector<DTPlayerInfo>& vecDTPlayerInfo);
//	const vector<DTPlayerInfo>& GetVectorDTPlayerInfo()	{ return m_vecDTPlayerInfo; }
//	void SetDuelTournamantType(CCDUELTOURNAMENTTYPE eType)		{ m_eDuelTournamentType = eType; }
//	CCDUELTOURNAMENTTYPE GetDuelTournamentType()		{ return m_eDuelTournamentType; }

//	void UpdateDuelTournamantMyCharInfoUI();
//	void UpdateDuelTournamantMyCharInfoPreviousUI();

// �����̸�
//	bool GetIsGameFinishLeaveBattle()			{ return m_bGameFinishLeaveBattle; }

	// �ٱ����� �������� �������̽��� (__forceinline �� dll-injection �� ���ظ� ���� ��)
	__forceinline ZGameClient* GetGameClient()			{ return m_spGameClient; }
	__forceinline ZGame* GetGame()						{ return m_pGame; }
	__forceinline ZCombatInterface* GetCombatInterface() { return m_pCombatInterface; }
//	__forceinline ZGameTypeManager* GetGameTypeManager()		{ return &m_GameTypeManager; }

	ZEffectManager* GetEffectManager()			{ return m_pEffectManager; }
	void SetGameClient(ZGameClient* pGameClient){ m_spGameClient = pGameClient; }
	
	ZIDLResource* GetIDLResource()			{ return &m_IDLResource; }
//	ZCombatMenu*	 GetCombatMenu()			{ return &m_CombatMenu; }

	ZBitmapManager<int>* GetItemThumbnailMgr()		{ return &m_ItemThumbnailMgr; }
};

#define BEGIN_WIDGETLIST(_ITEM, _IDLRESPTR, _CLASS, _INSTANCE)								\
{																							\
	CCWidgetList WidgetList;																	\
	(_IDLRESPTR)->FindWidgets(WidgetList, _ITEM);											\
	for (CCWidgetList::iterator itor = WidgetList.begin(); itor != WidgetList.end(); ++itor) \
{																							\
	if ((*itor) != NULL)																	\
{																							\
	_CLASS _INSTANCE = ((_CLASS)(*itor));

#define END_WIDGETLIST()		}}}


#define DEFAULT_INTERFACE_SKIN "Default"


#define WM_CHANGE_GAMESTATE		(WM_USER + 25)
void ZChangeGameState(GunzState state);		/// �����忡 �����ϱ� ���ؼ��� ����


inline void GetDuelTournamentGradeIconFileName(char* out_sz, int grade)
{
	sprintf(out_sz, "dt_grade%d.png", grade);
}

char* GetItemSlotName( const char* szName, int nItem);
bool SetWidgetToolTipText(char* szWidget,const char* szToolTipText, CCAlignmentMode mam=CCD_LEFT|CCD_TOP);

#endif