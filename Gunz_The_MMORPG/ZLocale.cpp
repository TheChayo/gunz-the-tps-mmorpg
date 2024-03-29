#pragma warning( disable : 4065)

#include "stdafx.h"
#include "ZLocale.h"
#include "ZConfiguration.h"
//#include "ZNetmarble.h"			// test
#include "ZApplication.h"
#include "ZGlobal.h"
//#include "ZSecurity.h"
//#include "ZPost.h"

// Added R349a
#include "ZStringResManager.h"

//#define _INDEPTH_DEBUG_

#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#endif



ZLocale* ZLocale::GetInstance()
{
//	static ZLocale m_stLocale;
//	return &m_stLocale;
	return NULL;
}

//ZLocale::ZLocale() : CCBaseLocale(), m_bTeenMode(false), m_pAuthInfo(NULL)
//{

//}

ZLocale::~ZLocale()
{
//	if(m_pAuthInfo) delete m_pAuthInfo;
}

bool ZLocale::IsTeenMode()
{
//#ifdef _DEBUG
//	return true;
//#endif


//	if (m_pAuthInfo) return m_pAuthInfo->GetTeenMode();

	return false;
}

void ZLocale::SetTeenMode(bool bTeenMode)
{
//	if (m_pAuthInfo) m_pAuthInfo->SetTeenMode(bTeenMode);
//	else _ASSERT(0);
}

bool ZLocale::OnInit()
{
	if( !CreateAuthInfo() ) return false;

	return true;
}

bool ZLocale::ParseArguments(const char* pszArgs)
{/*
	switch (m_iCountry)
	{
	case CCC_KOREA:
		{
			cclog("LOCALE:KOREA \n");
#ifdef LOCALE_KOREA
			if ( NetmarbleParseArguments( pszArgs))
			{
				ZApplication::GetInstance()->SetLaunchMode(ZApplication::ZLAUNCH_MODE_NETMARBLE);
				return true;
			}
			else
			{
				cclog( "Netmarble Certification Failed. Shutdown. \n");
				return false;
			}
#endif // LOCALE_KOREA
		}
		break;

	case CCC_JAPAN:
		{
			cclog("LOCALE:JAPAN \n");
#ifdef LOCALE_JAPAN
			if ( GameOnJPParseArguments( pszArgs))
			{
				ZApplication::GetInstance()->SetLaunchMode(ZApplication::ZLAUNCH_MODE_GAMEON);
				return true;
			}
			else
			{
				cclog( "GameOn Certification Failed. Shutdown. \n");
				return false;
			}

#endif // LOCALE_JAPAN
		}
		break;

	case CCC_NHNUSA :
		{
#ifdef LOCALE_NHNUSA
			if( NHN_USA_ParseArgument(pszArgs) )
			{
				// ZApplication::GetInstance()->SetLaunchMode( ZApplication::ZLAUNCH_MODE_NHNUSA );
				return true;
			}
			else 
			{
				cclog( "NHN Certification Failed. Shutdown. \n");
				return false;
			}
#endif // LOCALE_NHNUSA
		}
		break;

	default:
		{
			cclog("LOCALE:UNKNOWN \n");
		}
	};
*/
	return true;
}

bool ZLocale::CreateAuthInfo()
{
//	if (m_pAuthInfo) delete m_pAuthInfo;
//	m_pAuthInfo = NULL;
#ifdef _INDEPTH_DEBUG_
	cclog("ZLocale::CreateAuthInfo() returning NULL since authentication is not a issue.\n");
#endif
/*	switch (m_iCountry)
	{
	case CCC_KOREA:
		{
#ifdef LOCALE_KOREA
			m_pAuthInfo = new ZNetmarbleAuthInfo();
#endif // LOCALE_KOREA
		}
		break;
	case CCC_JAPAN:
		{
#ifdef LOCALE_JAPAN
			m_pAuthInfo = new ZGameOnJPAuthInfo();
#endif // LOCALE_JAPAN
		}
		break;

	case CCC_NHNUSA :
		{
#ifdef LOCALE_NHNUSA
			m_pAuthInfo = new ZNHN_USAAuthInfo();
			if( 0 == m_pAuthInfo ) return false;
			//if( !((ZNHN_USAAuthInfo*)m_pAuthInfo)->InitAuth() )
			//{
			//	delete m_pAuthInfo;
			//	m_pAuthInfo = 0;
			//	return false;
			//}

			return true;
#endif 
		}
		break;
	default:
		{

		}
	};
*/
	return true;
}

