#include "rulebiz35.h"
#include "rulebiz35-1.h"
RuleBiz35::RuleBiz35()
{
	m_busCim = "";
}

bool RuleBiz35::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.对应的单侧连接点（不是查所有的两个连接点）
	for (int i = 0 ;i<connIds.size();i++)
	{

		STRMAP connMap = connIds.at(i);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// 根据连接点，查找该连接点关联的设备集合
			LISTMAP unitsList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

			// 遍历该设备集合
			for (int k = 0;k<unitsList.size();k++)
			{
				STRMAP  unitMap = unitsList.at(k);
				MAP_ITERATOR unitIter = unitMap.find("id");
				string unitId ;
				if (unitIter != unitMap.end())
				{
					// 判断是否已经做为起始设备进行搜索，如果是则跳过
					if (passNodes.find(unitIter->second) != passNodes.end())
					{
						continue;
					}
				}

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				if (topoBiz(saveid,unitId,ruleMap,""))
				{
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
				
				//条件一、二成立时说明变压器间隔连通且另一边界为母线，接下来查询母线其他连通间隔是否含有变压器。查询母线另一侧的连接点
				// 判断条件一，条件二是否成立

				R_ITERATOR iter1 = ruleMap.find(1);
				R_ITERATOR iter2 = ruleMap.find(2);
				if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
				{
					RuleBiz35_1 r;
					r.topoByUnit(saveid,m_busCim,passNodes,ruleMap);
				}

				// 规则被触发
				if (ruleMap.size() == 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}


int RuleBiz35::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
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
	else if (bean.unittype() == eTRANSFORMER)
	{
		// 如果为变压器，满足条件一
		COM->triggerRule(ruleMap,1);
		
		return 0;
	}
	else if (bean.unittype() == eBUS)
	{
		// 如果为母线，条件二成立
		COM->triggerRule(ruleMap,2);
		m_busCim = bean.cimid();
		return 0;
	}
	return 0;
}