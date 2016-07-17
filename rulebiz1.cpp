#include "rulebiz1.h"

int RuleBiz1::topoBiz(int saveid,string unitcim,map<int,int>& ruleMap,string stationcim/* ="" */)
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
	else if (bean.unittype() == eGENERATOR)
	{
		// 如果为发电机，条件一成立
		COM->triggerRule(ruleMap,1);

		// 返回3，表示终止该连接点下的所有设备的遍历。
		return 3;
	}
	else
	{
		// 如果为非开关、刀闸、地刀、发电机的任何一种元件类型，条件二成立；
		COM->triggerRule(ruleMap,2);
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