void ZLocale::RouteToWebsite()
{
	if (strlen(Z_LOCALE_HOMEPAGE_URL) > 0)
	{
		ShellExecute(NULL, "open", Z_LOCALE_HOMEPAGE_URL, NULL, NULL, SW_SHOWNORMAL);

		char szMsgWarning[128]="";
		char szMsgCertFail[128]="";
//		ZTransMsg(szMsgWarning,MSG_WARNING);
//		ZTransMsg(szMsgCertFail,MSG_REROUTE_TO_WEBSITE);
		MessageBox(g_hWnd, szMsgCertFail, szMsgWarning, MB_OK);
	}
}

void ZLocale::PostLoginViaHomepage(CCUID* pAllocUID)
{
//	unsigned long nChecksum = CCGetMatchItemDescMgr()->GetChecksum();
//	unsigned long nChecksum = ZGetCCZFileChecksum(FILENAME_ZITEM_DESC);
//	unsigned long nChecksum = CCGetMatchItemDescMgr()->GetChecksum();
//	unsigned long nChecksum = ZGetCCZFileChecksum(FILENAME_ZITEM_DESC);
	unsigned long nChecksum = ZGetApplication()->GetFileListCRC();
	nChecksum = nChecksum ^ (*pAllocUID).High ^ (*pAllocUID).Low;

/*	switch (m_iCountry)
	{
#ifdef LOCALE_KOREA
	case CCC_KOREA:
		{
			char szSpareParam[3] = {1,2,0};
			ZNetmarbleAuthInfo* pAuthInfo = (ZNetmarbleAuthInfo*)m_pAuthInfo;
			ZPostNetmarbleLogin(pAuthInfo->GetAuthCookie(), pAuthInfo->GetDataCookie(), pAuthInfo->GetCpCookie(), szSpareParam, nChecksum);
		}
		break;
#endif //LOCALE_KOREA

#ifdef LOCALE_JAPAN
	case CCC_JAPAN:
		{
			char szEncryptMD5Value[ MAX_MD5LENGH ] = {0, };
			ZGetGameClient()->GetEncryptMD5HashValue( szEncryptMD5Value );

			ZGameOnJPAuthInfo* pAuth = (ZGameOnJPAuthInfo*)m_pAuthInfo;
			ZPostGameOnJPLogin( pAuth->GetString(), pAuth->GetStatIndex(), nChecksum, szEncryptMD5Value);

#ifdef _GAMEGUARD
			ZGameguard::m_IsResponseFirstGameguardAuth = false;
#endif	// _GAMEGUARD
		}
		break;
#endif	// LOCALE_JAPAN

#ifdef LOCALE_NHNUSA
	case CCC_NHNUSA :
		{
			ZNHN_USAAuthInfo* pNHNAuth = (ZNHN_USAAuthInfo*)m_pAuthInfo;
			if( 0 == pNHNAuth )
			{
				cclog( "error auth info\n" );
				return;
			}

			char szEncryptMD5Value[ MAX_MD5LENGH ] = {0, };
			ZGetGameClient()->GetEncryptMD5HashValue( szEncryptMD5Value );

			ZPostNHNUSALogin( const_cast<char*>(pNHNAuth->GetUserID().c_str())
				, const_cast<char*>(pNHNAuth->GetAuthStr())
				, nChecksum, szEncryptMD5Value);

#ifdef _GAMEGUARD
			ZGameguard::m_IsResponseFirstGameguardAuth = false;
#endif	// _GAMEGUARD
		}

		break;
#endif	// LOCALE_NHNUSA

	default:
		cclog("LoginViaHomepage - Unknown Locale \n");
		break;
	};*/
}


