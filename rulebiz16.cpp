#include "rulebiz16.h"

bool RuleBiz16::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关（无论断开闭合），仍然往下遍历
	if (bean.unittype() == eBREAKER )
	{
		return true;

	}
	else if (bean.unittype() == eSWITCH)
	{
		// 如果为刀闸且闭合，条件成立,规则触发
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);

			// 返回，停止拓扑
			return false;
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
	else
	{
		// 如果为非开关、刀闸、地刀的任何元件，条件成立
		COM->triggerRule(ruleMap,1);
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