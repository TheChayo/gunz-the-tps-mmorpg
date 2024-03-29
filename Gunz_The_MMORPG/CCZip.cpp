#include "stdafx.h"
#include "CCZip.h"
#include "zlib.h"
#include <memory.h>
#include <string.h>
#include "CCDebug.h"
#include <tchar.h>
#include <io.h>

typedef unsigned long dword;
typedef unsigned short word;

#define MRS_ZIP_CODE	0x05030207
#define MRS2_ZIP_CODE	0x05030208
#define CHECK_ZLIB_CRC32

#pragma pack(2)
struct CCZip::CCZIPLOCALHEADER{
	enum{
		SIGNATURE   = 0x04034b50,
		SIGNATURE2  = 0x85840000,
		COMP_STORE  = 0,
		COMP_DEFLAT = 8,
	};

	dword   sig;
	word    version;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;
	dword   ucSize;
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
};

struct CCZip::CCZIPDIRHEADER{
	enum{
		SIGNATURE = 0x06054b50,
	};

	dword   sig;
	word    nDisk;
	word    nStartDisk;
	word    nDirEntries;
	word    totalDirEntries;
	dword   dirSize;
	dword   dirOffset;
	word    cmntLen;
};

struct CCZip::CCZIPDIRFILEHEADER{
	enum{
		SIGNATURE   = 0x02014b50,
		SIGNATURE2  = 0x05024b80,
		COMP_STORE  = 0,
		COMP_DEFLAT = 8,
	};

	dword   sig;
	word    verMade;
	word    verNeeded;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;            // Compressed size
	dword   ucSize;           // Uncompressed size
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
	word    cmntLen;          // Comment field follows extra field.
	word    diskStart;
	word    intAttr;
	dword   extAttr;
	dword   hdrOffset;

	char *GetName   () const { return (char *)(this + 1);   }
	char *GetExtra  () const { return GetName() + fnameLen; }
	char *GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

//To apply different ...Toggling the data now becomes... applied algorithm.

void ConvertChar(char* pData,int _size)
{
	if(!pData) return;

	WORD w;
	BYTE b,bh;

	for(int i=0;i<_size;i++) {
		b = *pData ^ 0xFF;
		w = b<<3;
		bh = (w&0xff00)>>8;
		b = w&0xff;
		*pData = BYTE( b | bh );
		pData++;

	}
}

void RecoveryChar(char* pData,int _size)
{
	if(!pData) return;

	BYTE b,bh,d;

	for(int i=0;i<_size;i++) {

		b = *pData;
		bh = b&0x07;
		d = (bh<<5)|(b>>3);
		*pData = d ^ 0xff;
		pData++;
	}
}


CCZip::CCZip()
{
	m_fp = NULL;
	m_pDirData = NULL;
	m_ppDir = NULL;
	m_iDirEntries = 0;
	m_iZipMode = CCModeZip;
//	m_dwReadMode = CCZIPREADFLAG_ZIP | CCZIPREADFLAG_MRS | CCZIPREADFLAG_MRS2 | CCZIPREADFLAG_FILE;
	m_dwReadMode = 0;
}

CCZip::~CCZip()
{
	Finalize();
}

bool CCZip::isReadAble(unsigned long mode)
{
	if(m_iZipMode == CCModeZip) {
		return ( CCZIPREADFLAG_ZIP & mode) ? true : false ; 	
	}
	else if(m_iZipMode == CCModeMrs) {
		return ( CCZIPREADFLAG_MRS & mode) ? true : false ; 
	}
	else if(m_iZipMode == CCModeMrs2) {
		return ( CCZIPREADFLAG_MRS2 & mode) ? true : false ; 
	}
	return false;
}

bool CCZip::Initialize(FILE* fp,unsigned long ReadMode)
{
	if(fp==NULL) return false;

	//Zip, mrs1, mrs2 should read.
	//Publish to... zip prevents it from reading mode to put options. 
	//Files + zip you can not read or + mrs1 availability.
	//Zip Do you need to read?

	m_dwReadMode = ReadMode;

	if(isZip(fp)) {
		m_iZipMode = CCModeZip;
		//If the player that is not supported by ...
		if(isMode(CCZIPREADFLAG_ZIP)==false)
			return false;
	}
	else if(isVersion1Mrs(fp)) {
		m_iZipMode = CCModeMrs;
		if(isMode(CCZIPREADFLAG_MRS)==false)
			return false;
	}
	else {//mrs2 more ...
		m_iZipMode = CCModeMrs2;
		if(isMode(CCZIPREADFLAG_MRS2)==false)
			return false;
	}
	
	CCZIPDIRHEADER dh;

	fseek(fp, -(int)sizeof(dh), SEEK_END);
	long dhOffset = ftell(fp);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, fp);

