#include "rulebiz12.h"

bool RuleBiz12::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER || bean.unittype() == eSWITCH)
	{
		if (bean.state() == 1)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return false;
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
		return true;
	}
	else
	{
		return false;
	}
}