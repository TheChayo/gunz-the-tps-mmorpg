#include "stdafx.h"
#include "Core.h"
#include "CCWidget.h"
#include "TChar.h"
#include "CCFileDialog.h"
#include "CCPopupMenu.h"
#include "CCListBox.h"
#include "CCMultiColListBox.h"
#include "CCComboBox.h"
#include "CCGroup.h"
#include "CCSlider.h"
#include "CCHyperText.h"
#include "CCTextArea.h"
#include "CCResource.h"
#include "CCDragObject.h"
#include "CCPicture.h"
#include "CCAnimation.h"
#include "CCHotKey.h"
//#include "MActionKey.h"
#include "CCBmButton.h"
#include "CCTabCtrl.h"
#include "CCPanel.h"

class CCMainFrame : public CCWidget{
protected:
protected:
	virtual void OnDraw(CCDrawContext* pDC){
		// Draw Nothing
	}
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		return false;
	}
public:
	CCMainFrame(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
		: CCWidget(szName, pParent, pListener){
	}
	virtual ~CCMainFrame(void){
	}
};


Core* Core::m_pInstance = NULL;

void Core::DrawCandidateList(CCDrawContext* pDC, sPoint& p)
{
	if(GetCandidateCount()>0){
		sColor c = pDC->GetColor();

		sRect r(p.x, p.y, GetCandidateListWidth(), GetCandidateListHeight());

//		pDC->SetColor(sColor(0x30000000));		// �׸���
//		pDC->FillRectangle( r.x+5, r.y+5, r.w, r.h);
		
		pDC->SetColor(sColor(0xFF050505));		// ������ ����
		pDC->FillRectangle(r);

		pDC->SetColor(sColor(0xFF505050));		// ������ ��ο� �κ�
		pDC->Rectangle( r.x+1, r.y+1, r.w,   r.h);

		pDC->SetColor(sColor(0xFFB0B0B0));		// ������ ���� �κ�
		pDC->Rectangle(r);

		CCFont* pFont = pDC->GetFont();
		pDC->SetFont( CCFontManager::Get( "Default"));		// ������ ��Ʈ�� ����Ʈ�� �����Ѵ�.

		int nStart = GetCandidatePageStart();

		char temp[COMPOSITIONSTRING_LENGTH+4];
		for(int i=nStart; i<(int)min(GetCandidateCount(), nStart+GetCandidatePageSize()); i++)
		{
			const char* szCandidate = GetCandidate(i);
			if(i==GetCandidateSelection())
				pDC->SetColor(sColor(DEFCOLOR_NORMAL));
			else
				pDC->SetColor(sColor(0xFF909090));
			int nIndexInPage = i-nStart;
			sprintf(temp, "%d: %s", nIndexInPage+1, szCandidate);
			pDC->Text(p.x+4, p.y + nIndexInPage*pDC->GetFont()->GetHeight() + 4, temp);
		}

		// ���� ���� �ε��� �� �� ���� ���
		sprintf(temp, "(%d/%d)", GetCandidateSelection()+1, GetCandidateCount());
		pDC->SetColor(sColor(DEFCOLOR_NORMAL));
		pDC->Text(p.x + 4, p.y + GetCandidatePageSize()*pDC->GetFont()->GetHeight() + 4, temp);

		pDC->SetColor(c);
		pDC->SetFont( CCFontManager::Get( pFont->m_szName));		// ���� ��Ʈ�� ����
	}
}


Core::Core()
{
	_ASSERT(m_pInstance==NULL);	// Singleton!!!
	m_pInstance = this;
	m_pMainFrame = NULL;
	m_pDC = NULL;
	m_hImc = NULL;

//	m_nDragObjectID = -1;
	m_szDragObjectString[0] = 0;
	m_szDragObjectItemString[0] = 0;
	m_pDragObjectBitmap = NULL;
	m_bVisibleDragObject = false;
	m_pDropableObject = NULL;
	m_pDragSourceObject = NULL;

	m_iWorkspaceWidth = 640;
	m_iWorkspaceHeight = 480;

	m_fnGlobalEventCallBack = NULL;

//	memset(m_ActionKeyPressedTable, 0, sizeof(bool)*ACTIONKEYMAP_IDCOUNT);

	m_pCandidateList = NULL;
	m_iCandidateListSize = 0;

	m_iCompositionAttributeSize = 0;
	memset(m_iCompositionAttributes, 0, sizeof(BYTE)*(COMPOSITIONSTRING_LENGTH));
	//memset(m_dwCompositionClauses, 0, sizeof(DWORD)*(MIMECOMPOSITIONSTRING_LENGTH));

	m_iCompositionCaretPosition = 0;

	m_bEnableIME = false;
}

Core::~Core()
{
	m_fnGlobalEventCallBack = NULL;
	m_pInstance = NULL;
}

Core* Core::GetInstance(void)
{
	_ASSERT(m_pInstance!=NULL);
	return m_pInstance;
}

