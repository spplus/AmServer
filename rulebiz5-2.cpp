#include "rulebiz5-2.h"

bool RuleBiz5_2::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eSWITCH)
	{
		// ���Ϊ��բ���ҵ�բΪ�պϣ�����������
		COM->triggerRule(ruleMap,5);

		// ֹͣ��������
		return false;
	}
	else
	{
		// �����ݹ�����
		return true;
	}
}