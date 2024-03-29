#include "stdafx.h"

//include "ZGameClient.h"
#include "ZApplication.h"
#include "ZMsgBox.h"
#include "Core.h"

// Added R350a
#include "ZStringResManager.h"

// ZMsgBox Listener
class ZMsgBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		ZMsgBox* pMsgBox = (ZMsgBox*)pWidget;
		if (pMsgBox->GetCustomListener()) {
			bool bResult = pMsgBox->GetCustomListener()->OnCommand(pWidget, szMessage);
			pMsgBox->SetCustomListener(NULL);
			return bResult;
		}

		if(CCWidget::IsMsg(szMessage, CCMSGBOX_OK)==true){
			const char* pText = pWidget->GetText();

			// 인원이 꽉차 종료
//			if(!stricmp(pText, CCGetErrorString(MERR_CLIENT_FULL_PLAYERS)))
			if(!stricmp(pText, ZErrStr(MERR_CLIENT_FULL_PLAYERS)))
			{
				if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE)
				{

					ZApplication::Exit();
					return true;
				}
			}
		}

		pWidget->Show(false);

		return false;
	}
};
ZMsgBoxListener	g_MsgBoxListener;

CCListener* ZGetMsgBoxListener()
{
	return &g_MsgBoxListener;
}

// ZConfirmMsgBox Listener : Default Listener 일뿐. 물어볼때마다 CustomListener 지정해서 쓸것
class ZConfirmMsgBoxListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		ZMsgBox* pMsgBox = (ZMsgBox*)pWidget;
		if (pMsgBox->GetCustomListener()) {
			bool bResult = pMsgBox->GetCustomListener()->OnCommand(pWidget, szMessage);
			pMsgBox->SetCustomListener(NULL);
			return bResult;
		}

		pWidget->Show(false);
		return false;
	}
};
ZConfirmMsgBoxListener	g_CofirmMsgBoxListener;

CCListener* ZGetConfirmMsgBoxListener()
{
	return &g_CofirmMsgBoxListener;
}


#define TRANSIENT_TIME	200

void ZMsgBox::Show(bool bVisible, bool bModal)
{
	DWORD elapsed=(timeGetTime()-m_nShowTime);

	if(m_bNextVisible==m_bVisible && m_bVisible==bVisible && elapsed>TRANSIENT_TIME)
		return;

	// 상태가 변경되는 중간에 다른방향으로 가야할경우
	if(m_bNextVisible!=bVisible){
		if(elapsed<TRANSIENT_TIME)
			m_nShowTime=timeGetTime()-(TRANSIENT_TIME-elapsed);
		else
			m_nShowTime=timeGetTime();
	}
	m_bNextVisible = bVisible;

	CCMsgBox::Show(bVisible,bModal);
	m_bVisible = true;

	Enable(bVisible);
}

void ZMsgBox::OnDraw(CCDrawContext* pDC)
{
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

	CCMsgBox::OnDraw(pDC);
}

ZMsgBox::ZMsgBox(const char* szMessage, CCWidget* pParent, CCListener* pListener, CCMsgBoxType nType)
: CCMsgBox(szMessage,pParent,pListener,nType)
{
	m_bCanShade = false;
	m_bNextVisible = false;
	m_nShowTime = timeGetTime()-TRANSIENT_TIME*2;
	SetOpacity(0);
	m_pCustomListener = NULL;
}

ZMsgBox::~ZMsgBox()
{
}