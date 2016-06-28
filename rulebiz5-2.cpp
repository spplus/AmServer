#include "rulebiz5-2.h"

int RuleBiz5_2::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	if (bean.unittype() == eSWITCH)
	{
		// ���Ϊ��բ���ҵ�բΪ�պϣ�����������
		COM->triggerRule(ruleMap,5);

		// ֹͣ��������
		return 0;
	}
	else
	{
		// �����ݹ�����
		return 1;
	}
}