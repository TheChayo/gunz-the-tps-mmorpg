#include "stdafx.h"
#include "CCQuestMap.h"
#include "CCZFileSystem.h"
#include "CCDebug.h"

/////////////////////////////////////////////////
CCQuestMapCatalogue::CCQuestMapCatalogue()
{

}

CCQuestMapCatalogue::~CCQuestMapCatalogue() 
{
	Clear();
}

CCQuestMapsetInfo* CCQuestMapCatalogue::GetMapsetInfo(int nMapset)
{
	CCQuestMapsetMap::iterator itor = m_MapsetInfo.find(nMapset);
	if (itor != m_MapsetInfo.end())
	{
		return (*itor).second;
	}

	_ASSERT(0);
	return NULL;
}

CCQuestMapSectorInfo* CCQuestMapCatalogue::GetSectorInfo(int nSector)
{
	CCQuestMapSectorMap::iterator itor = m_SectorInfo.find(nSector);
	if (itor != m_SectorInfo.end())
	{
		return (*itor).second;
	}

 	_ASSERT(0);
	return NULL;
}

CCQuestMapSectorInfo* CCQuestMapCatalogue::GetSectorInfoFromName(char* szSectorTitle)
{
	// sector
	for (CCQuestMapSectorMap::iterator itor = m_SectorInfo.begin(); itor != m_SectorInfo.end(); ++itor)
	{
		CCQuestMapSectorInfo* pSector = (*itor).second;
		if (!stricmp(pSector->szTitle, szSectorTitle))
		{
			return pSector;
		}
	}

	return NULL;
}

void CCQuestMapCatalogue::Clear()
{
	// mapset
	for (CCQuestMapsetMap::iterator itor = m_MapsetInfo.begin(); itor != m_MapsetInfo.end(); ++itor)
	{
		delete (*itor).second;
	}

	m_MapsetInfo.clear();


	// sector
	for (CCQuestMapSectorMap::iterator itor = m_SectorInfo.begin(); itor != m_SectorInfo.end(); ++itor)
	{
		delete (*itor).second;
	}

	m_SectorInfo.clear();
}



void CCQuestMapCatalogue::InsertMapset(CCQuestMapsetInfo* pMapset)
{
	int nID = pMapset->nID;

	CCQuestMapsetMap::iterator itor = m_MapsetInfo.find(nID);
	if (itor != m_MapsetInfo.end())
	{
		// �̹� �����Ѵ�.
		_ASSERT(0);
		return;
	}

	m_MapsetInfo.insert(CCQuestMapsetMap::value_type(nID, pMapset));
}

void CCQuestMapCatalogue::InsertSector(CCQuestMapSectorInfo* pSector)
{
	int nID = pSector->nID;

	CCQuestMapSectorMap::iterator itor = m_SectorInfo.find(nID);
	if (itor != m_SectorInfo.end())
	{
		// �̹� �����Ѵ�.
		_ASSERT(0);
		return;
	}


	m_SectorInfo.insert(CCQuestMapSectorMap::value_type(nID, pSector));
}

///////////////////////////////////////////////////////////////////////////////
#define MTOK_QUESTMAP_TAG_MAPSET					"MAPSET"
#define MTOK_QUESTMAP_TAG_SECTOR					"SECTOR"
#define MTOK_QUESTMAP_TAG_LINK						"LINK"
#define MTOK_QUESTMAP_TAG_TARGET					"TARGET"

#define MTOK_QUESTMAP_ATTR_ID						"id"
#define MTOK_QUESTMAP_ATTR_TITLE					"title"
#define MTOK_QUESTMAP_ATTR_SECTOR					"sector"
#define MTOK_QUESTMAP_ATTR_NAME						"name"
#define MTOK_QUESTMAP_ATTR_MELEE_SPAWN				"melee_spawn"
#define MTOK_QUESTMAP_ATTR_RANGE_SPAWN				"range_spawn"
#define MTOK_QUESTMAP_ATTR_BOSS_SPAWN				"boss_spawn"


