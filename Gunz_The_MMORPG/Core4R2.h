#pragma once
#include "CCDrawContext.h"
#include "RFont.h"
#include "RBaseTexture.h"
#include "DirectX/D3dx9.h"

#define CORE_R2_CLASS_TYPE	0x1130		// RTTI와 같은 기능을 _DEBUG모드에서 구현하기 위한 ID

class CCDrawContextR2 : public CCDrawContext{
protected:
	LPDIRECT3DDEVICE9	m_pd3dDevice;
public:
	CCDrawContextR2(LPDIRECT3DDEVICE9 pd3dDevice);
	virtual ~CCDrawContextR2();

	// Basic Drawing Functions
	virtual void SetPixel(int x, int y, sColor& color);
	virtual void HLine(int x, int y, int len);
	virtual void VLine(int x, int y, int len);
	virtual void Line(int sx, int sy, int ex, int ey);
	virtual void Rectangle(int x, int y, int cx, int cy);
	virtual void FillRectangle(int x, int y, int cx, int cy);
	virtual void FillRectangleW(int x, int y, int cx, int cy);

	// Bitmap Drawing
	virtual void DrawEx(int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4);

	// Text
	virtual bool BeginFont();
	virtual bool EndFont();

	virtual int Text(int x, int y, const char* szText);

	virtual void SetClipRect(sRect& r);

private:
	virtual void Draw(CCBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool bMirrorX=false, bool bMirrorY=false );
};

class CCBitmapR2 : public CCBitmap{
public:
	LPDIRECT3DDEVICE9		m_pd3dDevice;		// Local Copy
	//LPD3DXSPRITE			m_pSprite;
	RealSpace2::RBaseTexture *m_pTexture;
//	LPDIRECT3DTEXTURE9		m_pTexture;
	int						m_nWidth;				// 텍스처가 언로드되었더라도 길이 정보를 알려줄 수 있도록 기억
	int						m_nHeight;
	bool					m_bUnloadedTemporarily;	// 텍스처를 임시로 언로드한 상태

	static	DWORD			m_dwStateBlock;

protected:
	void BeginState(CCDrawEffect effect);
	void EndState();

public:
	CCBitmapR2();
	virtual ~CCBitmapR2();

	virtual bool Create(const char* szAliasName, LPDIRECT3DDEVICE9 pd3dDevice, const char* szFileName,bool bUseFileSystem=true,DWORD colorkey=0);
//	virtual bool Create(const char* szAliasName, LPDIRECT3DDEVICE9 pd3dDevice, void* pData, int nSize);
	virtual void Destroy();

	// 텍스처를 임시로 메모리에서 내린다. Draw를 하려고 하면 다시 파일에서 로드된다 (메모리 확보 목적)
	void UnloadTextureTemporarily();

	virtual int GetWidth();
	virtual int GetHeight();

	void CheckDrawMode(float* fuv);

	virtual void Draw(float x, float y, float w, float h, float sx, float sy, float sw, float sh, 
		      DWORD dwColor, CCDrawEffect effect=CCDE_NORMAL, bool bMirrorX=false, bool bMirrorY=false);
	virtual void DrawEx(float tx1, float ty1, float tx2, float ty2, 
		        float tx3, float ty3, float tx4, float ty4, DWORD dwColor, CCDrawEffect effect=CCDE_NORMAL);

	void OnLostDevice();
	void OnResetDevice();
};

class CCFontR2 : public CCFont{
public:
	bool		m_bAntiAlias;
	float		m_fScale;
	_RS2::RFont	m_Font;
public:
	CCFontR2();
	virtual ~CCFontR2();

	virtual bool Create(const char* szAliasName, const char* szFontName, int nHeight, float fScale=1.0f, bool bBold=false, bool bItalic=false, int nOutlineStyle=0, int nCacheSize=-1, bool bAntiAlias = false, DWORD nColorArg1=0, DWORD nColorArg2=0);
	virtual void Destroy();

	virtual int GetHeight();
	virtual int GetWidth(const char* szText, int nSize=-1);

	virtual bool Resize(float ratio, int nMinimumHeight);

private:
	struct CreationInfo // 리사이즈시 재사용하기 위해 Create시 사용한 인자들을 저장해둠
	{
		char szFontName[128];
		int nHeight;
		bool bBold;
		bool bItalic;
		int nCacheSize;
		CreationInfo() : nHeight(5), bBold(false), bItalic(false), nCacheSize(-1) { szFontName[0]=0; }
	} m_creationInfo;
};