bool Core::Initialize(int nWorkspaceWidth, int nWorkspaceHeight, CCDrawContext* pDC, CCFont* pDefaultFont)
{
	_ASSERT(m_pMainFrame==NULL);

	m_pDC = pDC;

//	CCFontManager::Add(pDefaultFont);
	CCFontManager::SetDefaultFont(pDefaultFont);

	m_pMainFrame = new CCMainFrame("Core");
	m_pMainFrame->SetBounds(0, 0, nWorkspaceWidth, nWorkspaceHeight);

	SetWorkspaceSize(nWorkspaceWidth, nWorkspaceHeight);

	return true;
}

void Core::Finalize(void)
{
	if(m_pMainFrame!=NULL){
		delete m_pMainFrame;
		m_pMainFrame = NULL;
	}

	CCFontManager::Destroy();

	m_pDC = NULL;
}

bool Core::ProcessEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	if(!m_pMainFrame) return false;

	CCEvent e;
	int nResult = e.TranslateEvent(hwnd, message, wparam, lparam);
	if(nResult&EVENT_CORE_TRANSLATED){
		// Drag & Drop
		if(m_pDragSourceObject!=NULL){
#define DRAm_VISIBLE_LENGTH	2	// �巡�� ������Ʈ�� �����ֱ� �����ϴ� ����
			if(e.iMessage==CCWM_MOUSEMOVE){
				sPoint p = e.sPos;
				//sPoint p = CCEvent::GetMousePos();
				int px = m_GrabPoint.x - p.x;
				if ( px < 0)  px *= -1;
				int py = m_GrabPoint.y - p.y;
				if ( py < 0)  py *= -1;

				if( m_bVisibleDragObject==false &&
				    ((px > DRAm_VISIBLE_LENGTH) || (py > DRAm_VISIBLE_LENGTH)))
                    m_bVisibleDragObject = true;
				CCWidget* pFind = FindWidget(p);
				if(pFind!=NULL && pFind->IsDropable(m_pDragSourceObject)==true)
					m_pDropableObject = pFind;
				else
					m_pDropableObject = NULL;
			}
			if(e.iMessage==CCWM_LBUTTONUP){
				sPoint p = e.sPos;
				CCWidget* pFind = FindWidgetDropAble(p);
//				CCWidget* pFind = FindWidget(p);
				if(pFind!=NULL && pFind->IsDropable(m_pDragSourceObject)==true)
					pFind->Drop(m_pDragSourceObject, m_pDragObjectBitmap, m_szDragObjectString, m_szDragObjectItemString);	// �ش� ������ ���
				m_pDragSourceObject = NULL;
				m_pMainFrame->ReleaseCapture();
				return true;
			}
		}

		// ���� �̺�Ʈ ó��
		if (m_fnGlobalEventCallBack) {
			if (m_fnGlobalEventCallBack(&e) == true) return true;
		}

		// �Ϲ� �̺�Ʈ ó��
		if(m_pMainFrame->Event(&e)==true) return true;
		// ������ Accelerator ó��
		if(m_pMainFrame->EventAccelerator(&e)==true) return true;
		// Default Key(Enter, ESC) ó��
		if(m_pMainFrame->EventDefaultKey(&e)==true) return true;

	}
	if(nResult&EVENT_PROCESSED) return true;	// ������ ó����

	return false;
}

void Core::Run(void)
{
	if(!m_pMainFrame) return;

	m_pMainFrame->Run();
}

void Core::Draw(void)
{
	if(!m_pMainFrame) return;

	CCDrawContext* pDC = m_pDC;
	if(pDC==NULL) return;

	pDC->SetOpacity(0xFF);
	m_pMainFrame->Draw(pDC);

	sRect sr = m_pMainFrame->GetScreenRect();
	pDC->SetOrigin(sPoint(sr.x, sr.y));
	pDC->SetClipRect(sr);


	// Candidate List �׸���
	DrawCandidateList(pDC, m_CandidateListPos);


	// Drag & Drop
	/*
	CCDragObject* pDragObject = CCGetDragObject();
	if(pDragObject!=NULL){
		pDC->SetBitmap(pDragObject->GetBitmap());
		sPoint p = CCEvent::GetMousePos();
		pDC->Draw(p);
		pDC->Text(p, pDragObject->GetString());
	}
	*/
	//int nDragObjectID = GetDragObject();
	CCWidget* pDragSourceObject = GetDragObject();
	if(pDragSourceObject!=NULL && m_bVisibleDragObject==true){
		// ���� ��ġ���� ����� �� �ִ� ������ ǥ��
		if(m_pDropableObject!=NULL){
			sRect r = m_pDropableObject->GetScreenRect();
			pDC->SetColor(0, 0, 0, 32);
			pDC->FillRectangle(r);
		}

		// Ŀ���� �ٴ� �巡�� ������Ʈ �׸���
		pDC->SetBitmap(m_pDragObjectBitmap);
		sPoint p = CCEvent::GetMousePos();
		p.x-=(m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetWidth()/2:0);
		p.y-=(m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetHeight()/2:0);
		if (m_pDragObjectBitmap)
		{
			pDC->SetOpacity(128);
			pDC->Draw(p.x, p.y, m_pDragObjectBitmap->GetWidth(), m_pDragObjectBitmap->GetHeight());
			pDC->SetOpacity(0xFF);
		}
		p.x+=((m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetWidth():0) + 2);
		p.y+=((m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetHeight()/2:0)-pDC->GetFont()->GetHeight()/2);
		pDC->SetColor(0, 0, 0, 128);
		pDC->Text(p, m_szDragObjectString);
		pDC->SetColor(255, 255, 255, 128);
		p.x--;
		p.y--;
		pDC->Text(p, m_szDragObjectString);
	}
	
}

