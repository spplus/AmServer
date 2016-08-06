#include "rulebiz1.h"

RuleBiz1::RuleBiz1()
{
	isInit = true;
}

bool RuleBiz1::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	// 第一次执行时检查是否为推车刀闸，如果是则把推车刀闸加入到已操作列表
	if(isInit)
	{
		isInit = !isInit;
		checkIsCarSwitch(m_req);
	}

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

			//先判断集合内是否有间隔边界，有的话其他元件不再递归，跳过该连接点
			if(topoRange(unitsList, ruleMap))
			{
				continue;
			}

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

				if (topoRst == eRuleRecursion)
				{
					// 递归，以该元件为起点进行重新遍历
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
			}
		}
	}

	if (ruleMap.size()>0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int RuleBiz1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if ((bean.unittype() == eBREAKER || bean.unittype() == eSWITCH) && bean.state() == 1)
	{
		return 1;

	}
	else
	{
		return 0;
	}
}

bool RuleBiz1::topoRange(LISTMAP unitsList,RMAP& ruleMap)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("UnitType");
		int unitType = COM->str2i(unitIter->second);

		// 如果为发电机，满足条件一
		if(unitType == eGENERATOR)
		{
			COM->triggerRule(ruleMap,1);
			return true;
		}
		else if(unitType != eBREAKER && unitType != eSWITCH && unitType != eGROUNDSWITCH)
		{
			COM->triggerRule(ruleMap,2);
			return true;
		}	
	}

	return false;
}