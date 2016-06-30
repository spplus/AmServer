#include "rulebiz1.h"

int RuleBiz1::topoBiz(int saveid,string unitcim,map<int,int>& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER || bean.unittype() == eSWITCH)
	{
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}

	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
	else if (bean.unittype() == eGENERATOR)
	{
		// ���Ϊ�����������һ����
		COM->triggerRule(ruleMap,1);
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص�����������κ�һ��Ԫ�����ͣ�������������
		COM->triggerRule(ruleMap,2);
	}

	// �ж������Ƿ�ȫ��������������򷵻أ����򴥷�
	if (ruleMap.size()>0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}