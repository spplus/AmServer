#include "rulebiz20.h"

int RuleBiz20::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ��բ�򿪹��ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eSWITCH || bean.unittype() == eBREAKER)
	{
		if (bean.state() == 1)
		{
			return 1;
		}
		else
		{
			return 0;
		}

	}
	else if (bean.unittype() == eLINE && bean.ispower() == 1)
	{
		// ���Ϊ��������Ϊ��Ե�Դ��,��������һ
		R_ITERATOR iter = ruleMap.find(1);
		if (iter != ruleMap.end())
		{
			COM->triggerRule(ruleMap,1);

			// ���汾�����˵���ʼԪ����վ��CIM
			m_stationCim.push_back(stationcim);
		}
		else
		{
			
			// ����ٴα�������������Ϊ��Ե�Դ�㣬���Ҹôα�����ʼԪ����Ӧ��վ��cim��֮ǰ��¼�Ĳ�ͬ��������������
			if (m_stationCim.size()>0)
			{
				if(m_stationCim.at(0) != stationcim)
				{
					COM->triggerRule(ruleMap,2);
					return 0;
				}
				else
				{
					return 1;
				}
			}
		}
		
		return 0;
	}
	
	// �ж������Ƿ�ȫ��������������򷵻أ����򴥷�
	if (ruleMap.size()>0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}