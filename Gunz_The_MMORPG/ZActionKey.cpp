#include "stdafx.h"
#include "ZActionKey.h"
#include "Core.h"
#include "ZInput.h"
#include "ZOptionInterface.h"
#include "ZApplication.h"
#include "ZGlobal.h"

#include "ZGameInterface.h"
ZInput*			g_pInput;

class ZActionKeyLook : public CCEditLook{
public:
	virtual void OnFrameDraw(CCEdit* pEdit, CCDrawContext* pDC);
	virtual void OnDraw(CCEdit* pEdit, CCDrawContext* pDC);
};

void ZActionKeyLook::OnFrameDraw(CCEdit* pEdit, CCDrawContext* pDC)
{
	sRect r = pEdit->GetInitialClientRect();
	ZActionKey* pActionKey = (ZActionKey*)pEdit;	// -_-;
	sColor color = pActionKey->IsFocus() ? sColor(200,200,200,255) : sColor(100,100,100,255);
	pDC->SetColor(color);
	pDC->Rectangle(r);
}

void ZActionKeyLook::OnDraw(CCEdit* pEdit, CCDrawContext* pDC) 
{
	CCEditLook::OnDraw(pEdit,pDC,false);
}


ActionKeyForbidSet ZActionKey::m_ForbidKey;

ZActionKeyLook ZActionKey::m_DefaultLook;	
ZActionKeyLook* ZActionKey::m_pStaticLook = &ZActionKey::m_DefaultLook;

ZActionKey::ZActionKey(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCEdit(szName, pParent, pListener ), m_bReadyInput(false), m_nKey(-1), m_nAltKey(-1)
{
	LOOK_IN_CONSTRUCTOR();
}

void ZActionKey::OnDraw( CCDrawContext* pDC )
{
	if( GetLook() != NULL )
		GetLook()->OnDraw( this, pDC );
}

/*
void ZActionKey::GetActionKeyName(char* szActionKeyName)
{
	const char* szKeyName = Core::GetInstance()->GetActionKeyName(m_nKey);
	if(szKeyName!=NULL) strcpy(szActionKeyName, szKeyName);
	else strcpy(szActionKeyName, "N/A");
}
*/

void ZActionKey::GetActionKey(int* pKey)
{
	if(pKey!=NULL) *pKey = m_nKey;
}

void ZActionKey::GetActionAltKey(int* pKey)
{
	if(pKey!=NULL) *pKey = m_nAltKey;
}

void ZActionKey::ClearActionKey()
{
	m_nKey = -1;
	m_nAltKey = -1;

	SetText(ZInput::GetUndefinedKeyName());
}

void ZActionKey::UpdateText()
{
	char szBuffer[256] = "";
	ZGetInput()->GetInputKeyName(m_nKey, szBuffer,sizeof(szBuffer));

	if(m_nAltKey!=-1)
	{
		char szActionKeyName[128];
		ZGetInput()->GetInputKeyName(m_nAltKey, szActionKeyName,sizeof(szActionKeyName));
		strcat(szBuffer,"  or  ");
		strcat(szBuffer,szActionKeyName);
	}

	SetText(szBuffer);

	SetStartPos(0);
}

void ZActionKey::SetActionKey(int nKey)
{
	if(nKey==-1) return;
	if(IsForbidKey(nKey))//등록금지키라면..
		return;

	if(m_nKey==-1)	// 아무것도 설정이 안되어 있으면 등록
	{
		m_nKey = nKey;
	}else if(m_nAltKey==-1 && nKey!=m_nKey)	// alt.key 가 설정이 안되어있으면 등록
		m_nAltKey = nKey;
	else if(m_nAltKey==nKey)	// 이미 있는 키이면, 나머지를 삭제
	{
		m_nKey = m_nAltKey;
		m_nAltKey = -1;
	}else if(m_nKey==nKey)
		m_nAltKey = -1;
	else// 그렇지 않으면 key를 제거후 등록
	{
		m_nKey = m_nAltKey;
		m_nAltKey = nKey;
	}

	UpdateText();

	ZGetOptionInterface()->OnActionKeySet(this,nKey);
}

bool ZActionKey::DeleteActionKey(int nKey)
{
	bool bDeleted = false;

	if(m_nAltKey==nKey)
	{
		m_nAltKey = -1;
		bDeleted = true;
	}

	if(m_nKey==nKey)
	{
		m_nKey = m_nAltKey;
		m_nAltKey = -1;
		bDeleted = true;
	}

	return bDeleted;
}


/*
bool ZActionKey::RegisterActionKey(int nActionID)
{
	return ZGetInput()->RegisterActionKey(nActionID, m_nKey);
//	return Core::GetInstance()->RegisterActionKey(nActionID, m_nKey);
}

bool ZActionKey::UnregisterActionKey()
{
	return ZGetInput()->UnregisterActionKey(m_nKey);
//	return Core::GetInstance()->UnregisterActionKey(m_nKey);
}
*/

void ZActionKey::RegisterForbidKey(int nkey)
{
	m_ForbidKey.insert(nkey);
}

bool ZActionKey::IsForbidKey(int nkey)
{
	if(m_ForbidKey.empty())
		return false;

	if(m_ForbidKey.find(nkey)!=m_ForbidKey.end())
		return true;

	return false;
}

MGLOBALEVENTCALLBACK* ZActionKey::m_oldEventListener = NULL;
ZActionKey*	ZActionKey::m_exclusiveActionKey = NULL;

bool ZActionKey::OnExclusiveEvent(CCEvent* pEvent)
{
	_ASSERT(pEvent->iMessage == Z_DIRECTINPUTKEY_MESSAGE);

	// 포커스가 생길때의 이벤트 (lbutton 이나 tab )을 무시한다
	if(!m_exclusiveActionKey->m_bReadyInput)
	{
		m_exclusiveActionKey->m_bReadyInput = true;
	}else
	{
		// 한번 입력되었으면 끝.
//		if (pEvent->uKey != 1) // 1 -> ESC
		{
			m_exclusiveActionKey->SetActionKey(pEvent->uKey);
		}
		m_exclusiveActionKey->ReleaseFocus();
	}

	return true;
}

void ZActionKey::OnSetFocus()
{
	_ASSERT(m_exclusiveActionKey==NULL);

	m_exclusiveActionKey = this;
	m_bReadyInput = false;
	ZGetInput()->SetExclusiveListener(OnExclusiveEvent);
	ZGetGameInterface()->SetSkipGlobalEvent(true);
}

void ZActionKey::OnReleaseFocus()
{
	ZGetGameInterface()->SetSkipGlobalEvent(false);
	m_exclusiveActionKey->m_bReadyInput = false;
	ZGetInput()->SetExclusiveListener(NULL);
	m_exclusiveActionKey = NULL;
}
