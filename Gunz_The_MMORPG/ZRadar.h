#ifndef _ZRADAR_H
#define _ZRADAR_H

#include "ZInterface.h"
#include "CCPicture.h"

struct ZRadarNode
{
	bool			bShoted;
	unsigned long	nLastShotTime;
//	float			fRot;
	int				x[4];
	int				y[4];
};
class ZRadar : public ZInterface
{
protected:
	CCBitmapR2*		m_pBitmap;
	float			m_fMaxDistance;

	ZRadarNode		m_Nodes[8];
	void RotatePixel(int* poutX, int* poutY, int sx, int sy, int nHotSpotX, int nHotSpotY, float fAngle);
public:
	ZRadar(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZRadar();
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void OnDraw(CCDrawContext* pDC);
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	void OnAttack(rvector& pAttackerPos);

	void SetMaxDistance(float fDist);
	float GetMaxDistance() { return m_fMaxDistance; }
};

#endif