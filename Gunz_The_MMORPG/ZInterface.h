#ifndef _ZINTERFACE_H
#define _ZINTERFACE_H

#include "ZIDLResource.h"
#include <windows.h>
#include <io.h>
#include "ZGame.h"
#include "RealSpace2.h"
#include "RBaseTexture.h"
#include "Core.h"
#include "Core4R2.h"

_USING_NAMESPACE_REALSPACE2

// interface ���� base Ŭ����

class ZInterface : public CCWidget {
public:
	ZInterface(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZInterface();

	virtual bool OnCreate();										// �ʱ�ȭ
	virtual void OnDestroy();										// destroy �� �Ҹ�.

	virtual void OnDraw(CCDrawContext* pDC);

	virtual bool IsDone();											// ���� �������̽��� ���� false�� �����Ѵ�.

	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener){return false;}	// interface �� �׻� �̺�Ʈ�� ó���Ѵ�.

protected:
	bool m_bDone;
};

#endif