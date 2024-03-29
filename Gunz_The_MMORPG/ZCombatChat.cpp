#include "stdafx.h"

#include "ZCombatChat.h"
#include "ZGameInterface.h"
#include "ZInterfaceItem.h"
#include "ZApplication.h"
#include "ZInterfaceListener.h"
//#include "CCListBox.h"
#include "ZIDLResource.h"
//#include "ZPost.h"
//#include "CCChattingFilter.h"
#include "CCTextArea.h"
#include "ZConfiguration.h"

// Added
//#include "ZCombatInterface.h"
#include "CCLabel.h"

#define MAX_CHAT_OUTPUT_LINE 7


/////////////////
// ZTabPlayerList
ZTabPlayerList::ZTabPlayerList(const char* szName, CCWidget* pParent, CCListener* pListener)
: CCListBox(szName, pParent, pListener)
{
	SetChatControl(NULL);
}

bool ZTabPlayerList::OnShow()
{
	RemoveAll();

	// Push Target List
//for(ZCharacterManager::iterator i=ZGetCharacterManager()->begin(); i!=ZGetCharacterManager()->end();i++)
	{
//		ZCharacter *pChar = i->second;
//		if(pChar->IsAdminHide()) continue;
//		Add(pChar->GetProperty()->GetName());
	}

	return true;
}

void ZTabPlayerList::OnHide()
{
	if (m_pEditChat)
		m_pEditChat->SetFocus();
}

bool ZTabPlayerList::OnEvent(CCEvent* pEvent, CCListener* pListener)
{
	if(pEvent->iMessage==CCWM_KEYDOWN){
		if(pEvent->uKey==VK_ESCAPE) 
		{
			Show(false);
			return true;
		}
		else if( (pEvent->uKey==VK_TAB) || (pEvent->uKey==VK_RETURN) || (pEvent->uKey==VK_SPACE))
		{
			OnPickPlayer();
			Show(false);
			return true;
		}
	}
	return CCListBox::OnEvent(pEvent, pListener);
}

void ZTabPlayerList::OnPickPlayer()
{
	const char* pszString = GetSelItemString();
	if (m_pEditChat && pszString) {
		m_pEditChat->AddText(pszString);
	}
}
/////////////////

class MCombatChatInputListener : public CCListener{
public:
	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage){
		if(CCWidget::IsMsg(szMessage, CCEDIT_ENTER_VALUE)==true)
		{
			if (strlen(pWidget->GetText()) >= 256) return false;

			const char* szCommand = pWidget->GetText();
			if (szCommand[0] != '\0')
			{
				char szMsg[512];
				strcpy(szMsg, szCommand);
				strcpy(szMsg, pWidget->GetText());

				if (!ZGetConfiguration()->GetViewGameChat())
				{
					if (ZGetCombatInterface())
					{
//					ZGetCombatInterface()->ShowChatOutput(true);
					}
				}
//			ZApplication::GetGameInterface()->GetChat()->Input(szMsg);
			}

			pWidget->SetText("");
			if (ZGetCombatInterface())
			{
//			ZGetCombatInterface()->EnableInputChat(false);
			}
			return true;
		}
		else if(CCWidget::IsMsg(szMessage, CCEDIT_ESC_VALUE)==true)
		{
			pWidget->SetText("");
//		ZGetCombatInterface()->EnableInputChat(false);
		}
		else if ((CCWidget::IsMsg(szMessage, CCEDIT_CHAR_MSG)==true) || (CCWidget::IsMsg(szMessage, CCEDIT_KEYDOWN_MSG)==true))
		{
//		ZApplication::GetGameInterface()->GetChat()->FilterWhisperKey(pWidget);
		}


		return false;
	}
};
MCombatChatInputListener	g_CombatChatInputListener;

CCListener* ZGetCombatChatInputListener()
{
	return &g_CombatChatInputListener;
}


ZCombatChat::ZCombatChat()
{
	m_bChatInputVisible = true;
	m_nLastChattingMsgTime = 0;
	m_pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	m_pChattingOutput = NULL;
	m_pInputEdit = NULL;
	m_pTabPlayerList = NULL;
	m_bTeamChat = false;
	m_bShowOutput = true;
}

ZCombatChat::~ZCombatChat()
{

}

bool ZCombatChat::Create( const char* szOutputTxtarea,bool bUsePlayerList)
{
	CCWidget* pWidget = m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATINPUT);
	if (pWidget!=NULL)
	{
		pWidget->SetListener(ZGetCombatChatInputListener());
	}

	m_pChattingOutput = NULL;
//	pWidget = m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATOUTPUT);
	pWidget = m_pIDLResource->FindWidget( szOutputTxtarea);
	if (pWidget!=NULL)
	{
		m_pChattingOutput = ((CCTextArea*)pWidget);
	}

	if (m_pChattingOutput != NULL)
	{
		m_pChattingOutput->Clear();
//		m_pChattingOutput->RemoveAll();
//		m_pChattingOutput->GetScrollBar()->Show(false);
	}
	
	pWidget = m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATINPUT);
	if (pWidget!=NULL)
	{
		m_pInputEdit = (CCEdit*)pWidget;
		m_pInputEdit->Show(false);

		if(bUsePlayerList)
		{
			// TabPlayerList
			CCWidget* pPivot = m_pInputEdit->GetParent();
//		m_pTabPlayerList = new ZTabPlayerList("TabPlayerList", pPivot, ZGetCombatInterface());
			m_pTabPlayerList->Show(false);
			m_pTabPlayerList->SetBounds(m_pInputEdit->GetPosition().x, m_pInputEdit->GetPosition().y-120-5, 150, 120);
			m_pTabPlayerList->SetChatControl(m_pInputEdit);
			m_pInputEdit->SetTabHandler(m_pTabPlayerList);
		}
	}

	CCLabel* pLabelToTeam = (CCLabel*)m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATMODE_TOTEAM);
	if (pLabelToTeam) pLabelToTeam->Show(false);
	CCLabel* pLabelToAll = (CCLabel*)m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATMODE_TOALL);
	if (pLabelToAll) pLabelToAll->Show(false);

	return true;
}

