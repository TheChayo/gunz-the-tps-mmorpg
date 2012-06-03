#pragma once

#include "winsock2.h"
#include "windows.h"
#include <comutil.h>
#include <stdio.h>
#include <string>
using namespace std;

#define _CCSXML2


#ifdef _CCSXML2
	#import "msxml4/msxml4.dll" named_guids no_implementation
//	using namespace MSXML2;

	typedef MSXML2::IXMLDOMDocumentPtr				CCXmlDomDocPtr;
	typedef MSXML2::IXMLDOMNodePtr					CCXmlDomNodePtr;
	typedef MSXML2::IXMLDOMNodeListPtr				CCXmlDomNodeListPtr;
	typedef MSXML2::IXMLDOMElementPtr				CCXmlDomElementPtr;
	typedef MSXML2::IXMLDOMProcessingInstructionPtr CCXmlDomPIPtr;
	typedef MSXML2::IXMLDOMNamedNodeMapPtr			CCXmlDomNamedNodeMapPtr;
	typedef MSXML2::IXMLDOMTextPtr					CCXmlDomTextPtr;
	typedef MSXML2::IXMLDOMParseErrorPtr			CCXmlDomParseErrorPtr;
#else
	#import "msxml.dll" named_guids no_implementation

	typedef MSXML::IXMLDOMDocumentPtr				CCXmlDomDocPtr;
	typedef MSXML::IXMLDOMNodePtr					CCXmlDomNodePtr;
	typedef MSXML::IXMLDOMNodeListPtr				CCXmlDomNodeListPtr;
	typedef MSXML::IXMLDOMElementPtr				CCXmlDomElementPtr;
	typedef MSXML::IXMLDOMProcessingInstructionPtr	CCXmlDomPIPtr;
	typedef MSXML::IXMLDOMNamedNodeMapPtr			CCXmlDomNamedNodeMapPtr;
	typedef MSXML::IXMLDOCCTextPtr					CCXmlDomTextPtr;
	typedef MSXML::IXMLDOMParseErrorPtr				CCXmlDomParseErrorPtr;

//	using namespace MSXML;
#endif

class CCXmlDocument;

/// IXMLDOMNode ���� Ŭ����
class CCXmlNode
{
private:

protected:
	CCXmlDomNodePtr		m_pDomNode;			///< IXMLDOMNode ����Ʈ ������
public:
	/// Default constructor.
	CCXmlNode() { m_pDomNode = NULL; }
	CCXmlNode(CCXmlDomNodePtr a_pDomNode) { m_pDomNode = a_pDomNode; }
	/// Default destructor.
	virtual ~CCXmlNode() { m_pDomNode = NULL; }

	/// IXMLDOMNode�������̽��� ��ȯ
	CCXmlDomNodePtr	GetXmlDomNodePtr() { return m_pDomNode; }
	/// IXMLDOMNode�� ����
	void			SetXmlDomNodePtr(CCXmlDomNodePtr pNode) { m_pDomNode = pNode; }

	/// IXMLDOMNode�� NULL���� ����
	bool IsEmpty() { if (m_pDomNode == NULL) return true; else return false; }
	/// Node�� �̸��� ��ȯ.
	/// @param sOutStr			[out] ��ȯ��
	void GetNodeName(char* sOutStr);
	/// Node�� Text�� ��ȯ.
	/// @param sOutStr			[out] ��ȯ��
	void GetText(char* sOutStr, int nMaxCharNum = -1);
	/// Node�� Text�� ����.
	/// @param sOutStr			[in] ������ Text
	void SetText(const char* sText);
	
	/// Child Node�� ������ ��ȯ.
	int	GetChildNodeCount();
	/// Node�� Ÿ���� ��ȯ.
	DOMNodeType GetNodeType();
	/// Child Node�� �ֳ�?
	bool HasChildNodes();

	void NextSibling();
	void PreviousSibling();

	bool AppendChild(CCXmlNode node);

