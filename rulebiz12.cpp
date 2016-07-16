#include "rulebiz12.h"

int RuleBiz12::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	// 1.���Ϊ���أ��ҶϿ�����ֱ���˳��߼�
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			return 2;
		}
		else
		{
			// �պϣ�����������1
			COM->triggerRule(ruleMap,1);

			// ����������
			return 0;
		}
	}
	// 1.���Ϊ��բ�ұպϣ������������Ͽ�����ֹ
	else if (bean.unittype() == eSWITCH)
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
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������������
		R_ITERATOR iter = ruleMap.find(2);
		if (iter == ruleMap.end())
		{
			// ����������Ѿ����������ٴδ��������������ʾ��Υ������ֱ���˳�
			return 2;
		}
		else
		{
			COM->triggerRule(ruleMap,2);
		}
		
	}

	// �ж������Ƿ�ȫ��������������򷵻أ����򴥷�
	if (ruleMap.size() >= 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}