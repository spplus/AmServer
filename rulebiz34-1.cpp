#include "rulebiz34-1.h"

int RuleBiz34_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eGROUNDSWITCH && bean.state() == 0)
	{
		// ������Ԫ��Ϊ�ص�Ϊ�Ͽ�������������
		COM->triggerRule(ruleMap,3);
		
	}

	return 0;
}