#include "stdafx.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"
#include "CCErrorTable.h"
#include "CCBlobArray.h"
#include "CCObject.h"
#include "CCMatchObject.h"
#include "CCMatchItem.h"
#include "CCAgentObject.h"
#include "CCMatchNotify.h"
#include "Msg.h"
#include "CCMatchObjCache.h"
#include "CCMatchStage.h"
#include "CCMatchTransDataType.h"
#include "MMatchFormula.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCMatchShop.h"
#include "CCMatchTransDataType.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "MMatchStatus.h"
#include "MAsyncDBJob.h"
#include "CCMatchTransDataType.h"
#include "CCUtil.h"

void CopyChannelPlayerListNodeForTrans(MTD_ChannelPlayerListNode* pDest, CCMatchObject* pSrcObject)
{
	pDest->uidPlayer = pSrcObject->GetUID();
	strcpy(pDest->szName, pSrcObject->GetCharInfo()->m_szName);
	strcpy(pDest->szClanName, pSrcObject->GetCharInfo()->m_ClanInfo.m_szClanName);
	pDest->nLevel = (char)pSrcObject->GetCharInfo()->m_nLevel;
	pDest->nPlace = pSrcObject->GetPlace();
	pDest->nGrade = (unsigned char)pSrcObject->GetAccountInfo()->m_nUGrade;
	pDest->nPlayerFlags = pSrcObject->GetPlayerFlags();
	pDest->nCLID = pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID;

	CCMatchObjectDuelTournamentCharInfo* pDTCharInfo = pSrcObject->GetDuelTournamentCharInfo();
	pDest->nDTLastWeekGrade = pDTCharInfo ? pDTCharInfo->GetLastWeekGrade() : 0;

	MMatchClan* pClan = CCMatchServer::GetInstance()->GetClanMap()->GetClan(pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan)
		pDest->nEmblemChecksum = pClan->GetEmblemChecksum();
	else
		pDest->nEmblemChecksum = 0;
}


MMatchChannel* CCMatchServer::FindChannel(const CCUID& uidChannel)
{
	return m_ChannelMap.Find(uidChannel);
}

MMatchChannel* CCMatchServer::FindChannel(const MCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	return m_ChannelMap.Find(nChannelType, pszChannelName);
}


bool CCMatchServer::ChannelAdd(const char* pszChannelName, const char* pszRuleName, CCUID* pAllocUID, MCHANNEL_TYPE nType, int nMaxPlayers, int nLevelMin, int nLevelMax,
							  const bool bIsTicketChannel, const DWORD dwTicketItemID, const bool bIsUseTicket, const char* pszChannelNameStrResId)
{
	return m_ChannelMap.Add(pszChannelName, pszRuleName, pAllocUID, nType, nMaxPlayers, nLevelMin, nLevelMax, bIsTicketChannel, dwTicketItemID, bIsUseTicket, pszChannelNameStrResId);
}

