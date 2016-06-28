#include "rulebiz18.h"

int RuleBiz18::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.����Ԫ�����磬��������
	if (bean.iselectric() == 1)
	{
		COM->triggerRule(ruleMap,1);
		return 0;

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