	if( m_iZipMode>=CCModeMrs2 )							// mrs2 more data from the recovery.
		RecoveryChar((char*)&dh,sizeof(CCZIPDIRHEADER));		//v2 is greater than ...

	//If you are manipulating the data .... zip, mrs1, mrs2 Unless the all ...

	if( dh.sig != MRS2_ZIP_CODE && dh.sig != MRS_ZIP_CODE && dh.sig != CCZIPDIRHEADER::SIGNATURE ) {
		return false;		
	}

	fseek(fp, dhOffset - dh.dirSize, SEEK_SET);

	m_pDirData = new char[dh.dirSize + dh.nDirEntries*sizeof(*m_ppDir)];
	memset(m_pDirData, 0, dh.dirSize + dh.nDirEntries*sizeof(*m_ppDir));
	fread(m_pDirData, dh.dirSize, 1, fp);

	if( m_iZipMode>=CCModeMrs2 )
		RecoveryChar( (char*)m_pDirData , dh.dirSize );//mrs If the conversion.

	char *pfh = m_pDirData;
	m_ppDir = (const CCZIPDIRFILEHEADER **)(m_pDirData + dh.dirSize);

	for (int i = 0; i < dh.nDirEntries; i++){
		CCZIPDIRFILEHEADER& fh = *(CCZIPDIRFILEHEADER*)pfh;

		m_ppDir[i] = &fh;

		if(fh.sig != CCZIPDIRFILEHEADER::SIGNATURE){
			if(fh.sig != CCZIPDIRFILEHEADER::SIGNATURE2) {
				delete[] m_pDirData;
				m_pDirData = NULL;
				return false;
			}
		}

		{
			pfh += sizeof(fh);

			for (int j = 0; j < fh.fnameLen; j++)
			if (pfh[j] == '/')
			  pfh[j] = '\\';

			pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;
		}
	}

	m_iDirEntries = dh.nDirEntries;
	m_fp = fp;

	return true;
}

bool CCZip::Finalize()
{
	if(m_pDirData!=NULL) {
		delete[] m_pDirData;
		m_pDirData=NULL;
	}

	m_fp = NULL;
	m_ppDir = NULL;
	m_iDirEntries = 0;

	return true;
}

int CCZip::GetFileCount() const
{
	return m_iDirEntries;
}

void CCZip::GetFileName(int i, char *szDest) const
{
	if(szDest!=NULL){
		if (i < 0 || i >= m_iDirEntries){
			*szDest = '\0';
		}
		else{
			memcpy(szDest, m_ppDir[i]->GetName(), m_ppDir[i]->fnameLen);
			szDest[m_ppDir[i]->fnameLen] = '\0';
		}
	}
}

int t_strcmp(const char* str1,const char* str2)
{
	int len = strlen(str1);
	if(strlen(str2)!=len) return -1;
	
	for(int i=0;i<len;i++) {

		if(str1[i] != str2[i]) {
			if(	((str1[i]=='\\') || (str1[i]=='/')) && ((str1[i]=='\\') || (str1[i]=='/')) ) {
				continue;
			}
			else
				return -1;
		}
	}
	return 0;
}