/*
bool Core::EventActionKey(unsigned long int nKey, bool bPressed)
{
	CCEvent e;

	if(bPressed==true) e.iMessage = CCWM_ACTIONKEYDOWN;
	else e.iMessage = CCWM_ACTIONKEYUP;
	e.nKey = nKey;

	if (m_pMainFrame == NULL)
		return false;

	if (m_fnGlobalEventCallBack) m_fnGlobalEventCallBack(&e);
	m_pMainFrame->Event(&e);

	ACTIONKEYMAP::iterator i = m_ActionKeyMap.find(nKey);
	int nActionID = -1;
	if(i!=m_ActionKeyMap.end()) nActionID = (*i).second;
	if(bPressed==true){
		e.iMessage = CCWM_ACTIONPRESSED;
		if(nActionID>=0 && nActionID<ACTIONKEYMAP_IDCOUNT) m_ActionKeyPressedTable[nActionID] = true;
	}
	else{
		e.iMessage = CCWM_ACTIONRELEASED;
		if(nActionID>=0 && nActionID<ACTIONKEYMAP_IDCOUNT) m_ActionKeyPressedTable[nActionID] = false;
	}
	e.nKey = nActionID;

	if (m_fnGlobalEventCallBack) m_fnGlobalEventCallBack(&e);

	return m_pMainFrame->Event(&e);
}
*/

CCWidget* Core::GetMainFrame(void)
{
	return m_pMainFrame;
}

CCDrawContext* Core::GetDrawContext(void)
{
	return m_pDC;
}

void Core::Update(){
}

#ifdef WIN32
HWND m_hWnd = NULL;
void Core::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;

	// ime status â�� hide�Ѵ�
	HWND imehwnd=ImmGetDefaultIMEWnd(Core::GetInstance()->GetHWND());

	LRESULT lr=SendMessage(imehwnd,WM_IME_CONTROL ,IMC_CLOSESTATUSWINDOW,0);
	_ASSERT(lr==0);

	HIMC	hImc;
	hImc = ImmGetContext(Core::GetInstance()->GetHWND());
	if (hImc)
	{
		// Get current IME status
		DWORD dwIMEConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwIMEConvMode, &dwSentMode);
		ImmSetConversionStatus(hImc,IME_CMODE_ALPHANUMERIC, dwSentMode);

		Core::EnableIME(false);
	}
}
HWND Core::GetHWND(void)
{
	return m_hWnd;
}
#endif

void Core::EnableIME(bool bEnable)
{
	_ASSERT(GetHWND());
	if ( (bEnable == true) && CCEvent::GetIMESupport()) {
		if (m_hImc) {
			ImmAssociateContext(GetHWND(), m_hImc);
			m_hImc = NULL;	// EnableIME(false) �Ҷ� �ٽ� ���õȴ�
			::SetFocus(GetHWND());
		}
		m_bEnableIME = true;
	} else {
		// HIMC�� m_hImc�� �ӽ� ������ �״ٰ�, Enable�� �����Ѵ�.
		m_hImc = ImmGetContext(GetHWND());
		if (m_hImc) {
			ImmAssociateContext(GetHWND(), NULL);
			ImmReleaseContext(GetHWND(), m_hImc);
			::SetFocus(GetHWND());
		}
		//ImmDisableIME(0);
		m_bEnableIME = false;
	}

	// Composition���� ���ڿ� ����
	HIMC hImc = ImmGetContext(GetInstance()->GetHWND());
	if(hImc!=NULL){
		ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
		ImmReleaseContext(GetInstance()->GetHWND(), hImc);
	}
}

bool Core::IsEnableIME(void)
{
	return m_bEnableIME;
}


int Core::RegisterHotKey(unsigned long int nModifier, unsigned long int nVirtKey)
{
#ifdef WIN32
	_ASSERT(m_hWnd!=NULL);	// Should call SetHWND() before this function

	char szAtomName[64] = {0,};
	if(nModifier==CCMODIFIER_ALT) strcat(szAtomName, "Alt");
	if(nModifier==CCMODIFIER_CTRL) strcat(szAtomName, "Ctrl");
	if(nModifier==CCMODIFIER_SHIFT) strcat(szAtomName, "Shift");
	char szKey[16] = {0, };
	sprintf(szKey, "%d", nVirtKey);
	strcat(szAtomName, szKey);

	int nID = GlobalAddAtom(szAtomName);
	if(nID==0) return 0;

	if(::RegisterHotKey(m_hWnd, nID, nModifier, nVirtKey)==TRUE)
		return nID;

	return 0;
#else
	// Not Implemented
	_ASSERT(FALSE);
	return 0;
#endif
}
void Core::UnregisterHotKey(int nID)
{
#ifdef WIN32
	_ASSERT(m_hWnd!=NULL);	// Should call SetHWND() before this function
	::UnregisterHotKey(m_hWnd, nID);
	GlobalDeleteAtom(nID);
#else
	// Not Implemented
	_ASSERT(FALSE);
	return 0;
#endif
}

