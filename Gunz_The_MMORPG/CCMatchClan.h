#ifndef _CCMATCHCLAN_H
#define _CCMATCHCLAN_H


#include "CCMatchGlobal.h"
#include "CCUID.h"
#include "CCSmartRefresh.h"
#include <string>
#include <list>
using namespace std;

// Ŭ�� ���
enum CCMatchClanGrade
{
	CCG_NONE		= 0,		// Ŭ������ �ƴ�
	CCG_MASTER		= 1,		// Ŭ�� ������
	CCG_ADMIN		= 2,		// Ŭ�� ���

	CCG_MEMBER		= 9,		// �Ϲ� Ŭ����
	CCG_END
};

// Ŭ������� nSrcGrade�� nDstGrade���� ���ų� �� ������ �Ǻ��Ѵ�.
inline bool IsUpperClanGrade(CCMatchClanGrade nSrcGrade, CCMatchClanGrade nDstGrade)
{
	if ((nSrcGrade != CCG_NONE) && ((int)nSrcGrade <= (int)nDstGrade)) return true;
	return false;
}

///////////////////////////////////
class CCMatchObject;

/// Ŭ��
class CCMatchClan
{
private:
	int				m_nCLID;							///< Ŭ�� ������ ID
	char			m_szClanName[CLAN_NAME_LENGTH];		///< �̸�
	unsigned long	m_nDBRefreshLifeTime;

	struct ClanInfoEx
	{
		int	nLevel;							///< ����
		int	nTotalPoint;					///< ��Ż����Ʈ
		int	nPoint;							///< ����Ʈ
		int	nWins;							///< ���� - �¼�
		int	nLosses;						///< ���� - �м�
		int nRanking;						///< ��ŷ
		int	nTotalMemberCount;				///< ���ο�
		char szMaster[MATCHOBJECT_NAME_LENGTH];	
		char szEmblemUrl[256];					///< Ŭ����ũ URL
		int nEmblemChecksum;					///< Ŭ����ũ üũ��
	};

	ClanInfoEx		m_ClanInfoEx;
	CCUIDRefCache	m_Members;							///< �÷��̾��
	CCSmartRefresh	m_SmartRefresh;						///< �÷��̾� ĳ��

	int				m_nSeriesOfVictories;				///< Ŭ���� ���¼�
	list<int>		m_MatchedClanList;					///< �����ߴ� Ŭ��

	unsigned long	m_nEmptyPeriod;

	void	Clear();
	void InitClanInfoEx(const int nLevel, const int nTotalPoint, const int nPoint, const int nRanking,
		                const int nWins, const int nLosses, const int nTotalMemberCount, const char* szMaster,
						const char* szEmblemUrl, int nEmblemChecksum);
public:
	CCMatchClan();
	virtual ~CCMatchClan();

	// MMatchClanMap���� ����ϴ� �Լ�
	void Create(int nCLID, const char* szClanName);
	void AddObject(const CCUID& uid, CCMatchObject* pObj);
	void RemoveObject(const CCUID& uid);

	
	void Tick(unsigned long nClock);
	void SyncPlayerList(CCMatchObject* pObj, int nCategory);
	void InitClanInfoFromDB();			// db���� Ŭ�������� �ʱ�ȭ�Ѵ�.
	bool CheckLifePeriod();

	// get ������
	int			GetCLID()						{ return m_nCLID; }
	const char* GetName()						{ return m_szClanName; }
//	int			GetLevel()						{ return m_nLevel; }
	int			GetMemberCount()				{ return (int)m_Members.size(); }
	ClanInfoEx*	GetClanInfoEx()					{ return &m_ClanInfoEx; }
	bool		IsInitedClanInfoEx()			{ return (m_ClanInfoEx.nLevel != 0); }
	int			GetSeriesOfVictories()			{ return m_nSeriesOfVictories; }
	const char*	GetEmblemURL()					{ return m_ClanInfoEx.szEmblemUrl; }
	int			GetEmblemChecksum()				{ return m_ClanInfoEx.nEmblemChecksum; }
	

	// Inc
	void IncWins(int nAddedWins) { 
		m_ClanInfoEx.nWins += nAddedWins; m_nSeriesOfVictories++; 
	}
	void IncLosses(int nAddedLosses) { 
		m_ClanInfoEx.nLosses += nAddedLosses; m_nSeriesOfVictories=0; 
	}
	void IncPoint(int nAddedPoint)				{ m_ClanInfoEx.nPoint += nAddedPoint; 
													if (nAddedPoint > 0) m_ClanInfoEx.nTotalPoint += nAddedPoint; 
													if (m_ClanInfoEx.nPoint < 0) m_ClanInfoEx.nPoint =0;
												}
	void InsertMatchedClanID(int nCLID);	///< �����ߴ� Ŭ��

	CCUIDRefCache::iterator GetMemberBegin()		{ return m_Members.begin(); }
	CCUIDRefCache::iterator GetMemberEnd()		{ return m_Members.end(); }
};

///////////////////////////////////

class CCMatchClanMap : public map<int, CCMatchClan*>
{
private:
	unsigned long	m_nLastTick;						///< ƽ
	map<std::string, CCMatchClan*>	m_ClanNameMap;
	void CreateClan(int nCLID, const char* szClanName);
	void DestroyClan(int nCLID, CCMatchClanMap::iterator* pNextItor);
	bool CheckTick(unsigned long nClock);
public:
	CCMatchClanMap();
	virtual ~CCMatchClanMap();
	void Destroy(); 
	void Tick(unsigned long nClock);

	void AddObject(const CCUID& uid, CCMatchObject* pObj);
	void RemoveObject(const CCUID& uid, CCMatchObject* pObj);
	CCMatchClan* GetClan(const int nCLID);
	CCMatchClan* GetClan(const char* szClanName);
};


#endif