	/// �ش��̸��� ���� Child Node�� ã�´�.
	/// ������ NULL�� ����
	/// @param sNodeName		[in] ã�� Node�� �̸�
	bool FindChildNode(const char* sNodeName, CCXmlNode* pOutNode);

	/// �θ� Node�� ��ȯ. ������ NULL�� ��ȯ�Ѵ�.
	CCXmlNode GetParent() { if (m_pDomNode) return CCXmlNode(m_pDomNode->parentNode); else return CCXmlNode(); }
	/// �ε����� Child Node�� ��ȯ
	/// @param iIndex			[in] �ε���
	CCXmlNode GetChildNode(int iIndex);

	/// ���Ͻ��� �̿��Ͽ� �ش� ��带 ã�´�. ���� ���� ã�� ù��常 ��ȯ
	/// @param sQueryStr		[in] ���Ͻ�
	CCXmlNode SelectSingleNode(TCHAR* sQueryStr);
	/// ���ϽĿ� �´� �������� ��带 ã�´�.
	/// @param sQueryStr		[in] ���Ͻ�
	CCXmlDomNodeListPtr	SelectNodes(TCHAR* sQueryStr);

	CCXmlNode& operator= (CCXmlNode aNode) { m_pDomNode = aNode.GetXmlDomNodePtr(); return *this; }
};

/// IXMLDOMElement ���� Ŭ����
class CCXmlElement: public CCXmlNode
{
public:
	/// Default constructor.
	CCXmlElement() { }
	CCXmlElement(CCXmlDomElementPtr a_pDomElement)	{ m_pDomNode = a_pDomElement; }
	CCXmlElement(CCXmlDomNodePtr a_pDomNode)			{ m_pDomNode = a_pDomNode; }
	CCXmlElement(CCXmlNode aNode)						{ m_pDomNode = aNode.GetXmlDomNodePtr(); }
	/// Default destructor.
	virtual ~CCXmlElement() { }
	/// �±� �̸��� ��ȯ�Ѵ�.
	/// @param sOutStr			[out] �±� �̸�
	void GetTagName(char* sOutStr) { CCXmlNode::GetNodeName(sOutStr); }
	
	/// �ش��±׷� �ѷ����� Contents�� ��ȯ
	/// @param sOutStr			[out] ��ȯ��
	void GetContents(char* sOutStr) { CCXmlNode::GetText(sOutStr); }
	void GetContents(int* ipOutValue);
	void GetContents(bool* bpOutValue);
	void GetContents(float* fpOutValue);
	void GetContents(string* pstrValue);

	/// Contents�� ����
	void SetContents(const char* sStr) { CCXmlNode::SetText(sStr); }
	void SetContents(int iValue);
	void SetContents(bool bValue);
	void SetContents(float fValue);

	bool GetChildContents(char* sOutStr, const char* sChildTagName, int nMaxCharNum = -1);
	bool GetChildContents(int* iOutValue, const char* sChildTagName);
	bool GetChildContents(float* fOutValue, const char* sChildTagName);
	bool GetChildContents(bool* bOutValue, const char* sChildTagName);

	/// �Ӽ����� ��ȯ - �����˻��̶� �ð��� ���� �ɸ���.
	/// @param sOutText			[out] ��ȯ�� �Ӽ���
	/// @param sAttrName		[in] �Ӽ� �̸�
	bool GetAttribute(char* sOutText, const char* sAttrName, char* sDefaultText = "");
	bool GetAttribute(int* ipOutValue, const char* sAttrName, int nDefaultValue = 0);
	bool GetAttribute(bool* bOutValue, const char* sAttrName, bool bDefaultValue = false);
	bool GetAttribute(float* fpOutValue, const char* sAttrName, float fDefaultValue = 0.0f);
	bool GetAttribute(string* pstrOutValue, const char* sAttrName, char* sDefaultValue = "");
	/// �Ӽ��� �߰��Ѵ�.
	/// @param sAttrName		[in] �Ӽ� �̸�
	/// @param sAttrText		[in] �Ӽ���
	bool AddAttribute(const char* sAttrName, const char* sAttrText);
	bool AddAttribute(const char* sAttrName, int iAttrValue);
	bool AddAttribute(const char* sAttrName, bool bAttrValue);
	/// �Ӽ��� �缳��. �ش�Ӽ��� �������� ������ �߰��Ѵ�.
	/// @param sAttrName		[in] �Ӽ� �̸�
	/// @param sAttrText		[in] �Ӽ���
	bool SetAttribute(const char* sAttrName, char* sAttrText);
	bool SetAttribute(const char* sAttrName, int iAttrValue);
	/// �Ӽ��� �����.
	bool RemoveAttribute(const char* sAttrName);