/*
bool Core::RegisterActionKey(int nActionID, unsigned long int nKey)
{
	if(nActionID<0 || nActionID>=ACTIONKEYMAP_IDCOUNT){
		_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 ���̰��̿��� �Ѵ�.
		return false;
	}

	m_ActionKeyMap.insert(ACTIONKEYMAP::value_type(nKey, nActionID));
	return true;
}

bool Core::UnregisterActionKey(int nActionID)
{
	if(nActionID<0 || nActionID>=ACTIONKEYMAP_IDCOUNT){
		_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 ���̰��̿��� �Ѵ�.
		return false;
	}

	for(ACTIONKEYMAP::iterator i=m_ActionKeyMap.begin(); i!=m_ActionKeyMap.end(); i++){
		if((*i).second==nActionID){
			m_ActionKeyMap.erase(i);
			return true;
		}
	}
	return false;
}

const char* Core::GetActionKeyName(unsigned long int nKey)
{
	_ASSERT(FALSE);
	// Not Implemented. User must implement

	static char* szNullName = "Unknown";
	return szNullName;
}

unsigned long int Core::GetActionKey(int nActionID)
{
	for(ACTIONKEYMAP::iterator i=m_ActionKeyMap.begin(); i!=m_ActionKeyMap.end(); i++){
		if((*i).second==nActionID){
			return (*i).first;
		}
	}
	return -1;
}

bool Core::IsActionKeyPressed(int nActionID)
{
	if(nActionID<0 || nActionID>=ACTIONKEYMAP_IDCOUNT){
		_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 ���̰��̿��� �Ѵ�.
		return false;
	}
	return m_ActionKeyPressedTable[nActionID];
}
*/

void Core::SetGlobalEvent(MGLOBALEVENTCALLBACK pGlobalEventCallback)
{
	m_fnGlobalEventCallBack = pGlobalEventCallback;
}

/*
CCDragObject* m_pDragObject = NULL;

CCDragObject* MSetDragObject(CCDragObject* pDragObject)
{
	CCDragObject* pPrevDragObject = m_pDragObject;
	m_pDragObject = pDragObject;
	return pPrevDragObject;
}
CCDragObject* CCGetDragObject(void)
{
	return m_pDragObject;
}
*/

CCWidget* Core::SetDragObject(CCWidget* pSender, CCBitmap* pBitmap, const char* szString, const char* szItemString)
{
	CCWidget* pPrev = m_pDragSourceObject;
	m_pDragSourceObject = pSender;
	if(szString==NULL) m_szDragObjectString[0] = 0;
	else strncpy(m_szDragObjectString, szString, 256);
	if(szItemString==NULL) m_szDragObjectItemString[0] = 0;
	else strncpy(m_szDragObjectItemString, szItemString, 256);
	m_pDragObjectBitmap = pBitmap;
	m_GrabPoint = CCEvent::GetMousePos();
	m_bVisibleDragObject = false;
	return pPrev;
}
CCWidget* Core::GetDragObject(void)
{
	return m_pDragSourceObject;
}

/*
#define CORE_CCWIDGET	"GenericWidget"
#define CORE_CCLABEL			"Label"
#define CORE_CCBUTTON			"Button"
#define CORE_CCEDIT			"Edit"
#define CORE_CCLISTBOX		"ListBox"
#define CORE_CCCOMBOBOX		"ComboBox"
#define CORE_CCFILEBOX		"FileBox"
#define CORE_CCFRAME			"Frame"
#define CORE_CCGROUP			"Group"
#define CORE_CCPOPUPMENU			"Menu"
#define CORE_CCSCROLLBAR		"ScrollBar"
#define CORE_CCSLIDER			"Slider"
#define CORE_CCTEXTAREA		"TextArea"
#define CORE_CCMSGBOX		"MessageBox"
*/

/*
char* m_szWidgetClasses[] = {
	CORE_CCWIDGET,
	CORE_CCLABEL,
	CORE_CCBUTTON,
	CORE_CCEDIT,
	CORE_CCLISTBOX,
	CORE_CCCOMBOBOX,
	CORE_CCFILEBOX,
	CORE_CCFRAME,
	CORE_CCGROUP,
	CORE_CCPOPUPMENU,
	CORE_CCSCROLLBAR,
	CORE_CCSLIDER,
	CORE_CCTEXTAREA,
	CORE_CCMSGBOX,
};
*/

