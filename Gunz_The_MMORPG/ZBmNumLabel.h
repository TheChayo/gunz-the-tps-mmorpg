#ifndef ZBMNUMLABEL_H
#define ZBMNUMLABEL_H

#include "CCWidget.h"
#include "CCDrawContext.h"
#include "CCLookNFeel.h"

#define BMNUM_NUMOFCHARSET		16

/// ��Ʈ������ �׸� ���̺�.	�κ� ���� ���̺��̴�.
/// ��Ʈ�ʿ� 0~7 / 8 9 , / .   ������ �׷��� �־�� �Ѵ�.
class ZBmNumLabel : public CCWidget{
protected:
	CCBitmap*		m_pLabelBitmap;
	sSize			m_CharSize;
	CCAlignmentMode	m_AlignmentMode;
	int				m_nIndexOffset;
	int				m_nCharMargin[ BMNUM_NUMOFCHARSET];

	virtual void OnDraw(CCDrawContext* pDC);
public:
	ZBmNumLabel(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);

	void SetLabelBitmap(CCBitmap* pLabelBitmap);
	void SetCharSize(sSize &size);
	void SetNumber(int n,bool bAddComma = false);
	void SetIndexOffset(int nOffset) { m_nIndexOffset = nOffset; }
	void SetCharMargin( int* nMargin);
	void SetAlignmentMode( CCAlignmentMode am)			{ m_AlignmentMode = am; }

#define CORE_ZBMNUMLABEL	"BmNumLabel"
	virtual const char* GetClassName(){ return CORE_ZBMNUMLABEL; }
};

#endif