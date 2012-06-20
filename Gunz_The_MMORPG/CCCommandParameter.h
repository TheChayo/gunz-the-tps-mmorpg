#ifndef ASCOMMANDPARAMETER_H
#define ASCOMMANDPARAMETER_H

#include "CCUID.h"
#include "CCTypes.h"
#include "mempool.h"
#define _CRT_SECURE_NO_WARNINGS
class CCCommandParamCondition;
class CCCommandParamConditionMinMax;

/// Ŀ�ǵ� �Ķ���� Ÿ��
enum CCCommandParameterType{
	MPT_INT		= 0,
	MPT_UINT	= 1,
	MPT_FLOAT	= 2,
	MPT_BOOL	= 3,
	MPT_STR		= 4,
	MPT_VECTOR	= 5,
	MPT_POS		= 6,
	MPT_DIR		= 7,
	MPT_COLOR	= 8,
	MPT_UID		= 9,
	MPT_BLOB	= 10,

	MPT_CHAR	= 11,
	MPT_UCHAR	= 12,
	MPT_SHORT	= 13,
	MPT_USHORT	= 14,
	MPT_INT64	= 15,
	MPT_UINT64	= 16,
	// ���ڴ� ������ ���̴�. (Ȯ���� �����ϵ� ������ �Ұ�, ���� ���÷��� ����)

	MPT_SVECTOR	= 17,
	MPT_END		= 18,		///< �Ķ���� Ÿ�� �� ����
};

#define MAX_BLOB_SIZE		(0x100000)			// 1�ް�����Ʈ




/// Command Parameter Description
class CCCommandParameterDesc{
protected:
	CCCommandParameterType				m_nType;
	char								m_szDescription[64];
	vector<CCCommandParamCondition*>		m_Conditions;
	void								InitializeConditions();
public:
	CCCommandParameterDesc(CCCommandParameterType nType, char* szDescription);
	virtual ~CCCommandParameterDesc();

	CCCommandParameterType GetType(){ return m_nType; }
	const char* GetDescription(){ return m_szDescription; }

	void AddCondition(CCCommandParamCondition* pCondition);
	bool HasConditions() { return (!m_Conditions.empty()); }
	int GetConditionCount() { return (int)m_Conditions.size(); }
	CCCommandParamCondition* GetCondition(int n) { return m_Conditions[n]; }

};


/// Command Parameter Abstract Class
class CCCommandParameter{
protected:
	CCCommandParameterType	m_nType;
public:
	CCCommandParameter(CCCommandParameterType nType){ m_nType = nType; }
	virtual ~CCCommandParameter(){}

	CCCommandParameterType GetType(){ return m_nType; }

	/// ���� �Ķ��Ƽ Ÿ������ ����
	virtual CCCommandParameter* Clone() = 0;
	/// �� ����
	virtual void GetValue(void* p) = 0;
	/// �޸� ������ ����
	virtual int GetData(char* pData, int nSize) = 0;
	/// �޸� ������ ����
	virtual int SetData(char* pData) = 0;
	/// ù ������ ���
	virtual void *GetPointer() =0; 

	/// Ÿ�� �̸� ���
	virtual const char* GetClassName() = 0;
	/// ��Ʈ������ �� ���
	virtual void GetString(char* szValue) = 0;
	/// ������ ���
	virtual int GetSize() = 0;
};

/// ���� �Ķ����
class CCCommandParameterInt : public CCCommandParameter, public MemPool<CCCommandParameterInt> {
public:
	int		m_Value;
public:
	CCCommandParameterInt();
	CCCommandParameterInt(int Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "Int"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d", m_Value); }
	virtual int GetSize() { return sizeof(int); }
};

/// ���
class CCCommandParameterUInt : public CCCommandParameter, public MemPool<CCCommandParameterUInt> {
public:
	unsigned int		m_Value;
public:
	CCCommandParameterUInt();
	CCCommandParameterUInt(unsigned int Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "UInt"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u", m_Value); }
	virtual int GetSize() { return sizeof(unsigned int); }
};

/// �Ҽ� �Ķ����
class CCCommandParameterFloat : public CCCommandParameter, public MemPool<CCCommandParameterFloat> {
public:
	float	m_Value;
public:
	CCCommandParameterFloat();
	CCCommandParameterFloat(float Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "Float"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%f", m_Value); }
	virtual int GetSize() { return sizeof(float); }
};

/// ��Ʈ�� �Ķ����(65533������ ����)
class CCCommandParameterString : public CCCommandParameter{
public:
	char*	m_Value;
public:
	CCCommandParameterString();
	CCCommandParameterString(const char* Value);
	virtual ~CCCommandParameterString();

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "String"; }
	virtual void GetString(char* szValue)
	{ 
		// ���Լ��� �ݵ�� szValue������ ���̰� m_Value�� ���̺��� ���� ��.
		// �߿��� �κп� ���ɽô� ����˻簡 ���� �̷������ ��. - by �߱���.
		if( 0 != szValue )
		{
			if( 0 != m_Value )
				strcpy(szValue, m_Value); 
			else 
				strcpy(szValue, "\0" );
		}
	}
	virtual int GetSize();
};

/// 3D ���� �Ķ����
class CCCommandParameterVector : public CCCommandParameter {
public:
	float	m_fX;
	float	m_fY;
	float	m_fZ;
public:
	CCCommandParameterVector();
	CCCommandParameterVector(float x ,float y, float z);
	virtual ~CCCommandParameterVector();

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_fX; }
	virtual const char* GetClassName(){ return "Vector"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%.2f,%.2f,%.2f", m_fX, m_fY, m_fZ); }
	virtual int GetSize() { return (sizeof(float)*3); }
};

/// 3D ������ �Ķ����
class CCCommandParameterPos : public CCCommandParameterVector, public MemPool<CCCommandParameterPos> {
public:
	CCCommandParameterPos() : CCCommandParameterVector() { m_nType=MPT_POS; }
	CCCommandParameterPos(float x, float y, float z) : CCCommandParameterVector(x, y, z){ m_nType=MPT_POS; }
	virtual ~CCCommandParameterPos() { }