CCWidget* Core::NewWidget(const char* szClass, const char* szName, CCWidget* pParent, CCListener* pListener)
{
	if(strcmp(szClass, CORE_CCWIDGET)==0) return new CCWidget(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCLABEL)==0) return new CCLabel(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCBUTTON)==0) return new CCButton(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCBMBUTTON)==0) return new CCBmButton(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCEDIT)==0) return new CCEdit(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCLISTBOX)==0) return new CCListBox(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCMULTICOLLISTBOX)==0) return new CCMultiColListBox(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCCMBBOX)==0) return new CCComboBox(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCFILEBOX)==0) return new CCFileBox(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCFRAME)==0) return new CCFrame(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCGROUP)==0) return new CCGroup(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCPOPUPMENU)==0) return new CCPopupMenu(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCSCROLLBAR)==0) return new CCScrollBar(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCSLIDER)==0) return new CCSlider(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCTEXTAREA)==0) return new CCTextArea(256, szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCTABCTRL)==0) return new CCTabCtrl(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCMSGBOX)==0) return new CCMsgBox(szName, pParent, pListener, CCT_OK);
	else if(strcmp(szClass, CORE_CCPICTURE)==0) return new CCPicture(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCANIMATION)==0) return new CCAnimation(szName, NULL, pParent);
	else if(strcmp(szClass, CORE_CCMENUITEM)==0) return new CCMenuItem(szName);
	else if(strcmp(szClass, CORE_CCHOTKEY)==0) return new CCHotKey(szName, pParent, pListener);
//	else if(strcmp(szClass, CORE_CCACTIONKEY)==0) return new MActionKey(szName, pParent, pListener);
	else if(strcmp(szClass, CORE_CCPANEL)==0) return new CCPanel(szName, pParent, pListener);

	else _ASSERT(false);

	return new CCWidget(szName, pParent, pListener);
}

CCWidget* Core::FindWidgetDropAble(sPoint& p)
{
	return m_pMainFrame->FindDropAble(p);
}

CCWidget* Core::FindWidget(sPoint& p)
{
	return m_pMainFrame->Find(p);
}

CCWidget* Core::FindWidget(int x, int y)
{
	return m_pMainFrame->Find(x, y);
}


int Core::GetWorkspaceWidth(void)
{
	return m_iWorkspaceWidth;
}

int Core::GetWorkspaceHeight(void)
{
	return m_iWorkspaceHeight;
}

void Core::SetWorkspaceSize(int w, int h)
{
	m_iWorkspaceWidth = w;
	m_iWorkspaceHeight = h;
}

const char* Core::GetDefaultFontName(void) const
{
	static char* szFontName[] = {
		"Tahoma",		// Default
		"Gulim",		// �ѱ�
		"MS PGothic",	// �Ϻ�
		"MingLiU",		// ��ü
		"NSimSun"		// ��ü
	};

	int nFont = 0;
	if(GetPrimaryLanguageIdentifier()==LANG_KOREAN) nFont = 1;
	else if(GetPrimaryLanguageIdentifier()==LANG_JAPANESE) nFont = 2;
	else if(GetPrimaryLanguageIdentifier()==LANG_CHINESE){
		if(GetSubLanguageIdentifier()==SUBLANG_CHINESE_TRADITIONAL) nFont = 3;
		else nFont = 4;
	}

	return szFontName[nFont];
}

int Core::GetPrimaryLanguageIdentifier(void) const
{
	/*
	Language Identifiers
	+-------------------------+-------------------------+
	|      SubLanguage ID     |   Primary Language ID   |
	+-------------------------+-------------------------+
	15                    10  9                         0   bit

	ms-help://MS.VSCC.2003/MS.MSDNQTR.2003FEB.1042/intl/nls_8xo3.htm
	ms-help://MS.VSCC.2003/MS.MSDNQTR.2003FEB.1042/intl/nls_238z.htm
	*/
	HKL hKeyboardLayout = GetKeyboardLayout(0);
	WORD nLanguageIdentifier = LOWORD(hKeyboardLayout);
	return PRIMARYLANGID(nLanguageIdentifier);
}

int Core::GetSubLanguageIdentifier(void) const
{
	HKL hKeyboardLayout = GetKeyboardLayout(0);
	WORD nLanguageIdentifier = LOWORD(hKeyboardLayout);
	return SUBLANGID(nLanguageIdentifier);
}

const char* Core::GetLanguageIndicatorString(void) const
{
	/*
	static char* szIndicator[] = {
	"EN", "KO", "JP", "CH", "?",
	};
	*/
	static char* szIndicator[] = {
		"?", "EN", "��", "��", "��", "PT"
	};
	switch(GetPrimaryLanguageIdentifier()){
	case LANG_ENGLISH:
		return szIndicator[1];
	case LANG_KOREAN:
		return szIndicator[2];
	case LANG_JAPANESE:
		return szIndicator[3];
	case LANG_CHINESE:
		return szIndicator[4];
	case LANG_PORTUGUESE:
		return szIndicator[5];
	default:
		return szIndicator[0];
	}
}