void CCQuestMapCatalogue::ParseMapset(::CCXmlElement& element)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];


	CCQuestMapsetInfo* pMapsetInfo = new CCQuestMapsetInfo();
	int nAttrCount = element.GetAttributeCount();
	for (int i = 0; i < nAttrCount; i++)
	{
		element.GetAttribute(i, szAttrName, szAttrValue);
		if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_ID))
		{
			pMapsetInfo->nID = atoi(szAttrValue);
		}
		else if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_TITLE))
		{
			strcpy(pMapsetInfo->szTitle, szAttrValue);
		}
	}

	// sector ����� �̸� �д´�.
	ParseMapsetSector1Pass(element, pMapsetInfo);


	int nChildCount = element.GetChildNodeCount();
	::CCXmlElement chrElement;
	for (int i = 0; i < nChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_QUESTMAP_TAG_SECTOR))
		{
			int nAttrCount = chrElement.GetAttributeCount();
			int nSectorID = -1;
			for (int j = 0; j < nAttrCount; j++)
			{
				chrElement.GetAttribute(j, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_ID))
				{
					nSectorID = atoi(szAttrValue);
					break;
				}
			}

			CCQuestMapSectorInfo* pSector = GetSectorInfo(nSectorID);
			if (pSector)
			{
				ParseSector(chrElement, pSector);
			}
		}
	}	


	InsertMapset(pMapsetInfo);
}

void CCQuestMapCatalogue::ParseSector(::CCXmlElement& element, CCQuestMapSectorInfo* pSector)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];


	int nChildCount = element.GetChildNodeCount();

	::CCXmlElement chrElement;
	for (int i = 0; i < nChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_QUESTMAP_TAG_LINK))
		{
			int nLinkIndex = pSector->nLinkCount;

			int nLinkAttrCount = chrElement.GetAttributeCount();
			for (int j = 0; j < nLinkAttrCount; j++)
			{
				chrElement.GetAttribute(j, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_NAME))
				{
					strcpy(pSector->Links[nLinkIndex].szName, szAttrValue);
				}
			}


			int nLinkChildCount = chrElement.GetChildNodeCount();
			::CCXmlElement elementTarget;
			char szTargetTagName[128];

			for (int j = 0; j < nLinkChildCount; j++)
			{
				elementTarget = chrElement.GetChildNode(j);
				elementTarget.GetTagName(szTargetTagName);
				if (szTargetTagName[0] == '#') continue;

				if (!stricmp(szTargetTagName, MTOK_QUESTMAP_TAG_TARGET))
				{
					elementTarget.GetAttribute(szAttrValue, MTOK_QUESTMAP_ATTR_SECTOR, "");
					CCQuestMapSectorInfo* pTargetSector = GetSectorInfoFromName(szAttrValue);
					if (pTargetSector)
					{
						pSector->Links[nLinkIndex].vecTargetSectors.push_back(pTargetSector->nID);
					}
				}
			}

			pSector->nLinkCount++;

			// ��ũ���� 10���� ������ �ȵȴ�.
			_ASSERT(pSector->nLinkCount <= MAX_SECTOR_LINK);
		}
	}	

}

void CCQuestMapCatalogue::ParseMapsetSector1Pass(::CCXmlElement& elementMapset, CCQuestMapsetInfo* pMapset)
{
	char szTemp[256]="";
	int n = 0;
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];


	int nChildCount = elementMapset.GetChildNodeCount();

	::CCXmlElement chrElement;
	for (int i = 0; i < nChildCount; i++)
	{
		chrElement = elementMapset.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_QUESTMAP_TAG_SECTOR))
		{
			CCQuestMapSectorInfo* pSectorInfo = new CCQuestMapSectorInfo();

			int nAttrCount = chrElement.GetAttributeCount();
			for (int j = 0; j < nAttrCount; j++)
			{
				chrElement.GetAttribute(j, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_ID))
				{
					pSectorInfo->nID = atoi(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_TITLE))
				{
					strcpy(pSectorInfo->szTitle, szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_MELEE_SPAWN))
				{
					pSectorInfo->nSpawnPointCount[MNST_MELEE] = atoi(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_RANGE_SPAWN))
				{
					pSectorInfo->nSpawnPointCount[MNST_RANGE] = atoi(szAttrValue);
				}
				else if (!stricmp(szAttrName, MTOK_QUESTMAP_ATTR_BOSS_SPAWN))
				{
					pSectorInfo->nSpawnPointCount[MNST_BOSS] = atoi(szAttrValue);
					if (pSectorInfo->nSpawnPointCount[MNST_BOSS] > 0) pSectorInfo->bBoss = true;
				}

			}

			InsertSector(pSectorInfo);

			pMapset->vecSectors.push_back(pSectorInfo->nID);
		}
	}	
}

bool CCQuestMapCatalogue::ReadXml(const char* szFileName)
{
	CCXmlDocument xmlIniData;

	xmlIniData.Create();

	if (!xmlIniData.LoadFromFile(szFileName)) {
		xmlIniData.Destroy();
		return false;
	}

	::CCXmlElement rootElement, chrElement, attrElement;

	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_QUESTMAP_TAG_MAPSET))
		{
			ParseMapset(chrElement);
		}
	}

	xmlIniData.Destroy();

	InitBackLinks();


	return true;
}

