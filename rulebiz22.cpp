#include "rulebiz22.h"

int RuleBiz22::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ����Ԫ�����߶Ͽ��ĵ�բ�����أ���ֹ����·�ı���
	if (((bean.unittype() == eBREAKER 
		|| bean.unittype() == eSWITCH) 
		&& bean.state() == 0) || bean.iselectric() == 1 )
	{
		return 0;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		// ���Ϊ�ӵ�Ԫ��,��������һ
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