bool Core::IsNativeIME(void) const
{
	HIMC hImc = ImmGetContext(GetInstance()->GetHWND());
	if(hImc==NULL) return false;

	bool bNative = false;

	if(GetPrimaryLanguageIdentifier()==LANG_JAPANESE){
		// �Ϻ����ΰ�� ConversionMode�� �ƴ� OpenSatus�� �Է±Ⱑ Ȱ��ȭ �Ǿ����� �� �� �ִ�.
		// �Ϻ��� IME �̻���!
		bNative = (ImmGetOpenStatus(hImc)==TRUE);
	}
	else{
		// ����(IME_CMODE_ALPHANUMERIC) �Ǵ� ����Ƽ��(IME_CMODE_NATIVE) ��������� �Ǻ�
		DWORD dwConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

		bNative = (dwConvMode&IME_CMODE_NATIVE);
	}

	ImmReleaseContext(GetInstance()->GetHWND(), hImc);

	return bNative;
}

void Core::OpenCandidateList(void)
{
	// Candidate List ����
	HIMC hImc = ImmGetContext(GetInstance()->GetHWND());
	if(hImc==NULL) return;

	m_iCandidateListSize = ImmGetCandidateList(hImc, 0, NULL, 0);

	if(m_pCandidateList!=NULL) delete[] m_pCandidateList;
	m_pCandidateList = new unsigned char[m_iCandidateListSize];

	if(m_iCandidateListSize>0) { 
		ImmGetCandidateList(hImc, 0, (LPCANDIDATELIST)m_pCandidateList, m_iCandidateListSize);
	}

	ImmReleaseContext(GetInstance()->GetHWND(), hImc);
}

void Core::CloseCandidateList(void)
{
	// Candidate List ����
	if(m_pCandidateList!=NULL) delete[] m_pCandidateList;
	m_pCandidateList = NULL;
	m_iCandidateListSize = 0;
}

const char* Core::GetCandidate(int nIndex) const
{
	if(m_pCandidateList==NULL) return NULL;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	if(nIndex>=(int)pCandidateList->dwCount) return NULL;

	char* pCandidate = (char*)((BYTE*)pCandidateList+pCandidateList->dwOffset[nIndex]);
	return pCandidate;
}

int Core::GetCandidateCount(void) const
{
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwCount;
}

int Core::GetCandidateSelection(void) const
{
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwSelection;
}

int Core::GetCandidatePageStart(void) const
{
	if(m_pCandidateList==NULL) return 0;

	// GetCandidatePageStart(); �� �Ϻ���� ���װ� �����Ƿ�, �������� ���
	int nStart = GetCandidatePageSize() * (GetCandidateSelection()/GetCandidatePageSize());

	return nStart;

	/*
	// �Ϻ�� �����ϰ� �۵��Ǵ� ���� �ڵ�
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwPageStart;
	*/
}

int Core::GetCandidatePageSize(void) const
{
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwPageSize;
}

void Core::SetCandidateListPosition(sPoint& p, int nWidgetHeight)
{
	sPoint cp = p;

	// ���� ���� üũ
	if((cp.x+GetCandidateListWidth())>=CCGetWorkspaceWidth()){
		cp.x = CCGetWorkspaceWidth()-GetCandidateListWidth();
	}
	else{
//		cp.x -= 4;
	}
	// ���� ���� üũ
	if((cp.y+GetCandidateListHeight()+nWidgetHeight+8)>=CCGetWorkspaceHeight()){
		cp.y -= GetCandidateListHeight() + 6;
	}
	else{
		cp.y += (nWidgetHeight+6);
	}

	m_CandidateListPos = cp;
}

int Core::GetCandidateListWidth(void)
{
	int w = 60;
	if(GetCandidateCount()>0){
		const char* szCandidate = GetCandidate(0);
		w = max(w, CCFontManager::Get( "Default")->GetWidth(szCandidate)+100);	// �ٸ� ���ڿ��� �ʺ� �� Ŭ �� �����Ƿ� �������� ����� �ش�.
	}
	return w + 4;
}

int Core::GetCandidateListHeight(void)
{
	return (CCFontManager::Get( "Default")->GetHeight()*(GetCandidatePageSize()+1) + 6);
}

// ��Ƽ���� ������ ���� ���� ���� ���
int Core::DrawCompositionAttribute(CCDrawContext* pDC, sPoint& p, const char* szComposition, int i)
{
	if(i>=(int)strlen(szComposition)) return 0;
	//if(pCore->GetPrimaryLanguageIdentifier()!=LANG_JAPANESE) return;

	const BYTE* pCompAttr = GetCompositionAttributes();
	DWORD nCompAttrSize = GetCompositionAttributeSize();

	if(i>=(int)nCompAttrSize) return 0;	// Composition Attribute ������ ����� ���

	CCFont* pFont = pDC->GetFont();
	int nFontHeight = pFont->GetHeight();
	sColor c = pDC->GetColor();

	int nCharSize = 1;
	bool bTwoByteChar = IsHangul(szComposition[i]);
	if(bTwoByteChar) nCharSize = 2;
	int nWidth = pFont->GetWidth(&(szComposition[i]), nCharSize);

	if(pCompAttr[i]==ATTR_TARGET_CONVERTED)				// ��ȯ�� ����
		pDC->SetColor(sColor(255, 0, 0, 128));
	else if(pCompAttr[i]==ATTR_TARGET_NOTCONVERTED)		// ��ȯ���� �ʴ� ����
		pDC->SetColor(sColor(0, 196, 0, 128));
	else												// ��ȯ ���� ��
		pDC->SetColor(sColor(128, 128, 128, 128));

	pDC->FillRectangle(p.x, p.y, nWidth, nFontHeight);

	pDC->SetColor(c);

	return nWidth;
}

