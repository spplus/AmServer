#include "rulebiz27-1.h"

int RuleBiz27_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.������Ԫ��������բ�ұպϣ�������������
	if (bean.unittype() == eSWITCH && bean.state() == 1)
	{
		COM->triggerRule(ruleMap,3);
	}

	// �����еݹ���������α����������˳�
	return 0;
}