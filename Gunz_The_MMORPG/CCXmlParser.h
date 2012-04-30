#pragma once
class MXmlElement;
class CCZFileSystem;

class MXmlParser
{
protected:
	virtual void ParseRoot(const char* szTagName, MXmlElement* pElement) = 0;
public:
	MXmlParser() {}
	virtual ~MXmlParser() {}
	bool ReadXml(const char* szFileName);								///< xml�κ��� ������ �д´�.
	bool ReadXml(CCZFileSystem* pFileSystem,const char* szFileName);		///< xml�κ��� ������ �д´�.
};
