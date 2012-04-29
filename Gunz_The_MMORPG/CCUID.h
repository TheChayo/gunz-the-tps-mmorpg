#pragma once
#include <map>
#include <vector>
#include <crtdbg.h>

using namespace std;

/// MAIET Unique ID
struct CCUID{
	unsigned long int	High;	///< High 4 Byte
	unsigned long int	Low;	///< High 4 Byte

	CCUID(void) { SetZero(); }
	CCUID(unsigned long int h, unsigned long int l){
		High = h;
		Low = l;
	}

	/// CCUID 0���� �ʱ�ȭ
	void SetZero(void){
		High = Low = 0;
	}
	void SetInvalid(void){
		SetZero();
	}

	/*
	/// CCUID 1��ŭ ����
	void Increase(void){
		if(Low==UINT_MAX){
			_ASSERT(High<UINT_MAX);
			High++;
			Low = 0;
		}
		else{
			Low++;
		}
	}
	/*/

	/// CCUID�� nSize��ŭ ����
	CCUID Increase(unsigned long int nSize=1){
		if(Low+nSize>UINT_MAX){
			_ASSERT(High<UINT_MAX);
			Low = nSize-(UINT_MAX-Low);
			High++;
		}
		else{
			Low+=nSize;
		}
		return *this;
	}

	/// CCUID�� ��ȿ���� �˻�, 0:0�̸� Invalid
	bool IsInvalid(void) const {
		// 0�̸� Invalid
		if(High==Low && Low==0) return true;
		return false;
	}
	/// CCUID�� ��ȿ���� �˻�, 0:0�̸� Invalid
	bool IsValid(void) const {
		// 0�̸� Invalid
		if(High==Low && Low==0) return false;
		return true;
	}

	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator > (const CCUID& a, const CCUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator >= (const CCUID& a, const CCUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>=b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator < (const CCUID& a, const CCUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator <= (const CCUID& a, const CCUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<=b.Low) return true;
		}
		return false;
	}

	/// ���� ���۷����� �����ε�
	inline CCUID& operator=(int v){
		High = 0;
		Low = v;
		return *this;
	}
	/// ���� ���۷����� �����ε�
	inline CCUID& operator=(const CCUID& a){
		High = a.High;
		Low = a.Low;
		return *this;
	}
	/// �� ���۷����� �����ε�
	inline friend bool operator==(const CCUID& a, const CCUID& b){
		if(a.High==b.High){
			if(a.Low==b.Low) return true;
		}
		return false;
	}
	/// �� ���۷����� �����ε�
	inline friend bool operator!=(const CCUID& a, const CCUID& b){
		if(a.Low!=b.Low) return true;
		if(a.High!=b.High) return true;
		return false;
	}
	/// ���� ���۷����� �����ε�
	inline friend CCUID& operator++(CCUID& a){
		a.Increase();
		return a;
	}

	/// Invalid�� UID ���
	static CCUID Invalid(void);
};

/// CCUID ����
struct CCUIDRANGE{
	CCUID	Start;
	CCUID	End;

	bool IsEmpty(void){
		return (Start==End);
	}
	void Empty(void){
		SetZero();
	}
	void SetZero(void){
		Start.SetZero();
		End.SetZero();
	}
};

/// int �ΰ������� CCUID ����
#define MAKECCUID(_high, _low)	CCUID(_high, _low)


/// CCUID Reference Map
/// - ���������� 1�� �����ϴ� ID�� �� �� �ִ� CCUID ����
class CCUIDRefMap : protected map<CCUID, void*>{
	CCUID	m_CurrentCCUID;		///< ���� �߱޵� CCUID
public:
	CCUIDRefMap(void);
	virtual ~CCUIDRefMap(void);

	/// ���۷����� CCUID�ϳ��� �Ҵ��Ѵ�.
	/// @param pRef	���۷��� ������
	/// @return		�Ҵ�� CCUID
	CCUID Generate(void* pRef);

	/// CCUID�� ���� ���۷��� �����͸� ����.
	/// @param uid	CCUID
	/// @return		���۷��� ������, CCUID�� �������� ������ NULL�� ����
	void* GetRef(CCUID& uid);

	/// ��ϵ� CCUID ����.
	/// @param uid	CCUID
	/// @return		��ϵǾ��� ���۷��� ������, CCUID�� �������� ������ NULL�� ����
	void* Remove(CCUID& uid);

	/// nSize��ŭ Ȯ��
	CCUIDRANGE Reserve(int nSize);

	/// ���ݱ��� �߱޵� UID ����
	CCUIDRANGE GetReservedCount(void);
};

/*
/// CCUID Reference Array
/// - ���� ���� �뷮�� CCUID�� �����س��� ���� CCUID ����
class CCUIDRefArray : protected vector<void*>{
	CCUID	m_CurrentCCUID;		///< ���� CCUID
public:
	CCUIDRefArray(void);
	virtual ~CCUIDRefArray(void);

	/// ���۷����� CCUID�ϳ��� �Ҵ��Ѵ�.
	/// @param pRef	���۷��� ������
	/// @return		�Ҵ�� CCUID
	CCUID Generate(void* pRef);
	/// CCUID�� ���� ���۷��� �����͸� ����.
	/// @param uid	CCUID
	/// @return		���۷��� ������, CCUID�� �������� ������ NULL�� ����
	void* GetRef(CCUID& uid);
};
*/

/// CCUID Reference Map Cache
/// - ��ü UID�� �˻� ���ϸ� ���̱� ���� ĳ�� Ŭ����
class CCUIDRefCache : public map<CCUID, void*>{
public:
	CCUIDRefCache(void);
	virtual ~CCUIDRefCache(void);

	/// ���۷����� CCUID�ϳ��� �Ҵ��Ѵ�.
	/// @param pRef	���۷��� ������
	/// @return		�Ҵ�� CCUID
	void Insert(const CCUID& uid, void* pRef);
	/// CCUID�� ���� ���۷��� �����͸� ����.
	/// @param uid	CCUID
	/// @return		���۷��� ������, CCUID�� �������� ������ NULL�� ����
	void* GetRef(const CCUID& uid);
	/// ��ϵ� CCUID ����.
	/// @param uid	CCUID
	/// @return		��ϵǾ��� ���۷��� ������, CCUID�� �������� ������ NULL�� ����
	void* Remove(const CCUID& uid);
};
