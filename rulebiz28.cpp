#include "rulebiz28.h"

 RuleBiz28::RuleBiz28()
{
	m_isFirst = true;
}

void RuleBiz28::setBeginCim(string bcim)
{
	m_breakerCim = bcim;
}

bool RuleBiz28::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

				// 元件类型
				int unitType = COM->getIval(unitMap,"UnitType");

				// 元件状态
				int state = COM->getIval(unitMap,"State");

				// 检查A是否包含在结果中，如不包含，持续遍历
				if (unitId == m_breakerCim)
				{
					COM->triggerRule(ruleMap,1);
					return true;
				}
				else if (unitType == eTRANSFORMER || 
					((unitType == eBREAKER||unitType == eSWITCH)&& state == 0))
				{
					// 变压器、断开的开关和刀闸为该条路遍历的终点
					continue;
				}
				else
				{
					// 递归查询
					if (ruleMap.size()>0)
					{
						topoByUnit(saveid,unitId,passNodes,ruleMap);
					}
					else
					{
						return true;
					}
					
				}

			}
		}

		// 首次拓扑，只拓扑单侧连接点
		if (m_isFirst)
		{
			break;
		}

		// 标记为第一次完成
		m_isFirst = false;

	}
	return false;
}
