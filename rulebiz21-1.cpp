﻿#include "rulebiz21-1.h"

int RuleBiz21_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.如果结果元件包含刀闸且断开，满足条件二。
	if (bean.unittype() == eSWITCH )
	{
	
		if (bean.state() == 0)
		{
			COM->triggerRule(ruleMap,2);
		}
	}

	// 不进行递归遍历，本次遍历结束后退出
	return 0;
}