#include "rulebiz25.h"
#include "rulebiz25-1.h"

RuleBiz25::RuleBiz25()
{
	m_optype = 1;
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
				int topoRst = topoBiz(saveid,unitId,ruleMap,connIter->second);
			
				// 判断是否直接退出
				if (topoRst == eRuleExit)
				{
					return false;
				}
			}

		}

	}
	// 判断满足条件1的开关所在的连接点下有没有刀闸
	if (m_breakList.size() > 0)
	{
		PBNS::StateBean breakBean = m_breakList.at(0);

		// 如果不包含刀闸，满足条件二，触发规则
		if (m_switchList.size() == 0)
		{
			return true;
		}


		// 标记是否找打同一连接点下的刀闸
		int	flag = -1;
		// 如果包含刀闸，则判断是否与满足条件1的开关在同一个连接点
		for (int i = 0;i<m_switchList.size();i++)
		{
			PBNS::StateBean switchBean = m_switchList.at(i);
			if (switchBean.stationcim() == breakBean.stationcim())
			{
				flag = i;
				break;
			}
		}
		// 如果没找到，则满足条件二，触发规则
		if (flag == -1)
		{
			return true;
		}
		else
		{
			PBNS::StateBean sbean = m_switchList.at(flag);
			// 如果刀闸状态为断开，则满足条件三
			if (m_optype == 0 && sbean.state() == 0)
			{
				COM->triggerRule(ruleMap,3);
				return true;
			}
		}
	}

	return false;
}


int RuleBiz25::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string conncim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	
	// 保存连接点cim
	bean.set_stationcim(conncim);

	// 1.如果包含开关且闭合，满足条件一，否则直接跳出逻辑。
	if (bean.unittype() == eSWITCH && bean.cimid() != m_opcim)
	{
		m_switchList.push_back(bean);
	}
	else if (bean.unittype() == eBREAKER)
	{
		// 如果包含开关且闭合，满足条件一，否则直接跳出逻辑。
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);
			m_breakList.push_back(bean);
		}
		else
		{
			return eRuleExit;
		}
	}
	return 0;
}