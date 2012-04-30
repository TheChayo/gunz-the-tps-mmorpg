#pragma once
// Ŭ���̾�Ʈ�� ������ �׺�����ۿ� ���ؼ� �������� ����ϴ� ������.
class CCMatchBaseGambleItem
{
protected :
	CCUID	m_uidItem;
	DWORD	m_dwGambleItemID;
	int		m_nItemCount;
	
protected :
	CCMatchBaseGambleItem() {}

public :
	CCMatchBaseGambleItem( const CCUID& uidItem, const DWORD dwGambleItemID, const int nItemCount = 1) 
	{
		m_uidItem		 = uidItem;
		m_dwGambleItemID = dwGambleItemID;
		m_nItemCount	 = nItemCount;
	}

	virtual ~CCMatchBaseGambleItem() {}

	const CCUID& GetUID() const			{ return m_uidItem; }
	const DWORD GetGambleItemID() const { return m_dwGambleItemID; }
	const int	GetItemCount() const	{ return m_nItemCount; }

	void SetItemCount(int nVal)	{ m_nItemCount = nVal; }
};