	virtual CCCommandParameter* Clone(){ return new CCCommandParameterPos(m_fX, m_fY, m_fZ); }
	virtual const char* GetClassName(){ return "Pos"; }
};

/// 3D �𷺼� �Ķ����
class CCCommandParameterDir : public CCCommandParameterVector, public MemPool<CCCommandParameterDir> {
public:
	CCCommandParameterDir() : CCCommandParameterVector() { m_nType=MPT_DIR; }
	CCCommandParameterDir(float x, float y, float z) : CCCommandParameterVector(x, y, z){ m_nType=MPT_DIR; }
	virtual ~CCCommandParameterDir() { }

	virtual CCCommandParameter* Clone(){ return new CCCommandParameterDir(m_fX, m_fY, m_fZ); }
	virtual const char* GetClassName(){ return "Dir"; }
};

/// RGB �÷� �Ķ����(���߿� Alpha�� �߰��� ����)
class CCCommandParameterColor : public CCCommandParameterVector, public MemPool<CCCommandParameterColor> {
public:
	CCCommandParameterColor() : CCCommandParameterVector() { m_nType=MPT_COLOR; }
	CCCommandParameterColor(float r, float g, float b) : CCCommandParameterVector(r, g, b){ m_nType=MPT_COLOR; }
	virtual ~CCCommandParameterColor() { }

	virtual CCCommandParameter* Clone(){ return new CCCommandParameterColor(m_fX, m_fY, m_fZ); }
	virtual const char* GetClassName(){ return "Color"; }
};

/// Bool �Ķ����
class CCCommandParameterBool : public CCCommandParameter, public MemPool<CCCommandParameterBool> {
	bool	m_Value;
public:
	CCCommandParameterBool() : CCCommandParameter(MPT_BOOL) { }
	CCCommandParameterBool(bool bValue) : CCCommandParameter(MPT_BOOL) {
		m_Value = bValue;
	}

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer(); 
	virtual const char* GetClassName(){ return "Bool"; }
	virtual void GetString(char* szValue){ if(m_Value==true) strcpy(szValue, "true"); else strcpy(szValue, "false"); }
	virtual int GetSize() { return sizeof(bool); }
};

/// CCUID �Ķ����
class CCCommandParameterUID : public CCCommandParameter, public MemPool<CCCommandParameterUID> {
public:
	CCUID	m_Value;
public:
	CCCommandParameterUID();
	CCCommandParameterUID(const CCUID& uid);
	virtual ~CCCommandParameterUID();

	virtual CCCommandParameterUID* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "UID"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u:%u", m_Value.High, m_Value.Low); }
	virtual int GetSize() { return sizeof(CCUID); }
};

class CCCommandParameterBlob : public CCCommandParameter{
public:
	void*	m_Value;
	unsigned int	m_nSize;
public:
	CCCommandParameterBlob();
	CCCommandParameterBlob(const void* Value, int nSize);
	virtual ~CCCommandParameterBlob();

