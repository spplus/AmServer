#include "rulebiz38-1.h"

RuleBiz38_1::RuleBiz38_1()
{
	m_flag = false;
}

int RuleBiz38_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH)
	{
		// ������Ԫ��Ϊ��բ�����۱պ϶Ͽ���������������բ��һ������ӵ�
		m_flag = true;
		return 1;
	}
	else if (bean.unittype() == eBUS)
	{
		if (m_flag)
		{
			// ������Ԫ������ĸ�ߣ�������������
			COM->triggerRule(ruleMap,3);
		}
	}
	return 0;
}