int CCZip::GetFileIndex(const char* szFileName) const
{
	if(szFileName==NULL) return -1;

	char szSourceName[256];
	for(int i=0; i<GetFileCount();i++){
		GetFileName(i, szSourceName);
//		if(strcmp(szFileName, szSourceName)==0) 
		if(t_strcmp(szFileName, szSourceName)==0) 
			return i;
	}

	return -1;
}

int CCZip::GetFileLength(int i) const
{
	if(i<0 || i>=m_iDirEntries)
		return 0;
	else
		return m_ppDir[i]->ucSize;
}

int CCZip::GetFileLength(const char* filename)
{
	int index = GetFileIndex(filename);

	if(index == -1) return 0;

	return GetFileLength(index);
}

unsigned int CCZip::GetFileCRC32(int i)
{
	if(i<0 || i>=m_iDirEntries)
		return 0;
	else
		return m_ppDir[i]->crc32;
}

unsigned int CCZip::GetFileCRC32(const char* filename)
{
	int index = GetFileIndex(filename);

	if(index == -1) return 0;

	return GetFileCRC32(index);
}

unsigned int CCZip::GetFileTime(int i)
{
	if(i<0 || i>=m_iDirEntries)
		return 0;
	else
		return MAKELONG(m_ppDir[i]->modTime,m_ppDir[i]->modDate);
}

unsigned int CCZip::GetFileTime(const char* filename)
{
	int index = GetFileIndex(filename);

	if(index == -1) return 0;

	return GetFileCRC32(index);
}


bool CCZip::ReadFile(int i, void* pBuffer, int nMaxSize)
{
	if (pBuffer==NULL || i<0 || i>=m_iDirEntries)
		return false;

	fseek(m_fp, m_ppDir[i]->hdrOffset, SEEK_SET);
	CCZIPLOCALHEADER h;

	fread(&h, sizeof(h), 1, m_fp);

	if(m_iZipMode >= CCModeMrs2)
		RecoveryChar((char*)&h,sizeof(h));

	if(h.sig!=CCZIPLOCALHEADER::SIGNATURE)
		if(h.sig!=CCZIPLOCALHEADER::SIGNATURE2) 
			return false;

	fseek(m_fp, h.fnameLen + h.xtraLen, SEEK_CUR);//MRS do not want me to recover.

	if(h.compression==CCZIPLOCALHEADER::COMP_STORE){

		fread(pBuffer, h.cSize, 1, m_fp);
		return true;
	}
	else if(h.compression!=CCZIPLOCALHEADER::COMP_DEFLAT)
		return false;

	char *pData = new char[h.cSize];
	if(pData==NULL) return false;

	memset(pData, 0, h.cSize);

	int pos = ftell(m_fp);

	fread(pData, h.cSize, 1, m_fp);

	z_stream stream;
	int err;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	stream.next_in = (Bytef*)pData;
	stream.avail_in = (uInt)h.cSize;
	stream.next_out = (Bytef*)pBuffer;
	stream.avail_out = min((unsigned int)nMaxSize, h.ucSize);

	err = inflateInit2(&stream, -MAX_WBITS);
	if(err == Z_OK){
		err = inflate(&stream, Z_FINISH);
		inflateEnd(&stream);
		if (err==Z_STREAM_END) err = Z_OK;
		inflateEnd(&stream);
	}

	delete[] pData;

#ifdef CHECK_ZLIB_CRC32
	uLong crc = crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (const Bytef*)pBuffer, stream.total_out);

	if(h.crc32 != crc)
	{
		cclog("crc error \n");
		return false;
	}
#endif

	if(err!=Z_OK) 
		return false;

	return true;
}

bool CCZip::ReadFile(const char* filename, void* pBuffer, int nMaxSize)
{
	int index = GetFileIndex(filename);

	if(index == -1) return false;

	return ReadFile(index , pBuffer , nMaxSize);
}

static char _fileheaderReader[1024*16];//sizeof (fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen size.Roughly
static int	_fileheaderReaderSize = 0;

