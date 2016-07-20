#include "rulebiz4.h"


bool RuleBiz4::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
			// �ж��Ƿ��Ѿ����ҹ������ӵ㣬����������������������
			if (passNodes.find(connIter->second) != passNodes.end())
			{
				continue;
			}
			else
			{
				passNodes.insert(MAPVAL(connIter->second,connIter->second));
			}

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
					}
				}

				// ���β�ѯ��Ԫ��CIMID
				unitId = unitIter->second;

				// �������˵�ҵ������������ʵ��
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
				bool ret = false;

				if (topoRst == eRuleRecursion)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					ret = topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
				// �ж��Ƿ�ֱ���˳�
				else if (topoRst == eRuleExit)
				{
					return false;
				}
				// ���������ӵ��µ������豸
				else if (topoRst == eRuleBreak)
				{
					break;
				}
				
			}

		}
	}

	// ������һ�����������£��������Σ�ʱ�����򱻴�����
	if (ruleMap.size() == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


int RuleBiz4::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER || bean.unittype() == eSWITCH)
	{
		if (bean.state() == 1)
		{
			return eRuleRecursion;
		}
		else
		{
			return eRuleNotRecursion;
		}

	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������һ����
		R_ITERATOR iter = ruleMap.find(1);
		if (iter == ruleMap.end())
		{
			COM->triggerRule(ruleMap,2);

			return eRuleExit;
		}
		else
		{
			COM->triggerRule(ruleMap,1);

			// ���������ӵ�ʣ���豸
			return eRuleBreak;
		}
	}
}