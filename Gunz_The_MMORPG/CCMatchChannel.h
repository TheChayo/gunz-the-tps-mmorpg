#pragma once

#include <map>
#include <list>
using namespace std;

#include "CCMatchGlobal.h"
#include "CCUID.h"
#include "CCMatchChannel.h"
#include "CCPageArray.h"
#include "CCSmartRefresh.h"
#include "CCMatchChannelRule.h"


#define DEFAULT_CHANNEL_MAXPLAYERS			200
#define DEFAULT_CHANNEL_MAXSTAGES			100
#define MAX_CHANNEL_MAXSTAGES				500
#define NUM_PLAYERLIST_NODE					6
#define CHANNEL_NO_LEVEL					(-1)
#define CHANNELNAME_STRINGRESID_LEN			64		// �����δ� �̺��� ����� ������ �׷� ��찡 ������ �ʵ��� ASSERT�� Ȯ��

class CCMatchObject;
class CCMatchStage;
class CCCommand;


enum CCCHANNEL_TYPE {
	CCCHANNEL_TYPE_PRESET	= 0,		// �Ϲ�ä��
	CCCHANNEL_TYPE_USER		= 1,		// �缳ä��
	CCCHANNEL_TYPE_PRIVATE	= 2,		// ���ä�� - ���� �Ⱦ���
	CCCHANNEL_TYPE_CLAN		= 3,		// Ŭ��ä��
	CCCHANNEL_TYPE_QUEST		= 4,		// ����Ʈä�� 2006-05-23�߰� - by SungE
	CCCHANNEL_TYPE_DUELTOURNAMENT = 5,	// �����ʸ�Ʈä�� 2009-09-21�߰� - by ȫ����
	CCCHANNEL_TYPE_MAX
};


// ä�� ����Ʈ �޶�� ��û�Ҷ� ������ ����ü
struct CCCHANNELLISTNODE {
	CCUID			uidChannel;						// ä�� UID
	short			nNo;							// ä�ι�ȣ
	unsigned char	nPlayers;						// �����ο�
	short			nMaxPlayers;					// �ִ��ο�
	short			nLevelMin;						// �ּҷ���
	short			nLevelMax;						// �ִ뷹��
	char			nChannelType;					// ä��Ÿ��
	char			szChannelName[CHANNELNAME_LEN];	// ä���̸�
	char			szChannelNameStrResId[CHANNELNAME_STRINGRESID_LEN];	// ä���̸��� ��Ʈ�����ҽ� ID (Ŭ���̾�Ʈ���� ����ä���� ������ �̸��� ǥ���ϵ��� �ϱ� ����)
	bool			bIsUseTicket;
	unsigned int	nTicketID;
};

// typedef map<string, CCMatchObject*>			CCObjectStrMap;
typedef map<int, CCMatchStage*>				CCChannelStageMap;
typedef CCPageArray<CCMatchObject*>			CCChannelUserArray;


class CCMatchChannel {
private:
	CCUID			m_uidChannel;
	char			m_szChannelName[CHANNELNAME_LEN];
	char			m_szChannelNameStrResId[CHANNELNAME_STRINGRESID_LEN];
	CCCHANNEL_TYPE	m_nChannelType;
	int				m_nMaxPlayers;
	int				m_nLevelMin;
	int				m_nLevelMax;
	int				m_nMaxStages;
	char			m_szRuleName[CHANNELRULE_LEN];
	CCCHANNEL_RULE	m_nRuleType;
	//bool			m_bNewbieChannel;		// ����ä���� ���� �ʺ��� �� �� �ִ�.
	
	bool			m_bIsUseTicket;			// ����� ��� ����.
	bool			m_bIsTicketChannel;		// ������� ����ϴ� ä������.
    DWORD			m_dwTicketItemID;		// ����� ItemID.
	
	CCUIDRefCache	m_ObjUIDCaches;			// ä����ü �÷��̾��
	CCUIDRefCache	m_ObjUIDLobbyCaches;	// �κ� �ִ� �÷��̾��
//	CCObjectStrMap	m_ObjStrCaches;

	CCMatchStage*	m_pStages[MAX_CHANNEL_MAXSTAGES];
	list<int>		m_UnusedStageIndexList;

	CCChannelUserArray			m_UserArray;
	CCSmartRefresh				m_SmartRefresh;

	unsigned long	m_nChecksum;	// ��Ϲ� ���� ���ſ�
	unsigned long	m_nLastChecksumTick;

	unsigned long	m_nLastTick;
	unsigned long	m_nEmptyPeriod;

	void JoinLobby(const CCUID& uid, const CCMatchObject* pObj);
	void LeaveLobby(const CCUID& uid);
protected:
	inline bool IsChecksumUpdateTime(unsigned long nTick);
	void UpdateChecksum(unsigned long nTick);
	unsigned long GetEmptyPeriod()	{ return m_nEmptyPeriod; }

public:
	bool CheckTick(unsigned long nClock);
	void Tick(unsigned long nClock);

	unsigned long GetChecksum()		{ return m_nChecksum; }
	bool CheckLifePeriod();

public:
	bool Create(const CCUID& uid, const char* pszName, const char* pszRuleName, 
				CCCHANNEL_TYPE nType=CCCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, 
				int nLevelMin=CHANNEL_NO_LEVEL, int nLevelMax=CHANNEL_NO_LEVEL,
				const bool bIsTicketChannel = false, const DWORD dwTicketItemID = 0, const bool bIsUseTicket = false,
				const char* pszNameStrResId = NULL);
	void Destroy();

