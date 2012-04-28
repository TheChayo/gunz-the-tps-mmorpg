#pragma	once


#include <string>
#include "RTypes.h"
#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN
using namespace std;

class CCXmlElement;

class ROcclusion {
public:
	ROcclusion();
	~ROcclusion();

	inline void CalcPlane() { D3DXPlaneFromPoints(&plane,pVertices,pVertices+1,pVertices+2); }

	int nCount;				// ���ؽ��� ��
	rvector *pVertices;		// ���ؽ�
	rplane	*pPlanes;		// ī�޶���ġ�� occlusion�� �Ѻ��� �մ� ����� �����ĵ�
	rplane plane;			// occlusion�� �ִ� ���
	string	Name;
};


class ROcclusionList : public list<ROcclusion*> {
public:
	virtual ~ROcclusionList();
	bool Open(CCXmlElement *pElement);
	bool Save(CCXmlElement *pElement);

	void UpdateCamera(rmatrix &matWorld,rvector &cameraPos);
	bool IsVisible(rboundingbox &bb);

};

_NAMESPACE_REALSPACE2_END