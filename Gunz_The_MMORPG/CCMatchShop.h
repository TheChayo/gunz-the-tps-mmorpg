#pragma once
#include "winsock2.h"
#include "CCXml.h"
#include "CCUID.h"
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

#include "CCMatchItem.h"
#include "CCQuestItem.h"
#include "CCMatchDBGambleItem.h"

/// ���� ������ ���
/// - ���� ���������� CCMatchItem.h��, �Ⱓ������ ���⿡ �ִ� ������ �Ǿ������.
/// - �ű���� �ʹ� ������ ������ ���⿡ �Ⱓ������ �ø��� ������ ��ħ.
/// - ���Ŀ� �������������� CCMatchItem.h�� �ִ� BountyPrice, BountyValue�� ����� �̻�����ּ���. - bird
/// - ���� ������ ��������� �۾��� �ּ�ȭ �ϱ� ���ؼ� ZItem�� �״�� �����ϵ��� ��. - by SungE 2007-06-28
struct ShopItemNode
{
	unsigned int	nItemID;			/// ������ ID
	int				nItemCount;
	bool			bIsRentItem;		/// ��� ����. �Ⱓ ���������� ����(�Ϲ� �����۸� ����)
	int				nRentPeriodHour;	/// ��� ����. �Ⱓ(1 = 1�ð�)
	

	ShopItemNode() : nItemID(0), nItemCount(0), bIsRentItem(false), nRentPeriodHour(0) {}
};

/// ����
class CCMatchShop
{
private:
protected:
	vector<ShopItemNode*>					m_ItemNodeVector;
	map<unsigned int, ShopItemNode*>		m_ItemNodeMap;

	void ParseSellItem(CCXmlElement& element);
	bool ReadXml(const char* szFileName);


public:
	CCMatchShop();
	virtual ~CCMatchShop();
	bool Create(const char* szDescFileName);
	void Destroy();

	void Clear();
	int GetCount() { return static_cast< int >( m_ItemNodeVector.size() ); }
	bool IsSellItem(const unsigned long int nItemID);		// �ش� �������� �Ȱ� �ִ��� ����
	ShopItemNode* GetSellItemByIndex(int nListIndex);			// ������ �Ȱ� �ִ� nListIndex��° ������ �� ��ȯ
	ShopItemNode* GetSellItemByItemID(int nItemID);				// ������ �Ȱ� �ִ� nItemID ������ �� ��ȯ
	
	static CCMatchShop* GetInstance();


#ifdef _DEBUG
	void MakeShopXML();
#endif
};

inline CCMatchShop* MGetMatchShop() { return CCMatchShop::GetInstance(); }

#define MTOK_SELL					"SELL"
#define MTOK_SELL_ITEMID			"itemid"
#define MTOK_SELL_ITEM_COUNT		"item_Count"		
#define MTOK_SELL_RENT_PERIOD_HOUR	"rent_period"
