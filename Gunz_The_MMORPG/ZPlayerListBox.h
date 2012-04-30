#pragma once
#include "MListBox.h"
#include "map"
#include "vector"
#include "CCUID.h"
#include "mmatchobject.h"
#include "ZEmblemInterface.h"
#include "ZApplication.h"

using namespace std;

class MBitmap;
class MScrollBar;

enum ePlayerState
{
	PS_LOGOUT = 0,
	PS_FIGHT,
	PS_WAIT,
	PS_LOBBY,
	PS_END,
};

/*
struct sPlayerInfo
{
	int Level;
	char szName[128];
	int state;
};
*/

class ZPlayerListItem : public MListItem {
public:
	ZPlayerListItem() {
		m_PlayerUID = CCUID(0,0);
		m_Grade = MMUG_FREE;
		m_Color = MCOLOR(0xFFCDCDCD);
	}

	void SetColor(MCOLOR c) {
		m_Color = c;
	}

	const MCOLOR GetColor(void) { 
		return m_Color; 
	}

public:

	CCUID				m_PlayerUID;
	CCMatchUserGradeID	m_Grade;		//�̰� �Ⱦ��� �� ����
	MCOLOR				m_Color;


	char			m_szName[MATCHOBJECT_NAME_LENGTH];
	char			m_szClanName[CLAN_NAME_LENGTH];
	char			m_szLevel[128];

};

class ZLobbyPlayerListItem : public ZPlayerListItem{
protected:
	MBitmap* m_pBitmap;
	MBitmap* m_pBmpDTGradeIcon;
//	MBitmap* m_pBitmapEmblem;
	unsigned int m_nClanID;

public:
	ePlayerState	m_nLobbyPlayerState;

public:
	ZLobbyPlayerListItem(const CCUID& puid, MBitmap* pBitmap, unsigned int nClanID, const char* szLevel, const char* szName, 
		const char *szClanName, ePlayerState nLobbyPlayerState,CCMatchUserGradeID Grade, MBitmap* pBmpDTGradeIcon)
	{
		m_pBitmap = pBitmap;
		m_pBmpDTGradeIcon = pBmpDTGradeIcon;
		m_nClanID = nClanID;
		ZGetEmblemInterface()->AddClanInfo(m_nClanID);
//		m_pBitmapEmblem = pBitmapEmblem;
		m_PlayerUID = puid;
		m_nLobbyPlayerState = nLobbyPlayerState;
		if (szLevel) strcpy(m_szLevel, szLevel);
		else m_szLevel[0] = NULL;
		if (szName) strcpy(m_szName, szName);
		else m_szName[0] = NULL;
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;
		m_Grade = Grade;
	}

	virtual ~ZLobbyPlayerListItem() {
		ZGetEmblemInterface()->DeleteClanInfo(m_nClanID);
	}

	ZLobbyPlayerListItem(void)
	{
		m_pBitmap = NULL;
		m_pBmpDTGradeIcon = NULL;
		m_nClanID = 0;
//		m_pBitmapEmblem = NULL;
		m_PlayerUID = CCUID(0,0);
		m_nLobbyPlayerState = (ePlayerState)0;
		m_szLevel[0] = 0;
		m_szName[0] = 0;
		m_szClanName[0] = 0;
		m_Grade = MMUG_FREE;
	}

	virtual const char* GetString(void)
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szLevel;
		else if(i==3) return m_szName;
		else if(i==5) return m_szClanName;
		return NULL;
	}

	virtual MBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		else if (i == 2)
		{
			return m_pBmpDTGradeIcon;
		}
		else if (i == 4)
		{
			if ( strcmp( m_szClanName, "") == 0)
				return NULL;
			else
				return ZGetEmblemInterface()->GetClanEmblem(m_nClanID);
		}
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
};

