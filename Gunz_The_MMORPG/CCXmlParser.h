#pragma once
class CCXmlElement;
class CCZFileSystem;

class CCXmlParser
{
protected:
	virtual void ParseRoot(const char* szTagName, CCXmlElement* pElement) = 0;
public:
	CCXmlParser() {}
	virtual ~CCXmlParser() {}
	bool ReadXml(const char* szFileName);								///< xml�κ��� ������ �д´�.
	bool ReadXml(CCZFileSystem* pFileSystem,const char* szFileName);		///< xml�κ��� ������ �д´�.
};
