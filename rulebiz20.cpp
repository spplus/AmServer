#include "rulebiz20.h"

int RuleBiz20::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为刀闸或开关且闭合，继续遍历，断开则终止
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
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
	else if (bean.unittype() == eLINE && bean.ispower() == 1)
	{
		// 如果为进出线且为相对电源点,满足条件一
		R_ITERATOR iter = ruleMap.find(1);
		if (iter != ruleMap.end())
		{
			COM->triggerRule(ruleMap,1);

			// 保存本次拓扑的起始元件的站点CIM
			m_stationCim.push_back(stationcim);
		}
		else
		{
			
			// 如果再次遍历到进出线且为相对电源点，并且该次遍历起始元件对应的站点cim与之前记录的不同，满足条件二；
			if (m_stationCim.size()>0)
			{
				if(m_stationCim.at(0) != stationcim)
				{
					COM->triggerRule(ruleMap,2);
					return 0;
				}
				else
				{
					return 1;
				}
			}
		}
		
		return 0;
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