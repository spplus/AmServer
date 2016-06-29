#include "rulebiz21.h"

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

				// 本轮拓扑的业务处理，具体子类实现
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
			
				// 判断是否直接退出
				if (topoRst == 2)
				{
					return false;
				}

			}
			// 如果结果原件不包含母线，满足条件二；如果包含母线，则在上面一层循环，将退出，不会到这里
			COM->triggerRule(ruleMap,2);




		}

	}
	return false;
}


int RuleBiz21::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为母线，不违背规则，直接跳出逻辑。
	if (bean.unittype() == eBUS )
	{
		return 2;
	}
	else if (bean.unittype() == eBREAKER)
	{
		// 如果为开关且断开，满足条件一
		if (bean.state() == 0)
		{
			COM->triggerRule(ruleMap,1);
		}
	}
	return 1;
}