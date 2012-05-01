#pragma once

// ==============================================================================
// Admin�� ���� �������� ����(?)�� ���� Ŭ�����Դϴ�.
// ���� ���� ��ŷ ����� ���Ͽ� �������� �������� �����Դϴ�.
// ���Ŀ��� ���ľߵ��ٵ�-.,- ��� ���ľߵ���..������
//																2010-08-09 ȫ����
// ==============================================================================

// DB ���� AccountPenlatyCode�� ��ġ�Ͽ��� �Ѵ�.
enum CCPenaltyCode
{
	MPC_NONE = 0,
	MPC_CONNECT_BLOCK,
	MPC_CHAT_BLOCK,
	MPC_SHOP_BLOCK,
	MPC_MAX,
};

struct CCPenaltyInfo
{
	CCPenaltyCode	nPenaltyCode;
	SYSTEMTIME		sysPenaltyEndDate;
};

class CCMatchAccountPenaltyInfo
{
protected:
	CCPenaltyInfo m_PenaltyTable[MPC_MAX];

	SYSTEMTIME GetPenaltyEndDate(int nPenaltyHour);
public:
	CCMatchAccountPenaltyInfo();
	~CCMatchAccountPenaltyInfo();

	void ClearPenaltyInfo(CCPenaltyCode nCode);

	void SetPenaltyInfo(CCPenaltyCode nCode, int nPenaltyHour);
	void SetPenaltyInfo(CCPenaltyCode nCode, SYSTEMTIME sysPenaltyEndDate);

	bool IsBlock(CCPenaltyCode nCode);

	const CCPenaltyInfo* GetPenaltyInfo(CCPenaltyCode nCode) { return &m_PenaltyTable[nCode]; }
};