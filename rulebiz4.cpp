#include "rulebiz4.h"

bool RuleBiz4::topoBiz(int saveid,string unitcim,vector<int>& ruleList)
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
	else
	{
			// 如果为非开关、刀闸、地刀的任何元件，条件一成立
			ruleList.pop_back();
	}


	// 判断条件是否全部触发，如果是则返回，规则触发
	if (ruleList.size()>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}