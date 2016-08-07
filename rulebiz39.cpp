#include "rulebiz39.h"
#include "rulebiz39-1.h"
RuleBiz39::RuleBiz39()
{
	busNum = 0;
	curConn = "";
	isInit = true;
}

bool RuleBiz39::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	//第一次执行时，连接点为开关两侧，进行记录
	if(isInit)
	{
		isInit = !isInit;

		for (int i = 0 ;i<connIds.size();i++)
		{
			connBreaker.insert(MAPVAL(connIds.at(i).find("connId")->second,""));
		}
	}

	// 3.对应的单侧连接点（不是查所有的两个连接点）
	for (int i = 0 ;i<connIds.size();i++)
	{

		STRMAP connMap = connIds.at(i);
		MAP_ITERATOR connIter = connMap.find("connId");
		curConn = connIter->second;//记录当前连接点cim
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

			//检查集合是否包含间隔边界
			int range = topoRange(saveid,unitsList,ruleMap);
			if(range == 2)//找到两个母线，退出循环
				break;
			else if(range == 0)//找到间隔边界
				continue;

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

				if(topoBiz(saveid,unitId,ruleMap,"") == 1)//如果是刀闸或开关递归
					topoByUnit(saveid,unitId,passNodes,ruleMap);
			}
		}
	}

	if(busNum < 2)//如果边界不是两条母线，不触发规则
		return false;
	else
	{
		for(int k = 0;k<switchList.size();k++)
		{
			STRMAP s = switchList.at(k);
			if(COM->str2i(s.find("state")->second) == 0 &&
				connBreaker.find(s.find("connId")->second) != connBreaker.end())
				return true;
		}
	}

	return false;
}


int RuleBiz39::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH)
	{
		//保存刀闸状态以及对应的连接点cim
		STRMAP s;
		s.insert(MAPVAL("state", COM->i2str(bean.state())));
		s.insert(MAPVAL("connId", curConn));
		switchList.push_back(s);
		return 1;
	}	
	
	return 0;
}

int RuleBiz39::topoRange(int saveid,LISTMAP unitsList,RMAP& ruleMap)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("id");
		PBNS::StateBean bean = getUnitByCim(saveid,unitIter->second);

		if(bean.unittype() == eBUS)//找到母线
		{
			busNum++;

			if(busNum == 1)
			{
				COM->triggerRule(ruleMap,1);
				return 0;
			}
			else
			{
				COM->triggerRule(ruleMap,2);
				return 2;
			}
		}
		else if(bean.unittype() != eBREAKER && bean.unittype() != eSWITCH && bean.unittype() != eGROUNDSWITCH)
			return 0;//找到非母线的间隔边界，直接退出
	}

	return 1;
}