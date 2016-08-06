#include "rulebiz45.h"

RuleBiz45::RuleBiz45()
{
	isInit = true;
}

bool RuleBiz45::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

			//检查元件集合是否存在间隔边界
			if(topoRange(unitsList,ruleMap))
				continue;//找到边界，跳过该连接点

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

int RuleBiz45::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
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

	return 0;
}

bool RuleBiz45::topoRange(LISTMAP unitsList,RMAP& ruleMap)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("UnitType");
		int unitType = COM->str2i(unitIter->second);

		if(unitType == eCAPACITOR)//找到电容器，条件一成立
		{
			COM->triggerRule(ruleMap,1);
			return true;
		}	
		else if(unitType != eBREAKER && unitType != eSWITCH && unitType != eGROUNDSWITCH)
		{
			COM->triggerRule(ruleMap,2);;//找到非电容器的间隔边界，条件二成立
			return true;
		}	
	}

	return false;
}