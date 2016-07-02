#include "rulebiz34-1.h"

int RuleBiz34_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eGROUNDSWITCH && bean.state() == 0)
	{
		// 如果结果元件为地刀为断开，满足条件三
		COM->triggerRule(ruleMap,3);
		
	}

	return 0;
}