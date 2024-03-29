#ifndef ZFRAME_H
#define ZFRAME_H

#include "CCFrame.h"
#include "CCPopupMenu.h"
#include "CCSlider.h"

class CCFrame;

class ZFrame : public CCFrame
{
protected:
	bool				m_bExclusive;
	unsigned long int	m_nShowTime;
	bool				m_bNextVisible;

	virtual void OnDraw(CCDrawContext* pDC);

public:
	ZFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZFrame();

	void Show(bool bVisible, bool bModal=false);
};


#endif