bool CCMatchServer::ChannelJoin(const CCUID& uidPlayer, const MCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	if ((nChannelType < 0) || (nChannelType >= MCHANNEL_TYPE_MAX)) return false;

	int nChannelNameLen = (int)strlen(pszChannelName);
	if ((nChannelNameLen >= CHANNELNAME_LEN) || (nChannelNameLen <= 0)) return false;


	CCUID uidChannel = CCUID(0,0);

	// ���� �̸��� ä���� �����ϴ��� �˻��Ѵ�.
	// ���� �ִٸ� �� ä�η� �ٰ� ����.
	// ������ ä�� ����.
	MMatchChannel* pChannel = FindChannel(nChannelType, pszChannelName);
	
	if (pChannel == NULL)
	{
		// ������ ä���̸� ä���� ���� �� ����.
		if (nChannelType == MCHANNEL_TYPE_PRESET) 
			return false;

		if( nChannelType == MCHANNEL_TYPE_DUELTOURNAMENT )
			return false;

		//bool bbadf = GetChannelMap()->GetClanChannelTicketInfo().m_bIsTicketChannel; //debug
/*
		// ������� ����ϰ�, Ŭ��ä���� ���鶧�� ������ ������� �ִ��� �˻縦 ����� �Ѵ�.
		if( MGetServerConfig()->IsUseTicket() && 
			MCHANNEL_TYPE_CLAN == nChannelType && 
			GetChannelMap()->GetClanChannelTicketInfo().m_bIsTicketChannel )
		{
			CCMatchObject* pObj = GetObject( uidPlayer );
			if( 0 != pObj )
			{
				// ���� ���� �˻� �� �ڰ� �̴��̸� �Ϲ� ä�η� �̵��� ����� ��� �Ѵ�.
				// ���� ���¿��� �׳� ���� ��Ű�� ������ ������ ���Ҽ��� �ִ�. - by SungE

				if( !pObj->GetCharInfo()->m_ItemList.IsHave( 
					GetChannelMap()->GetClanChannelTicketInfo().m_dwTicketItemID) )
				{
					// ������� �������� �Ϲ� ä�η� �̵�������� �Ѵ�.

					RouteResponseToListener(pObj, MC_MATCH_RESPONSE_RESULT, MERR_CANNOT_JOIN_NEED_TICKET);

					const CCUID& uidChannel = FindFreeChannel( uidPlayer );
					if( CCUID(0, 0) == uidChannel )
					{
						ASSERT( 0 && "�� �� �ִ� ä���� ã�� ������.");
						cclog( "CCMatchServer_Channel.cpp - ChannelJoin : Can't find free channel.\n" );
						return false;
					}

					return ChannelJoin( uidPlayer, uidChannel );
				}
			}
			else
			{
				// ������ ����. 
				// �׳� ���� ���Ḧ ��Ų��.

				return false;
			}
		}
*/
		if (!ChannelAdd(pszChannelName, GetDefaultChannelRuleName(), &uidChannel, nChannelType)) 
			return false;
	}
	else
	{
		uidChannel = pChannel->GetUID();
	}
	
	return ChannelJoin(uidPlayer, uidChannel);
}

