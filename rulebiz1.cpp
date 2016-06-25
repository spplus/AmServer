#include "rulebiz1.h"

bool RuleBiz1::topoBiz(int saveid,string unitcim,vector<int>& ruleList)
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
		if (ruleList.size()>0)
		{
			ruleList.pop_back();
		}
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص�����������κ�һ��Ԫ�����ͣ�������������
		if (ruleList.size()>0)
		{
			ruleList.pop_back();
		}
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