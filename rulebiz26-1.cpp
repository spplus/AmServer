#include "rulebiz26-1.h"

int RuleBiz26_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果元件包含刀闸且闭合，满足条件三；
	if (bean.unittype() == eSWITCH && bean.state() == 1)
	{
		COM->triggerRule(ruleMap,3);
	}

	// 不进行递归遍历，本次遍历结束后退出
	return 0;
}