bool CCZip::UpgradeMrs(char* mrs_name) // Mrs To Mrs2
{
	FILE* fp = fopen(mrs_name, "rb+");

	if(fp==NULL) {
		cclog("%s 파일이 읽기 전용인지 확인하세요!~ \n",mrs_name);
		return false;
	}

	if( isVersion1Mrs(fp)==false ) // mrs1 만...
	{
		fclose(fp);
		return false;
	}

	//Restore ...
	fseek(fp, 0, SEEK_SET);
	int code = CCZIPLOCALHEADER::SIGNATURE;
	fwrite(&code, 4, 1, fp);


	CCZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(CCZIPDIRHEADER), SEEK_END);

	long dhOffset = ftell(fp);

	fread(&dh, sizeof(dh), 1, fp);

	dh.sig = CCZIPDIRHEADER::SIGNATURE;

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	//Recovery.

	DWORD _sig = CCZIPDIRFILEHEADER::SIGNATURE;

	for(int i=0;i<dir_data_size-3;i++) {

		if((BYTE)pDirData[i] == 0x80) {
			if((BYTE)pDirData[i+1] == 0x4b) {
				if((BYTE)pDirData[i+2] == 0x02) {
					if((BYTE)pDirData[i+3] == 0x05) {
						memcpy(&pDirData[i], &_sig,4);
					}
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////
	// local header 

	char* pTPos = pDirData;

	CCZIPDIRFILEHEADER fh;
	CCZIPLOCALHEADER h;

	for (int i = 0; i < dh.nDirEntries; i++) {

		fh = *(CCZIPDIRFILEHEADER*)pTPos;

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(&h, sizeof(h), 1, fp);

		_fileheaderReaderSize = h.fnameLen + h.xtraLen;

		if(_fileheaderReaderSize)
			fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( (char*)&h , sizeof(h) );
		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fwrite(&h,sizeof(h), 1, fp);

		if(_fileheaderReaderSize)
			fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen;
	}

	////////////////////////////////////////////////////////////////
	// mrs signature

	ConvertChar( (char*)pDirData , dir_data_size );

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);

	//////////////////////////////////////////////////////////

	fseek(fp, dhOffset - dh.dirSize, SEEK_SET);

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	dh.sig = MRS2_ZIP_CODE;

	ConvertChar((char*)&dh,sizeof(CCZIPDIRHEADER));

	fseek(fp, -(int)sizeof(CCZIPDIRHEADER), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	fclose(fp);


	return true;
}

bool CCZip::ConvertZip(char* zip_name)
{
	FILE* fp = fopen(zip_name, "rb+");

	if(fp==NULL) {
		cclog("%s  file is read-only check!~ \n",zip_name);
		return false;
	}

	CCZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(CCZIPDIRHEADER), SEEK_END);

	long dhOffset = ftell(fp);

	fread(&dh, sizeof(dh), 1, fp);

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	////////////////////////////////////////////////////////////////
	// local header 

	char* pTPos = pDirData;

	CCZIPDIRFILEHEADER fh;
	CCZIPLOCALHEADER h;

	for (int i = 0; i < dh.nDirEntries; i++) {

		fh = *(CCZIPDIRFILEHEADER*)pTPos;
/*
		_fileheaderReaderSize = sizeof(h) + fh.fnameLen + fh.xtraLen;

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);
		fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += _fileheaderReaderSize + fh.cmntLen;
*/
		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(&h, sizeof(h), 1, fp);

		_fileheaderReaderSize = h.fnameLen + h.xtraLen;

		if(_fileheaderReaderSize)
			fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		ConvertChar( (char*)&h , sizeof(h) );
		ConvertChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fwrite(&h,sizeof(h), 1, fp);

		if(_fileheaderReaderSize)
			fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen;
	}

	////////////////////////////////////////////////////////////////
	// mrs signature

	ConvertChar( (char*)pDirData , dir_data_size );

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);

	//////////////////////////////////////////////////////////

	fseek(fp, dhOffset - dh.dirSize, SEEK_SET);

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	dh.sig = MRS2_ZIP_CODE;

	ConvertChar((char*)&dh,sizeof(CCZIPDIRHEADER));

	fseek(fp, -(int)sizeof(CCZIPDIRHEADER), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	fclose(fp);

	return true;
}

//Recover previous versions of files that should be ...

bool CCZip::RecoveryMrs(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	int code = CCZIPLOCALHEADER::SIGNATURE;
	fwrite(&code, 4, 1, fp);

	CCZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(CCZIPDIRHEADER), SEEK_END);
	long dhOffset = ftell(fp);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, fp);

	dh.sig = CCZIPDIRHEADER::SIGNATURE;

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	DWORD _sig = CCZIPDIRFILEHEADER::SIGNATURE;

	for(int i=0;i<dir_data_size-3;i++) {

		if((BYTE)pDirData[i] == 0x80) {
			if((BYTE)pDirData[i+1] == 0x4b) {
				if((BYTE)pDirData[i+2] == 0x02) {
					if((BYTE)pDirData[i+3] == 0x05) {
						memcpy(&pDirData[i], &_sig,4);
					}
				}
			}
		}
	}

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	fseek(fp, -(int)sizeof(CCZIPDIRHEADER), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	return true;
}

bool CCZip::RecoveryMrs2(FILE* fp)
{
	CCZIPDIRHEADER dh;
	fseek(fp, -(int)sizeof(dh), SEEK_END);
	long dhOffset = ftell(fp);
	memset(&dh, 0, sizeof(dh));
	fread(&dh, sizeof(dh), 1, fp);

	RecoveryChar((char*)&dh,sizeof(dh));

	dh.sig = CCZIPDIRHEADER::SIGNATURE; // ZipCode

	long dir_data_pos = dhOffset - dh.dirSize;
	long dir_data_size = dh.dirSize;

	//////////////////////////////////////////////////////////////////

	fseek(fp, dir_data_pos, SEEK_SET);

	char* pDirData = new char[dir_data_size];
	memset(pDirData, 0, dir_data_size);
	fread(pDirData, dir_data_size, 1, fp);

	RecoveryChar( (char*)pDirData , dir_data_size );//mrs If the conversion.

	fseek(fp, dir_data_pos, SEEK_SET);
	fwrite(pDirData,dir_data_size,1,fp);


	////////////////////////////////////////////////////////////////
	// local header 

	char* pTPos = pDirData;

	CCZIPDIRFILEHEADER	fh;
	CCZIPLOCALHEADER h;

	for (int i = 0; i < dh.nDirEntries; i++) {

		fh = *(CCZIPDIRFILEHEADER*)pTPos;

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fread(&h, sizeof(h), 1, fp);

		RecoveryChar( (char*)&h , sizeof(h) );

		_fileheaderReaderSize = h.fnameLen + h.xtraLen;

		if(_fileheaderReaderSize)
			fread(_fileheaderReader, _fileheaderReaderSize, 1, fp);

		RecoveryChar( _fileheaderReader , _fileheaderReaderSize );

		fseek(fp, fh.hdrOffset, SEEK_SET);

		fwrite(&h,sizeof(h), 1, fp);

		if(_fileheaderReaderSize)
			fwrite(_fileheaderReader , _fileheaderReaderSize, 1, fp);

		pTPos += sizeof(fh) + fh.fnameLen + fh.xtraLen + fh.cmntLen;

	}

	//////////////////////////////////////////////////////////////////

	delete [] pDirData;

	///////////////////////////////////////////////////////////////////

	fseek(fp, -(int)sizeof(dh), SEEK_END);

	fwrite(&dh, sizeof(dh), 1, fp);

	return true;
}

bool CCZip::isZip(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	DWORD sig = 0;
	fread(&sig, 4, 1, fp);

	if(sig == CCZIPLOCALHEADER::SIGNATURE)
		return true;

	return false;
}

bool CCZip::isVersion1Mrs(FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	DWORD sig = 0;
	fread(&sig, 4, 1, fp);

	if(sig == CCZIPLOCALHEADER::SIGNATURE2)
		return true;

	return false;
}

bool CCZip::RecoveryZip(char* zip_name)
{
	FILE* fp = fopen(zip_name, "rb+");

	if(fp==NULL) {
		cclog("%s file is read-only check!~ \n",zip_name);
		return false;
	}

	//Mrs1 identify whether ...How to identify files are already out he was in this, but ...

	if( isVersion1Mrs(fp) ) {	//the first model.
		RecoveryMrs( fp );
	}
	else {
		RecoveryMrs2( fp );		//v2 sig value of future versions of the header nine minutes ....
	}

	fclose(fp);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////

FNode::FNode()
{
	memset(m_name,0,256);
	m_size	 = 0;
	m_offset = 0;
}

void FNode::SetName(char* str)	
{
	if(strlen(str) > 255) return;
	strcpy(m_name,str);
	str[255] = 0;
}

/////////////////////////////////////////////////////////////////////////////////

FFileList::FFileList()
{

}

FFileList::~FFileList() 
{
	DelAll();
}

void FFileList::Add(FNode* pNode) 
{
	push_back(pNode);
}

void FFileList::DelAll() 
{
	if(size()==0) return;

	iterator node;

	for(node = begin(); node != end(); ) 
	{
		delete (*node);
		(*node) = NULL;
		node = erase(node);
	}
}

void FFileList::UpgradeMrs() 
{
	iterator node;
	FNode* pNode = NULL;

	for(node = begin(); node != end(); ++node) 
	{
		pNode = (*node);

		if(CCZip::UpgradeMrs( pNode->m_name ))
			cclog(" Upgrade mrs : %s\n",pNode->m_name);
	}
}

void FFileList::ConvertZip() 
{
	iterator node;
	FNode* pNode = NULL;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		if(CCZip::ConvertZip( pNode->m_name ))
			cclog("convert zip : %s\n",pNode->m_name);
	}
}

void FFileList::RecoveryZip() 
{
	iterator node;
	FNode* pNode = NULL;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		CCZip::RecoveryZip( pNode->m_name );
	}
}

