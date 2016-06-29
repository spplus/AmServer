#include "rulebiz21-1.h"

int RuleBiz21_1::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.������Ԫ��������բ�ҶϿ���������������
	if (bean.unittype() == eSWITCH )
	{
		// ���Ϊ�����ҶϿ�����������һ
		if (bean.state() == 0)
		{
			COM->triggerRule(ruleMap,3);
		}
	}

	// �����еݹ���������α����������˳�
	return 0;
}