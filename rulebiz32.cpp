#include "rulebiz32.h"


int RuleBiz32::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
	{
		// ���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (bean.unittype() == eREACTOR)
	{
		// ���Ϊ�翹������������һ
		COM->triggerRule(ruleMap,1);
		return 1;
	}
	else if (bean.unittype() == eCAPACITOR)
	{
		// ���Ϊ������������������
		COM->triggerRule(ruleMap,2);
		return 0;
	}
	else if (bean.unittype() == eTRANSFORMER)
	{
		// ���Ϊ��ѹ������ֹ����·�ı���
		return 0;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����翹������ѹ�����κ�Ԫ��������A��ָ�����Ե翹����A����һ�����ӵ�������±�����ֱ������
		return 1;
	}

}