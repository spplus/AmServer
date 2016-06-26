#include "rulebiz4.h"

bool RuleBiz4::topoBiz(int saveid,string unitcim,vector<int>& ruleList)
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
			ruleList.pop_back();
	}


	// �ж������Ƿ�ȫ��������������򷵻أ����򴥷�
	if (ruleList.size()>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}