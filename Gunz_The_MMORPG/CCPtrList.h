#pragma once
/* 
	CCPtrList.h
		
		This file contains a linked list class that will take care of records.
		Classes including in this header are

		CLASSES
		--------------------------------
			- CCPtrRecord
				 \_ A single record with a pointer to the next and previous
					class

			- CCPtrList
				 \_ This is a wrapper that contains a list for all records. It has
					functions to sort the records, swap them, delete, move, set,
					and retrieve records.

		FUNCTIONS
		--------------------------------
			- CCPtrRecord
				/_ void SetPrevPointer(CCPtrRecord<_T>* pprev)
				|_ void SetNextPointer(CCPtrRecord<_T>* nnext)
				|_ _T*	GetPrevPointer()
				|_ _T*	GetNextPointer()
				\_ _T*	Get()

			- CCPtrList
				/_ bool	InsertBefore(_T* lpRecord)
				|_ bool	AddAfter(_T* lpRecord)
				|_ bool	Insert(_T* lpRecord)
				|_ bool	Add(_T* lpRecord)
				|_ bool	AddSorted(_T* lpRecord)
				|_ void	Delete()
				|_ void	Delete(int index)
				|_ void	DeleteAll()
				|_ void	DeleteRecord()
				|_ void	DeleteRecord(int index)
				|_ void	DeleteRecordAll()
				|_ int	GetCount()
				|_ int	GetIndex()	
				|_ bool	PrevRecord()
				|_ bool	NextRecord()
				|_ void	MoveRecord(int index)
				|_ void	MoveFirst()
				|_ void	MoveLast()
				|_ _T*	Get()
				|_ _T*	Get(int index)
				|_ void	Swap(int iIndex1, int iIndex2)
				|_ void	Sort()
				|_ virtual int Compare(_T* lpRecord1, _T* lpRecord2)
				\_ void	QuickSort(int first, int last)
*/
#include <crtdbg.h>			// _ASSERT

// ����Ʈ�� ������ ���ڵ�
template<class _T>
class CCPtrRecord{
	CCPtrRecord<_T>*	m_lpPrevious;
	CCPtrRecord<_T>*	m_lpNext;
public:
	_T*			m_lpContent;

	// ��� �ʱ�ȭ
	CCPtrRecord(){
		m_lpNext		=NULL;
		m_lpPrevious	=NULL;
		m_lpContent		=NULL;
	}
	// ���� ��� ����
	void SetPreviousPointer(CCPtrRecord<_T>* pprevious){
		m_lpPrevious=pprevious;
	}
	// ���� ��� ���
	CCPtrRecord<_T>* GetPreviousPointer(){
		return m_lpPrevious;
	}
	// ���� ��� ����
	void SetNextPointer(CCPtrRecord<_T>* pnext){
		m_lpNext=pnext;
	}
	// ���� ��� ���
	CCPtrRecord<_T>* GetNextPointer(){
		return m_lpNext;
	}
	// ���� ���
	_T *Get(){
		return m_lpContent;
	}
	/*
	���۷����� ��ȣ���� ����������, ������� �ʴ´�.
	_T &Get(){
		return *m_lpContent;
	}
	*/
};

// ����Ʈ Ŭ����
template<class _T>
class CCPtrList{
private:
	CCPtrRecord<_T>*		m_lpFirstRecord;	// ù��° ���ڵ�
	CCPtrRecord<_T>*		m_lpLastRecord;		// ù��° ���ڵ�
	CCPtrRecord<_T>*		m_lpCurrentRecord;	// ���� ���ڵ�
	int						m_nListCount;		// ���ڵ��� ����
	int						m_nCurrentPosition;	// ���� ��ġ

	//MString					m_lpszErrorString;	// ���� ��Ʈ��
public:
	// ���� �ʱ�ȭ
	CCPtrList();
	// ����Ʈ ����(�˾Ƽ� ���ڵ带 ��� �����Ѵ�)
	virtual ~CCPtrList();

	// ���� �޼��� ���
	//MString GetErrorMessage(){return m_lpszErrorString;}

	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	bool InsertBefore(_T *lpRecord);
	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	bool AddAfter(_T *lpRecord);
	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	bool Insert(_T *lpRecord);
	// �� �ڿ� ���ڵ� �߰�(new�� ������ ��ü�� �״�� ������.)
	bool Add(_T *lpRecord);
	// ��Ʈ�� ���鼭 �߰�(��, Compare��ƾ�� �־�� �Ѵ�.)
	bool AddSorted(_T *lpRecord);
	
