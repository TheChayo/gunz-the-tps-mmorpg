#pragma once

#include "CCWidget.h"
#include "CCLookNFeel.h"

class CCMsgBox;
class CCButtonGroup;

#define CORE_CCBUTTON			"Button"

#define CCBTN_CLK_MSG		"click"	///< ��ư�� Ŭ���ɶ� Listener���� �߻��Ǵ� �޼���
#define CCBTN_RCLK_MSG		"rclick"///< R ��ư�� Ŭ���ɶ� Listener���� �߻��Ǵ� �޼���
#define CCBTN_DN_MSG			"down"	///< ��ư�� ������
#define CCBTN_UP_MSG			"up"	///< ��ư�� �������
#define CCBTN_RDN_MSG		"rdown"	///< R��ư�� ������
#define CCBTN_RUP_MSG		"rup"	///< R��ư�� �������
#define CCBTN_IN_MSG			"in"	///< ��ư ���� Ŀ���� �÷� ����
#define CCBTN_OUT_MSG		"out"	///< ��ư ���� Ŀ���� ������ �÷� ����


/// ��ư Ÿ��
enum CCButtonType{
	CCBT_NORMAL = 0,		///< �Ϲ� ��ư
	CCBT_PUSH,			///< Ǫ�� ��ư
	CCBT_PUSH2,
};

/// ����Ʈ Ű ���
enum CCButtonKeyAssigned{
	CCBKA_NONE = 0,		///< �ƹ�Ű�� ��ϵǾ� ���� ���� ��ư
	CCBKA_ENTER,			///< Enter Ű ������ ����Ǵ� ��ư
	CCBKA_ESC,			///< ESC Ű ������ ����Ǵ� ��ư
};

class CCButton;
/// Button�� Draw �ڵ尡 �ִ� Ŭ����, �� Ŭ������ ��ӹ޾Ƽ� Ŀ���ҷ��� ���� �� �ִ�.
class CCButtonLook{
protected:
	bool	m_bWireLook;
protected:
	virtual void OnDownDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnUpDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnOverDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnDisableDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual void OnCheckBoxDraw(CCButton* pButton, CCDrawContext* pDC, bool bPushed){};
public:
	virtual void OnDrawText(CCButton* pButton, sRect& r, CCDrawContext* pDC);
	virtual void OnDraw(CCButton* pButton, CCDrawContext* pDC);
	virtual sRect GetClientRect(CCButton* pButton, sRect& r);
	void SetWireLook(bool b) {	m_bWireLook = b;	}
	bool GetWireLook() const{	return m_bWireLook; }
public:
	CCButtonLook():m_bWireLook(false){}
};


/// ��ư Ŭ����
class CCButton : public CCWidget{
protected:
	bool		m_bMouseOver;
	bool		m_bLButtonDown;
	bool		m_bRButtonDown;
	sColor		m_TextColor;
	bool		m_bShowText;
	CCAlignmentMode	m_AlignmentMode;
	CCButtonType		m_Type;
	bool		m_bChecked;				///< CCBT_PUSH�� ���
	bool		m_bComboDropped;
	bool		m_bStretch;
	
	CCButtonGroup	*m_pButtonGroup;	///< �����ִ� �׷�
	int				m_nIndexInGroup;	///< ���� �׷쳻������ index

public:
	bool		m_bEnableEnter;			///< ��Ŀ�� ������ EnterŰ�� ��ư ������ Ȱ��ȭ ( Default : true )
	bool		m_bHighlight;			///< Highlight(&) ����
	CCBitmap*	m_pIcon;				///< ������
	CCButtonKeyAssigned	m_uKeyAssigned;	///< Key Assigned
	CCMsgBox*	m_pMsgBox;				///< �޼��� �ڽ��� ���� Ȯ�� �޼���
	string		m_strIDLConfirmText;	///< xml�� ���ǵ� Ȯ�� �޽���

	sPoint		m_ClickPos;
	sPoint		m_LDragStartClickPos;
	int			m_LDragVariationX;
	int			m_LDragVariationY;

private:
	//void DrawButton(CCDrawContext* pDC);
protected:
	//virtual void OnDraw(CCDrawContext* pDC);
	/// ���콺�� ��ư �ȿ� ���ö� �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnMouseIn(void);
	/// ���콺�� ��ư ������ ���� ������ �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnMouseOut(void);
	/// ��ư�� ������ �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnButtonDown(void);
	/// ��ư�� ������� ������ �߻��ϴ� �̺�Ʈ �ڵ鷯
	virtual void OnButtonUp(void);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);

	/// Ŭ���Ǿ�������
	virtual void OnButtonClick(void);	

	virtual bool OnShow(void);
	virtual void OnHide(void);
public:
	CCButton(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~CCButton(void);
	/// ��ư �ؽ�Ʈ �÷� ����
	void SetTextColor(sColor color);
	/// ��ư �ؽ�Ʈ �÷� ���
	sColor GetTextColor(void);
	/// ��ư�� �ؽ�Ʈ �����ֱ�
	void ShowText(bool bShow=true);
	virtual bool DefaultCommand(void);

	/// ��ư�� �ؽ�Ʈ ���� ���
	CCAlignmentMode GetAlignment(void);
	/// ��ư�� �ؽ�Ʈ ���� ����
	CCAlignmentMode SetAlignment(CCAlignmentMode am);

	/// ��ư Ÿ�� ����
	void SetType(CCButtonType t);
	/// ��ư Ÿ�� ���
	CCButtonType GetType(void);

	/// Push ��ư�� ��� üũ
	void SetCheck(bool bCheck);
	/// Push ��ư�� ��� üũ�� ���
	bool GetCheck(void);

	/// ��ư�� ������ �ִ°�? ( ���콺�� ������ ���¸� �� �� �ִ�. )
	bool IsButtonDown(void);	// Async Button Down ?
	/// ���콺�� ��ư ���� �÷��� �ִ°�?
	bool IsMouseOver(void);

	/// Confirm Message Box
	void SetConfirmMessageBox(const char* szMessage);			/// ����Ʈ Ȯ�� �޽���
	void SetAlterableConfirmMessage(const char* szMessage);		/// Ư���� ��쿡 Ȯ�� �޽����� �ѵ��� �ٲ�ġ�� �� �� �ִ�
	void RestoreIDLConfirmMessage();							/// Ȯ�� �޽����� �ٲ�ġ���� �ʿ��� å������ ��������� �Ѵ�~

	virtual bool OnCommand(CCWidget* pWidget, const char* szMessage);

	int GetLDragVariationX() {
		return m_LDragVariationX;
	}

	int GetLDragVariationY() {
		return m_LDragVariationY;
	}

	DECLARE_LOOK(CCButtonLook)
	DECLARE_LOOK_CLIENT()

	void SetComboDropped(bool b) {
		m_bComboDropped = b;
	}
	bool GetComboDropped() const{
		return m_bComboDropped;
	}

	void SetStretch( bool b) {
		m_bStretch = b;
	}

	bool GetStretch( ) const {
		return m_bStretch;
	}

	void SetButtonGroup(CCButtonGroup *pGroup);

	int GetIndexInGroup() { 
		return m_nIndexInGroup; 
	}

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);

	virtual const char* GetClassName(void){ return CORE_CCBUTTON; }
};



// ��ư �׷��� �����ϴµ� �ʿ��� Ŭ����
class CCButtonGroup {
	friend CCButton;
public:
	CCButtonGroup();
	~CCButtonGroup();

protected:
	int		m_nCount;
	CCButton *m_pPrevious;
};