bool CCQuestMapCatalogue::ReadXml(CCZFileSystem* pFileSystem,const char* szFileName)
{
	CCXmlDocument	xmlIniData;
	xmlIniData.Create();

	char *buffer;
	CCZFile mzf;

	if(pFileSystem) {
		if(!mzf.Open(szFileName,pFileSystem))  {
			if(!mzf.Open(szFileName))  {
				xmlIniData.Destroy();
				return false;
			}
		}
	} 
	else  {

		if(!mzf.Open(szFileName)) {

			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	if(!xmlIniData.LoadFromMemory(buffer)) {
		xmlIniData.Destroy();
		return false;
	}

	delete[] buffer;
	mzf.Close();


	::CCXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++) {

		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);

		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, MTOK_QUESTMAP_TAG_MAPSET)) {
			ParseMapset(chrElement);
		}
	}

	xmlIniData.Destroy();

	InitBackLinks();

	return true;
}


void CCQuestMapCatalogue::DebugReport()
{
	FILE* fp = fopen("report_questmap.txt", "wt");
	if (fp == NULL) return;

	for (CCQuestMapsetMap::iterator itor = m_MapsetInfo.begin(); itor != m_MapsetInfo.end(); ++itor)
	{
		CCQuestMapsetInfo* pMapset = (*itor).second;
		fprintf(fp, " + <MAPSET> %s (%d)\n", pMapset->szTitle, pMapset->nID);

		for (int i = 0; i < (int)pMapset->vecSectors.size(); i++)
		{
			CCQuestMapSectorInfo* pSector = GetSectorInfo(pMapset->vecSectors[i]);
			if (pSector)
			{
				fprintf(fp, "  <SECTOR> %s (%d)\n", pSector->szTitle, pSector->nID);

				for (int j = 0; j < pSector->nLinkCount; j++)
				{
					fprintf(fp, "    <LINK> %s\n", pSector->Links[j].szName);

					for (int k = 0; k < (int)pSector->Links[j].vecTargetSectors.size(); k++)
					{
						CCQuestMapSectorInfo* pTargetSector = 
							GetSectorInfo(pSector->Links[j].vecTargetSectors[k]);
						if (pTargetSector)
						{
							fprintf(fp, "      <TARGET> %s\n", pTargetSector->szTitle);
						}
					}
				}
			}
		}
		fprintf(fp, "\n\n");
	}

	fclose(fp);
}


void CCQuestMapCatalogue::InitBackLinks()
{
	for (CCQuestMapSectorMap::iterator itorA = m_SectorInfo.begin(); itorA != m_SectorInfo.end(); ++itorA)
	{
		CCQuestMapSectorInfo* pSectorA = (*itorA).second;

		for (CCQuestMapSectorMap::iterator itorB = m_SectorInfo.begin(); itorB != m_SectorInfo.end(); ++itorB)
		{
			CCQuestMapSectorInfo* pSectorB = (*itorB).second;
			if (pSectorA == pSectorB) continue;

			for (int i = 0; i < pSectorB->nLinkCount; i++)
			{
				int target_count = (int)pSectorB->Links[i].vecTargetSectors.size();
				for (int j = 0; j < target_count; j++)
				{
					if (pSectorB->Links[i].vecTargetSectors[j] == pSectorA->nID)
					{
						CCQuestSectorBacklink backlink;
						backlink.nSectorID = pSectorB->nID;
						backlink.nLinkIndex = i;
						pSectorA->VecBacklinks.push_back(backlink);
					}
				}
			}
		}

	}
	



}

bool CCQuestMapCatalogue::IsHacked()
{
	// �����̹����� ������ ���� Ư�� ���� ������ �ٲ������ ��츦 ���� ������� ���� (����Ʈ�� �̷��� �� ���� ����)
	// ���� ���� �ι� �����ϰ� �ִ°�? �� Ȯ���Ͽ� Ž������.. �������θ� ���Ƶ� ���� �ʼ����� �����ϴ� �ǹ̰� ��������.
	// �ٸ� ���̸��� �״�� �ΰ� �ʸ��ҽ��� �ٲ������ ������ ��ŷ�ϸ� ��å�� ����.

	set<string> setTitle;

	CCQuestMapSectorMap::iterator itSector;
	for (itSector=m_SectorInfo.begin(); itSector!=m_SectorInfo.end(); ++itSector)
	{
		const char* szTitle = itSector->second->szTitle;

		// �¿� �̸��� ã�ƺ��� ������ ����ִ´�. ���� ������ ��ŷ
		if (setTitle.end() != setTitle.find(string(szTitle)))
			return true;
		setTitle.insert(string(szTitle));
	}

	return false;
}