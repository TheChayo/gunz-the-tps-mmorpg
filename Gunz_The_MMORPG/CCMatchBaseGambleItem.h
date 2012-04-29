#pragma once
// Ŭ���̾�Ʈ�� ������ �׺�����ۿ� ���ؼ� �������� ����ϴ� ������.
class MMatchBaseGambleItem
{
protected :
	CCUID	m_uidItem;
	DWORD	m_dwGambleItemID;
	int		m_nItemCount;
	
protected :
	MMatchBaseGambleItem() {}

public :
	MMatchBaseGambleItem( const CCUID& uidItem, const DWORD dwGambleItemID, const int nItemCount = 1) 
	{
		m_uidItem		 = uidItem;
		m_dwGambleItemID = dwGambleItemID;
		m_nItemCount	 = nItemCount;
	}

	virtual ~MMatchBaseGambleItem() {}

	const CCUID& GetUID() const			{ return m_uidItem; }
	const DWORD GetGambleItemID() const { return m_dwGambleItemID; }
	const int	GetItemCount() const	{ return m_nItemCount; }

	void SetItemCount(int nVal)	{ m_nItemCount = nVal; }
};