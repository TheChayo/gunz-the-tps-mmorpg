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

struct RCHARINFO : public MemPoolSm<RCHARINFO> {		// �ѱ����� ������ ���´�
	int nWidth;
	int nFontTextureID;
	int nFontTextureIndex;
};

typedef map<WORD,RCHARINFO*> RCHARINFOMAP;

struct RFONTTEXTURECELLINFO;

typedef list<RFONTTEXTURECELLINFO*> RFONTTEXTURECELLINFOLIST;


// Ŀ�ٶ� �ؽ����� RFontTexture���� �� ���� ������ ����ִ� ����ü
struct RFONTTEXTURECELLINFO {
	int nID;
	int nIndex;
	RFONTTEXTURECELLINFOLIST::iterator itr;
};


class RFontTexture {		// �������ڸ� �����ϰ� �ִ� Ŀ�ٶ� �ؽ��� ����

	// �ؽ��Ŀ� �� ���ڸ� �׸��� �ӽ� dc & dibsection
	HDC		m_hDC;
	DWORD	*m_pBitmapBits;
	HBITMAP m_hbmBitmap;
//	HFONT	hPrevFont;
	HBITMAP m_hPrevBitmap;


	LPDIRECT3DTEXTURE9		m_pTexture;
	int m_iWidth;
	int	m_iHeight;
	int m_iX,m_iY;
	int m_iCell;			// ���� ���� = nX * nY
	int m_LastUsedID;

	int m_iCellSize;

	RFONTTEXTURECELLINFO	*m_CellInfo;
	RFONTTEXTURECELLINFOLIST m_PriorityQueue;	// ���� �ֱٿ� ���� ���� �������� �����Ѵ�

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
	bool IsNeedUpdate(int nIndex, int nID);		// ���ŵǾ�� �ϴ��� �˻�
	
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

	static	bool	m_bInFont;		// beginfont endfont ���̿� �ִ���.

public:
	RFont(void);
	virtual ~RFont(void);

	bool Create(const TCHAR* szFontName, int nHeight, bool bBold=false, bool bItalic=false, int nOutlineStyle=0, int nCacheSize=-1, bool bAntiAlias=false, DWORD nColorArg1=0, DWORD nColorArg2=0);
	void Destroy(void);

	bool BeginFont();
	bool EndFont();

	void DrawText(float x, float y, const TCHAR* szText, DWORD dwColor=0xFFFFFFFF, float fScale=1.0f);

	int GetHeight(void){ return m_iHeight; }
	int GetTextWidth(const TCHAR* szText, int nSize=-1);
};

// debug
bool DumpFontTexture();
bool RFontCreate();
void RFontDestroy();

_NAMESPACE_REALSPACE2_END
