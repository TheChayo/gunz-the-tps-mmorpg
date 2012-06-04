#include "stdafx.h"
#include "ZMap.h"
#include "ZApplication.h"
#include "CCComboBox.h"
#include "ZChannelRule.h"

#include "ZGameClient.h"

// Added R347a
#include "ZGameInterface.h"

void ZGetCurrMapPath(char* outPath)
{
// ���߿� ������ �����Ҷ����� �׳� �� �ϳ��� ���
#ifdef _QUEST


	if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		strcpy(outPath, PATH_QUEST_MAPS);
		return;
	}
#endif

	if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_QUEST)
	{
		strcpy(outPath, PATH_QUEST_MAPS);
	}
	else
	{
		strcpy(outPath, PATH_GAME_MAPS);
	}
}

bool InitMaps(CCWidget *pWidget)
{
	if(!pWidget) return false;

	CCComboBox* pCombo=(CCComboBox*)pWidget;
	pCombo->RemoveAll();

	// ����Ʈ ���� scenario.xml�� ����� �ʵ鸸 �о���δ�.
	if ((ZGetGameClient()) && ( ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType())))
	{
		LIST_SCENARIONAME* pSenarioDesc = ZApplication::GetStageInterface()->GetSenarioDesc();

		// �Ϲ� �ó������� �о�´�. 1000������ ����
		for ( int i = 1000;  ; i++)
		{
			LIST_SCENARIONAME::iterator itr = pSenarioDesc->find( i);


			// ���̸� ����
			if ( itr == pSenarioDesc->end())
				return true;


			// ������ �޺� �ڽ��� �̹� �����ϴ��� �˻�.
			CCSenarioList Scenario = (*itr).second;
			bool bFind = false;

			for ( int k = 0;  k < pCombo->GetCount();  k++)
			{
				if ( !strcmp( Scenario.m_szMapSet, pCombo->GetString( k)))
				{
					bFind = true;
					break;
				}
			}

			// �޺� �ڽ��� ������ �޺� �ڽ��� �߰�.
			if ( !bFind)
				pCombo->Add( Scenario.m_szMapSet);

		}

		return true;
	}

	CCChannelRule* pRule = ZGetChannelRuleMgr()->GetCurrentRule();
	if (pRule == NULL) {
		cclog("Init maps : no Current ChannelRule \n");
		return false;
	}

	bool bRelayMapInsert = false;
	int nMapCount = CCGetMapDescMgr()->GetMapCount();
	for( int i=0 ; i< nMapCount ; i++)
	{

//#ifdef _DEBUG	// ����׽� ���� ������
//		pCombo->Add(CCGetMapDescMgr()->GetMapName(i));
//		continue;
//#endif

		bool bDuelMode = false;
		if ( ZGetGameClient() && (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == CCMATCH_GAMETYPE_DUEL))
			bDuelMode = true;

		// �����̸��� ������ �־��ش�.
		if(!bRelayMapInsert)
		{
			for(int j=0 ; j< CCGetMapDescMgr()->GetMapCount() ; j++)
			{
				if(strcmp(CCMATCH_MAPNAME_RELAYMAP, CCGetMapDescMgr()->GetMapName(j)) == 0)
				{
					pCombo->Add(CCGetMapDescMgr()->GetMapName(j));
					bRelayMapInsert = true;
					//--nMapCount;	// �����̸��� �߰������� MapCount�� �ٿ��ش�.
					continue;
				}
			}
		}

		if ( pRule->CheckMap( CCGetMapDescMgr()->GetMapName(i), bDuelMode))
			if(strcmp(CCMATCH_MAPNAME_RELAYMAP, CCGetMapDescMgr()->GetMapName(i)) != 0)
				pCombo->Add(CCGetMapDescMgr()->GetMapName(i));
	}

	return true;
}