class ZFriendPlayerListItem : public ZPlayerListItem{
protected:
	char		m_szLocation[128];
	MBitmap*	m_pBitmap;

public:
	ePlayerState	m_nLobbyPlayerState;

public:
	ZFriendPlayerListItem(const CCUID& puid, MBitmap* pBitmap, const char* szName, const char* szClanName,const char* szLocation, 
		ePlayerState nLobbyPlayerState,CCMatchUserGradeID Grade)
	{
		m_pBitmap = pBitmap;
		m_PlayerUID = puid;
		m_nLobbyPlayerState = nLobbyPlayerState;
		if (szName) strcpy(m_szName, szName);
		else m_szName[0] = NULL;
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;

		m_szLevel[0] = NULL;

		if (szLocation)
			strcpy(m_szLocation, szLocation);
		else
			m_szLocation[0] = NULL;

		m_Grade = Grade;
	}

	ZFriendPlayerListItem(void)
	{
		m_pBitmap = NULL;
		m_PlayerUID = CCUID(0,0);
		m_nLobbyPlayerState = (ePlayerState)0;
		m_szName[0] = PS_END;
		m_szLevel[0] = NULL;
		m_szLocation[0] = NULL;

		m_Grade = MMUG_FREE;
	}

	virtual const char* GetString(void)
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szName;
		//else if(i==2) return m_szLocation;
		return NULL;
	}

	virtual MBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
	const char* GetLocation() { return m_szLocation; }
};

class ZClanPlayerListItem : public ZPlayerListItem{
protected:
	MBitmap* m_pBitmap;
	CCMatchClanGrade	m_ClanGrade;

public:
	ePlayerState	m_nLobbyPlayerState;

public:
	ZClanPlayerListItem(const CCUID& puid, MBitmap* pBitmap, const char* szName, const char* szClanName,const char* szLevel, 
		ePlayerState nLobbyPlayerState,CCMatchClanGrade clanGrade)
	{
		m_pBitmap = pBitmap;
		m_PlayerUID = puid;
		m_nLobbyPlayerState = nLobbyPlayerState;
		if (szName)
			strcpy(m_szName, szName);
		else
			m_szName[0] = NULL;
		if (szLevel) strcpy(m_szLevel, szLevel);
		else m_szLevel[0] = NULL;
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;

		m_ClanGrade = clanGrade;
		m_Color = MCOLOR(0xFFCDCDCD);
	}

	ZClanPlayerListItem(void)
	{
		m_pBitmap = NULL;
		m_PlayerUID = CCUID(0,0);
		m_nLobbyPlayerState = (ePlayerState)0;
		m_szName[0] = PS_END;
		m_szClanName[0] = PS_END;
		m_szLevel[0] = 0;
		m_Grade = MMUG_FREE;
		m_ClanGrade = CCG_NONE;
		m_Color = MCOLOR(0xFFCDCDCD);
	}

	virtual const char* GetString(void)
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szName;
		else if(i==3) return m_szClanName;
		return NULL;
	}

	virtual MBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
};

enum eStagePlayerState
{
	SPS_NONE = 0,
	SPS_SHOP,
	SPS_EQUIP,
	SPS_READY,
	SPS_END
};

struct sStagePlayerInfo
{
	int Level;
	char szName[128];
	int state;
	bool isMaster;
	int	nTeam;
};


class ZStagePlayerListItem : public ZPlayerListItem{
public:
	int		m_nTeam;
	bool	m_bEnableObserver;			// ���������� �ƴ���... : ��ȯ�̰� �߰�
	unsigned int m_nClanID;

public:
	MBitmap* m_pBitmap;
	MBitmap* m_pBmpDTGradeIcon;
//	MBitmap* m_pBitmapEmblem;

public:
	ZStagePlayerListItem(const CCUID& puid, MBitmap* pBitmap, unsigned int nClanID, const char* szName, const char* szClanName, 
		const char* szLevel,CCMatchUserGradeID Grade, MBitmap* pBmpDTGradeIcon)
	{
		m_pBitmap = pBitmap;
		m_pBmpDTGradeIcon = pBmpDTGradeIcon;
		m_nClanID = nClanID;
		ZGetEmblemInterface()->AddClanInfo(m_nClanID);
//		m_pBitmapEmblem = pBitmapEmblem;
		m_PlayerUID = puid;
		strcpy(m_szName, szName);
		if(szClanName) strcpy(m_szClanName, szClanName);
		else m_szClanName[0] = NULL;

		strcpy(m_szLevel, szLevel);
		m_nTeam = 0;
		m_Grade = Grade;
	}

