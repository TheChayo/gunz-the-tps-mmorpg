#ifndef _ZRULE_DUEL_H
#define _ZRULE_DUEL_H

#include "ZRule.h"


class ZRuleDuel : public ZRule
{
public:
	CCTD_DuelQueueInfo QInfo;

	ZRuleDuel(ZMatch* pMatch);
	virtual ~ZRuleDuel();

	virtual bool OnCommand(CCCommand* pCommand);

	int	GetQueueIdx(const CCUID& uidChar);			// 0 : è�Ǿ�   1 : ������  2~ : ������
};

#endif