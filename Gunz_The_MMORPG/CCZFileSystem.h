#pragma once

#include <list>
#include <map>
#include <string>
#include "CCZip.h"

using namespace std;

#define DEF_EXT	"mrs"

// File Descrition ( by Zip File System )
struct CCZFILEDESC{
	char	m_szFileName[_MAX_PATH];		// ���ϸ� ( ���� ������ ��쵵 �ְ�, zip ���Ͼȿ� ���� �ִ� ��쵵 �ִ�. )
	char	m_szZFileName[_MAX_PATH];		// "" �̸� ���� ������ �ǹ��Ѵ�.
	int		m_iSize;
	unsigned int	m_crc32;
	unsigned long	m_modTime;	// zip ���� �ȿ� �ִ� ���ϵ��� ���� dos time ���� �����Ѵ�
};

typedef map<string,CCZFILEDESC*>	CCFLIST;
typedef CCFLIST::iterator			CCFLISTITOR;

// iterator �� �����������Ƿ� create �� ���Ŀ� ���ϸ���� �߰��Ǹ� ����ϴ�. �׷���� �ڵ� ������ �ʿ�
void GetRefineFilename(char *szRefine, const char *szSource);

// ���ϸ����� ����� �� ���� ���ڰ� ���ԵǾ� ������ ��ü���ڷ� ��ü�Ѵ�. ������ ���ϸ� �Ծ��� ���Ұ����� �ܿ��� ���������� �� �����Ƿ�
// �� �Լ��� ���ĵ� ���ϸ� �Ծ࿡ ������ �����Ѵٴ� ������ ���ٴ� ���� ����
void ValidateFilename(char* szOut, const char* szSrc, char substitute);

unsigned CCGetCRC32(const char *data, int nLength);
class CCZFileCheckList;

class CCZFileSystem{
	char		m_szBasePath[256];
	CCFLIST		m_ZFileList;
	CCFLISTITOR	m_iterator;
	int			m_nIndex;

	char		m_szUpdateName[256];

	CCZFileCheckList *m_pCheckList;

protected:
	bool AddItem(CCZFILEDESC*);

	void RemoveFileList(void);
	// RefreshFileList Pure File List
	void RefreshFileList(const char* szBasePath);
	// Expand Z File List From File List
//	void ExpandZFileList(CCFLIST* pZFList);
	// '\' -> '/'

	int GetUpdatePackageNumber(const char *szPackageFileName);

public:
	CCZFileSystem(void);
	virtual ~CCZFileSystem(void);

	bool Create(const char* szBasePath,const char* szUpdateName=NULL);
	void Destroy(void);

	int GetFileCount(void) const;
	const char* GetFileName(int i);
	const CCZFILEDESC* GetFileDesc(int i);

	const char *GetBasePath() { return m_szBasePath; }

	CCZFILEDESC* GetFileDesc(const char* szFileName);

	unsigned int GetCRC32(const char* szFileName);
	unsigned int GetTotalCRC();

	int GetFileLength(const char* szFileName);
	int GetFileLength(int i);

	bool IsZipFile(const char* szFileName);

	bool ReadFile(const char* szFileName, void* pData, int nMaxSize);

	void SetFileCheckList(CCZFileCheckList *pCheckList) { m_pCheckList = pCheckList; }
	CCZFileCheckList *GetFileCheckList()	{ return m_pCheckList; }


	void SetPrivateKey( const unsigned char* pPrivateKey, size_t length);
};


class CCZFile{
public:
	enum SeekPos { begin = 0x0, current = 0x1, end = 0x2 };

protected:
	FILE*	m_fp;
	bool	m_IsZipFile;
	bool	m_IsBufferd;
	bool	m_IsEncrypted;

	CCZip	m_Zip;

	char*	m_pData;
	unsigned long	m_nFileSize;
	unsigned long	m_nEncryptFileSize;

	int		m_nPos;

	int		m_nIndexInZip;
	unsigned int	m_crc32;

	char	m_FileName[256];
	char	m_ZipFileName[256];

	static  unsigned long m_dwReadMode;

public:

	CCZFile();
	virtual ~CCZFile();

	bool Create();

//	bool Open(const char* szFileName);		// �� ����� �׳� ���Ͽ��� ���� �ϴ� ������� �Ʒ� ��ǿ� ��ģ��.
	bool Open(const char* szFileName, CCZFileSystem* pZFS = NULL);
	bool Open(const char* szFileName, const char* szZipFileName, bool bFileCheck = false , unsigned int crc32 = 0);

	bool Seek(long off,int mode);

	void Close(void);

	static void SetReadMode(unsigned long mode) {	m_dwReadMode = mode; }
	static unsigned long GetReadMode(void)  { return m_dwReadMode; }
	static bool isMode(unsigned long mode ) { return (m_dwReadMode & mode) ? true : false ; }

	unsigned long GetLength(void);
	bool Read(void* pBuffer, int nMaxSize);

};

class CCZFileCheckList
{
	unsigned int				m_crc32;		// ���� ��ü�� crc32
	map<string,unsigned int>	m_fileList;

public:
	bool Open(const char *szFileName, CCZFileSystem *pfs = NULL);

	unsigned int GetCRC32(const char *szFileName);	// Ư�������� crc32�� ����
	unsigned int GetCRC32()	{ return m_crc32; }		// �������� crc32�� ����
};


/*
	////	���� ����		////

	CCZFileSystem fs;

	fs.Create("./");

	char buffer[1000];

	CCZFile mzf;

	mzf.Open("6.txt");

	int size = mzf.GetLength();
	mzf.Read(buffer,size);

	mzf.Close();

	mzf.Open("5.txt","test2.zip");
//	mzf.Read(buffer,mzf.GetLength());
	mzf.Read(buffer,4);
	mzf.Read((buffer+4),6);

	mzf.Seek(10,CCZFile::begin);
	mzf.Read(buffer,4);

	mzf.Seek(2,CCZFile::current);
	mzf.Read(buffer,4);

	mzf.Seek(-2,CCZFile::end);
	mzf.Read(buffer,4);

	mzf.Open("4.txt","test2.zip");
	mzf.Read(buffer,mzf.GetLength());

	mzf.Close();

	mzf.Open("5.txt",&fs);
	mzf.Read(buffer,mzf.GetLength());

	mzf.Open("4.txt",&fs);
	mzf.Read(buffer,mzf.GetLength());

	mzf.Open("6.txt",&fs);
	mzf.Read(buffer,mzf.GetLength());

	mzf.Close();

	fs.Destroy();
	*/