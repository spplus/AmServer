#include "rulebiz5.h"
#include "rulebiz5-1.h"
#include "rulebiz5-2.h"

int RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果为开关或刀闸且闭合，继续遍历，断开则终止
	if (bean.unittype() == eBREAKER)
	{
		// 如果为开关（B），且开关为闭合，直接退出逻辑，
		if (bean.state() == 0)
		{
			// 如开关（B）为断开，满足条件一
			COM->triggerRule(ruleMap,1);	

			return 0;
		}
		else
		{
			// 且开关为闭合，直接退出逻辑，
			return 2;
		}
	}
	else if (bean.unittype() == eSWITCH )
	{
		/*
		包含开关的结果元件集合如包含刀闸（非起始条件自己）且闭合，满足条件二，
		*/
		if (bean.state() == 1 && bean.cimid() != m_opcim)
		{
			COM->triggerRule(ruleMap,2);
		}

		/*R_ITERATOR iter3 = ruleMap.find(3);
		if (iter3 != ruleMap.end())
		{
			RuleBiz5_1 r51;
			STRMAP passedNodes;
			r51.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);
		}*/

		//// 判断是否满足条件三
		//iter3 = ruleMap.find(3);
		//if (iter3 == ruleMap.end())
		//{
		//	// 分支一：刀闸（A）为闭合，满足条件四，规则被触发；
		//	if (bean.state() == 1)
		//	{
		//		COM->triggerRule(ruleMap,4);

		//		// 为了便于检查规则的触发边界，再次把条件5也弹出
		//		COM->triggerRule(ruleMap,5);

		//		return 0;
		//	}
		//	else
		//	{
		//		/*
		//		分支二：刀闸（A）为断开，继续对上述结果元件中的开关查询另一侧的连接点，
		//		再查询连接点对应的结果元件集合（联合查询出UnitType），
		//		针对结果元件集合，如果为刀闸，且刀闸为闭合，满足条件五，规则被触发。
		//		*/
		//		RuleBiz5_2 r52;
		//		STRMAP passedNodes;
		//		r52.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);

		//		// 判断是否满足条件5
		//		R_ITERATOR iter5 = ruleMap.find(5);
		//		if (iter5 == ruleMap.end())
		//		{
		//			// 规则被触法，弹出条件4
		//			COM->triggerRule(ruleMap,4);

		//			// 停止拓扑
		//			return 0;
		//		}
		//	}
		//}

	}
	else if (bean.unittype() == eBUS)
	{
		// 如果为母线，满足条件3
		COM->triggerRule(ruleMap,3);

		// 规则被触发
		return 4;
	}
	else
	{
		// 如果为非开关、刀闸、地刀的任何元件，条件4成立
		COM->triggerRule(ruleMap,4);

		// 遍历开关（B）另一侧的连接点
		RuleBiz5_1 r51;
		r51.setReq(m_req);
		r51.setPassedNodes(m_passedNodes);

		bool ret = r51.topoByUnit(saveid,bean.cimid(),m_passedNodes,ruleMap);
		
		// 如果发现两个闭合的刀闸，则规则触发
		if (ret)
		{
			return 4;
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