void Core::DrawCompositionAttributes(CCDrawContext* pDC, sPoint& p, const char* szComposition)
{
	if(strlen(szComposition)==0) return;

	int nWidth = 0;

	for(int i=0; szComposition[i]!=NULL; i++){
		bool bTwoByteChar = IsHangul(szComposition[i]);

		nWidth += DrawCompositionAttribute(pDC, sPoint(p.x+nWidth, p.y), szComposition, i);

		if(bTwoByteChar) i++;
	}
}

void Core::DrawIndicator(CCDrawContext* pDC, sRect& r)
{
	CCFont* pFont = pDC->GetFont();
	int nFontHeight = pFont->GetHeight();

	// �ش� �� ���� ǥ��
	const char* szLanguageIndicator = GetLanguageIndicatorString();

	int nIdicatorWidth = pFont->GetWidth(szLanguageIndicator);
	sColor c = pDC->GetColor();

	sRect fr(r.x+r.w-nIdicatorWidth-4, r.y+2, nIdicatorWidth+3, r.h-4);

	if(IsNativeIME()==true){
		pDC->SetColor(sColor(130,130,130));
		pDC->FillRectangle(fr);
		pDC->SetColor(sColor(0, 0, 0));
		pDC->Text(r.x+r.w-nIdicatorWidth-2, r.y + (r.h-nFontHeight)/2, szLanguageIndicator);
	}
	else{
//		pDC->SetColor(sColor(0, 0, 0));
//		pDC->FillRectangle(fr);
//		pDC->SetColor(sColor(DEFCOLOR_NORMAL));
	}

//	pDC->Text(r.x+r.w-nIdicatorWidth-2, r.y + (r.h-nFontHeight)/2, szLanguageIndicator);
	pDC->SetColor(c);
}



// Sample Widgets


TCHAR* szStar[] = {
	_T("������ �������� �ϴÿ���"),
	_T("������ ���� �� �ֽ��ϴ�."),
	_T("���� �ƹ� ������ ����"),
	_T("���� ���� ������ �� ���� ���մϴ�. "),
	_T("���� �ӿ� �ϳ� �� �������� ����  "),
	_T("���� �� ����� ����"),
	_T("���� ��ħ�� ���� ����̿�, "),
	_T("���Ϲ��� ���� ����̿�,"),
	_T("���� ���� û���� ������ ���� ����Դϴ�. "),
	_T("�� �ϳ��� �߾��"),
	_T("�� �ϳ��� �����"),
	_T("�� �ϳ���  �����԰�"),
	_T("�� �ϳ��� �����"),
	_T("�� �ϳ��� �ÿ�"),
	_T("�� �� ���� ��Ӵ�, ��Ӵ�"),
	_T("��Ӵ�, ���� �� �ϳ��� �Ƹ� �ٿ� �� �� ���� �ҷ� ���ϴ�. ���б� �� å���� "),
	_T("�����ߴ� ���̵��� �̸���, ��, ��, ��, �̷� �̱� �ҳ���� �̸���,"),
	_T("���� �Ʊ� ��Ӵ� �� �����ֵ��� �̸���, ������ �̿� ������� �̸���,"),
	_T("��ѱ�, ������, �䳢, ���,  ���, �����ý� ��, ���̳� ������ ����,"),
	_T("�̷� ������ �̸��� �ҷ� ���ϴ�."),
	_T("�̳׵��� �ʹ��� �ָ� �ֽ��ϴ�."),
	_T("���� �ƽ����� �� ����,"),
	_T("��Ӵ�,"),
	_T("�׸��� ����� �ָ� �ϰ����� ��ʴϴ�."),
	_T("���� �������� �׸���"),
	_T("�� ���� ������ ���� ��� ����"),
	_T("�� �̸��ڸ� �� ����,"),
	_T("������ ���� ���������ϴ�."),
	_T("����, ���� ���� ��� ������"),
	_T("�β����� �̸��� �����ϴ� ����Դϴ�."),
	_T("�׷��� �ܿ��� ������ ���� ������ ���� ����,"),
	_T("���� ���� �Ķ� �ܵ� �Ǿ����"),
	_T("�� �̸��� ���� ��� ������,"),
	_T("�ڶ�ó�� Ǯ�� ������ �ſܴ�. "),
};


