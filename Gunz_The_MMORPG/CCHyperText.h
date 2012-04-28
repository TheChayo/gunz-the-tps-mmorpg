/*
	CCHyperText.h
	Programming by Joongpil Cho

	���� ���� ������ ����ϰ� ����.	�Ϲ����� �ؽ�Ʈ�� �����ϳ� [[...]]���� ��ɾ ���ؼ� �ؽ�Ʈ�� ������ 
	��Ÿ���ų� �׸��� ������ �� �ִ�.
*/
#include <crtdbg.h>
#include <stdio.h>
#include "CCList.h"

typedef enum {
	CCHTE_PLAINTEXT = 0,	//TAG���� �Ϲ� �ؽ�Ʈ. (�±� �ƴ�)
	CCHTE_STD,			//MAIET Hyper Text���� ǥ���ϴ� �±�
	CCHTE_IMAGE,			//�̹��� �±�
	CCHTE_STYLE,			//��Ÿ�� �±�
	CCHTE_LINK,			//������ ��ũ ����
	CCHTE_LINKEND,		//������ ��ũ�� ��
	CCHTE_BR,			//���� �ٷ�...
	CCHTE_DEFAULT,		//�ʱ⼳�����
}CCHT_ELEMENT;

typedef enum {
	CCHTA_TEXT = 0,		//PLAINTEXT�� ���� ����, char*�� ��� �ִ�.
	CCHTA_BACKGROUND,	//STD�� ���� ���� �÷� �� Ȥ�� �̹��� ������ �ü� �ִ�.
	CCHTA_COLOR,			//COLOR��, #���� �����ϴ� 16���� 6�ڸ�, �� 2�ڸ��� �ϳ��� ���������� ǥ���Ѵ�. (#RGB)
	CCHTA_SIZE,			//SIZE��, ������()
	CCHTA_ALIGN,			//ALIGN��, ������()
	CCHTA_TYPE,			//TYPE��, ������()
	CCHTA_SRC,			//SRC��, ���ڿ�
	CCHTA_BOLD,			//BOLD��, ������()
	CCHTA_HIGHLIGHT,
	CCHTA_HREF,			//��ũ ���۷���, ���ڿ�
	CCHTA_XMARGIN,		//�׸��� X�� ����
	CCHTA_YMARGIN,		//�׸��� Y�� ����
}CCHT_ARGUMENT;

typedef class CCHTA_IntegerArg<CCHTA_SIZE>		CCHTA_Size;
typedef class CCHTA_IntegerArg<CCHTA_ALIGN>		CCHTA_Align;
typedef class CCHTA_IntegerArg<CCHTA_TYPE>		CCHTA_Type;
typedef class CCHTA_IntegerArg<CCHTA_BOLD>		CCHTA_Bold;
typedef class CCHTA_IntegerArg<CCHTA_XMARGIN>	CCHTA_XMargin;
typedef class CCHTA_IntegerArg<CCHTA_YMARGIN>	CCHTA_YMargin;

typedef class CCHTA_ColorArg<CCHTA_COLOR>		CCHTA_Color;
typedef class CCHTA_ColorArg<CCHTA_HIGHLIGHT>	CCHTA_Highlight;

typedef class CCHTA_StringArg<CCHTA_TEXT>		CCHTA_Text;
typedef class CCHTA_StringArg<CCHTA_SRC>		CCHTA_Src;
typedef class CCHTA_StringArg<CCHTA_HREF>		CCHTA_HRef;

// ������ Text Element�� ���� ������ġ���� ��
class CCHyperTextArg {
public:
	CCHT_ARGUMENT		uId;			// ������Ʈ �ƱԸ�Ʈ
	
	CCHyperTextArg(CCHT_ARGUMENT id){
		uId = id;
	}
};

template<CCHT_ARGUMENT Arg>
class CCHTA_IntegerArg : public CCHyperTextArg {
public:
	int					nVal;

