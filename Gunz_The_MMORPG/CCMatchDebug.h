#pragma once
// cscommon���� ����׿� ���� ���� ����

//#define _DEBUG_PUBLISH				// ����׸��� �ۺ��� - �׽�Ʈ�ϱ� ����
#pragma warning( disable: 4996 )
#define _CRT_SECURE_NO_WARNINGS
#if defined(_DEBUG_PUBLISH) && defined(_DEBUG)
	#ifdef _ASSERT
	#undef _ASSERT
	#endif
	#define _ASSERT(X)		(()0)

	#ifdef OutputDebugString
	#undef OutputDebugString
	#endif
	#define OutputDebugString(X)		(()0)
#endif

// �����Ϸ��ɼ� /GS�ɼ��� ����Ҷ� ���� �������� ���� ��� �ߴ� �޽��� �ڽ��� ����æ��.
//void SetSecurityErrorHandler(_secerr_handler_func func); __report_gsfailure
void __cdecl ReportBufferOverrun(int code, void * unused);