	int GetAttributeCount();
	/// �Ӽ��� ��ȯ - ���� GetAttribute�Լ����� ������.
	void GetAttribute(int index, char* szoutAttrName, char* szoutAttrValue);

	/// �ڽ� Element�� �߰��Ѵ�.
	/// @param sTagName			[in] �±� �̸�
	/// @param sTagText			[in] �±� Contents
	bool AppendChild(const char* sTagName, const char* sTagText = NULL);
	bool AppendChild(::CCXmlElement aChildElement);

	/// �ڽ� Element�� �߰��Ѵ�.
	/// @param sTagName			[in] �±� �̸�
	CCXmlElement	CreateChildElement(const char* sTagName);

	/// �ؽ�Ʈ�� �߰��Ѵ�.
	bool AppendText(const char* sText);

	CCXmlElement& operator= (::CCXmlElement aElement) { m_pDomNode = aElement.GetXmlDomNodePtr(); return *this; }
	CCXmlElement& operator= (CCXmlNode aNode) { m_pDomNode = aNode.GetXmlDomNodePtr(); return *this; }
};

/// XML Document Ŭ����.
class CCXmlDocument
{
private:
	bool							m_bInitialized;		///< �ʱ�ȭ�Ǿ����� ����
	CCXmlDomDocPtr*					m_ppDom;			///< IXMLDOMDocument
protected:

public:
	/// Default constructor.
	CCXmlDocument();
	/// Default destructor.
	virtual ~CCXmlDocument();

	/// �ʱ�ȭ. �� �Լ��� �� Ŭ������ ����ϱ� ���� �� ����Ǿ�� �Ѵ�
	bool				Create();
	/// ������.
	bool				Destroy();

	/// XML ������ �д´�.
	bool				LoadFromFile(const char* m_sFileName);
	/// XML �޸𸮹��۷κ��� �д´�.
	bool				LoadFromMemory(char* szBuffer, LANGID lanid = LANG_KOREAN);

	/// XML ���Ϸ� ����.
	bool				SaveToFile(const char* m_sFileName);

	/// processing instruction node�� ����.
	bool				CreateProcessingInstruction( const char* szHeader = "version=\"1.0\"");
	/// Node�� ����. ���� Child Node�� ������ �Բ� �����Ѵ�.
	bool				Delete(CCXmlNode* pNode);

	CCXmlElement			CreateElement(const char* sName);

	bool				AppendChild(CCXmlNode node);

	/// XML DOM tree �ֻ��� Node�� ��ȯ.
	CCXmlDomDocPtr		GetDocument()	{ return (*m_ppDom); }
	/// �ֻ��� Element�� ��ȯ.
	CCXmlElement			GetDocumentElement()	{ return CCXmlElement((*m_ppDom)->documentElement); }

	/// �±� �̸����� Element�� ã�´�. ���� ���� ã�� ù��常 ��ȯ.
	CCXmlNode			FindElement(TCHAR* sTagName);

	bool				IsInitialized() { return m_bInitialized; }
};

// Utils
#define _BSTRToAscii(s) (const char*)(_bstr_t)(s)
BSTR _AsciiToBSTR(const char* ascii);
