#include "rulebiz21.h"

int RuleBiz21::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为母线，不违背规则，直接跳出逻辑。
	if (bean.unittype() == eBUS )
	{
		return 2;
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