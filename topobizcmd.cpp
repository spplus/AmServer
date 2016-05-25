#include "topobizcmd.h"

void TopoBizCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TOPO_ENTIRE:		// ��վ����
		topoEntire();
		break;
	}
}

void TopoBizCmd::topoEntire()
{
	// �Ѿ��������������豸ID����
	STRMAP passedNodes;


	// 1.��ѯ���з������豸
	string sql ;
	char * p = "select Id,StationId from units where UnitType=5;";
	LISTMAP	 powerList;
	MAP_ITERATOR iter;
	powerList = App_Dba::instance()->getList(p);
	
	for (int i = 0;i<powerList.size();i++)
	{
		STRMAP power = powerList.at(i);
		iter = power.find("Id");
		string powerid ,stationid;
		if (iter != power.end())
		{
			powerid = iter->second;

		}
		iter = power.find("StationId");
		if (iter != power.end())
		{
			stationid = iter->second;
		}
		// �ѵ�ǰԪ�����뵽�ѷ����б�
		passedNodes.insert(MAPVAL(powerid,powerid));

		// 2.���ݷ�����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
		LISTMAP connIds = getConnIdByUnitsId(powerid);

		// 3.�������ӵ�ID�����ӹ�ϵ���ѯ�������豸
		
		for (int j = 0;j<connIds.size();j++)
		{

			// �ж��Ƿ��Ѿ���Ϊ��ʼ�豸���������������������
		

		}

	}
	


	

	// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻

	// 5.������豸���ǿ����豸��������Ϊ���磻

	// 6.������豸Ϊ���أ���Ϊ�Ͽ��������ٱ������豸�Ĺ����豸��

	// 7.�ظ�����2

	// 8.����ôα��������豸վ��ID����ʼ�豸��վ��ID����ͬ���Ҹ��豸Ϊ�����ߣ����Ǹý�����Ϊ��Ե�Դ�㣻

	// 9.�Ѿ���Ϊ��ʼ����豸��������Ϊ��ʼ����б���
}

LISTMAP TopoBizCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	return connList;
}


LISTMAP TopoBizCmd::getUnitsByConnId(string connid)
{
	LISTMAP unitsList ;

	return unitsList;
}