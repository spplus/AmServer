#include "rulebiz35-1.h"

RuleBiz35_1::RuleBiz35_1()
{
	m_flag = false;
}

int RuleBiz35_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
	{
		// ���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}	
	if (bean.unittype() == eTRANSFORMER)
	{
		// ���Ϊ��ѹ����������������
		COM->triggerRule(ruleMap,3);
		m_flag = true;
		return 1;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		// ������Ԫ��Ϊ�ص�Ϊ�Ͽ������������ġ�
		if (m_flag && bean.state() == 0)
		{
			COM->triggerRule(ruleMap,4);
		}

		return 0;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص�����ѹ�����κ�һ��Ԫ�����ͣ���ֹ����·����
		return 0;
	}
}