	const char* GetName()			{ return m_szChannelName; }
	const char* GetNameStringResId(){ return m_szChannelNameStrResId; }
	const char* GetRuleName()		{ return m_szRuleName; }
	CCUID GetUID()					{ return m_uidChannel; }
	CCCHANNEL_TYPE GetChannelType()	{ return m_nChannelType; }
	CCCHANNEL_RULE GetRuleType()		{ return m_nRuleType; }

	int GetMaxPlayers()				{ return m_nMaxPlayers; }
	int GetLevelMin()				{ return m_nLevelMin; }
	int GetLevelMax()				{ return m_nLevelMax; }
	int	GetMaxStages()				{ return m_nMaxStages; }
	size_t GetObjCount()			{ return m_ObjUIDCaches.size(); }
	int GetPlayers();
	CCUIDRefCache::iterator GetObjBegin()		{ return m_ObjUIDCaches.begin(); }
	CCUIDRefCache::iterator GetObjEnd()			{ return m_ObjUIDCaches.end(); }
	CCUIDRefCache::iterator GetLobbyObjBegin()	{ return m_ObjUIDLobbyCaches.begin(); }
	CCUIDRefCache::iterator GetLobbyObjEnd()		{ return m_ObjUIDLobbyCaches.end(); }


	void AddObject(const CCUID& uid, CCMatchObject* pObj);
	void RemoveObject(const CCUID& uid);
public:
	bool AddStage(CCMatchStage* pStage);
	void RemoveStage(CCMatchStage* pStage);
	bool IsEmptyStage(int nIndex);
	CCMatchStage* GetStage(int nIndex);
	int GetPrevStageCount(int nStageIndex);	// nStageIndex�� �������� �ʴ� nStageIndex������ ������� �� ���� 
	int GetNextStageCount(int nStageIndex);	// nStageIndex�� �������� �ʴ� nStageIndex������ ������� �� ���� 
	
	//bool IsNewbieChannel()			{ return m_bNewbieChannel; }

	const bool	IsUseTicket()		{ return m_bIsUseTicket; }
	const bool	IsTicketChannel()	{ return m_bIsTicketChannel; }
	const DWORD GetTicketItemID()	{ return m_dwTicketItemID; }

public:
	CCChannelUserArray* GetUserArray()	{ return &m_UserArray; }

public:
	void SyncPlayerList(CCMatchObject* pObj, int nPage);
};


struct CCMatchClanChannelTicketInfo
{
	CCMatchClanChannelTicketInfo() 
	{
		m_bIsTicketChannel	= false;
		m_dwTicketItemID	= 0;
	}

	bool	m_bIsTicketChannel;
	DWORD	m_dwTicketItemID;
};


class CCMatchChannelMap : public map<CCUID, CCMatchChannel*> {
private:
	CCUID						m_uidGenerate;
	unsigned long				m_nChecksum;
	map<CCUID, CCMatchChannel*>	m_TypesChannelMap[CCCHANNEL_TYPE_MAX];
	CCMatchClanChannelTicketInfo m_ClanChannelTicketInfo;

	void Insert(const CCUID& uid, CCMatchChannel* pChannel)	{	insert(value_type(uid, pChannel));	}
	CCUID UseUID()				{	m_uidGenerate.Increase();	return m_uidGenerate;	}
	
//	void UpdateChecksum(unsigned long nClock);

public:
	CCMatchChannelMap()			{	m_uidGenerate = CCUID(0,0);	m_nChecksum=0; }
	virtual ~CCMatchChannelMap()	{	}
	void Destroy();
	
	CCMatchChannel* Find(const CCUID& uidChannel);
	CCMatchChannel* Find(const CCCHANNEL_TYPE nChannelType, const char* pszChannelName);

	bool Add(const char* pszChannelName, const char* pszRuleName, CCUID* pAllocUID, CCCHANNEL_TYPE nType=CCCHANNEL_TYPE_PRESET, int nMaxPlayers=DEFAULT_CHANNEL_MAXPLAYERS, int nLevelMin=-1, int nLevelMax=-1,
		const bool bIsTicketChannel = false, const DWORD dwTicketItemID = 0, const bool bIsUseTicket = false, const char* pszChannelNameStrResId = NULL);
	bool Remove(const CCUID& uidChannel, CCMatchChannelMap::iterator* pNextItor);
	void Update(unsigned long nClock);

	unsigned long GetChannelListChecksum() { return m_nChecksum; }
	int GetChannelCount(CCCHANNEL_TYPE nChannelType);
	
	map<CCUID, CCMatchChannel*>::iterator GetTypesChannelMapBegin(CCCHANNEL_TYPE nType);
	map<CCUID, CCMatchChannel*>::iterator GetTypesChannelMapEnd(CCCHANNEL_TYPE nType);

	const CCMatchClanChannelTicketInfo& GetClanChannelTicketInfo() const { return m_ClanChannelTicketInfo; }

	void SetIsTicketClanChannel( const bool bIsTicketClanChannel )	{ m_ClanChannelTicketInfo.m_bIsTicketChannel = bIsTicketClanChannel; }
	void SetClanChannelTicketItemID( const DWORD dwTicketItemID )	{ m_ClanChannelTicketInfo.m_dwTicketItemID = dwTicketItemID; }
};