#include "rulebiz1.h"

bool RuleBiz1::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
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
		return true;
	}
	else
	{
		return false;
	}
}