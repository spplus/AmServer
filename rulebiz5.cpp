#include "rulebiz5.h"

bool RuleBiz5::topoBiz(int saveid,string unitcim,vector<int>& ruleList)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			ruleList.pop_back();
		}
	}
	else if (bean.unittype() == eSWITCH)
	{

	}
	else if (bean.unittype() == eBUS)
	{
		 ruleList.pop_back();
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