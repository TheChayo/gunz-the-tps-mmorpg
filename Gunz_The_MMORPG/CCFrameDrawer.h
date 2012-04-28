#pragma once
#include "CCDrawContext.h"

enum CCFDTextStyle{
	CCFDTS_NORMAL,
	CCFDTS_ACTIVE,
	CCFDTS_DISABLE,
};

////////////////////////////////////////////////////
// Frame�� �׸��� ���� Ŭ����
// �̸� ���� ��Ų�� ������ �� �ִ�.
// Default Frame Drawer
class MFrameDrawer{
public:
	virtual void DrawOuterBevel(CCDrawContext* pDC, sRect& r);		// �ٱ��� ���� �׸���
	virtual void DrawInnerBevel(CCDrawContext* pDC, sRect& r);		// ���� ���� �׸���
	virtual void DrawFlatBevel(CCDrawContext* pDC, sRect& r);		// ��� ���� �׸��� ( ���� ��������... )
	virtual void DrawOuterPlane(CCDrawContext* pDC, sRect& r);		// �ٱ��� ��
	virtual void DrawInnerPlane(CCDrawContext* pDC, sRect& r);		// ���� ��
	virtual void DrawFlatPlane(CCDrawContext* pDC, sRect& r);		// ��� ��
	virtual void DrawOuterBorder(CCDrawContext* pDC, sRect& r);		// �ٱ��� ��(Bevel+Plane) �׸���
	virtual void DrawInnerBorder(CCDrawContext* pDC, sRect& r);		// ���� ��(Bevel+Plane) �׸���
	virtual void DrawFlatBorder(CCDrawContext* pDC, sRect& r);		// �Ϲ����� ��� ��(Bevel+Plane) �׸���
	virtual void Text(CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode am=MAM_NOTALIGN, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false);
	virtual void Text(CCDrawContext* pDC, sPoint& p, const char* szText, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false, sRect* r=NULL);
	virtual void TextMC(CCDrawContext* pDC, sRect& r, const char* szText, CCAlignmentMode am=MAM_NOTALIGN, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false);
	virtual void TextMC(CCDrawContext* pDC, sPoint& p, const char* szText, CCFDTextStyle nTextStyle=CCFDTS_NORMAL, bool bHighlight=false, sRect* r=NULL);
};

#endif