bool CCMatchServer::ChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	bool bEnableInterface = true;
	CCUID uidChannelTmp = uidChannel;

	MMatchChannel* pChannel = FindChannel(uidChannelTmp);
	if (pChannel == NULL) return false;

	// ������� �ʿ��� ä�ο����� ������ ������� �ִ��� �˻縦 ����� �Ѵ�.
	if ( MGetServerConfig()->IsUseTicket())
	{
		bool bCheckTicket = false;

		CCMatchObject* pObj = GetObject(uidPlayer);
		if ( !pObj)	return false;

		if ( MGetServerConfig()->GetServerMode() == MSM_NORMAL)	{ // �Ϲ� �����϶�
			// ����/�缳/Ŭ�� ä���̸� �κ� �������̽��� disable ��Ų��.
			// �� �ܿ��� Ƽ�� ä���̸� Ƽ�� �˻縸 ���ش�.
			if ( stricmp( pChannel->GetRuleName() , MCHANNEL_RULE_NOVICE_STR) == 0) bEnableInterface = false;			
			else if ( pChannel->IsTicketChannel())									bCheckTicket = true;
		} else if ( (MGetServerConfig()->GetServerMode() == MSM_CLAN) || (MGetServerConfig()->GetServerMode() == MSM_TEST)) { // Ŭ�� �����϶�
			// Ŭ��/�缳 ä���̸� Ƽ�� �˻��Ѵ�.
			// ���� ä���̸� �κ� �������̽��� disable ��Ų��.
			// �� �ܿ��� Ƽ�� ä���̸� Ƽ�� �˻縸 ���ش�.
			if ( (pChannel->GetChannelType() == MCHANNEL_TYPE_CLAN) || (pChannel->GetChannelType() == MCHANNEL_TYPE_USER)) bCheckTicket = true;			
			else if ( stricmp( pChannel->GetRuleName() , MCHANNEL_RULE_NOVICE_STR) == 0)
				bEnableInterface = false;			
			else if ( pChannel->IsTicketChannel())
				bCheckTicket = true;
		} else { // �׿� �����϶�
			// ����/�缳/Ŭ�� ä���̸� �κ� �������̽��� disable ��Ų��.
			// �� �ܿ��� Ƽ�� ä���̸� Ƽ�� �˻縸 ���ش�.
			if ( stricmp( pChannel->GetRuleName() , MCHANNEL_RULE_NOVICE_STR) == 0) bEnableInterface = false;			
			else if ( pChannel->IsTicketChannel())									bCheckTicket = true;
		}


		// ����϶�...
		if ( IsAdminGrade( pObj)) {
			bCheckTicket		= false;
			bEnableInterface	= true;
		}

		if ( bCheckTicket) { // Ƽ�� �˻�
			// ���� ���� �˻� �� �ڰ� �̴��̸� �Ϲ� ä�η� �̵��� ����� ��� �Ѵ�.
			// ���� ���¿��� �׳� ���� ��Ű�� ������ ������ ���Ҽ��� �ִ�. - by SungE
			if( !pObj->GetCharInfo()->m_ItemList.IsHave( GetChannelMap()->GetClanChannelTicketInfo().m_dwTicketItemID))	{
				
				// ���� �Ұ� �޽���
				if ( pObj->GetPlace() == MMP_LOBBY)
					RouteResponseToListener( pObj, MC_MATCH_RESPONSE_RESULT, MERR_CANNOT_JOIN_NEED_TICKET);


				// ������� �������� �Ϲ� ä�η� �̵�������� �Ѵ�.
				const CCUID& uidFreeChannel = FindFreeChannel( uidPlayer);
				
				if( CCUID(0, 0) == uidChannel) {
					ASSERT( 0 && "�� �� �ִ� ä���� ã�� ������.");
					cclog( "CCMatchServer_Channel.cpp - ChannelJoin : Can't find free channel.\n" );
					return false;
				}

				uidChannelTmp		= uidFreeChannel;				
				bEnableInterface	= false;
			}
		}
	}

	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (pObj == NULL) return false;

	const int ret = ValidateChannelJoin(uidPlayer, uidChannelTmp);
	if (ret != MOK) {
		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_RESULT, ret);
		return false;
	}

	// Leave Old Channel
	MMatchChannel* pOldChannel = FindChannel(pObj->GetChannelUID());
	if (pOldChannel) {
		pOldChannel->RemoveObject(uidPlayer);
	}

	// Join
	pObj->SetChannelUID(uidChannelTmp);
	pObj->SetLadderChallenging(false);
	pObj->SetPlace(MMP_LOBBY);
	pObj->SetStageListTransfer(true);	// turn on Auto refresh stage list
	pObj->SetStageCursor(0);

	pChannel = FindChannel(uidChannelTmp);
	if (pChannel == NULL) return false;

	pChannel->AddObject(uidPlayer, pObj);
	ResponseChannelJoin(uidPlayer, uidChannelTmp, (int)pChannel->GetChannelType(), pChannel->GetNameStringResId(), bEnableInterface);
	ResponseChannelRule(uidPlayer, uidChannelTmp);	// Channel ��Ģ�� �����ش�.		

	
	if( pChannel->GetRuleType() != MCHANNEL_RULE_DUELTOURNAMENT ) {
		// ��� ��ʸ�Ʈ�� �ƴ� ���, Stage List�� ������.
		StageList(uidPlayer, 0, false);		
	} else {
		// ��� ��ʸ�Ʈ�� ���, �ڽ��� ���� �� ��ŷ ������ �����ش�.
		// ResponseDuelTournamentCharInfo(uidPlayer);		
	}

	ChannelResponsePlayerList(uidPlayer, uidChannelTmp, 0);
	return true;
}

void CCMatchServer::ResponseChannelJoin(CCUID uidPlayer, CCUID uidChannel, int nChannelType
									   , const char* szChannelStrResId, bool bEnableInterface)
{
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchChannel *pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_RESPONSE_JOIN), CCUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidChannel));
	pNew->AddParameter(new MCommandParameterInt(nChannelType));

	if (szChannelStrResId[0] != 0) {
		pNew->AddParameter(new MCommandParameterString((char*)szChannelStrResId));	// ���� ä���̸� Ŭ�󿡼� ���� ���� ������ �� �ֵ��� ��Ʈ�����ҽ�ID�� �ѱ��
	} else {
		pNew->AddParameter(new MCommandParameterString((char*)pChannel->GetName()));
	}
	
	pNew->AddParameter(new MCommandParameterBool(bEnableInterface));
	RouteToListener(pObj, pNew);
}

bool CCMatchServer::ChannelLeave(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return false;
	pChannel->RemoveObject(uidPlayer);

	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (pObj == NULL) return false;

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_LEAVE),CCUID(0,0),m_This);
	pNew->AddParameter(new MCommandParameterUID(uidPlayer));
	pNew->AddParameter(new MCommandParameterUID(pChannel->GetUID()));
	RouteToListener(pObj, pNew);

	if (pObj) 
	{
		pObj->SetChannelUID(CCUID(0,0));
		pObj->SetPlace(MMP_OUTSIDE);
		pObj->SetStageListTransfer(false);	// turn off Auto refresh stage list
	}
	return true;
}

