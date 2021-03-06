﻿#include "rulebiz21.h"
#include "rulebiz21-1.h"

RuleBiz21::RuleBiz21()
{
	m_breakerCim = "";
}
bool RuleBiz21::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
			
				// 判断是否直接退出
				if (topoRst == 2)
				{
					return false;
				}

			}
			// 如果结果原件不包含母线，满足条件二；如果包含母线，则在上面一层循环，将退出，不会到这里
			//COM->triggerRule(ruleMap,2);

			// 当条件一满足时，以开关为起始元件继续遍历另一端的连接点，以及连接点对应的结果元件，如果结果元件包含刀闸且断开，满足条件三。
			if (m_breakerCim.length() > 0)
			{
				RuleBiz21_1 r;
				r.setReq(m_req);
				r.topoByUnit(saveid,m_breakerCim,passNodes,ruleMap);
			}
		}
	
	}
	
	// 判断是否触发规则
	if (ruleMap.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


int RuleBiz21::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为母线，不违背规则，直接跳出逻辑。
	if (bean.unittype() == eBUS )
	{
		return eRuleExit;
	}
	else if (bean.unittype() == eBREAKER)
	{
		// 如果为开关且断开，满足条件一
		if (bean.state() == 0)
		{
			COM->triggerRule(ruleMap,1);
			m_breakerCim = bean.cimid();
		}
	}
	return 0;
}