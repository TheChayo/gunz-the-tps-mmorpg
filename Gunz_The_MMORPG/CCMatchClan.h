#pragma once

#include "CCMatchGlobal.h"
#include "CCUID.h"
#include "CCSmartRefresh.h"
#include <string>
#include <list>
using namespace std;

// 클랜 등급
enum CCMatchClanGrade
{
	CCG_NONE		= 0,		// 클랜원이 아님
	CCG_MASTER		= 1,		// 클랜 마스터
	CCG_ADMIN		= 2,		// 클랜 운영자

	CCG_MEMBER		= 9,		// 일반 클랜원
	CCG_END
};

// 클랜등급이 nSrcGrade가 nDstGrade보다 같거나 더 높은지 판별한다.
inline bool IsUpperClanGrade(CCMatchClanGrade nSrcGrade, CCMatchClanGrade nDstGrade)
{
	if ((nSrcGrade != CCG_NONE) && ((int)nSrcGrade <= (int)nDstGrade)) return true;
	return false;
}

///////////////////////////////////
class CCMatchObject;

/// 클랜
class CCMatchClan
{
private:
	int				m_nCLID;							///< 클랜 고유의 ID
	char			m_szClanName[CLAN_NAME_LENGTH];		///< 이름
	unsigned long	m_nDBRefreshLifeTime;

	struct ClanInfoEx
	{
		int	nLevel;							///< 레벨
		int	nTotalPoint;					///< 토탈포인트
		int	nPoint;							///< 포인트
		int	nWins;							///< 전적 - 승수
		int	nLosses;						///< 전적 - 패수
		int nRanking;						///< 랭킹
		int	nTotalMemberCount;				///< 총인원
		char szMaster[MATCHOBJECT_NAME_LENGTH];	
		char szEmblemUrl[256];					///< 클랜마크 URL
		int nEmblemChecksum;					///< 클랜마크 체크섬
	};

	ClanInfoEx		m_ClanInfoEx;
	CCUIDRefCache	m_Members;							///< 플레이어들
	CCSmartRefresh	m_SmartRefresh;						///< 플레이어 캐슁

	int				m_nSeriesOfVictories;				///< 클랜전 연승수
	list<int>		m_MatchedClanList;					///< 대전했던 클랜

	unsigned long	m_nEmptyPeriod;

	void	Clear();
	void InitClanInfoEx(const int nLevel, const int nTotalPoint, const int nPoint, const int nRanking,
		                const int nWins, const int nLosses, const int nTotalMemberCount, const char* szMaster,
						const char* szEmblemUrl, int nEmblemChecksum);
public:
	CCMatchClan();
	virtual ~CCMatchClan();

	// CCMatchClanMap에서 사용하는 함수
	void Create(int nCLID, const char* szClanName);
	void AddObject(const CCUID& uid, CCMatchObject* pObj);
	void RemoveObject(const CCUID& uid);

	
	void Tick(unsigned long nClock);
	void SyncPlayerList(CCMatchObject* pObj, int nCategory);
	void InitClanInfoFromDB();			// db에서 클랜정보를 초기화한다.
	bool CheckLifePeriod();

	// get 씨리즈
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
	void InsertMatchedClanID(int nCLID);	///< 대전했던 클랜

	CCUIDRefCache::iterator GetMemberBegin()		{ return m_Members.begin(); }
	CCUIDRefCache::iterator GetMemberEnd()		{ return m_Members.end(); }
};

///////////////////////////////////

class CCMatchClanMap : public map<int, CCMatchClan*>
{
private:
	unsigned long	m_nLastTick;						///< 틱
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