void ZCombatChat::Destroy()
{
	if (m_pTabPlayerList) 
	{
		delete m_pTabPlayerList;
		m_pTabPlayerList = NULL;
	}
	if (m_pInputEdit)
	{
		m_pInputEdit->SetListener(NULL);
	}
	m_pChattingOutput = NULL;
}

void ZCombatChat::Update()
{
	UpdateChattingBox();
	if ((m_pInputEdit) && (m_bChatInputVisible))
	{
		if (!m_pInputEdit->IsFocus())
		{
			if (m_pTabPlayerList && !m_pTabPlayerList->IsFocus())
				EnableInput(false);
		}
	}
}

void ZCombatChat::UpdateChattingBox()
{
	if (m_pChattingOutput == NULL) return;

	if (m_pChattingOutput->GetLineCount() > 0)
	{
		unsigned long int nNowTime = timeGetTime();

#define CHAT_DELAY_TIME	5000
		if ((nNowTime - m_nLastChattingMsgTime) > CHAT_DELAY_TIME)
		{
			m_pChattingOutput->DeleteFirstLine();
			m_nLastChattingMsgTime = nNowTime;
		}
	}
}


void ZCombatChat::EnableInput(bool bEnable, bool bToTeam)
{
	CCLabel* pLabelToTeam = (CCLabel*)m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATMODE_TOTEAM);
	CCLabel* pLabelToAll = (CCLabel*)m_pIDLResource->FindWidget(ZIITEM_COMBAT_CHATMODE_TOALL);
	if (bEnable == true) {
		SetTeamChat(bToTeam);
		if (bToTeam) {
			if (pLabelToTeam!=NULL) pLabelToTeam->Show(true);
			if (pLabelToAll!=NULL) pLabelToAll->Show(false);
		} else {
			if (pLabelToTeam!=NULL) pLabelToTeam->Show(false);
			if (pLabelToAll!=NULL) pLabelToAll->Show(true);
		}
	} else {
		if (pLabelToTeam!=NULL) pLabelToTeam->Show(false);
		if (pLabelToAll!=NULL) pLabelToAll->Show(false);
	}

	if (m_pInputEdit)
	{
		m_pInputEdit->Show(bEnable);
	}

	if (bEnable)
	{
		if (m_pInputEdit)
		{
			if (!m_pInputEdit->IsFocus()) m_pInputEdit->SetFocus();
		}
	}
	else
	{
//		ZApplication::GetGameInterface()->SetFocus();
	}

	m_bChatInputVisible = bEnable;

//	if (ZGetGame()->m_pMyCharacter == NULL) return;

	if(bEnable)
	{
//	ZPostPeerChatIcon(true);
	}else
	{
//	ZPostPeerChatIcon(false);
	}
}

void ZCombatChat::OutputChatMsg(const char* szMsg)
{
	if (m_pChattingOutput == NULL) return;

	if (m_pChattingOutput->GetLineCount() == 0)
		for (int i = 0; i < (MAX_CHAT_OUTPUT_LINE-1); i++) m_pChattingOutput->AddText("");
	m_pChattingOutput->AddText(szMsg);

	ProcessChatMsg();
}

void ZCombatChat::OutputChatMsg(sColor color, const char* szMsg)
{
	if (m_pChattingOutput == NULL) return;

	if (m_pChattingOutput->GetLineCount() == 0)
		for (int i = 0; i < (MAX_CHAT_OUTPUT_LINE-1); i++) m_pChattingOutput->AddText("");
	m_pChattingOutput->AddText(szMsg, color);

	ProcessChatMsg();
}

void ZCombatChat::ProcessChatMsg()
{
	while ((m_pChattingOutput->GetLineCount() > MAX_CHAT_OUTPUT_LINE))
	{
		m_pChattingOutput->DeleteFirstLine();
	}

	
	if (m_pChattingOutput->GetLineCount() >= 1)
	{
		m_nLastChattingMsgTime = timeGetTime();
	}
}


void ZCombatChat::OnDraw(CCDrawContext* pDC)
{
	if (m_pInputEdit)
	{
		if (m_pInputEdit->IsVisible())
		{
			pDC->SetColor(0xFF, 0xFF, 0xFF, 50);
			pDC->FillRectangle(m_pInputEdit->GetScreenRect());
		}
	}
}

void ZCombatChat::SetFont( CCFont* pFont)
{
	m_pChattingOutput->SetFont( pFont);
}


void ZCombatChat::ShowOutput(bool bShow)
{
	if (m_pChattingOutput) m_pChattingOutput->Show(bShow);
	m_bShowOutput = bShow;
}