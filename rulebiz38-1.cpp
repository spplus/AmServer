#include "rulebiz38-1.h"

RuleBiz38_1::RuleBiz38_1()
{
	m_flag = false;
}

int RuleBiz38_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH)
	{
		// 如果结果元件为刀闸（无论闭合断开），继续遍历刀闸另一侧的连接点
		m_flag = true;
		return 1;
	}
	else if (bean.unittype() == eBUS)
	{
		if (m_flag)
		{
			// 如果结果元件包含母线，条件三成立。
			COM->triggerRule(ruleMap,3);
		}
	}
	return 0;
}