#include "rulebiz28.h"

 RuleBiz28::RuleBiz28()
{
	m_isFirst = true;
}

void RuleBiz28::setBeginCim(string bcim)
{
	m_breakerCim = bcim;
}

bool RuleBiz28::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// �ѵ�ǰԪ�����뵽�ѷ����б�
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.��Ӧ�ĵ������ӵ㣨���ǲ����е��������ӵ㣩
	for (int i = 0 ;i<connIds.size();i++)
	{
		
		STRMAP connMap = connIds.at(i);
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

				// Ԫ������
				int unitType = COM->getIval(unitMap,"UnitType");

				// Ԫ��״̬
				int state = COM->getIval(unitMap,"State");

				// ���A�Ƿ�����ڽ���У��粻��������������
				if (unitId == m_breakerCim)
				{
					COM->triggerRule(ruleMap,1);
					return true;
				}
				else if (unitType == eTRANSFORMER || 
					((unitType == eBREAKER||unitType == eSWITCH)&& state == 0))
				{
					// ��ѹ�����Ͽ��Ŀ��غ͵�բΪ����·�������յ�
					continue;
				}
				else
				{
					// �ݹ��ѯ
					if (ruleMap.size()>0)
					{
						topoByUnit(saveid,unitId,passNodes,ruleMap);
					}
					else
					{
						return true;
					}
					
				}

			}
		}

		// �״����ˣ�ֻ���˵������ӵ�
		if (m_isFirst)
		{
			break;
		}

		// ���Ϊ��һ�����
		m_isFirst = false;

	}
	return false;
}