	virtual ~ZStagePlayerListItem()
	{
		ZGetEmblemInterface()->DeleteClanInfo(m_nClanID);
	}

	ZStagePlayerListItem(void)
	{
		m_pBitmap = NULL;
		m_pBmpDTGradeIcon = NULL;
		m_nClanID = 0;
//		m_pBitmapEmblem = NULL;
		m_PlayerUID = CCUID(0,0);
		m_szName[0] = 0;
		m_szLevel[0] = 0;
		m_Grade = MMUG_FREE;
	}

	virtual const char* GetString(void)
	{
		return m_szName;
	}

	virtual const char* GetString(int i)
	{
		if(i==1) return m_szLevel;
		else if(i==3) return m_szName;
		else if(i==5) return m_szClanName;
		return NULL;
	}

	virtual MBitmap* GetBitmap(int i)
	{
		if (i == 0) return m_pBitmap;
		else if (i == 2)
		{
			return m_pBmpDTGradeIcon;
		}
		else if (i == 4)
		{
			if ( strcmp( m_szClanName, "") == 0)
				return NULL;
			else
				return ZGetEmblemInterface()->GetClanEmblem(m_nClanID);
		}
		return NULL;
	}

	CCUID& GetUID() { return m_PlayerUID; }
};

class ZPlayerListBoxLook : public MListBoxLook
{
public:
	virtual void OnItemDraw2(MDrawContext* pDC, MRECT& r, const char* szText, MCOLOR color, bool bSelected, bool bFocus, int nAdjustWidth = 0);
	virtual void OnItemDraw2(MDrawContext* pDC, MRECT& r, MBitmap* pBitmap, bool bSelected, bool bFocus, int nAdjustWidth);
	virtual void OnDraw(MListBox* pListBox, MDrawContext* pDC);

	virtual MRECT GetClientRect(MListBox* pListBox, MRECT& r);
};

class ZPlayerListBox : public MListBox
{
public:
	enum PLAYERLISTMODE {
		PLAYERLISTMODE_CHANNEL = 0,
		PLAYERLISTMODE_STAGE ,
		PLAYERLISTMODE_CHANNEL_FRIEND ,
		PLAYERLISTMODE_STAGE_FRIEND ,
		PLAYERLISTMODE_CHANNEL_CLAN ,
		PLAYERLISTMODE_STAGE_CLAN ,

		PLAYERLISTMODE_END,	// = count
	};

private:
//	MBitmap*					m_pBitmap;
//	MBitmap*					m_pBitmapIn;

	MBmButton*					m_pButton;

//	map< CCUID, sPlayerInfo*>	mPlayers;
	vector<CCUID>				mPlayerOrder;

	int				mSelectedPlayer;
	int				mStartToDisplay;
	float			m_SlotWidth;
	float			m_SlotHeight;

	int				m_nOldW;
	PLAYERLISTMODE		m_nMode;


public:

	

protected:
	void SetupButton(const char *szOn, const char *szOff);

public:
	void InitUI(PLAYERLISTMODE nMode);
	void RefreshUI();

	PLAYERLISTMODE GetMode() { return m_nMode; }
	void SetMode(PLAYERLISTMODE mode);

//	void SetBitmap( MBitmap* pBitmap );
//	MBitmap* GetBitmap() {	return m_pBitmap; }
//	MBitmap* GetBitmapIn() { return m_pBitmapIn; }

