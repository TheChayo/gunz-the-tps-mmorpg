#pragma once


#include "CCUID.h"
#include <map>
using namespace std;


class MCommand;


#define CHATROOM_MAX_ROOMMEMBER	64


class CCMatchChatRoom {
protected:
	CCUID			m_uidChatRoom;
	CCUID			m_uidMaster;
	char			m_szName[ MAX_CHATROOMNAME_STRING_LEN ];
    CCUIDRefCache	m_PlayerList;

public:
	CCMatchChatRoom(const CCUID& uidRoom, const CCUID& uidMaster, const char* pszName);
	virtual ~CCMatchChatRoom();

	const CCUID& GetUID()	{ return m_uidChatRoom; }
	const CCUID& GetMaster()	{ return m_uidMaster; }
	const char* GetName()	{ return m_szName; }
	size_t GetUserCount()	{ return m_PlayerList.size(); }

	bool AddPlayer(const CCUID& uidPlayer);
	void RemovePlayer(const CCUID& uidPlayer);
	bool IsFindPlayer(const CCUID& uidPlayer);							// �ش� �÷��̾ �ִ��� Ȯ���Ѵ�

	void RouteChat(const CCUID& uidSender, char* pszMessage);
	void RouteInfo(const CCUID& uidReceiver);
	void RouteCommand(const MCommand* pCommand);
};


class CCMatchChatRoomMap : public map<CCUID, CCMatchChatRoom*> {
	CCUID	m_uidGenerate;
public:
	CCMatchChatRoomMap()			{	m_uidGenerate = CCUID(0,10);	}
	virtual ~CCMatchChatRoomMap(){}
	CCUID UseUID()				{	m_uidGenerate.Increase();	return m_uidGenerate;	}
	void Insert(const CCUID& uid, CCMatchChatRoom* pStage)	{	insert(value_type(uid, pStage));	}
};

class CCMatchChatRoomStringSubMap : public map<string, CCUID> {};


class CCMatchChatRoomMgr {
protected:
	CCMatchChatRoomMap			m_RoomMap;
	CCMatchChatRoomStringSubMap	m_RoomStringSubMap;

public:
	CCMatchChatRoomMgr();
	virtual ~CCMatchChatRoomMgr();

	CCMatchChatRoom* AddChatRoom(const CCUID& uidMaster, const char* pszName);
	void RemoveChatRoom(const CCUID& uidChatRoom);

	CCMatchChatRoom* FindChatRoom(const CCUID& uidChatRoom);
	CCMatchChatRoom* FindChatRoomByName(const char* pszName);

	void Update();
};
