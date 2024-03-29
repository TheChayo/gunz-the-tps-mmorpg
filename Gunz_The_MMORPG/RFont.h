#pragma once
#pragma warning(disable: 4786)

#include "RTypes.h"
#include "DirectX/D3dx9.h"
#include <map>
#include <list>
#include <string>
#include "Realspace2.h"
#include "MemPool.h"

using namespace std;


bool CheckFont();
void SetUserDefineFont(char* FontName);

_NAMESPACE_REALSPACE2_BEGIN

struct RCHARINFO : public MemPoolSm<RCHARINFO> {		// 한글자의 정보를 갖는다
	int nWidth;
	int nFontTextureID;
	int nFontTextureIndex;
};

typedef map<WORD,RCHARINFO*> RCHARINFOMAP;

struct RFONTTEXTURECELLINFO;

typedef list<RFONTTEXTURECELLINFO*> RFONTTEXTURECELLINFOLIST;


// 커다란 텍스쳐인 RFontTexture안의 각 셀의 정보를 담고있는 구조체
struct RFONTTEXTURECELLINFO {
	int nID;
	int nIndex;
	RFONTTEXTURECELLINFOLIST::iterator itr;
};


class RFontTexture {		// 여러글자를 저장하고 있는 커다란 텍스쳐 한장

	// 텍스쳐에 들어갈 글자를 그리는 임시 dc & dibsection
	HDC		m_hDC;
	DWORD	*m_pBitmapBits;
	HBITMAP m_hbmBitmap;
//	HFONT	hPrevFont;
	HBITMAP m_hPrevBitmap;


	LPDIRECT3DTEXTURE9		m_pTexture;
	int m_iWidth;
	int	m_iHeight;
	int m_iX,m_iY;
	int m_iCell;			// 셀의 개수 = nX * nY
	int m_LastUsedID;

	int m_iCellSize;

	RFONTTEXTURECELLINFO	*m_CellInfo;
	RFONTTEXTURECELLINFOLIST m_PriorityQueue;	// 가장 최근에 쓰인 것이 뒤쪽으로 정렬한다

	bool UploadTexture(RCHARINFO *pCharInfo,DWORD* pBitmapBits,int w,int h);

	bool InitCellInfo();
	void ReleaseCellInfo();

public:
	RFontTexture();
	~RFontTexture();

	bool Create();
	void Destroy();

	LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }

	int GetCharWidth(HFONT hFont, const TCHAR* szChar);
	bool MakeFontBitmap(HFONT hFont, RCHARINFO *pInfo, const TCHAR* szText, int nOutlineStyle, DWORD nColorArg1, DWORD nColorArg2);
	bool IsNeedUpdate(int nIndex, int nID);		// 갱신되어야 하는지 검사
	
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	int GetCellCountX() { return m_iX; }
	int GetCellCountY() { return m_iY; }
	
	int GetCellSize() { return m_iCellSize; }
	void ChangeCellSize(int size);
};


class RFont {
	HFONT	m_hFont;	// Font Handle
	int		m_iHeight;
	int		m_iOutlineStyle;
//	int		m_iSamplingMultiplier;
	bool	m_bAntiAlias;

	DWORD	m_ColorArg1;
	DWORD	m_ColorArg2;

	RCHARINFOMAP m_CharInfoMap;
	RFontTexture *m_pFontTexture;

	static	bool	m_bInFont;		// beginfont endfont 사이에 있는지.

public:
	RFont();
	virtual ~RFont();

	bool Create(const TCHAR* szFontName, int nHeight, bool bBold=false, bool bItalic=false, int nOutlineStyle=0, int nCacheSize=-1, bool bAntiAlias=false, DWORD nColorArg1=0, DWORD nColorArg2=0);
	void Destroy();

	bool BeginFont();
	bool EndFont();

	void DrawText(float x, float y, const TCHAR* szText, DWORD dwColor=0xFFFFFFFF, float fScale=1.0f);

	int GetHeight(){ return m_iHeight; }
	int GetTextWidth(const TCHAR* szText, int nSize=-1);
};

// debug
bool DumpFontTexture();
bool RFontCreate();
void RFontDestroy();

_NAMESPACE_REALSPACE2_END
