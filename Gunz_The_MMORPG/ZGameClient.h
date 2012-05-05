#ifndef _ZGAMECLIENT_H
#define _ZGAMECLIENT_H

#include "ZPrerequisites.h"
#include "CCMatchClient.h"
#include "CCSharedCommandTable.h"
#include "CCMatchStage.h"
#include "ZChannelRule.h"
#include "ZGame.h"
#include "ZNetAgreementBuilder.h"
#include "CCEmblemMgr.h"
#include "ZCommandUDPHackShield.h"

typedef bool(ZONCOMMANDCALLBACK)(CCCommand* pCommand);
class MListBox;
class ZCharacterViewList;
class UPnP;


class ZGameClient : public CCMatchClient
{
protected:
	char				m_szChannel[256];
	char				m_szChannelRule[128];
	char				m_szStageName[256];
	char				m_szChatRoomInvited[64];
	unsigned int		m_nRoomNo;
	int					m_nStageCursor;
	bool				m_bLadderGame;
	CCCHANNEL_TYPE		m_CurrentChannelType;
	bool				m_bEnableInterface;
	char				m_szVoteText[256];

private:
	unsigned long int		m_nPrevClockRequestAttribute;
	
	int						m_nBridgePeerCount;
	unsigned long int		m_tmLastBridgePeer;

	int						m_nCountdown;
	unsigned long int		m_tmLastCountdown;

	int						m_nRequestID;			///< Ŭ�� ������� ���̴� RequestID
	CCUID					m_uidRequestPlayer;		///< Ŭ�� �������� ��û�� 
	ZNetAgreementBuilder	m_AgreementBuilder;		///< ���Ǻ���
	CCMatchProposalMode		m_nProposalMode;

	// ��ǥ�������̽� ����
	bool					m_bVoteInProgress;		///< ��ǥ�� �������ΰ�
	bool					m_bCanVote;				///< ��ǥ���� �ִ°�

	// Ŭ�� �ں� ����
	CCEmblemMgr				m_EmblemMgr;

	// ��Ÿ�ɼ� ����
	bool					m_bPriorityBoost;		///< �����ִ�ȭ
	bool					m_bRejectNormalChat;	///< �Ϲ�ê ���
	bool					m_bRejectTeamChat;		///< ��ê ���
	bool					m_bRejectClanChat;		///< Ŭ��ê ���
	bool					m_bRejectWhisper;		///< �Ӹ����
	bool					m_bRejectInvite;		///< �ʴ����
	
public:
	// �����ʸ�Ʈ �� �������� (�α����Ҷ��� 1ȸ ����, ����� ���⿡ ���� ������ ui����)
	struct DTCHARINFO {
		int tournamentPoint, wins, losses, ranking, winners, lastWeekGrade;
	};
	const DTCHARINFO* GetMyDuelTournamentCharInfo()		{ return &m_dtCharInfo; }
	const DTCHARINFO* GetMyDuelTournamentCharInfoPrev() { return &m_dtCharInfoPrev; }
private:
	DTCHARINFO				m_dtCharInfo, m_dtCharInfoPrev;

protected:
	void SetChannelRuleName(const char* pszName)	{ strcpy(m_szChannelRule, pszName); }
	int GetBridgePeerCount()			{ return m_nBridgePeerCount; }
	void SetBridgePeerCount(int nCount)	{ m_nBridgePeerCount = nCount; }
	void UpdateBridgePeerTime(unsigned long int nClock)	{ m_tmLastBridgePeer = nClock; }
	void StartBridgePeer();

	void UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting, STAGE_STATE nStageState, const CCUID& uidMaster);
	void SetCountdown(int nCountdown)	{ m_nCountdown = nCountdown; m_tmLastCountdown = 0; }
	int GetCountdown()					{ return m_nCountdown; }
	bool Countdown(int nClock) {
		if (nClock - m_tmLastCountdown > 1000) {
			m_nCountdown--;
			m_tmLastCountdown = nClock;
			return true;
		}
		return false;
	}

	static int FindListItem(MListBox* pListBox, const CCUID& uid);

protected:
	ZONCOMMANDCALLBACK*		m_fnOnCommandCallback;
	
	bool					m_bIsBigGlobalClock;	// �۷ι� Ŭ���� ���ú��� �� ũ�� true
	unsigned long int		m_nClockDistance;	///< �۷ι� Ŭ������ �ð���

	CCMatchStageSetting		m_MatchStageSetting;
	bool					m_bForcedEntry;		///< �����ؼ� ������ ����
