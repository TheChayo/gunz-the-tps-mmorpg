#include "stdafx.h"
#include "ZNHN_USA_Poll.h"
#include "HanPollForClient.h"
#include "CCDebug.h"


#ifndef _DEBUG
//#pragma comment( lib, "HanPollForClient.lib")
#else
#pragma comment( lib, "HanPollForClientD.lib")
#endif



ZNHNUSA_Poll::ZNHNUSA_Poll()
{
	bInitialized = false;
}


bool ZNHNUSA_Poll::ZHanPollInit( const char* pszGameId, const char* pszMemberId, int nServiceCode)
{
	if ( bInitialized == true)
	{
		_ASSERT( 0);
		return false;
	}


	int nRetVal = HanPollInit( const_cast<char*>(pszGameId), const_cast<char*>(pszMemberId), nServiceCode);
	if ( nRetVal < 0)
	{
		cclog( "Poll init fail : error code = %d \n", nRetVal);
		return false;
	}

	bInitialized = true;

	return true;
}


bool ZNHNUSA_Poll::ZHanPollInitGameString( const char* pszGameString)
{
	if ( bInitialized == true)
	{
		_ASSERT( 0);
		return false;
	}

#ifdef _DEBUG
	cclog( "Game String = %s\n", pszGameString);
#endif

	int nRetVal = HanPollInitGameString( const_cast<char*>( pszGameString));
	if ( nRetVal < 0)
	{
		cclog( "Poll init fail : error code = %d \n", nRetVal);
		return false;
	}

	bInitialized = true;

	return true;
}


int ZNHNUSA_Poll::ZHanPollProcess( const char* pszData)
{
	if ( bInitialized == false)
		return 0;

	int nRetVal = HanPollProcess( const_cast<char*>( pszData));
	cclog( "Return value = %d\n", nRetVal);

	return nRetVal;
}
