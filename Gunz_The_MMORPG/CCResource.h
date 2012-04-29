#pragma once
#pragma warning(disable: 4786)

#include <map>
#include <string>
#include <list>

using namespace std;

#include "CCTypes.h"

// ���� ����
struct CCWIDGETINFO{
	char	szWidgetClass[256];
	char	szWidgetName[256];
	int		nResourceID;
};

// ���� ���ҽ�
struct CCWIDGETRESOURCE{
	sRect				Bounds;		// ���
	bool				bBounds;	// ��谡 �ٲ���°�?

	class CCWIDGETINFOLIST : public list<CCWIDGETINFO*>{
	public:
		virtual ~CCWIDGETINFOLIST(void){
			// Delete Automatically
			while(empty()==false){
				delete (*begin());
				erase(begin());
			}
		}
	} Children;	// �ڽ� ���� ����Ʈ
};

class CCResourceMap : public map<string, CCWIDGETRESOURCE*>{
public:
	virtual ~CCResourceMap(void){
		// Delete Automatically
		while(empty()==false){
			delete (*begin()).second;
			erase(begin());
		}
	}
};
