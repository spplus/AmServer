#include "rulebiz5-1.h"

RuleBiz5_1::RuleBiz5_1()
{
	count = 0;
}

int RuleBiz5_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 如果结果元件包含两个闭合的刀闸，满足条件五，规则被触发。
	if (bean.unittype() == eSWITCH)
	{
		count++;

		if (count == 2)
		{
			return 4;
		}
		// 返回false，停止继续拓扑
		return 0;
	}
	else
	{
		return 0;
	}
}