#include "rulebiz5.h"

bool RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			/*ruleList.pop_back();*/
		}
	}
	else if (bean.unittype() == eSWITCH)
	{

	}
	else if (bean.unittype() == eBUS)
	{
		 /*ruleList.pop_back();*/
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������һ����
		R_ITERATOR iter1 = ruleMap.find(1);
		if (iter1 != ruleMap.end())
		{
			ruleMap.erase(iter1);
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