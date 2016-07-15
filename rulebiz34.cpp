#include "rulebiz34.h"
#include "rulebiz34-1.h"
RuleBiz34::RuleBiz34()
{
	m_transCim = "";
}

bool RuleBiz34::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

				if (topoBiz(saveid,unitId,ruleMap,""))
				{
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
				
				// ����һ��������ʱ˵����ѹ�������ͨ����ʱ������ѯ��ѹ����һ������ӵ㣬�Լ����ӵ��Ӧ�Ľ��Ԫ����������Ԫ��Ϊ�ص�Ϊ�Ͽ�������������
				// �ж�����һ���������Ƿ����

				R_ITERATOR iter1 = ruleMap.find(1);
				R_ITERATOR iter2 = ruleMap.find(2);
				if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
				{
					RuleBiz34_1 r;
					r.topoByUnit(saveid,m_transCim,passNodes,ruleMap);
				}

				// ����������ʱ�����򱻴���
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


int RuleBiz34::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
	{
		// ���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}	
	else if (bean.unittype() == eTRANSFORMER)
	{
		// ���Ϊ��ѹ������������һ
		COM->triggerRule(ruleMap,1);
		m_transCim = bean.cimid();
		return 0;
	}
	else if (bean.unittype() == eGROUNDSWITCH)
	{
		return 0;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص�����ѹ�����κ�һ��Ԫ�����ͣ�����������
		COM->triggerRule(ruleMap,2);
		return 0;
	}

}