void FFileList::ConvertVtf() 
{
	iterator node;
	FNode* pNode = NULL;

	char temp[1024];
	char temp_arg[1024];
	int len;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

//		ShellExecute()
		strcpy(temp,pNode->m_name);
		len = strlen(temp);
		temp[len-3] = 0;
		strcat(temp,"tga");

		sprintf(temp_arg,"%s %s",pNode->m_name,temp);
		HINSTANCE hr = ShellExecute(NULL, _T("open"), _T("vtf2tga.exe"),_T(temp_arg), NULL, SW_HIDE);
//		ShellExecute()
//		_execl("vtf2tga.exe","%s %s",pNode->m_name,temp);
	}
}

void FFileList::ConvertNameMRes2Zip() 
{
	iterator node;
	FNode* pNode = NULL;

	char _buf_rename[256];
	int len;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		strcpy(_buf_rename,pNode->m_name);
		len = (int)strlen(pNode->m_name);

		_buf_rename[len-3] = NULL;
		strcat(_buf_rename,"zip");

		rename( pNode->m_name, _buf_rename);

		cclog("rename : %s -> %s \n",_buf_rename,pNode->m_name);
	}
}

void FFileList::ConvertNameZip2MRes() 
{
	iterator node;
	FNode* pNode = NULL;

	char _buf_rename[256];
	int len;

	for(node = begin(); node != end(); ++node) {
		pNode = (*node);

		strcpy(_buf_rename,pNode->m_name);
		len = (int)strlen(pNode->m_name);

		_buf_rename[len-3] = NULL;
		strcat(_buf_rename,"mrs");

		rename( pNode->m_name, _buf_rename);

		cclog("rename : %s -> %s \n",pNode->m_name,_buf_rename);
	}
}

