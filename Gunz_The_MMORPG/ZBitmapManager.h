#pragma once

#include "Core4R2.h"

// #��Ʈ�� �Ŵ���
// CCBitmapManager�� �ֱ� ������ ������ü ���·� ���ǰ� �ֽ��ϴ�.
// �� �Ŵ����� ������ ��Ʈ�� �׷��� �ٷ�� ���� �� �ν��Ͻ��ؼ� ����մϴ�.
// ������ ����� ǥ�� ������Ʈ �۾� �߿�,
// �ʿ��� ����� �������� �׶��׶� �ε��ϰ�, �ʿ��������(ex;����/���â���� ������) ���� ��ε��ϱ� ������
// ������ �������� �Ŵ����� �ʿ������� ��������ϴ�.

template <typename KEYTYPE>
class ZBitmapManager
{
//	typedef map<KEYTYPE, CCBitmap*>	MapBitmap;
//	typedef MapBitmap::iterator		ItorBitmap;

	map<KEYTYPE, CCBitmap*> m_mapBitmap;

public:
	ZBitmapManager() {}
	~ZBitmapManager()
	{
		Clear();
	}


	void Clear()
	{
		for (map<KEYTYPE, CCBitmap*>::iterator it=m_mapBitmap.begin(); it!=m_mapBitmap.end(); ++it)
			delete it->second;
		m_mapBitmap.clear();
	}

	// ��Ʈ���� �ؽ�ó�� �ӽ÷� �޸𸮿��� ��� ������.
	// ��Ʈ�� �뷮�� ������, �׻� �޸𸮿� �ε� ������ �ʿ�� ���� �͵��� ���� �������ִ� �뵵
	void UnloadTextureTemporarily()
	{
		for (map<KEYTYPE, CCBitmap*>::iterator it=m_mapBitmap.begin(); it!=m_mapBitmap.end(); ++it)
			((CCBitmapR2*)(it->second))->UnloadTextureTemporarily();
	}

	bool Add(KEYTYPE key, CCBitmap* pBitmap)
	{
		map<KEYTYPE, CCBitmap*>::iterator it = m_mapBitmap.find(key);
		if (it == m_mapBitmap.end()) {
			m_mapBitmap[key] = pBitmap;
			return true;
		}
		return false;
	}

	CCBitmap* Get(KEYTYPE key)
	{
		map<KEYTYPE, CCBitmap*>::iterator it = m_mapBitmap.find(key);
		if (it != m_mapBitmap.end())
			return it->second;
		return NULL;
	}

	void Delete(KEYTYPE key)
	{
		map<KEYTYPE, CCBitmap*>::iterator it = m_mapBitmap.find(key);
		if (it != m_mapBitmap.end()) {
			delete it->second;
			m_mapBitmap.erase(it);
		}
	}
};