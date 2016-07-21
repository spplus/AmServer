#include "rulebiz25.h"
#include "rulebiz25-1.h"

RuleBiz25::RuleBiz25()
{
	m_optype = 1;
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
				int topoRst = topoBiz(saveid,unitId,ruleMap,connIter->second);
			
				// �ж��Ƿ�ֱ���˳�
				if (topoRst == eRuleExit)
				{
					return false;
				}
			}

		}

	}
	// �ж���������1�Ŀ������ڵ����ӵ�����û�е�բ
	if (m_breakList.size() > 0)
	{
		PBNS::StateBean breakBean = m_breakList.at(0);

		// �����������բ����������������������
		if (m_switchList.size() == 0)
		{
			return true;
		}


		// ����Ƿ��Ҵ�ͬһ���ӵ��µĵ�բ
		int	flag = -1;
		// ���������բ�����ж��Ƿ�����������1�Ŀ�����ͬһ�����ӵ�
		for (int i = 0;i<m_switchList.size();i++)
		{
			PBNS::StateBean switchBean = m_switchList.at(i);
			if (switchBean.stationcim() == breakBean.stationcim())
			{
				flag = i;
				break;
			}
		}
		// ���û�ҵ�������������������������
		if (flag == -1)
		{
			return true;
		}
		else
		{
			PBNS::StateBean sbean = m_switchList.at(flag);
			// �����բ״̬Ϊ�Ͽ���������������
			if (m_optype == 0 && sbean.state() == 0)
			{
				COM->triggerRule(ruleMap,3);
				return true;
			}
		}
	}

	return false;
}


int RuleBiz25::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string conncim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);
	
	// �������ӵ�cim
	bean.set_stationcim(conncim);

	// 1.������������ұպϣ���������һ������ֱ�������߼���
	if (bean.unittype() == eSWITCH && bean.cimid() != m_opcim)
	{
		m_switchList.push_back(bean);
	}
	else if (bean.unittype() == eBREAKER)
	{
		// ������������ұպϣ���������һ������ֱ�������߼���
		if (bean.state() == 1)
		{
			COM->triggerRule(ruleMap,1);
			m_breakList.push_back(bean);
		}
		else
		{
			return eRuleExit;
		}
	}
	return 0;
}