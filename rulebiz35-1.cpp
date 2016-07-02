#include "rulebiz35-1.h"

RuleBiz35_1::RuleBiz35_1()
{
	m_flag = false;
}

int RuleBiz35_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
	{
		// 如果为开关或刀闸且闭合，继续遍历，断开则终止
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}	
	if (bean.unittype() == eTRANSFORMER)
	{
		// 如果为变压器，条件三成立。
		COM->triggerRule(ruleMap,3);
		m_flag = true;
		return 1;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		// 如果结果元件为地刀为断开，满足条件四。
		if (m_flag && bean.state() == 0)
		{
			COM->triggerRule(ruleMap,4);
		}

		return 0;
	}
	else
	{
		// 如果为非开关、刀闸、地刀、变压器的任何一种元件类型，终止该条路遍历
		return 0;
	}
}