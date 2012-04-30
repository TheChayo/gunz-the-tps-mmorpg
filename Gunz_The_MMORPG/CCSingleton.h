#include <cassert>

template <typename T> class CCSingleton
{
    static T* m_pSingleton;

public:
    CCSingleton( void )
    {
        assert( !m_pSingleton );

		/* 
		���߻���� �޾����� �����Ͱ� �ٸ��� ���� ������ �ذ��� ��Ŭ�� ���ø�

		gpg �� ���� ���
        int offset = (int)((T*)1) - (int)(CCSingleton <T>*)((T*)1);
        m_pSingleton = (T*)((int)this + offset);
		*/
        
		// �̰��� ǥ�ؿ� ����� ����ε�.
        m_pSingleton = static_cast<T *>(this); 

	}
   ~CCSingleton( void )
        {  assert( m_pSingleton );  m_pSingleton = 0;  }
    static T& GetInstance( void )
        {  assert( m_pSingleton );  return ( *m_pSingleton );  }
    static T* GetInstancePtr( void )
        {  return ( m_pSingleton );  }
};

template <typename T> T* CCSingleton <T>::m_pSingleton = 0;