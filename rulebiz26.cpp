#include "rulebiz26.h"
#include "rulebiz26-1.h"

RuleBiz26::RuleBiz26()
{
	m_breakerCim = "";
}

bool RuleBiz26::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
			
			// �Կ���Ϊ��ʼԪ������������һ�˵����ӵ㣬�Լ����ӵ��Ӧ�Ľ��Ԫ����������Ԫ��������բ�ұպϣ�������������
			if (m_breakerCim.length()>0)
			{
				RuleBiz26_1 r;
				r.setReq(m_req);
				r.topoByUnit(saveid,m_breakerCim,passNodes,ruleMap);

			}

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


int RuleBiz26::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.����������أ����۶Ͽ��պϣ���������������
	if (bean.unittype() == eBREAKER)
	{
		COM->triggerRule(ruleMap,2);
		m_breakerCim = bean.cimid();
	}
	else if (bean.unittype() == eBUS)
	{
		// ����������ĸ�ߣ���������һ
		COM->triggerRule(ruleMap,1);
	}
	
	return 0;
}