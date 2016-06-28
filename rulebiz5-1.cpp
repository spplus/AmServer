#include "rulebiz5-1.h"

int RuleBiz5_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eBUS)
	{
		// ���Ϊĸ�ߣ�����������
		COM->triggerRule(ruleMap,3);

		// ����false��ֹͣ��������
		return 0;
	}
	else
	{
		// ����true����������
		return 1;
	}
}