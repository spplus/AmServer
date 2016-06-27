#include "rulebiz16.h"

bool RuleBiz16::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���أ����۶Ͽ��պϣ�����Ȼ���±���
	if (bean.unittype() == eBREAKER )
	{
		return true;

	}
	else if (bean.unittype() == eSWITCH)
	{
		// ���Ϊ��բ�ұպϣ���������,���򴥷�
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);

			// ���أ�ֹͣ����
			return false;
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
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ������������
		COM->triggerRule(ruleMap,1);
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