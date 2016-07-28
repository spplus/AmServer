#include "rulebiz12.h"

RuleBiz12::RuleBiz12()
{
	m_hasBus = false;
	m_hasSwitch = false;
}

bool RuleBiz12::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{

	// 刀闸连接点CIM
	string sconncim = "";

	// 满足条件的刀闸的所在连接点CIM
	string bconncim = "";

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
		string curconncim = COM->getVal(connMap,"connId");

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
				if (topoRst == eRuleExit)
				{
					return false;
				}
				else if (topoRst == eRuleUser)
				{
					// 记录刀闸的连接点
					if (sconncim.length() > 0 && sconncim != curconncim )
					{
						m_hasSwitch = true;	
					}
					else
					{
						sconncim = curconncim;
					}
				}
				else if (topoRst == eRuleRecursion)
				{
					// 找到满足条件的开关
					bconncim == curconncim;
				}
			
			}

		}

	}

	// 触发规则1满足后
	if (ruleMap.size() == 0)
	{
		// 如果集合不包含母线，直接触发规则
		if (!m_hasBus)
		{
			return true;
		}
		else
		{
			// 如果集合包含母线，判断集合（A）是否包含刀闸且闭合（非起始条件自己），如包含，不违背规则
			if (m_hasSwitch || bconncim == sconncim)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		// 
	}
	

	//	

	return false;
}


int RuleBiz12::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	// 1.如果为开关，且断开，则直接退出逻辑
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			return eRuleExit;
		}
		else
		{
			// 闭合，则满足条件1
			COM->triggerRule(ruleMap,1);

			// 不继续遍历
			return eRuleRecursion;
		}
	}
	// 1.如果为刀闸且闭合，继续遍历，断开则终止
	else if (bean.unittype() == eSWITCH )
	{
		if (bean.state() == 1 && bean.cimid() == m_opcim)
		{
			return eRuleUser;
		}
		
	}
	else if (bean.unittype() == eBUS)
	{
			// 如果集合包含母线，判断集合（A）是否包含刀闸且闭合（非起始条件自己）,如包含，不违背规则，校验通过
			m_hasBus = true;

			// 否则规则被触发。
			return eRuleNotRecursion;
		
		
	}
	else
	{
		return eRuleNotRecursion;
	}
	
}