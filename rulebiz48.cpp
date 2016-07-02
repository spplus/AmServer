#include "rulebiz48.h"

int RuleBiz48::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
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
	else if (dtype == eCAPACITOR)
	{
		// ���Ϊ������������һ����
		COM->triggerRule(ruleMap,1);
	}
	else if (dtype == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص������������κ�Ԫ��������������
		COM->triggerRule(ruleMap,2);
	}
	
	return 0;
}