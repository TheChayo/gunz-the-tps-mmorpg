#pragma once

#include <list>
using namespace std;
#include "CCMatchGlobal.h"


#define MAX_FRIEND_COUNT	20


// ģ������
struct CCMatchFriendNode {
	unsigned long	nFriendCID;
	unsigned short	nFavorite;
	char			szName[MATCHOBJECT_NAME_LENGTH];

	unsigned char	nState;										// ��ġ����(�κ�, ������ ���)
	char			szDescription[MATCH_SIMPLE_DESC_LENGTH];	// ��ġ����(�κ��̸� ���)
};
class CCMatchFriendList : public list<CCMatchFriendNode*> {};


class CCMatchFriendInfo {
private:
	CCCriticalSection	m_csFriendListLock;
public:
	CCMatchFriendList	m_FriendList;
public:
	CCMatchFriendInfo();
	virtual ~CCMatchFriendInfo();
	bool Add(unsigned long nFriendCID, unsigned short nFavorite, const char* pszName);
	void Remove(const char* pszName);
	CCMatchFriendNode* Find(unsigned long nFriendCID);
	CCMatchFriendNode* Find(const char* pszName);
	void UpdateDesc();
};

#pragma pack(1)
struct CCFRIENDLISTNODE {
	unsigned char	nState;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szDescription[MATCH_SIMPLE_DESC_LENGTH];
};
#pragma pack()
