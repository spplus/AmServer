#include "rulebiz12.h"

int RuleBiz12::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	// 1.如果为开关，且断开，则直接退出逻辑
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			return 2;
		}
		else
		{
			// 闭合，则满足条件1
			COM->triggerRule(ruleMap,1);

			// 不继续遍历
			return 0;
		}
	}
	// 1.如果为刀闸且闭合，继续遍历，断开则终止
	else if (bean.unittype() == eSWITCH)
	{
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}

	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// 如果为非开关、刀闸、地刀的任何元件，条件二成立
		R_ITERATOR iter = ruleMap.find(2);
		if (iter == ruleMap.end())
		{
			// 如果条件二已经被触发，再次触发条件二，则表示不违背规则，直接退出
			return 2;
		}
		else
		{
			COM->triggerRule(ruleMap,2);
		}
		
	}

	// 判断条件是否全部触发，如果是则返回，规则触发
	if (ruleMap.size() >= 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}