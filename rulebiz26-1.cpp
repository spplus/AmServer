#include "rulebiz26-1.h"

int RuleBiz26_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ԫ��������բ�ұպϣ�������������
	if (bean.unittype() == eSWITCH && bean.state() == 1)
	{
		COM->triggerRule(ruleMap,3);
	}

	// �����еݹ���������α����������˳�
	return 0;
}