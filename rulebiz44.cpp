#include "rulebiz44.h"

RuleBiz44::RuleBiz44()
{
	m_unitCim = "";
	m_switchCount = 0;
	m_switchState = true;
}

bool RuleBiz44::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.对应的单侧连接点（不是查所有的两个连接点）
	for (int i = 0 ;i<connIds.size();i++)
	{

		STRMAP connMap = connIds.at(i);
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
				 topoBiz(saveid,unitId,ruleMap,"");
				
				// 分支一，如果包含的刀闸元件均闭合或者包含的刀闸仅有一个，直接退出逻辑
				 if (m_switchCount == 1 || (m_switchState && m_switchCount>0))
				 {
					 return false;
				 }

				 // 分支二，结果元件中只要有一个刀闸断开，条件一成立。继续查询所有刀闸另一侧的连接点，以及连接点对应的结果元件，如果每个刀闸对应的结果元件均包含母线，条件二成立；
				 bool flag = true;
				 for (int m = 0;m<m_switchList.size();m++)
				 {
					 string cim = m_switchList.at(m);
					 flag &= topoByUnit(saveid,cim,passNodes,ruleMap);
				 }

				 // 如果每个刀闸对应的结果元件均包含母线，条件二成立；
				 if (flag && m_switchCount>0)
				 {
					 COM->triggerRule(ruleMap,2);
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
		}
	}
	return false;
}


int RuleBiz44::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH)
	{
		// 累计刀闸数量
		m_switchCount++;

		// 本次刀闸的状态
		bool state = bean.state()==1?true:false;

		// 刀闸状态进行与操作，如果有一个为false，则为false
		m_switchState =& state;

		// 结果元件中只要有一个刀闸断开，条件一成立
		if (state == false)
		{
			COM->triggerRule(ruleMap,1);
		}
		m_switchList.push_back(bean.cimid());
	}	
	
	return 0;
}