	// ���� ��ġ�� ���ڵ带 ����
	void Delete();
	// ������ ��ġ�� ���ڵ带 ����
	void Delete(int iindex);
	// ��� ���ڵ� ����
	void DeleteAll();

	// Added by Kim young ho
	// ���� ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ���� 
	void DeleteRecord();
	// ������ ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ���� 
	void DeleteRecord(int iindex);
	// ��� ���ڵ� ����, but Record�� content�� �������� ���� 
	void DeleteRecordAll();

	// ���ڵ��� ���� ����
	int GetCount(){return m_nListCount;}
	// ���ڵ��� ���� ��ġ ����
	int GetIndex(){return m_nCurrentPosition;}

	////////////////////////////////////////////
	// ����Ʈ���� �����͸� �̿��� �̵��� �� ���
	// ���� ���ڵ��
	bool PreviousRecord();
	// ���� ���ڵ��
	bool NextRecord();
	// ������ ��ġ��
	void MoveRecord(int iindex);
	// �� ó������
	void MoveFirst();
	// �� ����������
	void MoveLast();

	// ���� ���ڵ��� ������ ����.
	_T *Get();

	// ������ �ε����� ���� ����.
	_T *Get(int iindex);

	// ������ �ε����� ���� ����.
	// ���۷����� ���� ��ȣ������ ������� �ʴ´�.
	//_T &operator[](int iindex);

	// �ΰ��� ���ڵ��� ���� Swap�Ѵ�.
	void Swap(int nIndex1,int nIndex2);

	// Record Compare Function. Sort�� ���� virtual�� ��¹޾ƾ� �Ѵ�.
	virtual int Compare(_T *lpRecord1,_T *lpRecord2){return -10;/*false*/}

	// Quick Sort�� �����Ѵ�.
	void Sort();
private:
	// Quick Sort�� ���� �Լ�
	void QuickSort(int first,int last);
};

// ���� �ʱ�ȭ
template<class _T>
CCPtrList<_T>::CCPtrList()
{
	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

// ����Ʈ ����(�˾Ƽ� ���ڵ带 ��� �����Ѵ�)
template<class _T>
CCPtrList<_T>::~CCPtrList()
{
	DeleteAll();

	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

template<class _T>
bool CCPtrList<_T>::InsertBefore(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL){
			//m_lpszErrorString="CCPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(m_lpCurrentRecord->GetPreviousPointer());
		ptemp->SetNextPointer(m_lpCurrentRecord);
		// ���� ���ڵ�

		// ����(��) ���ڵ�
		m_lpCurrentRecord->SetPreviousPointer(ptemp);

		if(ptemp->GetPreviousPointer()==NULL)
			m_lpFirstRecord = ptemp;
		else
			ptemp->GetPreviousPointer()->SetNextPointer(ptemp);

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return true;
}

// ���ڵ� ����
template<class _T>
bool CCPtrList<_T>::AddAfter(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL){
			//m_lpszErrorString="CCPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		// ���� ���ڵ�
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// �������� �߰��ϴ� ���̹Ƿ�
			m_lpLastRecord=ptemp;
		}

		// ����(��) ���ڵ�
		m_lpCurrentRecord->SetNextPointer(ptemp);

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;

		m_nCurrentPosition++;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return true;
}

// ���ڵ� ����
template<class _T>
bool CCPtrList<_T>::Insert(_T *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL){
			//m_lpszErrorString="CCPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CCPtrRecord<_T> *ptemp;
		ptemp=new CCPtrRecord<_T>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(_T));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		// ���� ���ڵ�
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// �������� �߰��ϴ� ���̹Ƿ�
			m_lpLastRecord=ptemp;
		}

		// ����(��) ���ڵ�
		m_lpCurrentRecord->SetNextPointer(ptemp);

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;

		m_nCurrentPosition++;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return true;
}

// ���ڵ� �߰�
template<class _T>
bool CCPtrList<_T>::Add(_T *lpRecord)
{
	MoveLast();
	return Insert(lpRecord);
}

