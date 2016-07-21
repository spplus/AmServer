#include "rulebiz5.h"
#include "rulebiz5-1.h"
#include "rulebiz5-2.h"

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

			return 0;
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

		/*R_ITERATOR iter3 = ruleMap.find(3);
		if (iter3 != ruleMap.end())
		{
			RuleBiz5_1 r51;
			STRMAP passedNodes;
			r51.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);
		}*/

		//// �ж��Ƿ�����������
		//iter3 = ruleMap.find(3);
		//if (iter3 == ruleMap.end())
		//{
		//	// ��֧һ����բ��A��Ϊ�պϣ����������ģ����򱻴�����
		//	if (bean.state() == 1)
		//	{
		//		COM->triggerRule(ruleMap,4);

		//		// Ϊ�˱��ڼ�����Ĵ����߽磬�ٴΰ�����5Ҳ����
		//		COM->triggerRule(ruleMap,5);

		//		return 0;
		//	}
		//	else
		//	{
		//		/*
		//		��֧������բ��A��Ϊ�Ͽ����������������Ԫ���еĿ��ز�ѯ��һ������ӵ㣬
		//		�ٲ�ѯ���ӵ��Ӧ�Ľ��Ԫ�����ϣ����ϲ�ѯ��UnitType����
		//		��Խ��Ԫ�����ϣ����Ϊ��բ���ҵ�բΪ�պϣ����������壬���򱻴�����
		//		*/
		//		RuleBiz5_2 r52;
		//		STRMAP passedNodes;
		//		r52.topoByUnit(saveid,bean.cimid(),passedNodes,ruleMap);

		//		// �ж��Ƿ���������5
		//		R_ITERATOR iter5 = ruleMap.find(5);
		//		if (iter5 == ruleMap.end())
		//		{
		//			// ���򱻴�������������4
		//			COM->triggerRule(ruleMap,4);

		//			// ֹͣ����
		//			return 0;
		//		}
		//	}
		//}

	}
	else if (bean.unittype() == eBUS)
	{
		// ���Ϊĸ�ߣ���������3
		COM->triggerRule(ruleMap,3);

		// ���򱻴���
		return 4;
	}
	else
	{
		// ���Ϊ�ǿ��ء���բ���ص����κ�Ԫ��������4����
		COM->triggerRule(ruleMap,4);

		// �������أ�B����һ������ӵ�
		RuleBiz5_1 r51;
		r51.setReq(m_req);
		r51.setPassedNodes(m_passedNodes);

		bool ret = r51.topoByUnit(saveid,bean.cimid(),m_passedNodes,ruleMap);
		
		// ������������պϵĵ�բ������򴥷�
		if (ret)
		{
			return 4;
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