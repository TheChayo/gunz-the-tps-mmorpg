#pragma once

#pragma warning(disable:4996)

#pragma warning(disable:4800)

#define WINVER 0x0601

// ����� include
#include <afxdb.h>
#include <afxtempl.h>
#include <afxdtctl.h>

#include <Winsock2.h>
#include <mswsock.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>
#include "CCWidget.h"
/*	// for debug publish
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0 

#undef _ASSERT
#define _ASSERT(expr) (()0)
#undef ASSERT
#define ASSERT(f)          (()0)
*/

// stl
#include <string.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

#ifndef _PUBLISH

//	#define _CLASSIC					// Ŭ���� ���� ������
#endif

#include "CCFeatureDefine.h"

#define _QUEST_ITEM		// ����Ʈ ������ ���߿�. by �߱���.
#define _MONSTER_BIBLE	// ���� ���� ���߿�. by �߱���. ��������� ��� ����.
#define _BLOCK_HACKER	// ��ŷ���� ������ DB�� ����� �۾�.


#include "CCLocaleDefine.h"
#include "CCDebug.h"
#include "CCMatchDebug.h"
#include "CCXml.h"

#include "CCUID.h"
#include "CCMatchGlobal.h"
#include "CCMatchUtil.h"
#include "CCSharedCommandTable.h"
#include "CCCommand.h"
#include "CCCommandParameter.h"
#include "CCCommandCommunicator.h"
#include "CCErrorTable.h"
#include "CCServer.h"
#include "CCMatchServer.h"
#include "CCMatchClient.h"
#include "CCObject.h"
#include "CCMatchItem.h"
#include "CCMatchObjCache.h"
#include "CCMatchStage.h"
#include "CCMatchObject.h"
#include "CCMatchChannel.h"


#include "CCMatchDebug.h"
