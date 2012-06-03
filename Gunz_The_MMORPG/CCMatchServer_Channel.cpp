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
#include "CCMatchFormula.h"
#include "CCMatchConfig.h"
#include "CCCommandCommunicator.h"
#include "CCMatchShop.h"
#include "CCMatchTransDataType.h"
#include "CCDebug.h"
#include "CCMatchAuth.h"
#include "CCMatchStatus.h"
#include "CCAsyncDBJob.h"
#include "CCMatchTransDataType.h"
#include "CCUtil.h"

void CopyChannelPlayerListNodeForTrans(CCTD_ChannelPlayerListNode* pDest, CCMatchObject* pSrcObject)
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

	CCMatchClan* pClan = CCMatchServer::GetInstance()->GetClanMap()->GetClan(pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan)
		pDest->nEmblemChecksum = pClan->GetEmblemChecksum();
	else
		pDest->nEmblemChecksum = 0;
}


CCMatchChannel* CCMatchServer::FindChannel(const CCUID& uidChannel)
{
	return m_ChannelMap.Find(uidChannel);
}

CCMatchChannel* CCMatchServer::FindChannel(const CCCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	return m_ChannelMap.Find(nChannelType, pszChannelName);
}


bool CCMatchServer::ChannelAdd(const char* pszChannelName, const char* pszRuleName, CCUID* pAllocUID, CCCHANNEL_TYPE nType, int nMaxPlayers, int nLevelMin, int nLevelMax,
							  const bool bIsTicketChannel, const DWORD dwTicketItemID, const bool bIsUseTicket, const char* pszChannelNameStrResId)
{
	return m_ChannelMap.Add(pszChannelName, pszRuleName, pAllocUID, nType, nMaxPlayers, nLevelMin, nLevelMax, bIsTicketChannel, dwTicketItemID, bIsUseTicket, pszChannelNameStrResId);
}

