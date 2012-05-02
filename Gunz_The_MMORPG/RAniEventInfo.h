////////////////////////////////////////////////////////////////////////
////2007.01.31
////RAniEventInfo.h
////�ִϸ��̼ǿ� �߰��Ǿ� �ִ� �̺�Ʈ�� ���� ������ ��� �ִ� ����
////�ۼ���: ȫ����
////////////////////////////////////////////////////////////////////////
#pragma once

#include "CCXml.h"
#include "CCDebug.h"
#include "CCZFileSystem.h"
#include "fileinfo.h"
#include <list>
using namespace std;

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

class RAniEventInfo
{
private:
	char m_cFileName[256];
	int m_iBeginFrame;
	char m_CCEventType[256];
	
public:
	RAniEventInfo(){}
	rvector m_vPos;

	//�д� �κ�
	char* GetFileName(){return m_cFileName;}
	int GetBeginFrame(){return m_iBeginFrame;}
	char* GetEventType(){return m_CCEventType;}
	
	//�����ϴ� �κ�
	void SetFileName(char * filename){ strcpy(m_cFileName, filename); }
	void SetBeginFrame(int BeginFrame){m_iBeginFrame = BeginFrame;}
	void SetEventType(char* EventType){strcpy(m_CCEventType, EventType);}
};

typedef list<RAniEventInfo*> AniNameEventSet;
typedef list<RAniEventInfo*>::iterator AniNameEventSetIter;

//�ϳ��� �ִϸ��̼ǿ� ��� �̺�Ʈ ������
typedef struct _RAniNameEventSet
{
	char					m_cAnimationName[256];
	AniNameEventSet			m_AniNameEventSet;		//�ִϸ��̼� �̸��� ���� �ֵ鳢�� ���ִ� ��
	AniNameEventSetIter		m_AniNameEventSetIter;	

	char* GetAnimationName(){return m_cAnimationName;}
	void SetAnimationName(char* AnimationName){strcpy(m_cAnimationName, AnimationName);}
	void Destroy()
	{ 
		while( !(m_AniNameEventSet.empty()) )
		{
			SAFE_DELETE(m_AniNameEventSet.back());
			m_AniNameEventSet.pop_back();
		}
	}

	//�ִϸ��̼� �̺�Ʈ ������ ������ �����ִ� �κ��� ����� �޽�����...
}RAniNameEventSet;


typedef list<RAniNameEventSet*> AniIDEventSet;
typedef list<RAniNameEventSet*>::iterator AniIDEventSetIter;
//�ϳ��� �޽��� ��� �̺�Ʈ �µ�..
typedef struct _RAniIDEventSet
{
	int						m_iID;	// npc ID
	AniIDEventSet			m_AniIDEventSet;	// �̺�Ʈ ������ ��� ����Ʈ
	AniIDEventSetIter		m_AniIDEventSetIter;	//���ͷ�����..	

	int GetID(){return m_iID;}
	void SetID(int id){m_iID = id;}

	RAniNameEventSet* GetAniNameEventSet(char* AnimationName);

	void Destroy()
	{ 
		while( !(m_AniIDEventSet.empty()) )
		{
			m_AniIDEventSet.back()->Destroy();
			SAFE_DELETE(m_AniIDEventSet.back());
			m_AniIDEventSet.pop_back();
		}
	}
	
}RAniIDEventSet;

//��� �̺�Ʈ �������� �����ϴ� Ŭ����

typedef list<RAniIDEventSet*> AniEventMgr;	//�� npc�� ����� �ִϸ��̼ǵ��� �����ϴ� ����Ʈ
typedef list<RAniIDEventSet*>::iterator AniEventMgrIter;

class RAniEventMgr
{
public:
	AniEventMgr				m_AniEventMgr;
	AniEventMgrIter			m_AniEventMgrIter;

	bool ReadXml(CCZFileSystem* pFileSystem, const char* szFileName);
	void ParseAniEvent(::CCXmlElement* PNode, RAniIDEventSet* pAnimIdEventSet);
	RAniIDEventSet* GetAniIDEventSet(int id);
	
	void Destroy()
	{ 
		while( !(m_AniEventMgr.empty()) )
		{
			m_AniEventMgr.back()->Destroy();
			SAFE_DELETE(m_AniEventMgr.back());
			m_AniEventMgr.pop_back();
		}
	}
	RAniEventMgr(){}
	~RAniEventMgr(){ Destroy(); }
};










