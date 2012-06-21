#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include "fileinfo.h"
#include "CCDebug.h"
#include <string>
#include "CCPdb.h"
#include <windows.h>

using namespace std;

static char logfilename[256];
static int g_nLogMethod=CCLOGSTYLE_DEBUGSTRING;

const char* CCGetLogFileName(){
	return logfilename;
}

void InitLog(int logmethodflags, const char* pszLogFileName){
	g_nLogMethod=logmethodflags;
	if(g_nLogMethod&CCLOGSTYLE_FILE){
		GetFullPath(logfilename, pszLogFileName);
		FILE *pFile;
		pFile=fopen(logfilename,"w+");
		if( !pFile ) return;
		fclose(pFile);
	}
}

void __cdecl CCLog(const char *pFormat,...){
	char temp[16*1024];

	va_list args;

	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	if(g_nLogMethod&CCLOGSTYLE_FILE){
		FILE *pFile;
		pFile = fopen( logfilename, "a" );
		if( !pFile ) pFile=fopen(logfilename,"w");
		if( pFile==NULL ) return;
		fprintf(pFile,temp);
		fclose(pFile);
	}
	if(g_nLogMethod&CCLOGSTYLE_DEBUGSTRING){
		#ifdef _DEBUG
				OutputDebugString(temp);
		#endif
	}
}

#ifdef _WIN32
#include <windows.h>
#include <crtdbg.h>

void __cdecl CCMsg(const char *pFormat,...){
    char buff[256];
    wvsprintf(buff, pFormat, (char *)(&pFormat+1));
    lstrcat(buff, "\r\n");
    MessageBox( NULL, buff, "RealSpace Message", MB_OK );
	cclog(buff);cclog("\n");
}
#endif

void CCShowContextRecord(CONTEXT* p){
	cclog("[Context]\n");

	cclog("GS : %08x  FS : %08x  ES : %08x  DS : %08x\n", p->SegGs, p->SegFs, p->SegEs, p->SegDs);
	cclog("EDI: %08x  ESI: %08x  EBX: %08x  EDX: %08x\n", p->Edi, p->Esi, p->Ebx, p->Edx);
	cclog("ECX: %08x  EAX: %08x  EBP: %08x  EIP: %08x\n", p->Ecx, p->Eax, p->Ebp, p->Eip);
	cclog("CS : %08x  Flg: %08x  ESP: %08x  SS : %08x\n", p->SegCs, p->EFlags, p->Esp, p->SegSs);

	cclog("\n");
}

void CCShowStack(DWORD* sp, int nSize){
	cclog("[Stack]");

	for(int i=0; i<nSize; i++){
		if((i%8)==0) cclog("\n");
		cclog("%08x ", *(sp+i));
	}

	cclog("\n");
}

void GetMemoryInfo(DWORD* dwTotalMemKB, DWORD* dwAvailMemKB, DWORD* dwVirtualMemKB){
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if(!GlobalMemoryStatusEx(&statex)) return;

	DWORDLONG lMemTotalKB = (statex.ullTotalPhys / 1024 );
	*dwTotalMemKB = (DWORD)lMemTotalKB;

	DWORDLONG lAvailMemKB = (statex.ullAvailPhys / 1024 );
	*dwAvailMemKB = (DWORD)lAvailMemKB;

	DWORDLONG lVirtualMemKB = (statex.ullTotalVirtual / 1024);
	*dwVirtualMemKB = (DWORD)lVirtualMemKB;
}