void MCreateSample(void)
{
	CCWidget* pMainFrame = Core::GetInstance()->GetMainFrame();
	CCWidget* pNew = new CCFileDialog("*.*", pMainFrame, pMainFrame);
	//pNew->SetBounds(10, 10, 300, 300);
	pNew->Show(true);

	class CCFrameBitmap : public CCFrame{
		int			m_nBitmap;
	protected:
		virtual void OnDraw(CCDrawContext* pDC){
			CCFrame::OnDraw(pDC);

			sRect r = GetClientRect();

			if(CCBitmapManager::GetCount()==0) return;
			m_nBitmap %= CCBitmapManager::GetCount();
			pDC->SetBitmap(CCBitmapManager::Get(m_nBitmap));
			m_nBitmap++;
			pDC->Draw(r.x, r.y);
			//pDC->Draw(r);
		}
	public:
		CCFrameBitmap(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
			: CCFrame(szName, pParent, pListener){
			m_nBitmap = 0;
		}
		virtual ~CCFrameBitmap(void){
		}
	};

	CCWidget* pNewFrame = new CCFrameBitmap("Bitmap Test", pMainFrame, pMainFrame);
	pNewFrame->SetBounds(0, 20, 200, 200);
	pNewFrame->Show(true);

	class CCFrameInfo : public CCFrame{
		int m_nRenderCount;
		DWORD m_nPrevTime;
		int	m_nFPS;
	protected:
		virtual void OnDraw(CCDrawContext* pDC){
			CCFrame::OnDraw(pDC);

			sRect r = GetClientRect();
			char temp[256];

			pDC->SetColor(sColor(255,255,255));

			// FPS
			DWORD nCurrTime = timeGetTime();
			if(nCurrTime-m_nPrevTime>1000){
				m_nFPS = m_nRenderCount;
				m_nPrevTime = nCurrTime;
				m_nRenderCount = 0;
			}
			m_nRenderCount++;
			sprintf(temp, "FPS = %d", m_nFPS);
			pDC->Text(r.x, r.y, temp);

			// Cursor Pos
			/*
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(m_hWnd, &p);
			*/
			sPoint p = CCEvent::GetMousePos();
			sprintf(temp, "Cursor Pos = %d, %d", p.x, p.y);
			pDC->Text(r.x, r.y+GetFont()->GetHeight(), temp);

		}
	public:
		CCFrameInfo(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
			: CCFrame(szName, pParent, pListener){
			m_nRenderCount = 0;
			m_nPrevTime = timeGetTime();
			m_nFPS = 0;
		}
		virtual ~CCFrameInfo(void){
		}
	};

	CCWidget* pNewCursorInfo = new CCFrameInfo("����", pMainFrame, pMainFrame);
	pNewCursorInfo->SetBounds(400, 0, 200, 200);
	pNewCursorInfo->Show(true);

	class CCFrameStar : public CCFrame{
	protected:
		virtual void OnDraw(CCDrawContext* pDC){
			CCFrame::OnDraw(pDC);

			sRect r = GetClientRect();
			int nRawSize = sizeof(szStar)/sizeof(char*);
			static int nRaw = 0;
			pDC->SetColor(sColor(128,128,255));
			for(int i=0; i<20; i++){
				pDC->Text(r.x, r.y+i*(GetFont()->GetHeight()+2), szStar[(nRaw+i)%nRawSize]);
			}
			nRaw++;
			nRaw%=nRawSize;
		}
		virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
			/*
			if(pWidget->GetID()==CCGetResourceID("ID_OK") && IsMsg(szMessage, CCBTN_CLK_MSG)==true){
				Hide();
			}
			*/
			return false;
		}
	public:
		CCFrameStar(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL)
			: CCFrame(szName, pParent, pListener){
		}
		virtual ~CCFrameStar(void){
		}
	};

	CCWidget* pNewStar = new CCFrameStar("�� ��� ��", pMainFrame, pMainFrame);
	pNewStar->SetBounds(30, 250, 500, 200);
	pNewStar->Show(true);

	CCPopupMenu* pNewMenu = new CCPopupMenu("SampleMenu", pMainFrame, pMainFrame, CCPMT_HORIZONTAL);
	CCMenuItem* pFile = pNewMenu->AddMenuItem("File");
	CCMenuItem* pWidget = pNewMenu->AddMenuItem("Widget");
	CCPopupMenu* pFileMenu = pFile->CreateSubMenu();
	CCPopupMenu* pWidgetMenu = pWidget->CreateSubMenu();
	pFileMenu->AddMenuItem("Open");
	pFileMenu->AddMenuItem("Exit");
	pWidgetMenu->AddMenuItem("Info");
	pWidgetMenu->AddMenuItem("����¹�");

	pNewMenu->SetBounds(0, 0, CCGetWorkspaceWidth(), 16);
	pNewMenu->Show(0, 0, true);
	pNewMenu->Show(true);
}

void MDestroySample(void)
{
}

/*
TODO: Edit ������ ����
TODO: Text Area IME ����
*/