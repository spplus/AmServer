#include "topobizcmd.h"

void TopoBizCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TOPO_ENTIRE:		// 整站拓扑
		topoEntire();
		break;
	}
}

void TopoBizCmd::topoEntire()
{
	// 已经做个起点分析的设备ID集合
	STRMAP passedNodes;


	// 1.查询所有发动机设备
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
		// 把当前元件加入到已分析列表
		passedNodes.insert(MAPVAL(powerid,powerid));

		// 2.根据发动机元件ID，查找对应的连接点（可能是两个）
		LISTMAP connIds = getConnIdByUnitsId(powerid);

		// 3.根据连接点ID在连接关系表查询关联的设备
		
		for (int j = 0;j<connIds.size();j++)
		{

			// 判断是否已经做为起始设备进行搜索，如果是则跳过
		

		}

	}
	


	

	// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；

	// 5.如果该设备不是开关设备，则设置为带电；

	// 6.如果该设备为开关，且为断开，则不用再遍历该设备的关联设备；

	// 7.重复步骤2

	// 8.如果该次遍历出的设备站点ID与起始设备的站点ID不相同，且该设备为进出线，则标记该进出线为相对电源点；

	// 9.已经作为起始点的设备，不再作为起始点进行遍历
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