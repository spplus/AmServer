#include "rulebiz12.h"

RuleBiz12::RuleBiz12()
{
	m_hasBus = false;
	m_hasSwitch = false;
}

bool RuleBiz12::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				else if (topoRst == eRuleTriggered)
				{
					// ���򱻴���
					return true;
				}

				// �ж��Ƿ���򴥷�
				if (ruleMap.size() == 0)
				{
					return true;
				}
				else
				{
					return ret;
				}
			}

			// ������ϲ�����ĸ�ߣ�ֱ�Ӵ�������
			
			if (!m_hasBus)
			{
				return true;
			}

		}

	}
	return false;
}


int RuleBiz12::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	// 1.���Ϊ���أ��ҶϿ�����ֱ���˳��߼�
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			return eRuleExit;
		}
		else
		{
			// �պϣ�����������1
			COM->triggerRule(ruleMap,1);

			// ����������
			return eRuleTriggered;
		}
	}
	// 1.���Ϊ��բ�ұպϣ������������Ͽ�����ֹ
	else if (bean.unittype() == eSWITCH )
	{
		if (bean.state() == 1 && bean.cimid() == m_opcim)
		{
			m_hasSwitch = true;
		}
		return eRuleNotRecursion;

	}
	else if (bean.unittype() == eBUS)
	{
		// ������ϰ���ĸ�ߣ��жϼ��ϣ�A���Ƿ������բ�ұպϣ�����ʼ�����Լ���,���������Υ������У��ͨ��
		m_hasBus = true;
		if (m_hasSwitch)
		{
			return eRuleExit;
		}
		else
		{
			// ������򱻴�����
			return eRuleTriggered;
		}
		
	}
	else
	{
		return eRuleNotRecursion;
	}
	
}