#include "stdafx.h"
#include "ZStringResManager.h"
#include "ZConfiguration.h"
#include "ZLocale.h"

//#define _INDEPTH_DEBUG_

void ZStringResManager::MakeInstance()
{
	_ASSERT(m_pInstance == NULL);
	m_pInstance = new ZStringResManager();
	cclog("ZStringResManager::MakeInstance()\n");
}

void ZStringResManager::ResetInstance()
{
	FreeInstance();
	MakeInstance();
}


ZStringResManager::ZStringResManager() : CCBaseStringResManager()
{

}

ZStringResManager::~ZStringResManager()
{

}

bool ZStringResManager::OnInit()
{
	// messages
	string strFileName = m_strPath + FILENAME_MESSAGES;

//	if( ZGetConfiguration()->IsComplete() && ZGetLocale()->bIsComplete() )
//	{
//		if (!m_Messages.Initialize(strFileName.c_str(), ZGetLocale()->GetLanguage(), m_pFS))
//		{
//			_ASSERT(0);
//			cclog("Error!! - Messages Initalize Failed\n");
//			return false;
//		}
//	}
//	else
//	{
//		ASSERT( 0 );
//		cclog( "'Configuration' or 'ZLocale' must be completed before 'Message'.\n" );
//		return false;
//	}

	return true;
}

const char* ZStringResManager::GetMessageStr(int nID)
{
	return m_Messages.GetStr(nID);
}