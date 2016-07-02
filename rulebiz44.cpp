#include "rulebiz44.h"

RuleBiz44::RuleBiz44()
{
	m_unitCim = "";
	m_switchCount = 0;
	m_switchState = true;
}

bool RuleBiz44::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				 topoBiz(saveid,unitId,ruleMap,"");
				
				// ��֧һ����������ĵ�բԪ�����պϻ��߰����ĵ�բ����һ����ֱ���˳��߼�
				 if (m_switchCount == 1 || (m_switchState && m_switchCount>0))
				 {
					 return false;
				 }

				 // ��֧�������Ԫ����ֻҪ��һ����բ�Ͽ�������һ������������ѯ���е�բ��һ������ӵ㣬�Լ����ӵ��Ӧ�Ľ��Ԫ�������ÿ����բ��Ӧ�Ľ��Ԫ��������ĸ�ߣ�������������
				 bool flag = true;
				 for (int m = 0;m<m_switchList.size();m++)
				 {
					 string cim = m_switchList.at(m);
					 flag &= topoByUnit(saveid,cim,passNodes,ruleMap);
				 }

				 // ���ÿ����բ��Ӧ�Ľ��Ԫ��������ĸ�ߣ�������������
				 if (flag && m_switchCount>0)
				 {
					 COM->triggerRule(ruleMap,2);
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


int RuleBiz44::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	if (bean.unittype() == eSWITCH)
	{
		// �ۼƵ�բ����
		m_switchCount++;

		// ���ε�բ��״̬
		bool state = bean.state()==1?true:false;

		// ��բ״̬����������������һ��Ϊfalse����Ϊfalse
		m_switchState =& state;

		// ���Ԫ����ֻҪ��һ����բ�Ͽ�������һ����
		if (state == false)
		{
			COM->triggerRule(ruleMap,1);
		}
		m_switchList.push_back(bean.cimid());
	}	
	
	return 0;
}