#include "rulebiz32.h"


int RuleBiz32::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
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
	else if (bean.unittype() == eREACTOR)
	{
		// 如果为电抗器，满足条件一
		COM->triggerRule(ruleMap,1);
		return 1;
	}
	else if (bean.unittype() == eCAPACITOR)
	{
		// 如果为电容器，满足条件二
		COM->triggerRule(ruleMap,2);
		return 0;
	}
	else if (bean.unittype() == eTRANSFORMER)
	{
		// 如果为变压器，终止该条路的遍历
		return 0;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// 如果为非开关、刀闸、地刀、电抗器、变压器的任何元件（后用A代指），对电抗器和A的另一侧连接点持续往下遍历，直至最终
		return 1;
	}

}