/*
// RAONHAJE �ӽ��ڵ�
#include "CMLexicalAnalyzer.h"
bool StageGo(CCMatchServer* pServer, const CCUID& uidPlayer, char* pszChat)
{
	CCMatchObject* pChar = pServer->GetObject(uidPlayer);
	if (pChar == NULL)	return false;
	if (pChar->GetPlace() != MMP_LOBBY) return false;
	MMatchChannel* pChannel = pServer->FindChannel(pChar->GetChannelUID());
	if (pChannel == NULL) return false;

	bool bResult = false;
	CMLexicalAnalyzer lex;
	lex.Create(pszChat);

	if (lex.GetCount() >= 1) {
		char* pszCmd = lex.GetByStr(0);
		if (pszCmd) {
			if (stricmp(pszCmd, "/go") == 0) {
				if (lex.GetCount() >= 2) {
					char* pszTarget = lex.GetByStr(1);
					if (pszTarget) {
						int nRoomNo = atoi(pszTarget);
						CCMatchStage* pStage = pChannel->GetStage(nRoomNo-1);
						if (pStage) {
							//pServer->StageJoin(uidPlayer, pStage->GetUID());
							MCommand* pNew = pServer->CreateCommand(MC_MATCH_REQUEST_STAGE_JOIN, pServer->GetUID());
							pNew->AddParameter(new MCommandParameterUID(uidPlayer));
							pNew->AddParameter(new MCommandParameterUID(pStage->GetUID()));
							pServer->Post(pNew);
							bResult = true;
						}
					}
				}
			}	// go
		}
	}

	lex.Destroy();
	return bResult;
}
*/

bool CCMatchServer::ChannelChat(const CCUID& uidPlayer, const CCUID& uidChannel, char* pszChat)
{
	if( 0 == strlen(pszChat) ) return false;
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return false;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) return false;
	if (pObj->GetAccountInfo()->m_nUGrade == MMUG_CHAT_LIMITED) return false;

	int nGrade = (int) pObj->GetAccountInfo()->m_nUGrade;

/*
	// RAONHAJE : GO �ӽ��ڵ�
	if (pszChat[0] == '/')
		if (StageGo(this, uidPlayer, pszChat))
			return true;
*/
	///< ȫ����(2009.08.04)
	///< ���� �ش� ����ڰ� �ִ� Channel�� ������ Channel UID�� �ٸ� ���!
	///< �ٸ� ä�ο��Ե� Msg�� ���� �� �ִ� ������ ���� (��ŷ ���α׷� ����)
	if( uidChannel != pObj->GetChannelUID() )
	{
		//LOG(LOG_FILE,"CCMatchServer::ChannelChat - Different Channel(S:%d, P:%d)", uidChannel, pObj->GetChannelUID());
		return false;
	}

	CCUID uidStage = pObj->GetStageUID();
	if( uidStage != CCUID(0, 0) )
	{
		//LOG(LOG_FILE,"CCMatchServer::ChannelChat - Player In Stage(S:%d), Not Lobby", uidStage);
		return false;
	}

	if( pObj->GetAccountPenaltyInfo()->IsBlock(MPC_CHAT_BLOCK) ) {
		return false;
	}

	///< ä�� �޼����� ���踦 �õ��� ���, '���' ���	
	if( pObj->IsChatBanUser() == true )	return false;
	if( pObj->CheckChatFlooding() )
	{
		pObj->SetChatBanUser();
		
		MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_DUMB_CHAT), pObj->GetUID(), m_This);
		Post(pCmd);

		LOG(LOG_FILE, "CCMatchServer::ChannelChat - Set Dumb Player(CCUID:%d%d, Name:%s)", pObj->GetUID().High, pObj->GetUID().Low, pObj->GetName());
		return false;
	}

	///<�������....

	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_CHAT), CCUID(0,0), m_This);
	pCmd->AddParameter(new MCommandParameterUID(uidChannel));
	pCmd->AddParameter(new MCommandParameterString(pObj->GetCharInfo()->m_szName));
	pCmd->AddParameter(new MCommandParameterString(pszChat));
	pCmd->AddParameter(new MCommandParameterInt(nGrade));

	RouteToChannelLobby(uidChannel, pCmd);
	return true;
}

