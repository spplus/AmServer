#include "rulebiz22.h"

int RuleBiz22::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为带电元件或者断开的刀闸、开关，终止该条路的遍历
	if (((bean.unittype() == eBREAKER 
		|| bean.unittype() == eSWITCH) 
		&& bean.state() == 0) || bean.iselectric() == 1 )
	{
		return 0;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		// 如果为接地元件,满足条件一
		COM->triggerRule(ruleMap,1);
		
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