#include "rulebiz21.h"

int RuleBiz21::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊĸ�ߣ���Υ������ֱ�������߼���
	if (bean.unittype() == eBUS )
	{
		return 2;
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