bool CCMatchServer::ChannelJoin(const CCUID& uidPlayer, const CCCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	if ((nChannelType < 0) || (nChannelType >= CCCHANNEL_TYPE_MAX)) return false;

	int nChannelNameLen = (int)strlen(pszChannelName);
	if ((nChannelNameLen >= CHANNELNAME_LEN) || (nChannelNameLen <= 0)) return false;


	CCUID uidChannel = CCUID(0,0);

	// ���� �̸��� ä���� �����ϴ��� �˻��Ѵ�.
	// ���� �ִٸ� �� ä�η� �ٰ� ����.
	// ������ ä�� ����.
	CCMatchChannel* pChannel = FindChannel(nChannelType, pszChannelName);
	
	if (pChannel == NULL)
	{
		// ������ ä���̸� ä���� ���� �� ����.
		if (nChannelType == CCCHANNEL_TYPE_PRESET) 
			return false;

		if( nChannelType == CCCHANNEL_TYPE_DUELTOURNAMENT )
			return false;

		//bool bbadf = GetChannelMap()->GetClanChannelTicketInfo().m_bIsTicketChannel; //debug
/*
		// ������� ����ϰ�, Ŭ��ä���� ���鶧�� ������ ������� �ִ��� �˻縦 ����� �Ѵ�.
		if( CCGetServerConfig()->IsUseTicket() && 
			CCCHANNEL_TYPE_CLAN == nChannelType && 
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

	CCMatchChannel* pChannel = FindChannel(uidChannelTmp);
	if (pChannel == NULL) return false;

	// ������� �ʿ��� ä�ο����� ������ ������� �ִ��� �˻縦 ����� �Ѵ�.
	if ( CCGetServerConfig()->IsUseTicket())
	{
		bool bCheckTicket = false;

		CCMatchObject* pObj = GetObject(uidPlayer);
		if ( !pObj)	return false;

		if ( CCGetServerConfig()->GetServerMode() == CSM_NORMAL)	{ // �Ϲ� �����϶�
			// ����/�缳/Ŭ�� ä���̸� �κ� �������̽��� disable ��Ų��.
			// �� �ܿ��� Ƽ�� ä���̸� Ƽ�� �˻縸 ���ش�.
			if ( stricmp( pChannel->GetRuleName() , CCCHANNEL_RULE_NOVICE_STR) == 0) bEnableInterface = false;			
			else if ( pChannel->IsTicketChannel())									bCheckTicket = true;
		} else if ( (CCGetServerConfig()->GetServerMode() == CSM_CLAN) || (CCGetServerConfig()->GetServerMode() == CSM_TEST)) { // Ŭ�� �����϶�
			// Ŭ��/�缳 ä���̸� Ƽ�� �˻��Ѵ�.
			// ���� ä���̸� �κ� �������̽��� disable ��Ų��.
			// �� �ܿ��� Ƽ�� ä���̸� Ƽ�� �˻縸 ���ش�.
			if ( (pChannel->GetChannelType() == CCCHANNEL_TYPE_CLAN) || (pChannel->GetChannelType() == CCCHANNEL_TYPE_USER)) bCheckTicket = true;			
			else if ( stricmp( pChannel->GetRuleName() , CCCHANNEL_RULE_NOVICE_STR) == 0)
				bEnableInterface = false;			
			else if ( pChannel->IsTicketChannel())
				bCheckTicket = true;
		} else { // �׿� �����϶�
			// ����/�缳/Ŭ�� ä���̸� �κ� �������̽��� disable ��Ų��.
			// �� �ܿ��� Ƽ�� ä���̸� Ƽ�� �˻縸 ���ش�.
			if ( stricmp( pChannel->GetRuleName() , CCCHANNEL_RULE_NOVICE_STR) == 0) bEnableInterface = false;			
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
	CCMatchChannel* pOldChannel = FindChannel(pObj->GetChannelUID());
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

	
	if( pChannel->GetRuleType() != CCCHANNEL_RULE_DUELTOURNAMENT ) {
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

	CCMatchChannel *pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_RESPONSE_JOIN), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidChannel));
	pNew->AddParameter(new CCCommandParameterInt(nChannelType));

	if (szChannelStrResId[0] != 0) {
		pNew->AddParameter(new CCCommandParameterString((char*)szChannelStrResId));	// ���� ä���̸� Ŭ�󿡼� ���� ���� ������ �� �ֵ��� ��Ʈ�����ҽ�ID�� �ѱ��
	} else {
		pNew->AddParameter(new CCCommandParameterString((char*)pChannel->GetName()));
	}
	
	pNew->AddParameter(new CCCommandParameterBool(bEnableInterface));
	RouteToListener(pObj, pNew);
}

bool CCMatchServer::ChannelLeave(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return false;
	pChannel->RemoveObject(uidPlayer);

	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if (pObj == NULL) return false;

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_LEAVE),CCUID(0,0),m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
	pNew->AddParameter(new CCCommandParameterUID(pChannel->GetUID()));
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
#include "CCLexicalAnalyzer.h"
bool StageGo(CCMatchServer* pServer, const CCUID& uidPlayer, char* pszChat)
{
	CCMatchObject* pChar = pServer->GetObject(uidPlayer);
	if (pChar == NULL)	return false;
	if (pChar->GetPlace() != MMP_LOBBY) return false;
	CCMatchChannel* pChannel = pServer->FindChannel(pChar->GetChannelUID());
	if (pChannel == NULL) return false;

	bool bResult = false;
	CCLexicalAnalyzer lex;
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
							CCCommand* pNew = pServer->CreateCommand(MC_MATCH_REQUEST_STAGE_JOIN, pServer->GetUID());
							pNew->AddParameter(new CCCommandParameterUID(uidPlayer));
							pNew->AddParameter(new CCCommandParameterUID(pStage->GetUID()));
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
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return false;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) return false;
	if (pObj->GetAccountInfo()->m_nUGrade == CCMUGCHAT_LIMITED) return false;

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
		
		CCCommand* pCmd = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_DUMB_CHAT), pObj->GetUID(), m_This);
		Post(pCmd);

		LOG(LOG_FILE, "CCMatchServer::ChannelChat - Set Dumb Player(CCUID:%d%d, Name:%s)", pObj->GetUID().High, pObj->GetUID().Low, pObj->GetName());
		return false;
	}

	///<�������....

	CCCommand* pCmd = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_CHAT), CCUID(0,0), m_This);
	pCmd->AddParameter(new CCCommandParameterUID(uidChannel));
	pCmd->AddParameter(new CCCommandParameterString(pObj->GetCharInfo()->m_szName));
	pCmd->AddParameter(new CCCommandParameterString(pszChat));
	pCmd->AddParameter(new CCCommandParameterInt(nGrade));

	RouteToChannelLobby(uidChannel, pCmd);
	return true;
}

void CCMatchServer::OnRequestRecommendedChannel(const CCUID& uidComm)
{
	CCUID uidChannel = FindFreeChannel( uidComm );

	if (CCUID(0,0) == uidChannel ) 
	{
		if( !ChannelAdd(GetDefaultChannelName(), GetDefaultChannelRuleName(), &uidChannel, CCCHANNEL_TYPE_PRESET) )
		{
			cclog( "Channel Add fail for recommand.\n" );
			return;	// �������� �����ϸ� ����....
		}		
	}

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL),
									uidComm, m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidChannel));
	Post(pNew);
}

void CCMatchServer::OnRequestChannelJoin(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	ChannelJoin(uidPlayer, uidChannel);
}

void CCMatchServer::OnRequestChannelJoin(const CCUID& uidPlayer, const CCCHANNEL_TYPE nChannelType, const char* pszChannelName)
{
	if ((nChannelType < 0) || (nChannelType >= CCCHANNEL_TYPE_MAX)) return;

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

	pObj->SetChannelListTransfer(true, CCCHANNEL_TYPE(nChannelType));
}

void CCMatchServer::OnStopChannelList(const CCUID& uidPlayer)
{
	CCMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	pObj->SetChannelListTransfer(false);

}

void CCMatchServer::ChannelList(const CCUID& uidPlayer, CCCHANNEL_TYPE nChannelType)
{
	CCMatchObject* pChar = GetObject(uidPlayer);
	if (! IsEnabledObject(pChar)) return;

	if (pChar->GetPlace() != MMP_LOBBY) return;		// �κ� �ƴϸ� ����
	if ((nChannelType < 0) || (nChannelType >= CCCHANNEL_TYPE_MAX)) return;

	// Count Active Channels
	int nChannelCount = (int)m_ChannelMap.GetChannelCount(nChannelType);
	if (nChannelCount <= 0) return;

// ä�θ���Ʈ�� �ִ� 100�������� ����Ʈ�� ������.
#define MAX_CHANNEL_LIST_NODE		100

	nChannelCount = min(nChannelCount, MAX_CHANNEL_LIST_NODE);

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_LIST), CCUID(0,0), m_This);

	void* pChannelArray = CCMakeBlobArray(sizeof(CCCHANNELLISTNODE), nChannelCount);
	int nIndex=0;
	for (map<CCUID, CCMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(nChannelType); 
		itor!=m_ChannelMap.GetTypesChannelMapEnd(nChannelType); itor++) {

		if (nIndex >= nChannelCount) break;

		CCMatchChannel* pChannel = (*itor).second;

		CCCHANNELLISTNODE* pNode = (CCCHANNELLISTNODE*)CCGetBlobArrayElement(pChannelArray, nIndex++);
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
	pNew->AddParameter(new CCCommandParameterBlob(pChannelArray, CCGetBlobArraySize(pChannelArray)));
	CCEraseBlobArray(pChannelArray);

	RouteToListener(pChar, pNew);
}





//void CCMatchServer::OnChannelRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage)
//{
//	ChannelResponsePlayerList(uidPlayer, uidChannel, nPage);
//}

void CCMatchServer::OnChannelRequestPlayerList(const CCUID& uidPlayer, const CCUID& uidChannel, int nPage)
{
	CCMatchChannel* pChannel = FindChannel(uidChannel);
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
	CCMatchChannel* pChannel = FindChannel(uidChannel);
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


	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST), CCUID(0,0), m_This);
	//pNew->AddParameter(new CCCommandParameterUID(uidChannel));
	pNew->AddParameter(new CCCommandParameterUChar((unsigned char)nObjCount));
	pNew->AddParameter(new CCCommandParameterUChar((unsigned char)nPage));

	void* pPlayerArray = CCMakeBlobArray(sizeof(CCTD_ChannelPlayerListNode), nNodeCount);

	int nArrayIndex=0;
	for (CCUIDRefCache::iterator i=FirstItor; i != pChannel->GetObjEnd(); i++) 
	{
		CCMatchObject* pScanObj = (CCMatchObject*)(*i).second;

		CCTD_ChannelPlayerListNode* pNode = (CCTD_ChannelPlayerListNode*)CCGetBlobArrayElement(pPlayerArray, nArrayIndex++);

		if (IsEnabledObject(pScanObj))
		{
			CopyChannelPlayerListNodeForTrans(pNode, pScanObj);		
		}

		if (nArrayIndex >= nNodeCount) break;
	}

	pNew->AddParameter(new CCCommandParameterBlob(pPlayerArray, CCGetBlobArraySize(pPlayerArray)));
	CCEraseBlobArray(pPlayerArray);
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
	CCMatchChannel* pChannel = FindChannel(uidChannel);
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

	CCCommand* pNew = new CCCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CHANNEL_RESPONSE_ALL_PLAYER_LIST), CCUID(0,0), m_This);
	pNew->AddParameter(new CCCommandParameterUID(uidChannel));

	void* pPlayerArray = CCMakeBlobArray(sizeof(CCTD_ChannelPlayerListNode), nNodeCount);

	for (int i = 0; i < nNodeCount; i++)
	{
		CCMatchObject* pScanObj = ppTransObjectArray[i];

		CCTD_ChannelPlayerListNode* pNode = (CCTD_ChannelPlayerListNode*)CCGetBlobArrayElement(pPlayerArray, i);

		if (IsEnabledObject(pScanObj))
		{
			CopyChannelPlayerListNodeForTrans(pNode, pScanObj);
		}
	}

	pNew->AddParameter(new CCCommandParameterBlob(pPlayerArray, CCGetBlobArraySize(pPlayerArray)));
	CCEraseBlobArray(pPlayerArray);
	RouteToListener(pObj, pNew);
}



void CCMatchServer::ResponseChannelRule(const CCUID& uidPlayer, const CCUID& uidChannel)
{
	CCMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;
	CCMatchObject* pObj = (CCMatchObject*)GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) return;

	CCCommand* pNew = CreateCommand(MC_MATCH_CHANNEL_RESPONSE_RULE, CCUID(0,0));
	pNew->AddParameter( new CCCommandParameterUID(uidChannel) );
	pNew->AddParameter( new CCCmdParamStr(const_cast<char*>(pChannel->GetRuleName())) );
	RouteToListener(pObj, pNew);
}


const CCUID CCMatchServer::FindFreeChannel(  const CCUID& uidPlayer  )
{
	CCUID uidChannel = CCUID(0,0);

	if (uidChannel == CCUID(0,0) &&
		CCGetServerConfig()->IsEnabledDuelTournament() && 
		CCGetServerConfig()->IsSendLoginUserToDuelTournamentChannel())
	{
		for(map<CCUID, CCMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(CCCHANNEL_TYPE_DUELTOURNAMENT); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(CCCHANNEL_TYPE_DUELTOURNAMENT); itor++) {

				CCUID uid = (*itor).first;
				if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
					CCMatchChannel* pChannel = FindChannel(uid);
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
		for(map<CCUID, CCMatchChannel*>::const_iterator itor=m_ChannelMap.GetTypesChannelMapBegin(CCCHANNEL_TYPE_PRESET); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(CCCHANNEL_TYPE_PRESET); itor++) {
				CCUID uid = (*itor).first;
				if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
					CCMatchChannel* pChannel = FindChannel(uid);
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
//	for(map<CCUID, CCMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(CCCHANNEL_TYPE_PRESET); 
//		itor!=m_ChannelMap.GetTypesChannelMapEnd(CCCHANNEL_TYPE_PRESET); itor++) {
//		CCUID uid = (*itor).first;
//		CCMatchChannel* pChannel = FindChannel(uid);
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
		for(map<CCUID, CCMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(CCCHANNEL_TYPE_PRESET); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(CCCHANNEL_TYPE_PRESET); itor++) {

			CCUID uid = (*itor).first;
			if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
				CCMatchChannel* pChannel = FindChannel(uid);
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
		for(map<CCUID, CCMatchChannel*>::iterator itor=m_ChannelMap.GetTypesChannelMapBegin(CCCHANNEL_TYPE_USER); 
			itor!=m_ChannelMap.GetTypesChannelMapEnd(CCCHANNEL_TYPE_USER); itor++) {
			CCUID uid = (*itor).first;
			if (MOK == ValidateChannelJoin(uidPlayer, uid)) {
				CCMatchChannel* pChannel = FindChannel(uid);
				if (pChannel) {
					uidChannel = uid;
					break;
				}
			}
		}
	}

	return uidChannel;
}
