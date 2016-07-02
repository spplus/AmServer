#include "rulebiz46.h"

int RuleBiz46::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	eDeviceType dtype = (eDeviceType)bean.unittype();
	if (dtype == eSWITCH || dtype == eBREAKER)
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
	else if (dtype == eREACTOR)
	{
		// ���Ϊ�翹��������һ����
		COM->triggerRule(ruleMap,1);
	}
	else if (dtype == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����翹�����κ�Ԫ��������������
		COM->triggerRule(ruleMap,2);
	}
	
	return 0;
}