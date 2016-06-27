#include "rulebiz5-2.h"

bool RuleBiz5_2::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eSWITCH)
	{
		// 如果为刀闸，且刀闸为闭合，满足条件五
		COM->triggerRule(ruleMap,5);

		// 停止继续拓扑
		return false;
	}
	else
	{
		// 继续递归拓扑
		return true;
	}
}