	// mode PLAYERLISTMODE_CHANNEL
	void AddPlayer(CCUID& puid, ePlayerState state, int  nLevel,char* szName, char* szClanName, unsigned int nClanID, CCMatchUserGradeID nGrade, int duelTournamentGrade );

	// mode PLAYERLISTMODE_STAGE
	void AddPlayer(CCUID& puid, CCMatchObjectStageState state, int nLevel, char* szName, char* szClanName, unsigned int nClanID, bool isMaster,CCMatchTeam nTeam, int duelTournamentGrade);

	// mode PLAYERLISTMODE_CHANNEL_FRIEND, PLAYERLISTMODE_STAGE_FRIEND
	void AddPlayer(ePlayerState state, char* szName, char* szLocation);

	// mode PLAYERLISTMODE_CHANNEL_CLAN
	void AddPlayer(CCUID& puid, ePlayerState state, char* szName, int  nLevel ,CCMatchClanGrade nGrade );

	void DelPlayer(CCUID& puid);
	void UpdatePlayer(CCUID& puid,CCMatchObjectStageState state, char* szName, int  nLevel ,bool isMaster,CCMatchTeam nTeam);
	void UpdatePlayer(CCUID& puid,CCMatchObjectStageState state, bool isMaster,CCMatchTeam nTeam);
	void UpdateEmblem(CCUID& puid);

	void UpdateList(int mode);

	ZPlayerListItem* GetUID(CCUID uid);
	const char* GetPlayerName( int nIndex);

	CCUID GetSelectedPlayerUID();
	void SelectPlayer(CCUID);

	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage);

//	CCUID	m_MyUID;
//	CCUID	m_uidChannel;
	int		m_nTotalPlayerCount;
	int		m_nPage;

	void SetWidth( float t ) { m_SlotWidth = t;	}
	void SetHeight( float t ) { m_SlotHeight = t; }

	PLAYERLISTMODE GetPlayerListMode()		{ return m_nMode; }
public:
	ZPlayerListBox(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~ZPlayerListBox(void);

	DECLARE_LOOK(ZPlayerListBoxLook)
	DECLARE_LOOK_CLIENT()

	void OnSize(int w,int h);

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
	void AddTestItems();
};

////////////////////////////////////////////////////////////////////////////////////////

/*
class ZStagePlayerListBox : public MListBox
{
private:
	MBitmap*		m_pBitmap;

	map< CCUID, sStagePlayerInfo*>	mPlayers;
	vector<CCUID>					mPlayerOrder;

	int				mSelectedPlayer;
	int				mStartToDisplay;
	float			m_SlotWidth;
	float			m_SlotHeight;

	int				m_nOldW;
protected:

public:
	void SetBitmap( MBitmap* pBitmap );
	MBitmap* GetBitmap() {	return m_pBitmap; }

	void AddPlayer(CCMatchObjCache* pCache);
	void AddPlayer(CCUID& puid, CCMatchObjectStageState state, char* szName, int  nLevel ,bool isMaster,CCMatchTeam nTeam);
//	void AddPlayer(CCUID& puid, eStagePlayerState state, char* szName, int  nLevel ,bool isMaster,int nTeam);
	void DelPlayer(CCUID& puid);
	void UpdatePlayer(CCUID& puid,eStagePlayerState state, char* szName, int  nLevel ,bool isMaster,int nTeam);

	ZStagePlayerListItem* GetUID(CCUID uid);

	CCUID	m_MyUID;
	CCUID	m_uidChannel;
	int		m_nTotalPlayerCount;
	int		m_nPage;

	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);


	void SetWidth( float t ) { m_SlotWidth = t;	}
	void SetHeight( float t ) { m_SlotHeight = t; }

	void Resize(float x,float y);

	float OnReSize();

public:
	ZStagePlayerListBox(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~ZStagePlayerListBox(void);

	DECLARE_LOOK(ZPlayerListBoxLook)
	DECLARE_LOOK_CLIENT()
};
*/