template<class _T>
bool CCPtrList<_T>::AddSorted(_T *lpRecord)
{
	int nTotalCount = GetCount();
	if(nTotalCount==0) return Add(lpRecord);

	int nStart = 0;
	int nEnd = nTotalCount - 1;
	while(1){
		int nCount = nEnd - nStart;
		int nMiddlePos = nCount / 2;
		if(nMiddlePos<0) nMiddlePos = 0;
		int nCurrPos = nStart+nMiddlePos;
		int nCompare = Compare(Get(nCurrPos), lpRecord);
		_ASSERT(nCompare!=-10);	// Invalide Comparision
		if(nStart==nEnd){
			if(nCompare<0){
				if(nCurrPos==nTotalCount-1) return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
			else{
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}

		if( nCompare > 0 ){
			nEnd = nCurrPos - 1;
			if(nEnd<nStart){
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}
		else if( nCompare == 0 ){
			MoveRecord(nCurrPos);
			return InsertBefore(lpRecord);
		}
		else{
			nStart = nCurrPos + 1;
			if(nStart>nEnd){
				if(nCurrPos==nTotalCount-1) return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
		}
	}
}

// ���� ��ġ�� ���ڵ带 ����
template<class _T>
void CCPtrList<_T>::Delete()
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCPtrRecord<_T>* pprevious;
	CCPtrRecord<_T>* pnext;

	pprevious	=m_lpCurrentRecord->GetPreviousPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	// �� ���ڵ�
	if(pprevious!=NULL){
	// �� ���ڵ尡 ó���� �ƴϸ�...
		pprevious->SetNextPointer(pnext);
	}
	else{
	// �� ���ڵ尡 ù ���ڵ��� ���
		m_lpFirstRecord=pnext;
	}
	
	// �� ���ڵ�
	if(pnext!=NULL){
	// �� ���ڵ尡 �������� �ƴϸ�...
		pnext->SetPreviousPointer(pprevious);
	}
	else{
	// �� ���ڵ尡 �������̸�
		m_lpLastRecord=pprevious;
	}

	// ���� ���ڵ��� ���� �޸𸮿��� ����
	//_ASSERT(m_lpCurrentRecord->m_lpContent!=NULL);
	//delete m_lpCurrentRecord->m_lpContent;
	// ���� ���ڵ� �޸𸮿��� ����
	delete m_lpCurrentRecord;

	// ���� ������ ����
	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_nCurrentPosition--;
			_ASSERT(m_nCurrentPosition>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount--;
}

// ���ϴ� �ε����� ����
template<class _T>
void CCPtrList<_T>::Delete(int iindex)
{
	MoveRecord(iindex);
	Delete();
}

// ��� ���ڵ� ����
template<class _T>
void CCPtrList<_T>::DeleteAll()
{
	while(m_nListCount!=0)
		Delete();
	m_nCurrentPosition=0;
}

// ���� ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ����  
template<class _T>
void CCPtrList<_T>::DeleteRecord()
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CCPtrRecord<_T>* pprevious;
	CCPtrRecord<_T>* pnext;

	pprevious	=m_lpCurrentRecord->GetPreviousPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	// �� ���ڵ�
	if(pprevious!=NULL){
	// �� ���ڵ尡 ó���� �ƴϸ�...
		pprevious->SetNextPointer(pnext);
	}
	else{
	// �� ���ڵ尡 ù ���ڵ��� ���
		m_lpFirstRecord=pnext;
	}
	
	// �� ���ڵ�
	if(pnext!=NULL){
	// �� ���ڵ尡 �������� �ƴϸ�...
		pnext->SetPreviousPointer(pprevious);
	}
	else{
	// �� ���ڵ尡 �������̸�
		m_lpLastRecord=pprevious;
	}

	// ���� ���ڵ� �޸𸮿��� ����
	delete m_lpCurrentRecord;

	// ���� ������ ����
	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_nCurrentPosition--;
			_ASSERT(m_nCurrentPosition>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount--;
}

// ���ϴ� �ε����� ����, but Record�� content� �������� ���� 
template<class _T>
void CCPtrList<_T>::DeleteRecord(int iindex)
{
	MoveRecord(iindex);
	DeleteRecord();
}

// ��� ���ڵ� ����, but Record�� content�� �������� ���� 
template<class _T>
void CCPtrList<_T>::DeleteRecordAll()
{
	while(m_nListCount!=0)
		DeleteRecord();
	m_nCurrentPosition=0;
}


// ���� ���ڵ��
template<class _T>
bool CCPtrList<_T>::PreviousRecord()
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetPreviousPointer())!=NULL){
		m_nCurrentPosition--;
		_ASSERT(m_nCurrentPosition>=0);

		m_lpCurrentRecord=m_lpCurrentRecord->GetPreviousPointer();
		return true;
	}	
	else{
		//m_lpszErrorString="CCPtrList::PreviousRecord() - Out of Range";
		return false;
	}
}

// ���� ���ڵ��
template<class _T>
bool CCPtrList<_T>::NextRecord()
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetNextPointer())!=NULL){
		m_nCurrentPosition++;
		_ASSERT(m_nCurrentPosition<m_nListCount);

		m_lpCurrentRecord=m_lpCurrentRecord->GetNextPointer();
		return true;
	}	
	else{
		//m_lpszErrorString="CCPtrList::NextRecord() - Out of Range";
		return false;
	}
}

