#include "rulebiz46.h"

int RuleBiz46::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	eDeviceType dtype = (eDeviceType)bean.unittype();
	if (dtype == eSWITCH || dtype == eBREAKER)
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
	else if (dtype == eREACTOR)
	{
		// 如果为电抗器，条件一成立
		COM->triggerRule(ruleMap,1);
	}
	else if (dtype == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// 如果为非开关、刀闸、地刀、电抗器的任何元件，条件二成立
		COM->triggerRule(ruleMap,2);
	}
	
	return 0;
}