#include "rulebiz20.h"

RuleBiz20::RuleBiz20()
{
	isInit = true;
}

bool RuleBiz20::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

			//如该集合均不带电，跳过
			if(!checkEcletric(saveid,unitsList))
				continue;

			//如果第二次找到站点不同的电源点，直接满足条件触发规则
			int range = topoRange(unitsList,ruleMap,beginBean.stationcim());

			if(range == 0)
				return true;
			else if(range == 2)
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

	return false;
}

int RuleBiz20::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为刀闸或开关且闭合，继续遍历，断开则终止
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
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
}

//检查当前集合带电状态,如果均不带电，跳过该连接点逻辑
bool RuleBiz20::checkEcletric(int saveid,LISTMAP unitsList)
{
	bool hasElec = false;

	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("id");
		string unitcim = unitIter->second;
		PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
		if(bean.iselectric() == 1)//带电
		{
			hasElec = true;
		}
	}

	return hasElec;
}

//在当前集合查找间隔边界,并针对边界类型做不同处理
int RuleBiz20::topoRange(LISTMAP unitsList,RMAP& ruleMap,string stationcim)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("UnitType");
		int unitType = COM->str2i(unitIter->second);

		if(unitType == eLINE)//是进出线
		{
			if(COM->str2i(unitMap.find("IsPower")->second) == 1)//是相对电源点
			{
				R_ITERATOR iter = ruleMap.find(1);
				if (iter != ruleMap.end())//第一次找到
				{
					COM->triggerRule(ruleMap,1);

					// 保存本次拓扑的起始元件的站点CIM
					m_stationCim.push_back(stationcim);
				}
				else//第二次找到
				{
					// 如果该次遍历起始元件对应的站点cim与之前记录的不同，满足条件二；
					if (m_stationCim.size()>0)
					{
						if(m_stationCim.at(0) != stationcim)
						{
							COM->triggerRule(ruleMap,2);
							return 0;//触发规则
						}
						else
						{
							return 1;//继续执行
						}
					}
				}
			}
		}
		else if(unitType != eBREAKER && unitType != eSWITCH)
			return 2;//退出该次循环
	}

	return 1;//继续执行
}