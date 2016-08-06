#include "rulebiz22.h"

int RuleBiz22::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eGROUNDSWITCH)
	{
		return 4;//找到接地元件，触发
		
	}

	return 0;
}