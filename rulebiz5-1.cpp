#include "rulebiz5-1.h"

bool RuleBiz5_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eBUS)
	{
		// ���Ϊĸ�ߣ�����������
		COM->triggerRule(ruleMap,3);

		// ����false��ֹͣ��������
		return false;
	}
	else
	{
		// ����true����������
		return true;
	}
}