#include "rulebiz12.h"

int RuleBiz12::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
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
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������һ����
		R_ITERATOR iter = ruleMap.find(1);
		if (iter == ruleMap.end())
		{
			COM->triggerRule(ruleMap,2);
		}
		else
		{
			COM->triggerRule(ruleMap,1);
		}
		
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