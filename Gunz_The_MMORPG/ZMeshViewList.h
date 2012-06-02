#ifndef ZMESHVIEWLIST_H
#define ZMESHVIEWLIST_H

#include "CCGroup.h"
#include "RMesh.h"

//class CCButton;
class CCBmButton;
class ZMeshView;

// �������� �׸��� ���� CCGroup�� ��ӹ޴´�.
class ZMeshViewList : public CCGroup{
protected:
	int	m_nItemStartIndex;	// ������ ���� �ε���
	int	m_nItemWidth;		// ������ ���� ũ��
	//CCButton*	m_pLeft;	// ���� �̵�
	//CCButton*	m_pRight;	// ������ �̵�
	CCBmButton* m_pBmLeft;
	CCBmButton* m_pBmRight;
	//list<ZMeshView*>	m_Items;
	float m_ScrollButtonWidth;

protected:
	virtual void OnDraw(CCDrawContext* pDC);
	virtual void OnSize(int w, int h);
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);
	int GetItemVisibleWidth();
	int GetItemWidth();
	int GetVisibleCount();
	void RecalcBounds();
public:
	ZMeshViewList(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZMeshViewList();

	int GetItemCount();
	ZMeshView* GetItem(int i);

	void Add(RealSpace2::RMesh* pMeshRef);
	void RemoveAll();
	void Remove(int i);

	void SetItemWidth(int nWidth);
};

#endif