protected:
	virtual bool OnCommand(CCCommand* pCommand);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockConnect(SOCKET sock);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
	virtual int OnConnected(SOCKET sock, CCUID* pTargetUID, CCUID* pAllocUID, unsigned int nTimeStamp);
	virtual void OnRegisterCommand(CCCommandManager* pCommandManager);
	virtual void OnPrepareCommand(CCCommand* pCommand);	///< Ŀ�ǵ带 ó���ϱ� ����
	virtual int OnResponseMatchLogin(const CCUID& uidServer, int nResult, const char* szServerName, const CCMatchServerMode nServerMode,
									 const char* szAccountID, const CCMatchUserGradeID nUGradeID, const CCMatchPremiumGradeID nPGradeID,
									 const CCUID& uidPlayer, bool bEnabledSurvivalMode, bool bEnabledDuelTournament, unsigned char* szEncryptMsg);	// update sgk 0409
	virtual void OnBridgePeerACK(const CCUID& uidChar, int nCode);
	virtual void OnObjectCache(unsigned int nType, void* pBlob, int nCount);
	virtual void OnAgentError(int nError);

	virtual void OnPrepareRun(void);

protected:
	void OnMatchNotify(unsigned int nMsgID);
//	void OnPeerList(const CCUID& uidStage, void* pBlob, int nCount);
//	void OnAddPeer(const CCUID& uidChar, const char* szIP, const int nPort =				
//		MATCHCLIENT_DEFAULT_UDP_PORT, CCTD_CharInfo* pCharInfo = NULL, int nTeam = 0);
	void OnAnnounce(unsigned int nType, char* szMsg);
	void OnResponseResult(const int nResult);

	void OnChannelResponseJoin(const CCUID& uidChannel, CCCHANNEL_TYPE nChannelType, const char* szChannelName, bool bEnableInterface);
	void OnChannelChat(const CCUID& uidChannel, char* szName, char* szChat,int nGrade);
	void OnChannelList(void* pBlob, int nCount);
	void OnChannelResponseRule(const CCUID& uidchannel, const char* pszRuleName);

	void OnLadderPrepare(const CCUID& uidStage, const int nTeam);
	void OnLadderLaunch(const CCUID& uidStage, const char* pszMapName);
	void OnLadderResponseChallenge(const int nResult);

	void OnStageJoin(const CCUID& uidChar, const CCUID& uidStage, unsigned int nRoomNo, char* szStageName);
	void OnStageLeave(const CCUID& uidChar, const CCUID& uidStage);
