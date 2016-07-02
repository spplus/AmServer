#include "rulebiz43.h"


bool RuleBiz43::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);
	if (beginBean.isboard() == 1)
	{
		return true;
	}
	else
	{
		return false;
	}

}