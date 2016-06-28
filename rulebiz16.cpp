#include "rulebiz16.h"

int RuleBiz16::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���أ����۶Ͽ��պϣ�����Ȼ���±���
	if (bean.unittype() == eBREAKER )
	{
		return 1;

	}
	else if (bean.unittype() == eSWITCH)
	{
		// ���Ϊ��բ�ұպϣ���������,���򴥷�
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);

			// ���أ�ֹͣ����
			return 0;
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
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ������������
		COM->triggerRule(ruleMap,1);
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