void CCMatchServer::OnRequestRecommendedChannel(const CCUID& uidComm)
{
	CCUID uidChannel = FindFreeChannel( uidComm );

	if (CCUID(0,0) == uidChannel ) 
	{
		if( !ChannelAdd(GetDefaultChannelName(), GetDefaultChannelRuleName(), &uidChannel, MCHANNEL_TYPE_PRESET) )
		{
			cclog( "Channel Add fail for recommand.\n" );
			return;	// �������� �����ϸ� ����....
		}		
	}

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL),
									uidComm, m_This);
	pNew->AddParameter(new MCommandParameterUID(uidChannel));
	Post(pNew);
}

void CCMatchServer::OnRequestChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	ChannelJoin(uidPlayer, uidChannel);
}

void CCMatchServer::OnRequestChannelJoin(const CCUID& uidPlayer, const MCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	if ((nChannelType < 0) || (nChannelType >= MCHANNEL_TYPE_MAX)) return;

	ChannelJoin(uidPlayer, nChannelType, pszChannelName);
}

void CCMatchServer::OnChannelChat(const CCUID& uidPlayer, const CCUID& uidChannel, char* pszChat)
{
	ChannelChat(uidPlayer, uidChannel, pszChat);
}

void CCMatchServer::OnStartChannelList(const CCUID& uidPlayer, const int nChannelType)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	pObj->SetChannelListTransfer(true, MCHANNEL_TYPE(nChannelType));
}

void CCMatchServer::OnStopChannelList(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	pObj->SetChannelListTransfer(false);

}

void CCMatchServer::ChannelList(const CCUID& uidPlayer, MCHANNEL_TYPE nChannelType)
{
	CCMatchObject* pChar = GetObject(uidPlayer);
	if (! IsEnabledObject(pChar)) return;

	if (pChar->GetPlace() != MMP_LOBBY) return;		// �κ� �ƴϸ� ����
	if ((nChannelType < 0) || (nChannelType >= MCHANNEL_TYPE_MAX)) return;

	// Count Active Channels
	int nChannelCount = (int)m_ChannelMap.GetChannelCount(nChannelType);
	if (nChannelCount <= 0) return;

// ä�θ���Ʈ�� �ִ� 100�������� ����Ʈ�� ������.
#define MAX_CHANNEL_LIST_NODE		100

	nChannelCount = min(nChannelCount, MAX_CHANNEL_LIST_NODE);

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_LIST), CCUID(0,0), m_This);

	void* pChannelArray = MMakeBlobArray(sizeof(MCHANNELLISTNODE), nChannelCount);
	int nIndex=0;
	for (map<CCUID, MMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(nChannelType); 
		itor!=m_ChannelMap.GetTypesChannelMapEnd(nChannelType); itor++) {

		if (nIndex >= nChannelCount) break;

		MMatchChannel* pChannel = (*itor).second;

		MCHANNELLISTNODE* pNode = (MCHANNELLISTNODE*)MGetBlobArrayElement(pChannelArray, nIndex++);
		pNode->uidChannel = pChannel->GetUID();
		pNode->nNo = nIndex;
		pNode->nPlayers = (unsigned char)pChannel->GetObjCount();
		pNode->nMaxPlayers = pChannel->GetMaxPlayers();
		pNode->nChannelType = pChannel->GetChannelType();
		strcpy(pNode->szChannelName, pChannel->GetName());
		strcpy(pNode->szChannelNameStrResId, pChannel->GetNameStringResId());
		pNode->bIsUseTicket = pChannel->IsUseTicket();
		pNode->nTicketID = pChannel->GetTicketItemID();
	}
	pNew->AddParameter(new MCommandParameterBlob(pChannelArray, MGetBlobArraySize(pChannelArray)));
	MEraseBlobArray(pChannelArray);

	RouteToListener(pChar, pNew);
}





//void CCMatchServer::OnChannelRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage)
//{
//	ChannelResponsePlayerList(uidPlayer, uidChannel, nPage);
//}

void CCMatchServer::OnChannelRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage)
{
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

	CCRefreshClientChannelImpl* pImpl = pObj->GetRefreshClientChannelImplement();
	pImpl->SetCategory(nPage);
	pImpl->SetChecksum(0);
	pImpl->Enable(true);
	pChannel->SyncPlayerList(pObj, nPage);
}

