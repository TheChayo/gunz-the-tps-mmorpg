#pragma once
#include <list>
#include <string>
#include "RTypes.h"
#include "RToken.h"
#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN
using namespace std;

class CCXmlElement;





// ����, ���� ����Ʈ
struct RDummy {
	string	szName;
	rvector Position;
	rvector Direction;
};

class RDummyList : public list<RDummy*> 
{
private:
	void Clear();
public:
	virtual ~RDummyList();

	bool Open(CCXmlElement *pElement);
	bool Save(CCXmlElement *pElement);
};

_NAMESPACE_REALSPACE2_END