//	void OnStageEnterBattle(const CCUID& uidChar, const CCUID& uidStage, MCmdEnterBattleParam nParam, CCTD_PeerListNode* pPeerNode);
//	void OnStageLeaveBattle(const CCUID& uidChar, const CCUID& uidStage);
	void OnStageStart(const CCUID& uidChar, const CCUID& uidStage, int nCountdown);
	void OnStageRelayStart();
	void OnStageLaunch(const CCUID& uidStage, const char* pszMapName);
	void OnStageFinishGame(const CCUID& uidStage, const bool bIsRelayMapUnFinish);
	void OnStageMap(const CCUID& uidStage, char* szMapName, bool bIsRelayMap = false);
	void OnStageTeam(const CCUID& uidChar, const CCUID& uidStage, unsigned int nTeam);
	void OnStagePlayerState(const CCUID& uidChar, const CCUID& uidStage, CCMatchObjectStageState nStageState);
	void OnStageMaster(const CCUID& uidStage, const CCUID& uidChar);
	void OnStageChat(const CCUID& uidChar, const CCUID& uidStage, char* szChat);
	void OnStageList(int nPrevStageCount, int nNextStageCount, void* pBlob, int nCount);
	void OnStageRelayMapListUpdate(int nRelayMapType, int nRelayMapRepeatCount, void* pStageRelayMapListBlob);
	void OnStageRelayMapElementUpdate(int nRelayMapType, int nRelayMapRepeatCount);
	void OnResponseFriendList(void* pBlob, int nCount);
	void OnChannelPlayerList(int nTotalPlayerCount, int nPage, void* pBlob, int nCount);
	void OnChannelAllPlayerList(const CCUID& uidChannel, void* pBlob, int nBlobCount);
	void OnResponseStageSetting(const CCUID& uidStage, void* pStageBlob, int nStageCount, void* pCharBlob, 
		                        int nCharCount, STAGE_STATE nStageState, const CCUID& uidMaster);
	void OnResponseRecommandedChannel(const CCUID& uidChannel);
	void OnResponsePeerRelay(const CCUID& uidPeer);
	void OnResponseGameInfo(const CCUID& uidStage, void* pGameInfoBlob, void* pRuleInfoBlob, void* pPlayerInfoBlob);
	void OnResponseCharInfoDetail(void* pBlob);

	void OnLoadingComplete(const CCUID& uidChar, int nPercent);
	void OnForcedEntryToGame();

	void OnUserWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage);
	void OnChatRoomJoin(char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomLeave(char* pszPlayerName, char* pszChatRoomName);
	void OnChatRoomSelectWrite(char* pszChatRoomName);
	void OnChatRoomInvite(char* pszSenderName, char* pszRoomName);
	void OnChatRoomChat(char* pszChatRoomName, char* pszPlayerName, char* pszChat);

	void OnResponseUpdateStageEquipLook( const CCUID& uidPlayer, const int nParts, const int nItemID );

	// ���󰡱� ����.
	void OnFollowResponse( const int nMsgID );
	void OnExpiredRentItem(void* pBlob);	///< �Ⱓ�� ������ ��� ���� ��������

	void OnBirdTest();
protected:
	// Ŭ������
	void OnResponseCreateClan(const int nResult, const int nRequestID);
	void OnResponseAgreedCreateClan(const int nResult);
	void OnClanAskSponsorAgreement(const int nRequestID, const char* szClanName, CCUID& uidMasterObject, const char* szMasterName);
	void OnClanAnswerSponsorAgreement(const int nRequestID, const CCUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer);
	void OnClanResponseCloseClan(const int nResult);
	void OnClanResponseJoinClan(const int nResult);
	void OnClanAskJoinAgreement(const char* szClanName, CCUID& uidClanAdmin, const char* szClanAdmin);
	void OnClanAnswerJoinAgreement(const CCUID& uidClanAdmin, const char* szJoiner, const bool bAnswer);
	void OnClanResponseAgreedJoinClan(const int nResult);
	void OnClanUpdateCharClanInfo(void* pBlob);
	void OnClanResponseLeaveClan(const int nResult);
	void OnClanResponseChangeGrade(const int nResult);
	void OnClanResponseExpelMember(const int nResult);
	void OnClanMsg(const char* szSenderName, const char* szMsg);
	void OnClanMemberList(void* pBlob);
	void OnClanResponseClanInfo(void* pBlob);
	void OnClanStandbyClanList(int nPrevStageCount, int nNextStageCount, void* pBlob);
	void OnClanMemberConnected(const char* szMember);

	// ���ǰ���
	void OnResponseProposal(const int nResult, const CCMatchProposalMode nProposalMode, const int nRequestID);
	void OnAskAgreement(const CCUID& uidProposer, 
		                void* pMemberNamesBlob, 
						const CCMatchProposalMode nProposalMode, 
						const int nRequestID);
	void OnReplyAgreement(const CCUID& uidProposer, 
		                  const CCUID& uidChar, 
						  const char* szReplierName, 
						  const CCMatchProposalMode nProposalMode,
					      const int nRequestID, 
						  const bool bAgreement);
	void ReplyAgreement(const CCUID& uidProposer, const CCMatchProposalMode nProposalMode, bool bAgreement);
protected:
	void OnGameLevelUp(const CCUID& uidChar);
	void OnGameLevelDown(const CCUID& uidChar);

public:				// ���� -_-; ���÷��̿��� �� �̺�Ʈ�� ó���ؾ� ��;
	void OnSpawnWorldItem(void* pBlob);
	void OnObtainWorldItem(const CCUID& uidChar, const int nItemUID);
	void OnRemoveWorldItem(const int nItemUID);

	void OnNotifyActivatedTrapItemList(void* pBlob);

protected:
	// Local
	void OnLocalReport119();
protected:
	void OnAdminAnnounce(const char* szMsg, const ZAdminAnnounceType nType);
public:
	ZGameClient();
	virtual ~ZGameClient();

	void PriorityBoost(bool bBoost);
	bool GetPriorityBoost()				{ return m_bPriorityBoost; }
	bool GetRejectNormalChat()			{ return m_bRejectNormalChat; }
	void SetRejectNormalChat(bool bVal)	{ m_bRejectNormalChat = bVal; }
	bool GetRejectTeamChat()			{ return m_bRejectTeamChat; }
	void SetRejectTeamChat(bool bVal)	{ m_bRejectTeamChat = bVal; }
	bool GetRejectClanChat()			{ return m_bRejectClanChat; }
	void SetRejectClanChat(bool bVal)	{ m_bRejectClanChat = bVal; }
	bool GetRejectWhisper()				{ return m_bRejectWhisper; }
	void SetRejectWhisper(bool bVal)	{ m_bRejectWhisper = bVal; }
	bool GetRejectInvite()				{ return m_bRejectInvite; }
	void SetRejectInvite(bool bVal)		{ m_bRejectInvite = bVal; }

	unsigned long int GetClockCount(void) { return timeGetTime(); }		// ���� Ŭ�� ��ȯ
	unsigned long int GetGlobalClockCount(void);		///< ������ ����� Ŭ���� ��ȯ�Ѵ�.

	virtual void OutputMessage(const char* szMessage, CCZMOMType nType=CCZMDM_GENERAL);
	
	void SetOnCommandCallback(ZONCOMMANDCALLBACK pCallback) { m_fnOnCommandCallback = pCallback;}

//	void InitializeGame();
//	void StartGame();
//	void FinalizeGame();

	void Tick(void);
	void Disconnect()							{ CCMatchClient::Disconnect(m_Server); }
	
	CCMatchStageSetting* GetMatchStageSetting() { return &m_MatchStageSetting; }
	bool IsForcedEntry() { return m_bForcedEntry; }
	bool IsLadderGame() { return m_bLadderGame; }
	bool IsDuelTournamentGame() { return m_MatchStageSetting.GetGameType() == CCMATCH_GAMETYPE_DUELTOURNAMENT; }

	void ReleaseForcedEntry();
	void ClearStageSetting();
public:
	void RequestPrevStageList();
	void RequestNextStageList();
	void RequestStageList(int nPage);
	void StartStageList();
	void StopStageList();
	void StartChannelList(CCCHANNEL_TYPE nChannelType);
	void StopChannelList();
	//void SetChannelType(CCCHANNEL_TYPE type);

	const char*		GetChannelName()		{ return m_szChannel; }
	CCCHANNEL_TYPE	GetChannelType()		{ return m_CurrentChannelType; }
	bool			GetEnableInterface()	{ return m_bEnableInterface; }
	const char*		GetChannelRuleName()	{ return m_szChannelRule; }
	const char*		GetStageName()			{ return m_szStageName; }
	int				GetStageNumber()		{ return m_nRoomNo; }
	
	
	
	const char* GetChatRoomInvited(){ return m_szChatRoomInvited; }
	void SetChatRoomInvited(const char* pszRoomName)	{ strcpy(m_szChatRoomInvited, pszRoomName); }

	bool AmIStageMaster() { return (m_MatchStageSetting.GetMasterUID() == GetPlayerUID()); }

	const char* GetVoteMessage() { return m_szVoteText; }
public:
	// Ŭ�� ����
	void AnswerSponsorAgreement(bool bAnswer);
	void AnswerJoinerAgreement(bool bAnswer);
	void RequestCreateClan(char* szClanName, char** ppMemberCharNames);

	// ���� ����
	void RequestProposal(const CCMatchProposalMode nProposalMode, char** ppReplierCharNames, const int nReplierCount);
	void ReplyAgreement(bool bAgreement);
public:	// ��ǥ����
	bool IsVoteInProgress()				{ return m_bVoteInProgress;	}
	void SetVoteInProgress(bool bVal)	{ m_bVoteInProgress = bVal; }
	bool CanVote()						{ return m_bCanVote; }
	void SetCanVote(bool bVal)			{ m_bCanVote = bVal; }

public:
	void RequestGameSuicide();
	// game ���� �ҷ��ش�
	//void OnGameRoundState(const CCUID& uidStage, int nRoundState, int nRound);
	void OnStageEnterBattle(const CCUID& uidChar, MCmdEnterBattleParam nParam);
public:
	// Validate ������
	int ValidateRequestDeleteChar();
public:
	// Request ������
	void RequestChannelJoin(const CCUID& uidChannel);
	void RequestChannelJoin(const CCCHANNEL_TYPE nChannelType, char* szChannelName);
	void RequestOnLobbyCreated();		// �κ� ���ö� ��û�ϴ� �޽�����
	void RequestOnGameDestroyed();		// ������ �������� ��û�ϴ� �޽�����
protected:
	void OnNotifyCallVote(const char* pszDiscuss, const char* pszArg);
	void OnNotifyVoteResult(const char* pszDiscuss, int nResult);
	void OnVoteAbort( const int nMsgCode );
protected:
	void OnBroadcastClanRenewVictories(const char* pszWinnerClanName, const char* pszLoserClanName, int nVictories);
	void OnBroadcastClanInterruptVictories(const char* pszWinnerClanName, const char* pszLoserClanName, int nVictories);
	void OnBroadcastDuelRenewVictories(const char* pszChampionName, const char* pszChannelName, int nRoomno, int nVictories);
	void OnBroadcastDuelInterruptVictories(const char* pszChampionName, const char* pszInterrupterName, int nVictories);
protected:
	// Emblem ����
	void ProcessEmblem(unsigned int nCLID, unsigned int nChecksum);
	void RequestEmblemURL(unsigned int nCLID);
	void OnClanResponseEmblemURL(unsigned int nCLID, unsigned int nEmblemChecksum, const char* szEmblemURL);
	void OnClanEmblemReady(unsigned int nCLID, const char* szURL);

public:
	CCEmblemMgr *GetEmblemManager() { return &m_EmblemMgr; }	

	// UPnP
protected:
	UPnP *m_pUPnP;

public:
	bool CreateUPnP(unsigned short nUDPPort);
	bool DestroyUPnP();
	UPnP* GetUDPInfo()		{ return m_pUPnP; }

	void ChangeQuestStage();

	void OnRecieveGambleItem( unsigned int nRecvItem, unsigned int nCnt, unsigned int nTime);

	// �ڱ��ڽ��� ���������� MD5 ���� ���ؼ� �װ��� ��ȣȭ�Ѵ�.
    void GetEncryptMD5HashValue(char* szEncryptMD5Value);							// update sgk 0724

protected:
	void OnDuelTournamentPrepare(CCDUELTOURNAMENTTYPE nType, CCUID uidStage, void* pBlobPlayerInfo);
	void OnDuelTournamentLaunch(const CCUID& uidStage, const char* pszMapName);

protected:
	ZCommandUDPHackShield m_UPDCommadHackShield;
public:
	bool IsUDPCommandValidationCheck(int nCommandID);

protected:
	void OnAdminResponseKickPlayer(int nResult);
	void OnAdminResponseBlockPlayer(int nResult);
	void OnAdminResponseMutePlayer(int nResult);
};





