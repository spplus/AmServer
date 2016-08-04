#include "rulebiz4.h"


bool RuleBiz4::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

			//先判断集合内是否有间隔边界，有的话其他元件不再递归，跳过该连接点
			if(topoRange(unitsList))
			{
				// 如果为非开关、刀闸、地刀的任何元件，条件一成立
				R_ITERATOR iter = ruleMap.find(1);
				if (iter == ruleMap.end())
				{
					COM->triggerRule(ruleMap,2);

					continue;
				}
				else
				{
					COM->triggerRule(ruleMap,1);

					continue;
				}
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

	// 当条件一成立两次以下（不含两次）时，规则被触发。
	if (ruleMap.size()>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


int RuleBiz4::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER || bean.unittype() == eSWITCH)
	{
		if (bean.state() == 1)
		{
			return eRuleRecursion;
		}
		else
		{
			return eRuleNotRecursion;
		}

	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
}

bool RuleBiz4::topoRange(LISTMAP unitsList)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("UnitType");
		int unitType = COM->str2i(unitIter->second);

		// 1.如果为非开关、刀闸、地刀，即间隔边界
		if (unitType != eBREAKER && unitType != eSWITCH && unitType != eGROUNDSWITCH)
			return true;
	}

	return false;
}