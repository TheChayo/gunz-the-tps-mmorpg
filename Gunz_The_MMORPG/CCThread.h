#pragma once
#include <windows.h>
#include <MMsystem.h>


class CCThread {
protected:
	HANDLE		m_hThread;
	DWORD		m_idThread;

public:
	CCThread();
	virtual ~CCThread();
	void Create();
	void Destroy();

	HANDLE GetThreadHandle()	{ return m_hThread; }
	DWORD GetThreadID()			{ return m_idThread; }

	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

	virtual void OnCreate()		{}
	virtual void OnDestroy()	{}
	virtual void Run()			{}
};

class CCTime {
	DWORD	m;
	DWORD	q;
	DWORD	a;
	DWORD	r;
	DWORD	seed;

public:
	CCTime() {
		m=2147483647; q=127773; a=16807; r=2836;
		seed = timeGetTime();
	}
	unsigned long Random();
	int MakeNumber(int nFrom, int nTo);
	static void GetTime(struct timeval *t);
	static struct timeval TimeSub(struct timeval Src1, struct timeval Src2);
	static struct timeval TimeAdd(struct timeval Src1, struct timeval Src2);
};

#pragma comment(lib, "winmm.lib")
