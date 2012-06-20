#include "stdafx.h"
#include "CCMatchDBMgr.h"
#include <windows.h>
#include <Mmsystem.h>
#include "CCAsyncProxy.h"
#include "CCMatchConfig.h"
#include "CCCrashDump.h"
#ifndef _PUBLISH
	#include "CCProcessController.h"
#endif

CCAsyncProxy::CCAsyncProxy()
{
	m_nThreadCount = 0;
	BYTE nInitVal = (BYTE)(INVALID_HANDLE_VALUE);
	FillMemory(m_ThreadPool, sizeof(HANDLE)*MAX_THREADPOOL_COUNT, nInitVal);
}

CCAsyncProxy::~CCAsyncProxy()
{
}

bool CCAsyncProxy::Create(int nThreadCount)
{
	m_hEventShutdown = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventFetchJob = CreateEvent(NULL, FALSE, FALSE, NULL);

	InitializeCriticalSection(&m_csCrashDump);

	nThreadCount = __min(nThreadCount, MAX_THREADPOOL_COUNT);
	for (int i=0; i<nThreadCount; i++) {
		DWORD dwThreadId=0;
		HANDLE hThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThreadId);
		if (hThread == NULL)
			return false;
		m_ThreadPool[i] = hThread;
		m_nThreadCount++;
	}
	return true;
}

void CCAsyncProxy::Destroy()
{
	for (int i=0; i<m_nThreadCount; i++) {
		SetEvent(GetEventShutdown());
		Sleep(100);
	}

	WaitForMultipleObjects(m_nThreadCount,  m_ThreadPool, TRUE, 2000);

	for (int i=0; i<MAX_THREADPOOL_COUNT; i++) {
		if (INVALID_HANDLE_VALUE != m_ThreadPool[i]) {
			TerminateThread(m_ThreadPool[i], 0);
			CloseHandle(m_ThreadPool[i]);
			m_ThreadPool[i] = INVALID_HANDLE_VALUE;
			m_nThreadCount--;			
		}
	}

	DeleteCriticalSection(&m_csCrashDump);

	CloseHandle(m_hEventFetchJob); m_hEventFetchJob = NULL;
	CloseHandle(m_hEventShutdown); m_hEventShutdown = NULL;
}

void CCAsyncProxy::PostJob(CCAsyncJob* pJob)
{
	m_WaitQueue.Lock();
		pJob->SetPostTime(timeGetTime());
		m_WaitQueue.AddUnsafe(pJob);	
	m_WaitQueue.Unlock();

	SetEvent(GetEventFetchJob());
}

DWORD WINAPI CCAsyncProxy::WorkerThread(LPVOID pJobContext)
{
	CCAsyncProxy* pProxy = (CCAsyncProxy*)pJobContext;

	__try{
		pProxy->OnRun();
	} __except(pProxy->CrashDump(GetExceptionInformation())) 
	{
		// ������ �����ϵ��� �ϱ������̴�.
		#ifndef _PUBLISH
			char szFileName[_MAX_DIR];
			GetModuleFileName(NULL, szFileName, _MAX_DIR);
			HANDLE hProcess = CCProcessController::OpenProcessHandleByFilePath(szFileName);
			TerminateProcess(hProcess, 0);
		#endif
	}

	ExitThread(0);
	return (0);
}

void CCAsyncProxy::OnRun()
{
	CCMatchDBMgr	DatabaseMgr;

	CString str = DatabaseMgr.BuildDSNString(CCGetServerConfig()->GetDB_DNS(), 
		                                      CCGetServerConfig()->GetDB_UserName(), 
											  CCGetServerConfig()->GetDB_Password());
	if (!DatabaseMgr.Connect())
	{
		char szLog[32];
		sprintf(szLog, "DBCONNECT FAILED ThreadID=%d \n", GetCurrentThreadId());
#ifdef _DEBUG
		OutputDebugString(szLog);
#endif
		MessageBox(NULL, szLog, "MatchServer DB Error", MB_OK);
	}

	#define CCASYNC_EVENTARRAY_SIZE	2
	HANDLE EventArray[CCASYNC_EVENTARRAY_SIZE];

	ZeroMemory(EventArray, sizeof(HANDLE)*CCASYNC_EVENTARRAY_SIZE);
	WORD wEventCount = 0;

	EventArray[wEventCount++] = GetEventShutdown();
	EventArray[wEventCount++] = GetEventFetchJob();

	bool bShutdown = false;
	while(!bShutdown) {
		#define TICK_ASYNCPROXY_LIVECHECK	1000
		DWORD dwResult = WaitForMultipleObjects(wEventCount, EventArray, 
												FALSE, TICK_ASYNCPROXY_LIVECHECK);
		if (WAIT_TIMEOUT == dwResult) {
			if (m_WaitQueue.GetCount() > 0) {
				SetEvent(GetEventFetchJob());
			}
			continue;
		}

		switch(dwResult) {
		case WAIT_OBJECT_0:		// Shutdown
			{
				bShutdown = true;
			}
			break;
		case WAIT_OBJECT_0 + 1:	// Fetch Job
			{
				m_WaitQueue.Lock();
					CCAsyncJob* pJob = m_WaitQueue.GetJobUnsafe();
				m_WaitQueue.Unlock();

				if (pJob) {
					pJob->Run(&DatabaseMgr);
					pJob->SetFinishTime(timeGetTime());

					m_ResultQueue.Lock();
						m_ResultQueue.AddUnsafe(pJob);
					m_ResultQueue.Unlock();
				}

				if (m_WaitQueue.GetCount() > 0) {
					SetEvent(GetEventFetchJob());
				}
			}
			break;
		};	// switch
	};	// while
}

DWORD CCAsyncProxy::CrashDump(PEXCEPTION_POINTERS ExceptionInfo)
{
	cclog("CrashDump Entered 1\n");
	EnterCriticalSection(&m_csCrashDump);
	cclog("CrashDump Entered 2\n");

	if (PathIsDirectory("Log") == FALSE)
		CreateDirectory("Log", NULL);

	time_t		tClock;
	struct tm*	ptmTime;

	time(&tClock);
	ptmTime = localtime(&tClock);

	char szFileName[_MAX_DIR];

	int nFooter = 1;
	while(TRUE) {
		sprintf(szFileName, "Log/CCAsyncProxy_%02d-%02d-%02d-%d.dmp", 
			ptmTime->tm_year+1900, ptmTime->tm_mon+1, ptmTime->tm_mday, nFooter);

		if (PathFileExists(szFileName) == FALSE)
			break;

		nFooter++;
		if (nFooter > 100) 
		{
			LeaveCriticalSection(&m_csCrashDump);
			return false;
		}
	}

	DWORD ret = CrashExceptionDump(ExceptionInfo, szFileName, true);

	cclog("CrashDump Leaving\n");
	LeaveCriticalSection(&m_csCrashDump);
	cclog("CrashDump Leaved\n");

	return ret;
}
