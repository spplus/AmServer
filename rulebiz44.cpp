#include "rulebiz44.h"

bool RuleBiz44::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.根据连接点ID在连接关系表查询关联的设备
	for (int i = 0 ;i<connIds.size();i++)
	{
		STRMAP connMap = connIds.at(i);
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

			//检查集合中是否包含间隔边界以及边界类型
			int range = checkList(saveid, unitsList);

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
				topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
				
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


int RuleBiz44::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	//如果是开关或刀闸且闭合，继续往下拓扑
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
	{
		if(bean.state() == 1)
		{}
		else
		{}
	}	
	
	return 0;
}

int RuleBiz44::checkList(int saveid,LISTMAP unitsList)
{
	for (int k = 0;k<unitsList.size();k++)
	{
		STRMAP  unitMap = unitsList.at(k);
		MAP_ITERATOR unitIter = unitMap.find("id");
		PBNS::StateBean bean = getUnitByCim(saveid,unitIter->second);
		if(bean.unittype() == eSWITCH || bean.unittype() == eBREAKER || bean.unittype() == eGROUNDSWITCH)//非边界
		{
			return 0;
		}
		else if(bean.unittype() == eLINE)//是进出线
		{
			bool change;//带电状态是否改变，改变为true

			for(int m = 0;m<expectMap.size();m++)
			{
				if(bean.cimid() == expectMap.at(m).cimid())
					bean.iselectric() == expectMap.at(m).iselectric() ? change = false : change = true;

				break;
			}
		}
		else
			return 2;
	}
}