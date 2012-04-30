#include "stdafx.h"
#include "CCXml.h"
#include "CCZFileSystem.h"
#include "ZChannelRule.h"
#include "ZApplication.h"
#include "ZGameClient.h"


ZChannelRuleMgr::ZChannelRuleMgr() : MChannelRuleMgr()
{

}

ZChannelRuleMgr::~ZChannelRuleMgr()
{

}

ZChannelRuleMgr* ZChannelRuleMgr::GetInstance()
{
	static ZChannelRuleMgr g_ChannelRuleMgr;
	return &g_ChannelRuleMgr;
}

MChannelRule* ZChannelRuleMgr::GetCurrentRule()
{
	return GetRule(ZGetGameClient()->GetChannelRuleName());
}

