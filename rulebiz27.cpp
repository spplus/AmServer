#include "rulebiz27.h"
#include "rulebiz27-1.h"

RuleBiz27::RuleBiz27()
{
	m_breakerCim = "";
}

bool RuleBiz27::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());

			}
			
			// ����һ��������ʱ���Կ���Ϊ��ʼԪ������������һ�˵����ӵ�
			R_ITERATOR iter1 = ruleMap.find(1);
			R_ITERATOR iter2 = ruleMap.find(2);
			if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
			{
				if (m_breakerCim.length()>0)
				{
					RuleBiz27_1 r;
					STRMAP passedNodes;
					r.topoByUnit(saveid,m_breakerCim,passedNodes,ruleMap);
				}
			}
			
			// ����һ��������ͬʱ����ʱ��������
			if (ruleMap.size() == 0)
			{
				return true;
			}
			else
			{
				return false;
			}

		}

	}
	return false;
}


int RuleBiz27::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ�����ҶϿ�����������һ��
	if (bean.unittype() == eBREAKER && bean.state() == 0)
	{
		COM->triggerRule(ruleMap,1);
		m_breakerCim = bean.cimid();
	}
	else if (bean.unittype() == eBUS)
	{
		// ����������ĸ�ߣ�����������
		COM->triggerRule(ruleMap,2);
	}
	
	return 0;
}