bool ZPostCommand(CCCommand* pCmd);

// �� �÷��̾��� uid
#define ZGetMyUID() (ZGetGameClient() ? ZGetGameClient()->GetPlayerUID() : CCUID(0,0))


CCCommand* ZNewCmd(int nID);

unsigned long int ZGetClockDistance(unsigned long int nGlobalClock, unsigned long int nLocalClock);

// Post Command Macro For Convenience
#define ZPOSTCMD0(_ID)									{ CCCommand* pC=ZNewCmd(_ID); ZPostCommand(pC); }
#define ZPOSTCMD1(_ID, _P0)								{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); ZPostCommand(pC); }
#define ZPOSTCMD2(_ID, _P0, _P1)						{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); ZPostCommand(pC); }
#define ZPOSTCMD3(_ID, _P0, _P1, _P2)					{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ZPostCommand(pC); }
#define ZPOSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ZPostCommand(pC); }
#define ZPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)			{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ZPostCommand(pC); }
#define ZPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ZPostCommand(pC); }
#define ZPOSTCMD7(_ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ CCCommand* pC=ZNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); ZPostCommand(pC); }

#define HANDLE_COMMAND(message, fn)    \
	case (message): return fn(pCommand);

bool GetUserInfoUID(CCUID uid,sColor& _color,char* sp_name,CCMatchUserGradeID& gid);


#endif