void CCMatchServer::ChannelResponsePlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage)
{
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

	int nObjCount = (int)pChannel->GetObjCount();
	int nNodeCount = 0;
	int nPlayerIndex;

	if (nPage < 0) nPage = 0;

	nPlayerIndex = nPage * NUM_PLAYERLIST_NODE;
	if (nPlayerIndex >= nObjCount) 
	{
		nPage = (nObjCount / NUM_PLAYERLIST_NODE);
		nPlayerIndex = nPage * NUM_PLAYERLIST_NODE;
	}

	CCUIDRefCache::iterator FirstItor = pChannel->GetObjBegin();

	for (int i = 0; i < nPlayerIndex; i++) 
	{
		if (FirstItor == pChannel->GetObjEnd()) break;
		FirstItor++;
	}

	nNodeCount = nObjCount - nPlayerIndex;
	if (nNodeCount <= 0) 
	{
		return;
	}
	else if (nNodeCount > NUM_PLAYERLIST_NODE) nNodeCount = NUM_PLAYERLIST_NODE;


	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST), CCUID(0,0), m_This);
	//pNew->AddParameter(new MCommandParameterUID(uidChannel));
	pNew->AddParameter(new MCommandParameterUChar((unsigned char)nObjCount));
	pNew->AddParameter(new MCommandParameterUChar((unsigned char)nPage));

	void* pPlayerArray = MMakeBlobArray(sizeof(MTD_ChannelPlayerListNode), nNodeCount);

	int nArrayIndex=0;
	for (CCUIDRefCache::iterator i=FirstItor; i != pChannel->GetObjEnd(); i++) 
	{
		CCMatchObject* pScanObj = (CCMatchObject*)(*i).second;

		MTD_ChannelPlayerListNode* pNode = (MTD_ChannelPlayerListNode*)MGetBlobArrayElement(pPlayerArray, nArrayIndex++);

		if (IsEnabledObject(pScanObj))
		{
			CopyChannelPlayerListNodeForTrans(pNode, pScanObj);		
		}

		if (nArrayIndex >= nNodeCount) break;
	}

	pNew->AddParameter(new MCommandParameterBlob(pPlayerArray, MGetBlobArraySize(pPlayerArray)));
	MEraseBlobArray(pPlayerArray);
	RouteToListener(pObj, pNew);
}

void CCMatchServer::OnChannelRequestAllPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, unsigned long int nPlaceFilter,
												 unsigned long int nOptions)
{
	ChannelResponseAllPlayerList(uidPlayer, uidChannel, nPlaceFilter, nOptions);
}


void CCMatchServer::ChannelResponseAllPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, unsigned long int nPlaceFilter,
												unsigned long int nOptions)
{
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (! IsEnabledObject(pObj)) return;

	int nNodeCount = 0;

	CCMatchObject* ppTransObjectArray[DEFAULT_CHANNEL_MAXPLAYERS];
	memset(ppTransObjectArray, 0, sizeof(CCMatchObject*) * DEFAULT_CHANNEL_MAXPLAYERS);

	// TransObjectArray�� ������ Object�� �����͸� �����س��´�.
	for (CCUIDRefCache::iterator i=pChannel->GetObjBegin(); i != pChannel->GetObjEnd(); i++) 
	{
		CCMatchObject* pScanObj = (CCMatchObject*)(*i).second;

		if (IsEnabledObject(pScanObj))
		{
			if (CheckBitSet(nPlaceFilter, (pScanObj->GetPlace())))
			{
				bool bScanObjOK = true;
				switch (nOptions)
				{
				case MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_NONCLAN:
					{
						if (pScanObj->GetCharInfo()->m_ClanInfo.IsJoined()) bScanObjOK = false;
					}
					break;
				case MCP_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST_MYCLAN:
					{
						if (!pObj->GetCharInfo()->m_ClanInfo.IsJoined()) 
						{
							bScanObjOK = false;
						}
						else if (pScanObj->GetCharInfo()->m_ClanInfo.m_nClanID != pObj->GetCharInfo()->m_ClanInfo.m_nClanID)
						{
							bScanObjOK = false;
						}
					}
					break;
				}

				if (bScanObjOK)
				{
					ppTransObjectArray[nNodeCount] = pScanObj;
					nNodeCount++;

					if (nNodeCount >= DEFAULT_CHANNEL_MAXPLAYERS) break;
				}
			}
		}
	}

	if (nNodeCount <= 0) return;

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_RESPONSE_ALL_PLAYER_LIST), CCUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidChannel));

	void* pPlayerArray = MMakeBlobArray(sizeof(MTD_ChannelPlayerListNode), nNodeCount);

	for (int i = 0; i < nNodeCount; i++)
	{
		CCMatchObject* pScanObj = ppTransObjectArray[i];

		MTD_ChannelPlayerListNode* pNode = (MTD_ChannelPlayerListNode*)MGetBlobArrayElement(pPlayerArray, i);

		if (IsEnabledObject(pScanObj))
		{
			CopyChannelPlayerListNodeForTrans(pNode, pScanObj);
		}
	}

	pNew->AddParameter(new MCommandParameterBlob(pPlayerArray, MGetBlobArraySize(pPlayerArray)));
	MEraseBlobArray(pPlayerArray);
	RouteToListener(pObj, pNew);
}



