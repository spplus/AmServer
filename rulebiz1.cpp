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
		R_ITERATOR iter1 = ruleMap.find(1);
		if (iter1 != ruleMap.end())
		{
			ruleMap.erase(iter1);
		}
	
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص�����������κ�һ��Ԫ�����ͣ�������������
		R_ITERATOR iter2 = ruleMap.find(2);
		if (iter2 != ruleMap.end())
		{
			ruleMap.erase(iter2);
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