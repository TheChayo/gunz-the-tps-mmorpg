#include "stdafx.h"
#include "CCPacket.h"
//#include "Msg.h"


// CSCommon���� #define�� ������� ������.


//#ifdef _HSHIELD
//	#include "CCPacketHShieldCrypter.h"
//#else
	#include "CCPacketCrypter.h"
//#endif

int CCPacketHeader::CalcPacketSize(CCPacketCrypter* pCrypter)
{
	unsigned short nPacketSize = 0;

	if (nMsg == MSGID_COMMAND)
	{
//#ifdef _HSHIELD
//		CCPacketHShieldCrypter::Decrypt((PBYTE)&(nSize), sizeof(unsigned short), (PBYTE)&nPacketSize);
//#else
		if (pCrypter)
		{
			pCrypter->Decrypt((char*)(&nSize), sizeof(unsigned short), (char*)&nPacketSize, sizeof(unsigned short));
		}
//#endif
	}
	else
	{
		nPacketSize = nSize;
	}


	return (int)nPacketSize;
}