	CCHTA_IntegerArg(int value) : CCHyperTextArg(Arg){
		nVal = value;
	}
};

template<CCHT_ARGUMENT Arg>
class CCHTA_StringArg : public CCHyperTextArg {
public:
	char*				val;

	CCHTA_StringArg(char *szText):CCHyperTextArg(Arg){
		val = strdup(szText);
	}
	~CCHTA_StringArg(){
		if(val) free(val);
	}
};

template<CCHT_ARGUMENT Arg>
class CCHTA_ColorArg : public CCHyperTextArg {
public:
	sColor				sColor;

	CCHTA_ColorArg(sColor color) : CCHyperTextArg(Arg){
		sColor = color;
	}
};

class CCHTA_Background : public CCHyperTextArg {
	CCHTA_Background() : CCHyperTextArg(CCHTA_BACKGROUND){
		sColor = sColor(0,0,0);
		szPath = NULL;
	}
public:
	char*				szPath;		//�̹��� �н�
	sColor				sColor;

	CCHTA_Background(sColor color) : CCHyperTextArg(CCHTA_BACKGROUND){
		szPath = NULL;
		sColor = color;
	}

	CCHTA_Background(char *path) : CCHyperTextArg(CCHTA_BACKGROUND){
		sColor = sColor(0,0,0);
		szPath = strdup(path);
	}

	~CCHTA_Background(){
		if(szPath) free(szPath);	
	}
};




class CCHyperTextElement
{
public:
	CCHT_ELEMENT					nType;	// ������Ʈ�� Ÿ��
	CCLinkedList<CCHyperTextArg>	Args;	// ������Ʈ�� ���� ����Ʈ

	//������, �ı���
	CCHyperTextElement(CCHT_ELEMENT type){
		nType		= type;
	}

	virtual ~CCHyperTextElement(){
		Args.DeleteAll();
	}

	void Add(CCHyperTextArg* pNew){
		Args.Add(pNew);
	}
};

class CCHyperText
{
private:
	char*			m_pBuffer;			// Text Buffer, CCHyperText�� �޸𸮿� �ִ� ���븸�� �Ľ��Ѵ�.
	int				m_nLen;				// ������ ũ��
	int				bp;					// Buffer�� ������
	int				m_nOffset;			// m_szScan�� ����������
	bool			m_bTagReady;
	char			m_szScan[20480];	// Scan�� ���� ����Ǵ� ���ڿ� ������, yytext�� ������ ������ �ϴ� ���̴�.

	void			Gather(char b){ m_szScan[m_nOffset++] = b; }
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS

	int				Scan();
	bool			Parse();
	char			Input(){ return m_pBuffer[bp++]; }
	void			Unput(){ bp--; }
	void			GetDigit(char c);

public:
	CCLinkedList<CCHyperTextElement>	Elements;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	
	CCHyperText();
	virtual ~CCHyperText(){ Close(); }

	/*	
		�� Ŭ������ ���� ����Ʈ�� ���� �ʴ´�.
		������ �ؽ�Ʈ�� ���ӳ��� ����Ǿ��� ������ ������ ��ӵǾ�� �ϹǷ�...

		szTextBuffer : �ؽ�Ʈ ����
	*/
	bool Open(char *szTextBuffer);
	void Close();

	bool IsValid(){ return (m_pBuffer==NULL)?false:true; }
};

inline void CCHyperText::GetDigit(char c)
{
	for(;isdigit(c) ;c=Input()) Gather(c);
	Unput();
}

class CCHyperTextFile
{
private:
	char*	m_pBuffer;
public:
	CCHyperTextFile()
	{
		m_pBuffer = NULL;
	}

	virtual ~CCHyperTextFile()
	{
		Close();
	}

	bool Open(char *szPath);

	char* GetBuffer(){ return m_pBuffer; }

	void Close()
	{
		if(m_pBuffer == NULL)
		{
			delete m_pBuffer;
			m_pBuffer = NULL;
		}
	}
};