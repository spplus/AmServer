#include "rulebiz26-1.h"

RuleBiz26_1::RuleBiz26_1()
{
	flag = true;//判断已找到的开关与母刀连接点，是否有其他闭合的刀闸，之后的递归不再进行此逻辑
	flag2 = false;//已找到闭合刀闸，再次以此遍历时找到的元件集合，特殊处理的标志
}

bool RuleBiz26_1::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	//找到母刀侧连接点的值,方法第一次执行时该数组中有该连接点，且包含在重复列表
	//检查该连接点对应的元件集合有没有闭合的刀闸
	if(flag)
	{
		flag = !flag;

		for (int j = 0;j<connIds.size();j++)
		{
			STRMAP connMap = connIds.at(j);
			MAP_ITERATOR connIter = connMap.find("connId");
			if (connIter != connMap.end())
			{
				if (passNodes.find(connIter->second) != passNodes.end())//找到母刀侧连接点
				{
					LISTMAP breakerList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

					for (int k = 0;k<breakerList.size();k++)//检查元件集合有没有闭合刀闸
					{
						STRMAP  unitMap = breakerList.at(k);
						MAP_ITERATOR unitIter = unitMap.find("id");
						PBNS::StateBean bean = getUnitByCim(saveid,unitIter->second);
						if(bean.unittype() == eSWITCH && bean.state() == 1 && bean.cimid() != curUnit)
						{
							return true;
						}
					}

					break;
				}
			}
		}
	}
	

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

			//已找到闭合刀闸，再次以此遍历时找到的元件集合,看间隔边界是否存在母线
			if(flag2)
			{
				flag2 = !flag2;

				for (int k = 0;k<unitsList.size();k++)
				{
					STRMAP  unitMap = unitsList.at(k);
					MAP_ITERATOR unitIter = unitMap.find("UnitType");
					if(COM->str2i(unitIter->second) == eBUS)//只要找到母线就退出，母联间隔，不违背规则
						return true;
				}
			}

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
					if(topoByUnit(saveid,unitId,passNodes,ruleMap))
						return true;
				}
				else if(rel == 2)
					return true;
			}
		}
	}

	COM->triggerRule(ruleMap,3);//中途没return说明触发了条件三
	return true;
}

int RuleBiz26_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	//26操作母刀才能进入26-1，重复的连接点已经单独处理，此时只可能找到线刀或母联刀闸的另一个母刀
	//如果元件包含刀闸且闭合，看间隔边界是不是母线，如不是，满足条件三
	if (bean.unittype() == eSWITCH)
	{
		if(bean.state() == 1)//找到闭合刀闸继续遍历一次寻找间隔边界
		{
			flag2 = true;
			return 1;
		}
		else
			return 2;//断开刀闸直接退出
	}

	return 0;
}