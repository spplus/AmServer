#include "rulebiz4.h"

int RuleBiz4::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER || bean.unittype() == eSWITCH)
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
		// 如果为非开关、刀闸、地刀的任何元件，条件一成立
		R_ITERATOR iter = ruleMap.find(1);
		if (iter == ruleMap.end())
		{
			COM->triggerRule(ruleMap,2);
		}
		else
		{
			COM->triggerRule(ruleMap,1);

			// 跳过该连接点剩余设备
			return 3;
		}
	}
	if (ruleMap.size()>0)

	// 判断条件是否全部触发，如果是则返回，规则触发
	{
		return 1;
	}
	else
	{
		return 0;
	}
}