	virtual CCCommandParameterBlob* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return m_Value; }
	virtual const char* GetClassName(){ return "Blob"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%02X%02X..", *((unsigned char*)(m_Value)), *((unsigned char*)(m_Value)+1)); }
	virtual int GetSize();
};


/// char�� �Ķ����
class CCCommandParameterChar : public CCCommandParameter, public MemPool<CCCommandParameterChar>
{
public:
	char	m_Value;
public:
	CCCommandParameterChar();
	CCCommandParameterChar(char Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "Char"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d", m_Value); }
	virtual int GetSize() { return sizeof(char); }
};


/// unsigned char�� �Ķ����
class CCCommandParameterUChar : public CCCommandParameter, public MemPool<CCCommandParameterUChar>
{
public:
	unsigned char	m_Value;
public:
	CCCommandParameterUChar();
	CCCommandParameterUChar(unsigned char Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "UChar"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u", m_Value); }
	virtual int GetSize() { return sizeof(unsigned char); }
};


/// short�� �Ķ����
class CCCommandParameterShort : public CCCommandParameter, public MemPool<CCCommandParameterShort>
{
public:
	short	m_Value;
public:
	CCCommandParameterShort();
	CCCommandParameterShort(short Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "Short"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d", m_Value); }
	virtual int GetSize() { return sizeof(short); }
};

/// unsigned short�� �Ķ����
class CCCommandParameterUShort : public CCCommandParameter, public MemPool<CCCommandParameterUShort>
{
public:
	unsigned short	m_Value;
public:
	CCCommandParameterUShort();
	CCCommandParameterUShort(unsigned short Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "UShort"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%u", m_Value); }
	virtual int GetSize() { return sizeof(unsigned short); }
};


/// int64�� �Ķ����
class CCCommandParameterInt64 : public CCCommandParameter, public MemPool<CCCommandParameterInt64>
{
public:
	int64	m_Value;
public:
	CCCommandParameterInt64();
	CCCommandParameterInt64(int64 Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "Int64"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%lld", m_Value); }
	virtual int GetSize() { return sizeof(int64); }
};

/// unsigned int64�� �Ķ����
class CCCommandParameterUInt64 : public CCCommandParameter, public MemPool<CCCommandParameterUInt64>
{
public:
	uint64	m_Value;
public:
	CCCommandParameterUInt64();
	CCCommandParameterUInt64(uint64 Value);

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_Value; }
	virtual const char* GetClassName(){ return "UInt64"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%llu", m_Value); }
	virtual int GetSize() { return sizeof(uint64); }
};


/// short�� 3D ���� �Ķ����
class CCCommandParameterShortVector : public CCCommandParameter, public MemPool<CCCommandParameterShortVector> {
public:
	short	m_nX;
	short	m_nY;
	short	m_nZ;
public:
	CCCommandParameterShortVector();
	CCCommandParameterShortVector(short x ,short y, short z);
	CCCommandParameterShortVector(float x ,float y, float z);	///< ���ο��� short�� ��ȯ���ش�.
	virtual ~CCCommandParameterShortVector();

	virtual CCCommandParameter* Clone();
	virtual void GetValue(void* p);
	virtual int GetData(char* pData, int nSize);
	virtual int SetData(char* pData);
	virtual void *GetPointer() { return &m_nX; }
	virtual const char* GetClassName(){ return "ShortVector"; }
	virtual void GetString(char* szValue){ sprintf(szValue, "%d,%d,%d", m_nX, m_nY, m_nZ); }
	virtual int GetSize() { return (sizeof(short)*3); }
};


/// Command Parameter Condition Abstract Class
class CCCommandParamCondition
{
public:
	CCCommandParamCondition() {}
	virtual ~CCCommandParamCondition() {}
	virtual bool Check(CCCommandParameter* pCP) = 0;
};

class CCCommandParamConditionMinMax : public CCCommandParamCondition
{
private:
	int m_nMin;
	int m_nMax;
public:
	CCCommandParamConditionMinMax(int nMin, int nMax) : m_nMin(nMin), m_nMax(nMax) {}
	virtual ~CCCommandParamConditionMinMax() {}
	virtual bool Check(CCCommandParameter* pCP);
};



// Short Name
typedef CCCommandParameterBlob			CCCmdParamBlob;
typedef	CCCommandParameterUID			CCCmdParamCCUID;
typedef CCCommandParameter				CCCmdParam;
typedef CCCommandParameterDesc			CCCmdParamDesc;
typedef CCCommandParameterInt			CCCmdParamInt;
typedef CCCommandParameterUInt			CCCmdParamUInt;
typedef CCCommandParameterFloat			CCCmdParamFloat;
typedef CCCommandParameterString		CCCmdParamStr;
typedef CCCommandParameterVector		CCCmdParamVector;
typedef CCCommandParameterPos			CCCmdParamPos;
typedef CCCommandParameterDir			CCCmdParamDir;
typedef CCCommandParameterColor			CCCmdParamColor;
typedef CCCommandParameterBool			CCCmdParamBool;
typedef CCCommandParameterChar			CCCmdParamChar;
typedef CCCommandParameterUChar			CCCmdParamUChar;
typedef CCCommandParameterShort			CCCmdParamShort;
typedef CCCommandParameterUShort			CCCmdParamUShort;
typedef CCCommandParameterInt64			CCCmdParamInt64;
typedef CCCommandParameterUInt64			CCCmdParamUInt64;
typedef CCCommandParameterShortVector	CCCmdParamShortVector;





#endif
