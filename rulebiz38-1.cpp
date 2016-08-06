#include "rulebiz38-1.h"

RuleBiz38_1::RuleBiz38_1()
{
	m_flag = false;
}

bool RuleBiz38_1::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

			//检查元件集合是否存在间隔边界
			int range = topoRange(saveid,unitsList,ruleMap);
			if(range == 2)//找到母线，母联间隔，触发规则
				return true;
			else if(range == 0)//找到其他边界，退出
				return false;
			

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
				int rel = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());

				// 判断是否递归
				if (rel == 1)
				{
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}

			}
		}
	}

	return true;
}

int RuleBiz38_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH && bean.state() == 1)
	{
		// 如果为刀闸且闭合，继续遍历
		return 1;
	}
}

int RuleBiz38_1::topoRange(int saveid,LISTMAP unitsList,RMAP& ruleMap)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("id");
		PBNS::StateBean bean = getUnitByCim(saveid,unitIter->second);

		if(bean.unittype() == eBUS)//找到母线说明是母联间隔，触发规则
		{
			COM->triggerRule(ruleMap,3);
			return 2;
		}
		else if(bean.unittype() != eBREAKER && bean.unittype() != eSWITCH && bean.unittype() != eGROUNDSWITCH)
			return 0;//找到非母线的间隔边界，直接退出
	}

	return 1;
}