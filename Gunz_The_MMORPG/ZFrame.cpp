#pragma warning ( disable : 4244 )
#include "stdafx.h"

#include <MMSystem.h>
#include <windows.h>
#include "ZFrame.h"
#include "Core.h"

#define TRANSIENT_TIME	300

// Breaks on 13=14 turn with timeGetTime
void ZFrame::Show(bool bVisible, bool bModal)
{
	/*
	DWORD elapsed=1;

	if(elapsed==NULL)
		MessageBox(NULL, NULL, NULL, MB_OK);
	if(timeGetTime()==NULL)
		MessageBox(NULL, NULL, NULL, MB_OKCANCEL);
	try{
		elapsed =timeGetTime();
	} catch(char * str) {
		MessageBox(NULL, str, "Corrupted!", MB_OK);
	}
	elapsed-=m_nShowTime;
/*
	if(m_bNextVisible==m_bVisible && m_bVisible==bVisible && elapsed>TRANSIENT_TIME)
		return;

	// ���°� ����Ǵ� �߰��� �ٸ��������� �����Ұ��
	if(m_bNextVisible!=bVisible){
		if(elapsed<TRANSIENT_TIME)
			m_nShowTime=timeGetTime()-(TRANSIENT_TIME-elapsed);
		else
			m_nShowTime=timeGetTime();
	}
	m_bNextVisible = bVisible;

	CCFrame::Show(bVisible,bModal);
	m_bVisible = true;

 	Enable(bVisible);

	if(bVisible)
		m_bExclusive=bModal;
*/
}

void ZFrame::OnDraw(CCDrawContext* pDC)
{
//	bool bExclusive=false;
	
	float fOpacity = 0;
	if(m_bNextVisible==false){	// Hide
		fOpacity = 1.0f-min(float(timeGetTime()-m_nShowTime)/(float)TRANSIENT_TIME, 1.0f);
		if(fOpacity==0.0f) {
			m_bVisible = false;
			m_bExclusive = false;
		}
		SetOpacity(unsigned char(fOpacity*0xFF));
	}
	else{	// Show
		fOpacity = min(float(timeGetTime()-m_nShowTime)/(float)TRANSIENT_TIME, 1.0f);
		SetOpacity(unsigned char(fOpacity*0xFF));
	}

	// ��׶� ��Ӱ� �����...
 	if( m_bExclusive ){

		sRect Full(0, 0, CCGetWorkspaceWidth()-1, CCGetWorkspaceHeight()-1);
		sRect PrevClip = pDC->GetClipRect();
		pDC->SetClipRect(Full);
		unsigned char oldopacity=pDC->SetOpacity(200*fOpacity);
		pDC->SetColor(0, 0, 0, 255);
		sPoint PrevOrigin = pDC->GetOrigin();
		pDC->SetOrigin(0, 0);
		pDC->FillRectangle(Full);

		// ����
		pDC->SetClipRect(PrevClip);
		pDC->SetOrigin(PrevOrigin);
		pDC->SetOpacity(oldopacity);
	}

	// ������ �׸���
	CCFrame::OnDraw(pDC);
}

ZFrame::ZFrame(const char* szName, CCWidget* pParent, CCListener* pListener)
			: CCFrame(szName, pParent, pListener)
{
	m_bCanShade = false;
	m_bNextVisible = false;
	m_nShowTime = timeGetTime()-TRANSIENT_TIME*2;
	SetOpacity(0);
}

ZFrame::~ZFrame()
{
}