DWORD CCFilterException(LPEXCEPTION_POINTERS p){
	char tmpbuf[128];
	_strtime( tmpbuf );
	cclog("Crash ( %s )\n",tmpbuf);

	cclog("Build "__DATE__" "__TIME__"\n\n");

	DWORD dwTotalMemKB = 0;
	DWORD dwAvailMemKB = 0;	
	DWORD dwVirtualMemKB = 0;

	GetMemoryInfo(&dwTotalMemKB, &dwAvailMemKB, &dwVirtualMemKB);

	cclog("dwTotalMemKB = %d KB\n", dwTotalMemKB);
	cclog("dwAvailMemKB = %d KB\n", dwAvailMemKB);
	cclog("dwVirtualMemKB = %d KB\n", dwVirtualMemKB);

	cclog("\n[Exception]\n");
	cclog("Address	:	%08x\n", p->ExceptionRecord->ExceptionAddress);
	cclog("ExpCode	:	%08x\n", p->ExceptionRecord->ExceptionCode);
	cclog("Flags	:	%08x\n", p->ExceptionRecord->ExceptionFlags);
	cclog("#Param	:	%08x\n", p->ExceptionRecord->NumberParameters);
	cclog("other	:	%08x\n", p->ExceptionRecord->ExceptionRecord);
	cclog("\n");
	
	CCShowContextRecord(p->ContextRecord);
	CCShowStack((DWORD*)p->ContextRecord->Esp, 128);

	cclog("\n");

	char szCrashLogFileName[ 1024 ] = {0,};
	_snprintf( szCrashLogFileName, 1024, "Crash_%s", CCGetLogFileName() );
	WriteCrashInfo( p, szCrashLogFileName );

	return EXCEPTION_EXECUTE_HANDLER;
}

void CCSEHTranslator(UINT nSeCode, _EXCEPTION_POINTERS* pExcPointers){
	CCFilterException(pExcPointers);

	raise(SIGABRT);
	_exit(3);
}

void CCInstallSEH(){
	#ifndef _DEBUG
		_set_se_translator(CCSEHTranslator);
	#endif
}

#ifndef _PUBLISH

#pragma comment(lib, "winmm.lib")

#define MAX_PROFILE_COUNT	10000

struct sProfileItem {
	char szName[256];
	DWORD dwStartTime;
	DWORD dwTotalTime;
	DWORD dwCalledCount;
};
sProfileItem g_ProfileItems[MAX_PROFILE_COUNT];

DWORD g_dwEnableTime;

void CCInitProfile(){
	for(int i=0;i<MAX_PROFILE_COUNT;i++){
		g_ProfileItems[i].szName[0]=0;
		g_ProfileItems[i].dwTotalTime=0;
		g_ProfileItems[i].dwCalledCount=0;
	}
	g_dwEnableTime=timeGetTime();
}

void CCBeginProfile(int nIndex,const char *szName){
	if(g_ProfileItems[nIndex].szName[0]==0)
		strcpy(g_ProfileItems[nIndex].szName,szName);

	g_ProfileItems[nIndex].dwStartTime=timeGetTime();
	g_ProfileItems[nIndex].dwCalledCount++;
}

void CCEndProfile(int nIndex){
	g_ProfileItems[nIndex].dwTotalTime+= 
		timeGetTime()-g_ProfileItems[nIndex].dwStartTime;
}

void CCSaveProfile(const char *filename){
	DWORD dwTotalTime = timeGetTime()-g_dwEnableTime;

	FILE *file=fopen(filename,"w+");

	fprintf(file," total time = %6.3f seconds \n",(float)dwTotalTime*0.001f);

	fprintf(file,"id   (loop ms)  seconds     %%        calledcount   name \n");
	fprintf(file,"=========================================================\n");

	float cnt = (float)g_ProfileItems[0].dwCalledCount;

	for(int i=0;i<MAX_PROFILE_COUNT;i++){
		if(g_ProfileItems[i].dwTotalTime>0){
			fprintf(file,"(%05d) %8.3f %8.3f ( %6.3f %% , %6u) %s \n",i,((float)g_ProfileItems[i].dwTotalTime) / cnt,
				0.001f*(float)g_ProfileItems[i].dwTotalTime, 
				100.f*(float)g_ProfileItems[i].dwTotalTime/(float)dwTotalTime
				,g_ProfileItems[i].dwCalledCount
				,g_ProfileItems[i].szName);
		}
	}
	fclose(file);
}

#else
void MInitProfile() {}
void CCBeginProfile(int nIndex,const char *szName) {}
void CCEndProfile(int nIndex) {}
void MSaveProfile(const char *file) {}
#endif