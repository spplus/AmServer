#include "rulebiz25-1.h"

int RuleBiz25_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.������Ԫ������ĸ�ߣ����������壻
	if (bean.unittype() == eBUS )
	{
		COM->triggerRule(ruleMap,5);
	}

	// �����еݹ���������α����������˳�
	return 0;
}