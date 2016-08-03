#include "rulebiz5.h"
#include "rulebiz5-1.h"
#include "rulebiz5-2.h"

RuleBiz5::RuleBiz5()
{
	m_breakerCim = "";
	range = false;
}
bool RuleBiz5::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

				// 判断是否是本次操作的设备，如果是，则跳过
				if (unitcim == unitId)
				{
					continue;
				}

				// 本轮拓扑的业务处理，具体子类实现
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
			
				// 判断是否直接退出
				if (topoRst == 2)
				{
					return false;
				}
				else if (topoRst == 4)
				{
					// 规则被触发
					return true;
				}
			}

			// 当条件一满足时，以开关为起始元件继续遍历另一端的连接点，以及连接点对应的结果元件，如果结果元件包含两个闭合的刀闸，满足条件五，规则被触发
			if (m_breakerCim.length() > 0)
			{
				if(range)
				{
					//间隔边界为非母线，操作刀闸为线刀
					RuleBiz5_1 r;
					r.setReq(m_req);
					r.topoByUnit(saveid,m_breakerCim,passNodes,ruleMap);
				}
				else
				{
					//间隔边界为母线，操作刀闸为母刀
					RuleBiz5_2 r;
					r.setReq(m_req);
					r.topoByUnit(saveid,m_breakerCim,passNodes,ruleMap);
				}
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
}

int RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if(bean.cimid().empty())
		return 0;

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER)
	{
		// 如果为开关（B），且开关为闭合，直接退出逻辑，
		if (bean.state() == 0)
		{
			m_breakerCim = bean.cimid();
			// 如开关（B）为断开，满足条件一
			COM->triggerRule(ruleMap,1);	

		}
		else
		{
			// 且开关为闭合，直接退出逻辑，
			return 2;
		}
	}
	else if (bean.unittype() == eSWITCH )
	{
		/*if (bean.state() == 1 && bean.cimid() != m_opcim)
		{
			COM->triggerRule(ruleMap,2);
		}*/

	}
	else if (bean.unittype() == eBUS)
	{
		// 如果为母线，满足条件3
		//COM->triggerRule(ruleMap,3);

	}
	else
	{
		// 如果为非开关、刀闸、地刀的任何元件，条件4成立
		range = true;
	}

	return 0;

}