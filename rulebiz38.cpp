#include "rulebiz38.h"
#include "rulebiz38-1.h"
RuleBiz38::RuleBiz38()
{
	m_unitCim = "";
}

bool RuleBiz38::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				int ret = topoBiz(saveid,unitId,ruleMap,"");
				if (ret ==1)
				{
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
				else if (ret == 2)
				{
					return false;
				}
				
				
				//����һ��������ʱ˵����ѹ�������ͨ����һ�߽�Ϊĸ�ߣ���������ѯĸ��������ͨ����Ƿ��б�ѹ������ѯĸ����һ������ӵ�
				// �ж�����һ���������Ƿ����

				R_ITERATOR iter1 = ruleMap.find(1);
				R_ITERATOR iter2 = ruleMap.find(2);
				if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
				{
					RuleBiz38_1 r;
					r.topoByUnit(saveid,m_unitCim,passNodes,ruleMap);
				}

				// ���򱻴���
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
	}
	return false;
}


int RuleBiz38::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eBREAKER)
	{
		// ���Ϊ�����ҶϿ���ֱ���˳��߼�����Υ������
		if (bean.state() == 0)
		{
			return 2;
		}
		else
		{
			// ���Ϊ�����ұպϣ�����һ����
			COM->triggerRule(ruleMap,1);
			m_unitCim = bean.cimid();
			return 0;
		}

	}	
	else if (bean.unittype() == eBUS)
	{
		// ���Ϊĸ�ߣ�����������
		COM->triggerRule(ruleMap,2);
		
		return 0;
	}
	return 0;
}