/////////////////////////////////////////////////////////////////////////////////

//List of files in the specified path

bool GetDirList(char* path,	FFileList& pList)
{
	struct _finddata_t file_t;
	long hFile;

	FNode* pNode;

	if( (hFile = _findfirst( path , &file_t )) != -1L ) {
		do{
			if(strcmp(file_t.name, "." )==0)	continue;
			if(strcmp(file_t.name, "..")==0)	continue;
			if( !(file_t.attrib & _A_SUBDIR) )	continue;

			pNode = new FNode;
			pNode->SetName(file_t.name);
			pList.Add(pNode);

		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

//List of directories in the specified path 

bool GetFileList(char* path,FFileList& pList)
{
	struct _finddata_t file_t;
	long hFile;

	FNode* pNode;

	if( (hFile = _findfirst( path , &file_t )) != -1L ) {
		do{
			if(strcmp(file_t.name, "." )==0) continue;
			if(strcmp(file_t.name, "..")==0) continue;
			if(file_t.attrib & _A_SUBDIR )	 continue;

			pNode = new FNode;
			pNode->SetName(file_t.name);
			pList.Add(pNode);

		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

bool GetFileListWin(char* path,FFileList& pList)
{

	WIN32_FIND_DATA		file_t;
	HANDLE				hFile;

	FNode* pNode;

	if( (hFile = FindFirstFile( path , &file_t )) != INVALID_HANDLE_VALUE ) {

		do {

			if(strcmp(file_t.cFileName, "." )==0)					continue;
			if(strcmp(file_t.cFileName, "..")==0)					continue;
			if(file_t.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	continue;

			pNode = new FNode;
			pNode->SetName(file_t.cFileName);
			pList.Add(pNode);

		} while( FindNextFile( hFile, &file_t ) );

		FindClose(hFile);
	}

	return true;
}


//Search all the subfolders of the desired file.

bool GetFindFileList(char* path,char* ext,FFileList& pList)
{
	struct _finddata_t file_t;
	long hFile;

	FNode* pNode;

	if( (hFile = _findfirst( path , &file_t )) != -1L ) {
		do{
			if(strcmp(file_t.name, "." )==0) continue;
			if(strcmp(file_t.name, "..")==0) continue;

			if(file_t.attrib & _A_SUBDIR ) {
				char _path[256];
				strcpy(_path,file_t.name);
				strcat(_path,"/");
				strcat(_path,path);

				GetFindFileList(_path,ext,pList);
				continue;
			}

			int len = (int)strlen(ext);
			int filelen = (int)strlen(file_t.name);

			char* pName = &file_t.name[filelen-len];

			if(stricmp(pName,ext)==0) {

				int len = (int)strlen(path);

				char temp_name[256];

				if(len > 3) {

					strncpy(temp_name,path,len-3);
					temp_name[len-3]=0;
					strcat(temp_name,file_t.name);
				}
				else {
					strcpy(temp_name,file_t.name);
				}

				pNode = new FNode;
				pNode->SetName(temp_name);
				pList.Add(pNode);

			}

		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

bool GetFindFileListWin(char* path,char* ext,FFileList& pList)
{

	WIN32_FIND_DATA		file_t;
	HANDLE				hFile;

	FNode* pNode;

	if( (hFile = FindFirstFile( path , &file_t )) != INVALID_HANDLE_VALUE ) {

		do{
			if(strcmp(file_t.cFileName, "." )==0)	continue;
			if(strcmp(file_t.cFileName, "..")==0)	continue;

			if(file_t.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	{
				char _path[1024];

				int len = strlen(path);

				strcpy(_path,path);
				_path[len-1] = 0;
				strcat(_path,file_t.cFileName);
				strcat(_path,"/*");

				GetFindFileListWin(_path,ext,pList);
				continue;
			}

			int len = (int)strlen(ext);
			int filelen = (int)strlen(file_t.cFileName);

			char* pName = &file_t.cFileName[filelen-len];

			if(stricmp(pName,ext)==0) {

				int len = (int)strlen(path);

				char temp_name[1024];

				if(len > 1) {

					strncpy(temp_name,path,len-1);
					temp_name[len-1]=0;
					strcat(temp_name,file_t.cFileName);
				}
				else {
					strcpy(temp_name,file_t.cFileName);
				}

				pNode = new FNode;
				pNode->SetName(temp_name);
				pList.Add(pNode);

			}

		} while( FindNextFile( hFile, &file_t ) );

		FindClose(hFile);
	}

	return true;
}
