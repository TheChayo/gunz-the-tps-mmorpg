#pragma once
#include <list>
#include <vector>
#include "RTypes.h"
#include "RNameSpace.h"
using namespace std;

class CCZFileSystem;

_NAMESPACE_REALSPACE2_BEGIN

class RPathNode;

class RPath {
public:
	int		nEdge;				// ������ ���� index
	int		nIndex;				// ��ǥ node
};

typedef vector<rvector*>	RVERTEXLIST;
typedef vector<RPath*>		RPATHLIST;

class RPathNode {
public:
	RPathNode();
	~RPathNode();

	rplane plane;
	RVERTEXLIST vertices;
	RPATHLIST	m_Neighborhoods;

	int	m_nIndex;				// ����Ʈ ���� ��ġ

	int	m_nSourceID;			// ���� �������� ID
	int	m_nGroupID;				// ����� Group ID		// �̵��� ��ΰ� �ִ� ������ ���� ID �� ���´�.

	void*	m_pUserData;

	void DrawWireFrame(DWORD color);
	void DeletePath(int nIndex);			// nIndex ���� ���� ���� �����Ѵ�.
};

class RPathList : public vector<RPathNode*> {
public:
	virtual ~RPathList();

	void DeleteAll();

	bool Save(const char *filename,int nSourcePolygon);
	bool Open(const char *filename,int nSourcePolygon,CCZFileSystem *pfs=NULL);

	bool ConstructNeighborhood();
	bool ConstructGroupIDs();
	bool EliminateInvalid();

	int GetGroupCount() { return m_nGroup; }

protected:
	int	m_nGroup;

	void MarkGroupID(RPathNode *pNode,int nGroupID);			// GroupID �� ����Ѵ�.
};

_NAMESPACE_REALSPACE2_END
