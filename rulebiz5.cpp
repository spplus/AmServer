#include "rulebiz5.h"
#include "rulebiz5-1.h"
#include "rulebiz5-2.h"

RuleBiz5::RuleBiz5()
{
	m_breakerCim = "";
	range = false;
}
bool RuleBiz5::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
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
				if (topoRst == 2)
				{
					return false;
				}
				else if (topoRst == 4)
				{
					// ���򱻴���
					return true;
				}
			}

			// ������һ����ʱ���Կ���Ϊ��ʼԪ������������һ�˵����ӵ㣬�Լ����ӵ��Ӧ�Ľ��Ԫ����������Ԫ�����������պϵĵ�բ�����������壬���򱻴���
			if (m_breakerCim.length() > 0)
			{
				RuleBiz5_1 r;
				r.setReq(m_req);
				r.topoByUnit(saveid,m_breakerCim,passNodes,ruleMap);
			}
		}
	
	}
	
	// �ж��Ƿ񴥷�����
	if (ruleMap.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER)
	{
		// ���Ϊ���أ�B�����ҿ���Ϊ�պϣ�ֱ���˳��߼���
		if (bean.state() == 0)
		{
			// �翪�أ�B��Ϊ�Ͽ�����������һ
			COM->triggerRule(ruleMap,1);	

		}
		else
		{
			// �ҿ���Ϊ�պϣ�ֱ���˳��߼���
			return 2;
		}
	}
	else if (bean.unittype() == eSWITCH )
	{
		/*
		�������صĽ��Ԫ�������������բ������ʼ�����Լ����ұպϣ�������������
		*/
		if (bean.state() == 1 && bean.cimid() != m_opcim)
		{
			COM->triggerRule(ruleMap,2);
		}

	}
	else if (bean.unittype() == eBUS)
	{
		// ���Ϊĸ�ߣ���������3
		COM->triggerRule(ruleMap,3);

	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������4����
		range = true;
	}

	// ��������������ʱ�����򱻴���
	if (ruleMap.size()<= 0)
	{
		return 4;
	}
	else
	{
		return 0;
	}
}