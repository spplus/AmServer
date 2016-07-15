#include "rulebiz27.h"
#include "rulebiz27-1.h"

RuleBiz27::RuleBiz27()
{
	m_breakerCim = "";
}

bool RuleBiz27::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.根据连接点ID在连接关系表查询关联的设备
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// 判断是否已经查找过的连接点，如果是则跳出，不是则加入
			if (passNodes.find(connIter->second) != passNodes.end())
			{
				continue;
			}
			else
			{
				passNodes.insert(MAPVAL(connIter->second,connIter->second));
			}

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
					}
				}

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 本轮拓扑的业务处理，具体子类实现
				topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());

			}
			
			// 条件一、二满足时，以开关为起始元件继续遍历另一端的连接点
			R_ITERATOR iter1 = ruleMap.find(1);
			R_ITERATOR iter2 = ruleMap.find(2);
			if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
			{
				if (m_breakerCim.length()>0)
				{
					RuleBiz27_1 r;
					STRMAP passedNodes;
					r.topoByUnit(saveid,m_breakerCim,passedNodes,ruleMap);
				}
			}
			
			// 条件一、二、三同时满足时触发规则
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
	return false;
}


int RuleBiz27::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关且断开，满足条件一。
	if (bean.unittype() == eBREAKER && bean.state() == 0)
	{
		COM->triggerRule(ruleMap,1);
		m_breakerCim = bean.cimid();
	}
	else if (bean.unittype() == eBUS)
	{
		// 如果结果包含母线，满足条件二
		COM->triggerRule(ruleMap,2);
	}
	
	return 0;
}