#include "rulebiz25.h"
#include "rulebiz25-1.h"

RuleBiz25::RuleBiz25()
{
	m_hasSwitch = false;
	m_hasSwitchState = 1;
	m_optype = 1;
	m_hasBus = false;
	m_switchCim = "";
}
void RuleBiz25::setOptype(int optype)
{
	m_optype = optype;
}
bool RuleBiz25::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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

				// �ж��Ƿ��Ǳ��β������豸������ǣ�������
				if (unitcim == unitId)
				{
					continue;
				}

				// �������˵�ҵ������������ʵ��
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
			
				// �ж��Ƿ�ֱ���˳�
				if (topoRst == eRuleExit)
				{
					return false;
				}

			}
			// �����������բ������������
			if (!m_hasSwitch)
			{
				COM->triggerRule(ruleMap,2);
			}
			
			R_ITERATOR iter1 = ruleMap.find(1);
			R_ITERATOR iter2 = ruleMap.find(2);

			// ����һ��ͬʱ����ʱ����������
			if (iter1 == ruleMap.end() && iter2 == ruleMap.end())
			{
				return true;
			}
			
			// ���������բ�������֧�߼�
			if (m_hasSwitch)
			{
				// ��ʼ����Ϊ�ͻ��˶Ͽ���բ������ֱ������
				if (m_optype == 0)
				{
					return false;
				}

				// ��������ĵ�բΪ�Ͽ�������������
				if (m_hasSwitchState == 0)
				{
					COM->triggerRule(ruleMap,3);
				}

				//  ���������жϿ���բΪ��ʼԪ������������һ�˵����ӵ㣬�Լ����ӵ��Ӧ�Ľ��Ԫ����������Ԫ������ĸ�ߣ����������壻
				RuleBiz25_1 r;
				r.setReq(m_req);
				r.topoByUnit(saveid,m_switchCim,passNodes,ruleMap);

			}

			// ������һ���������������ġ�����������ʱ��������
			iter2 = ruleMap.find(2);
			if (ruleMap.size() == 4 && iter2 != ruleMap.end())
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


int RuleBiz25::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.������������ұպϣ���������һ������ֱ�������߼���
	if (bean.unittype() == eSWITCH)
	{
		m_hasSwitch = true;
		
		m_hasSwitchState = bean.state();
		m_switchCim = bean.cimid();
	}
	else if (bean.unittype() == eBUS)
	{
		m_hasBus = true;

		// ����������ĸ�ߣ�����������
		COM->triggerRule(ruleMap,4);
	}
	else if (bean.unittype() == eBREAKER)
	{
		// ������������ұպϣ���������һ������ֱ�������߼���
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);
		}
		else
		{
			return eRuleExit;
		}
	}
	return 0;
}