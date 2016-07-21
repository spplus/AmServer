#include "rulebiz25.h"
#include "rulebiz25-1.h"

RuleBiz25::RuleBiz25()
{
	m_hasSwitch = false;
	m_hasSwitchState = 1;
	m_optype = 1;
	m_hasBus = false;
	m_switchCim = "";
}
void RuleBiz25::setOptype(int optype)
{
	m_optype = optype;
}
bool RuleBiz25::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				if (topoRst == eRuleExit)
				{
					return false;
				}

			}
			// 如果不包含刀闸，满足条件二
			if (!m_hasSwitch)
			{
				COM->triggerRule(ruleMap,2);
			}
			
			R_ITERATOR iter1 = ruleMap.find(1);
			R_ITERATOR iter2 = ruleMap.find(2);

			// 条件一二同时满足时，触发规则
			if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
			{
				return true;
			}
			
			// 如果包含刀闸，进入分支逻辑
			if (m_hasSwitch)
			{
				// 起始条件为客户端断开刀闸，否则直接跳出
				if (m_optype == 0)
				{
					return false;
				}

				// 如果包含的刀闸为断开，满足条件三
				if (m_hasSwitchState == 0)
				{
					COM->triggerRule(ruleMap,3);
				}

				//  以条件三中断开刀闸为起始元件继续遍历另一端的连接点，以及连接点对应的结果元件，如果结果元件包含母线，满足条件五；
				RuleBiz25_1 r;
				r.setReq(m_req);
				r.topoByUnit(saveid,m_switchCim,passNodes,ruleMap);

			}

			// 当条件一、条件三、条件四、条件五满足时触发规则。
			iter2 = ruleMap.find(2);
			if (ruleMap.size() == 4 && iter2 != ruleMap.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}
	return false;
}


int RuleBiz25::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果包含开关且闭合，满足条件一，否则直接跳出逻辑。
	if (bean.unittype() == eSWITCH)
	{
		m_hasSwitch = true;
		
		m_hasSwitchState = bean.state();
		m_switchCim = bean.cimid();
	}
	else if (bean.unittype() == eBUS)
	{
		m_hasBus = true;

		// 如果结果包含母线，满足条件四
		COM->triggerRule(ruleMap,4);
	}
	else if (bean.unittype() == eBREAKER)
	{
		// 如果包含开关且闭合，满足条件一，否则直接跳出逻辑。
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);
		}
		else
		{
			return eRuleExit;
		}
	}
	return 0;
}