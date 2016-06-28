#include "rulebiz5.h"
#include "rulebiz5-1.h"
#include "rulebiz5-2.h"

int RuleBiz5::topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */)
{
	PBNS::StateBean bean = getUnitByCim(saveid,unitcim);

	// 1.���Ϊ���ػ�բ�ұպϣ������������Ͽ�����ֹ
	if (bean.unittype() == eBREAKER)
	{
		if (bean.state() == 0)
		{
			// ���Ϊ���أ��ҿ���Ϊ�Ͽ�������������
			COM->triggerRule(ruleMap,2);
			
		}
	}
	else if (bean.unittype() == eSWITCH)
	{
		/*
		���Ϊ��բ��A�����Դ˵�բΪ��ʼԪ������������һ�˵����ӵ㣬
		�Լ����ӵ��Ӧ�Ľ��Ԫ����������Ԫ������ĸ�ߣ�����������
		*/
		R_ITERATOR iter3 = ruleMap.find(3);
		if (iter3 != ruleMap.end())
		{
			RuleBiz5_1 r51;
			STRMAP passedNodes;
			r51.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);
		}

		// �ж��Ƿ�����������
		iter3 = ruleMap.find(3);
		if (iter3 == ruleMap.end())
		{
			// ��֧һ����բ��A��Ϊ�պϣ����������ģ����򱻴�����
			if (bean.state() == 1)
			{
				COM->triggerRule(ruleMap,4);

				// Ϊ�˱��ڼ�����Ĵ����߽磬�ٴΰ�����5Ҳ����
				COM->triggerRule(ruleMap,5);

				return 0;
			}
			else
			{
				/*
				��֧������բ��A��Ϊ�Ͽ����������������Ԫ���еĿ��ز�ѯ��һ������ӵ㣬
				�ٲ�ѯ���ӵ��Ӧ�Ľ��Ԫ�����ϣ����ϲ�ѯ��UnitType����
				��Խ��Ԫ�����ϣ����Ϊ��բ���ҵ�բΪ�պϣ����������壬���򱻴�����
				*/
				RuleBiz5_2 r52;
				STRMAP passedNodes;
				r52.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);

				// �ж��Ƿ���������5
				R_ITERATOR iter5 = ruleMap.find(5);
				if (iter5 == ruleMap.end())
				{
					// ���򱻴�������������4
					COM->triggerRule(ruleMap,4);

					// ֹͣ����
					return 0;
				}
			}
		}

	}
	else if (bean.unittype() == eBUS)
	{
		// ���Ϊĸ�ߣ���������һ
		COM->triggerRule(ruleMap,1);

	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������һ����
		R_ITERATOR iter1 = ruleMap.find(1);
		if (iter1 != ruleMap.end())
		{
			ruleMap.erase(iter1);
		}
		
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