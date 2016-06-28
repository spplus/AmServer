#include "rulebiz5-2.h"

int RuleBiz5_2::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eSWITCH)
	{
		// 如果为刀闸，且刀闸为闭合，满足条件五
		COM->triggerRule(ruleMap,5);

		// 停止继续拓扑
		return 0;
	}
	else
	{
		// 继续递归拓扑
		return 1;
	}
}