void CCMatchServer::ResponseChannelRule(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) return;

	MCommand* pNew = CreateCommand(MC_MATCH_CHANNEL_RESPONSE_RULE, CCUID(0,0));
	pNew->AddParameter( new MCommandParameterUID(uidChannel) );
	pNew->AddParameter( new MCmdParamStr(const_cast<char*>(pChannel->GetRuleName())) );
	RouteToListener(pObj, pNew);
}


const CCUID CCMatchServer::FindFreeChannel(  const CCUID& uidPlayer  )
{
	CCUID uidChannel = CCUID(0,0);

	if (uidChannel == CCUID(0,0) &&
		MGetServerConfig()->IsEnabledDuelTournament() && 
		MGetServerConfig()->IsSendLoginUserToDuelTournamentChannel())
	{
		for(map<CCUID, MMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(MCHANNEL_TYPE_DUELTOURNAMENT); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(MCHANNEL_TYPE_DUELTOURNAMENT); itor++) {

				CCUID uid = (*itor).first;
				if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
					MMatchChannel* pChannel = FindChannel(uid);
					if (pChannel) {
						if (pChannel->GetMaxPlayers()*0.8 < pChannel->GetObjCount()) continue;
						uidChannel = uid;
						break;
					}
				}
			}
	}

	if (uidChannel == CCUID(0,0))
	{
		// Find proper channel by Level
		for(map<CCUID, MMatchChannel*>::const_iterator itor=m_ChannelMap.GetTypesChannelMapBegin(MCHANNEL_TYPE_PRESET); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(MCHANNEL_TYPE_PRESET); itor++) {
				CCUID uid = (*itor).first;
				if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
					MMatchChannel* pChannel = FindChannel(uid);
					if (pChannel) {
						if (pChannel->GetLevelMin() <= 0) continue;
						if (pChannel->GetMaxPlayers()*0.8 < pChannel->GetObjCount()) continue;
						uidChannel = uid;
						break;
					}
				}
			}
	}

	// ����� ������ ������ ����ä�η� �����ϰ� �������.
//#ifdef _DEBUG
//	for(map<CCUID, MMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(MCHANNEL_TYPE_PRESET); 
//		itor!=m_ChannelMap.GetTypesChannelMapEnd(MCHANNEL_TYPE_PRESET); itor++) {
//		CCUID uid = (*itor).first;
//		MMatchChannel* pChannel = FindChannel(uid);
//		if (pChannel) 
//		{
//			uidChannel = uid;
//				break;
//		}
//	}
//#endif

	// ������������ ������ ����ä�η� ����.
	if (uidChannel == CCUID(0,0))
	{
		for(map<CCUID, MMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(MCHANNEL_TYPE_PRESET); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(MCHANNEL_TYPE_PRESET); itor++) {

			CCUID uid = (*itor).first;
			if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
				MMatchChannel* pChannel = FindChannel(uid);
				if (pChannel) {
					if (pChannel->GetMaxPlayers()*0.8 < pChannel->GetObjCount()) continue;
					uidChannel = uid;
					break;
				}
			}
		}
	}

	// ���� ������ ������ �缳ä�η� ����.
	if (uidChannel == CCUID(0,0))
	{
		for(map<CCUID, MMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(MCHANNEL_TYPE_USER); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(MCHANNEL_TYPE_USER); itor++) {
			CCUID uid = (*itor).first;
			if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
				MMatchChannel* pChannel = FindChannel(uid);
				if (pChannel) {
					uidChannel = uid;
					break;
				}
			}
		}
	}

	return uidChannel;
}
