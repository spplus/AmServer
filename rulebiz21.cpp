#include "rulebiz21.h"

bool RuleBiz21::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// �ѵ�ǰԪ�����뵽�ѷ����б�
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.�������ӵ�ID�����ӹ�ϵ���ѯ�������豸
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

			// �������豸����
			for (int k = 0;k<unitsList.size();k++)
			{
				STRMAP  unitMap = unitsList.at(k);
				MAP_ITERATOR unitIter = unitMap.find("id");
				string unitId ;
				if (unitIter != unitMap.end())
				{
					// �ж��Ƿ��Ѿ���Ϊ��ʼ�豸���������������������
					if (passNodes.find(unitIter->second) != passNodes.end())
					{
						continue;
					}
				}

				// ���β�ѯ��Ԫ��CIMID
				unitId = unitIter->second;

				// �������˵�ҵ������������ʵ��
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
			
				// �ж��Ƿ�ֱ���˳�
				if (topoRst == 2)
				{
					return false;
				}

			}
			// ������ԭ��������ĸ�ߣ��������������������ĸ�ߣ���������һ��ѭ�������˳������ᵽ����
			COM->triggerRule(ruleMap,2);




		}

	}
	return false;
}


int RuleBiz21::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊĸ�ߣ���Υ������ֱ�������߼���
	if (bean.unittype() == eBUS )
	{
		return 2;
	}
	else if (bean.unittype() == eBREAKER)
	{
		// ���Ϊ�����ҶϿ�����������һ
		if (bean.state() == 0)
		{
			COM->triggerRule(ruleMap,1);
		}
	}
	return 1;
}