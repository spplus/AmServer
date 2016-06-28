#include "rulebiz5.h"
#include "rulebiz5-1.h"
#include "rulebiz5-2.h"

int RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			// 如果为开关，且开关为断开，满足条件二
			COM->triggerRule(ruleMap,2);
			
		}
	}
	else if (bean.unittype() == eSWITCH)
	{
		/*
		如果为刀闸（A），以此刀闸为起始元件继续遍历另一端的连接点，
		以及连接点对应的结果元件，如果结果元件包含母线，满足条件三
		*/
		R_ITERATOR iter3 = ruleMap.find(3);
		if (iter3 != ruleMap.end())
		{
			RuleBiz5_1 r51;
			STRMAP passedNodes;
			r51.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);
		}

		// 判断是否满足条件三
		iter3 = ruleMap.find(3);
		if (iter3 == ruleMap.end())
		{
			// 分支一：刀闸（A）为闭合，满足条件四，规则被触发；
			if (bean.state() == 1)
			{
				COM->triggerRule(ruleMap,4);

				// 为了便于检查规则的触发边界，再次把条件5也弹出
				COM->triggerRule(ruleMap,5);

				return 0;
			}
			else
			{
				/*
				分支二：刀闸（A）为断开，继续对上述结果元件中的开关查询另一侧的连接点，
				再查询连接点对应的结果元件集合（联合查询出UnitType），
				针对结果元件集合，如果为刀闸，且刀闸为闭合，满足条件五，规则被触发。
				*/
				RuleBiz5_2 r52;
				STRMAP passedNodes;
				r52.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);

				// 判断是否满足条件5
				R_ITERATOR iter5 = ruleMap.find(5);
				if (iter5 == ruleMap.end())
				{
					// 规则被触法，弹出条件4
					COM->triggerRule(ruleMap,4);

					// 停止拓扑
					return 0;
				}
			}
		}

	}
	else if (bean.unittype() == eBUS)
	{
		// 如果为母线，满足条件一
		COM->triggerRule(ruleMap,1);

	}
	else
	{
		// 如果为非开关、刀闸、地刀的任何元件，条件一成立
		R_ITERATOR iter1 = ruleMap.find(1);
		if (iter1 != ruleMap.end())
		{
			ruleMap.erase(iter1);
		}
		
	}


	// 判断条件是否全部触发，如果是则返回，规则触发
	if (ruleMap.size()>0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}