#include "rulebiz5-1.h"

RuleBiz5_1::RuleBiz5_1()
{
	count = 0;
}

int RuleBiz5_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// ������Ԫ�����������պϵĵ�բ�����������壬���򱻴�����
	if (bean.unittype() == eSWITCH)
	{
		count++;

		if (count == 2)
		{
			return 4;
		}
		// ����false��ֹͣ��������
		return 0;
	}
	else
	{
		return 0;
	}
}