// ������ ��ġ��
template<class _T>
void CCPtrList<_T>::MoveRecord(int iindex)
{
	// �ε����� ������ �����ȿ� �־�� �Ѵ�.
	_ASSERT(iindex<m_nListCount);
	_ASSERT(iindex>=0);
	
	if(iindex==0)
	{
		MoveFirst();
		return;
	}
	if(iindex>m_nCurrentPosition){
		while(iindex!=m_nCurrentPosition)
			NextRecord();
	}
	else if(iindex<m_nCurrentPosition){
		while(iindex!=m_nCurrentPosition)
			PreviousRecord();
	}
}

// �� ó������
template<class _T>
void CCPtrList<_T>::MoveFirst()
{
	m_nCurrentPosition=0;
	m_lpCurrentRecord=m_lpFirstRecord;
}

// �� ����������
template<class _T>
void CCPtrList<_T>::MoveLast()
{
	if(m_nListCount>0){
		m_nCurrentPosition=m_nListCount-1;
		m_lpCurrentRecord=m_lpLastRecord;
	}
}

// ���� ���ڵ��� ������ ����.
template<class _T>
_T *CCPtrList<_T>::Get()
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	return(m_lpCurrentRecord->Get());
}

// ������ �ε����� ���� ����.
template<class _T>
_T *CCPtrList<_T>::Get(int iindex)
{
	MoveRecord(iindex);
	return Get();
}

// ������ �ε����� ���� ����.
/*
template<class _T>
_T &CCPtrList<_T>::operator[](int iindex)
{
	MoveRecord(iindex);
	return Get();
}
*/

// Quick Sort�� �����Ѵ�.
template<class _T>
void CCPtrList<_T>::Sort()
{
	if(GetCount()<=1)return;
	QuickSort(0,GetCount()-1);
}

// Quick Sort�� ���� �Լ�
template<class _T>
void CCPtrList<_T>::QuickSort(int first,int last)
{
	int i,j;

	i=first,j=last;
	_T *pMiddle=Get((first+last)/2);

	// Compare()�� virtual�� ��¹��� ������ Assetion�� �߻���Ŵ
	_ASSERT(Compare(Get(i),pMiddle)!=-10);

	for(;;){
		while(Compare(Get(i),pMiddle)<0)i++;
		while(Compare(Get(j),pMiddle)>0)j--;
		if(i>=j)break;
		Swap(i,j);
		i++;j--;
	}
	if(first<i-1)QuickSort(first,i-1);
	if(j+1<last)QuickSort(j+1,last);
}

// �ΰ��� ���ڵ��� ���� Swap�Ѵ�.
template<class _T>
void CCPtrList<_T>::Swap(int nIndex1,int nIndex2)
{
	MoveRecord(nIndex1);
	CCPtrRecord<_T>* pRecord1=m_lpCurrentRecord;
	MoveRecord(nIndex2);
	CCPtrRecord<_T>* pRecord2=m_lpCurrentRecord;

	_T *pData=pRecord1->m_lpContent;
	pRecord1->m_lpContent=pRecord2->m_lpContent;
	pRecord2->m_lpContent=pData;
}
