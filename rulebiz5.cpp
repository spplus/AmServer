#include "rulebiz5.h"

bool RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			/*ruleList.pop_back();*/
		}
	}
	else if (bean.unittype() == eSWITCH)
	{

	}
	else if (bean.unittype() == eBUS)
	{
		 /*ruleList.pop_back();*/
	}
	else
	{
		// 如果为非开关、刀闸、地刀的任何元件，条件一成立
		R_ITERATOR iter1 = ruleMap.find(1);
		if (iter1 != ruleMap.end())
		{
			ruleMap.erase(iter1);
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