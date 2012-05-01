#ifndef _MPACKET_H
#define _MPACKET_H

#include "CCCommand.h"
#include "CCCommandManager.h"

#define MAX_PACKET_SIZE			16384		// �ִ� ��Ŷ ũ��� 16k

#define MSG_COMMAND	1000

#define MSGID_REPLYCONNECT	10
#define MSGID_RAWCOMMAND	100		// ��ȣȭ �ȵ� Ŀ�ǵ�
#define MSGID_COMMAND		101		// ��ȣȭ�� Ŀ�ǵ�
//#define MSGID_HSHIELDCOMMAND 102	// �ٽǵ� ��ȣȭ�� Ŀ�ǵ�




class CCPacketCrypter;

#pragma pack(1)

struct CCPacketHeader
{
	unsigned short nMsg;
	unsigned short nSize;
	unsigned short nCheckSum;

	CCPacketHeader() { nMsg=MSG_COMMAND; nSize=0; nCheckSum=0; }
	int CalcPacketSize(CCPacketCrypter* pCrypter);		// ���� ��ȣȭ�Ǿ����� ��� ��Ŷ�� ũ��� �� �Լ��θ� �� �� �ִ�.
														// nSize �������� ��ȣȭ�� ���� ������� �� ����
};


struct MReplyConnectMsg : public CCPacketHeader
{
	unsigned int	nHostHigh;
	unsigned int	nHostLow;
	unsigned int	nAllocHigh;
	unsigned int	nAllocLow;
	unsigned int	nTimeStamp;
};

struct CCCommandMsg : public CCPacketHeader
{
	char	Buffer[1];
};


#pragma pack()

// Tiny CheckSum for CCCommandMsg
inline unsigned short MBuildCheckSum(CCPacketHeader* pPacket, int nPacketSize)
{
	int nStartOffset = sizeof(CCPacketHeader);
	BYTE* pBulk = (BYTE*)pPacket;
	//int nPacketSize = min(65535, pPacket->nSize);
	nPacketSize = min(65535, nPacketSize);

	unsigned long nCheckSum = 0;
	for (int i=nStartOffset; i<nPacketSize; i++) {
		nCheckSum += pBulk[i];
	}
	nCheckSum -= (pBulk[0]+pBulk[1]+pBulk[2]+pBulk[3]);
	unsigned short nShortCheckSum = HIWORD(nCheckSum) + LOWORD(nCheckSum);
	return nShortCheckSum;
}


#endif
