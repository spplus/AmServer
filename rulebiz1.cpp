#include "rulebiz1.h"

bool RuleBiz1::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER || bean.unittype() == eSWITCH)
	{
		if (bean.state() == 1)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return false;
	}
	else if (bean.unittype() == eGENERATOR)
	{
		// 如果为发电机，条件一成立
		R_ITERATOR iter1 = ruleMap.find(1);
		if (iter1 != ruleMap.end())
		{
			ruleMap.erase(iter1);
		}
	
	}
	else
	{
		// 如果为非开关、刀闸、地刀、发电机的任何一种元件类型，条件二成立；
		R_ITERATOR iter2 = ruleMap.find(2);
		if (iter2 != ruleMap.end())
		{
			ruleMap.erase(iter2);
		}
	}

	// 判断条件是否全部触发，如果是则返回，规则触发
	if (ruleMap.size()>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}