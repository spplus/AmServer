#include "rulebiz5-1.h"

int RuleBiz5_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eBUS)
	{
		// 如果为母线，满足条件三
		COM->triggerRule(ruleMap,3);

		// 返回false，停止继续拓扑
		return 0;
	}
	else
	{
		// 返回true，继续拓扑
		return 1;
	}
}