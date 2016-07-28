#include "rulebiz18.h"

int RuleBiz18::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如有元件带电，满足条件
	if (bean.iselectric() == 1)
	{
		COM->triggerRule(ruleMap,1);
		return 0;

	}
	
	// 判断条件是否全部触发，如果是则返回，规则触发
	if (ruleMap.size()>0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}