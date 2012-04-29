#include "stdafx.h"
#include "CCMatchChatRoom.h"
#include "CCMatchServer.h"
#include "CCSharedCommandTable.h"



MMatchChatRoom::MMatchChatRoom(const CCUID& uidRoom, const CCUID& uidMaster, const char* pszName)
{
	m_uidChatRoom = uidRoom;
	m_uidMaster = uidMaster;

	const size_t NameLen = strlen( pszName) + 1;

	strncpy(m_szName, pszName
		, NameLen > MAX_CHATROOMNAME_STRING_LEN ? MAX_CHATROOMNAME_STRING_LEN : NameLen );
}

MMatchChatRoom::~MMatchChatRoom() 
{
}

bool MMatchChatRoom::AddPlayer(const CCUID& uidPlayer)
{
	CCUIDRefCache::iterator i = m_PlayerList.find(uidPlayer);
	if (i != m_PlayerList.end())
		return false;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchObject* pPlayer = pServer->GetObject(uidPlayer);
	if( !IsEnabledObject(pPlayer) )
		return false;

	pPlayer->SetChatRoomUID(GetUID());

	m_PlayerList.Insert(uidPlayer, pPlayer);
	return true;
}

bool MMatchChatRoom::IsFindPlayer(const CCUID& uidPlayer)	// �ش� �÷��̾ �ִ��� Ȯ���Ѵ�
{
	CCUIDRefCache::iterator i = m_PlayerList.find(uidPlayer);
	if (i == m_PlayerList.end())
		return false;
	return true;										// �÷��̾ ������ �� ��ȯ
}

void MMatchChatRoom::RemovePlayer(const CCUID& uidPlayer)
{
	CCUIDRefCache::iterator i = m_PlayerList.find(uidPlayer);
	if (i != m_PlayerList.end()) {
		m_PlayerList.erase(i);
	
		// 2008.08.28 ä�ù濡 Ż���� /ä�� �Ҹ��� �ϸ� ä�ù濡 �����Ǿ��ִ� ����鿡�� ä�ø� �������� ó��
		// ä�ù� Ż��� �÷��̾� ä�÷� ID�ʱ�ȭ http://dev:8181/projects/gunz/ticket/158
		CCMatchServer* pServer = CCMatchServer::GetInstance();
		CCMatchObject* pPlayer = pServer->GetObject(uidPlayer);
		if( !IsEnabledObject(pPlayer) )
			return;
		pPlayer->SetChatRoomUID(CCUID(0,0));
	}
}

void MMatchChatRoom::RouteChat(const CCUID& uidSender, char* pszMessage)
{
	if( (0 == pszMessage) || (256 < strlen(pszMessage)) )
		return;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	CCMatchObject* pSenderObj = pServer->GetObject(uidSender);
	if (pSenderObj == NULL)
		return;

	for (CCUIDRefCache::iterator i=m_PlayerList.begin(); i!=m_PlayerList.end(); i++) {
		CCUID uidTarget = (*i).first;
		CCMatchObject* pTargetObj = pServer->GetObject(uidTarget);
		if (pTargetObj) {
			MCommand* pCmd = pServer->CreateCommand(MC_MATCH_CHATROOM_CHAT, CCUID(0,0));
			pCmd->AddParameter(new MCmdParamStr( const_cast<char*>(GetName()) ));
			pCmd->AddParameter(new MCmdParamStr(pSenderObj->GetName()));
			pCmd->AddParameter(new MCmdParamStr(pszMessage));
			pServer->RouteToListener(pTargetObj, pCmd);
		}
	}
}

void MMatchChatRoom::RouteInfo(const CCUID& uidReceiver)
{
	
}

void MMatchChatRoom::RouteCommand(const MCommand* pCommand)
{
	if( 0 == pCommand )
		return;

	CCMatchServer* pServer = CCMatchServer::GetInstance();
	for (CCUIDRefCache::iterator i=m_PlayerList.begin(); i!=m_PlayerList.end(); i++) {
		CCUID uidTarget = (*i).first;
		CCMatchObject* pTargetObj = pServer->GetObject(uidTarget);
		if (pTargetObj) {
			MCommand* pRouteCmd = pCommand->Clone();
			pServer->RouteToListener(pTargetObj, pRouteCmd);
		}
	}
	delete pCommand;
}


MMatchChatRoomMgr::MMatchChatRoomMgr()
{
}

MMatchChatRoomMgr::~MMatchChatRoomMgr()
{
}

MMatchChatRoom* MMatchChatRoomMgr::AddChatRoom(const CCUID& uidMaster, const char* pszName)
{
	if( (0 == pszName) || (128 < strlen(pszName)) )
		return 0;

	if (FindChatRoomByName(pszName) != NULL)
		return NULL;

	CCMatchObject* pMaster = CCMatchServer::GetInstance()->GetObject( uidMaster );
	if( !IsEnabledObject(pMaster) )
		return 0;

	MMatchChatRoom* pRoom = new MMatchChatRoom(m_RoomMap.UseUID(), uidMaster, pszName);
	if( 0 == pRoom )
		return 0;

	m_RoomMap.Insert(pRoom->GetUID(), pRoom);

	string strName = pszName;
	m_RoomStringSubMap.insert( 
		MMatchChatRoomStringSubMap::value_type(strName, pRoom->GetUID()) 
	);

	return pRoom;
}

void MMatchChatRoomMgr::RemoveChatRoom(const CCUID& uidChatRoom)
{
	MMatchChatRoomMap::iterator i = m_RoomMap.find(uidChatRoom);
	if (i!=m_RoomMap.end()) {
		// Remove Sub Map
		MMatchChatRoom* pRoom = (*i).second;
		MMatchChatRoomStringSubMap::iterator itorSub = 
			m_RoomStringSubMap.find(pRoom->GetName());
		if (itorSub != m_RoomStringSubMap.end()) {
			m_RoomStringSubMap.erase(itorSub);
		}

		// Remove from Map
		m_RoomMap.erase(i);
	}
}

MMatchChatRoom* MMatchChatRoomMgr::FindChatRoom(const CCUID& uidChatRoom)
{
	MMatchChatRoomMap::iterator i = m_RoomMap.find(uidChatRoom);
	if (i!=m_RoomMap.end())
		return (*i).second;
	return NULL;
}

MMatchChatRoom* MMatchChatRoomMgr::FindChatRoomByName(const char* pszName)
{
	if( (0 == pszName) || (128 < strlen(pszName)) )
		return 0;

	MMatchChatRoomStringSubMap::iterator itorSub = 
		m_RoomStringSubMap.find(pszName);
	if (itorSub != m_RoomStringSubMap.end()) {
		CCUID uidRoom = (*itorSub).second;
		return FindChatRoom(uidRoom);
	}
	return NULL;
}

void MMatchChatRoomMgr::Update()
{
}
