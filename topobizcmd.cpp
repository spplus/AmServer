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
	char * p = "select Id,StationId from units where UnitType=5";
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
		
		// 根据元件进行拓扑
		topoByUnitId(powerid,stationid,passedNodes);

	}
	

	// 7.重复步骤2

	// 8.如果该次遍历出的设备站点ID与起始设备的站点ID不相同，且该设备为进出线，则标记该进出线为相对电源点；

	// 9.已经作为起始点的设备，不再作为起始点进行遍历
}

LISTMAP TopoBizCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select connId from Relations where UnitId=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid);
	connList = App_Dba::instance()->getList(sql.c_str());
	return connList;
}


LISTMAP TopoBizCmd::getUnitsByConnId(string connid)
{
	LISTMAP unitsList ;
	char* psql = "select b.id,b.UnitType,b.StationId from Relations a left join Units b on a.UnitId=b.id  where a.ConnId=%s";
	string sql = App_Dba::instance()->formatSql(psql,connid);
	unitsList = App_Dba::instance()->getList(sql.c_str());
	return unitsList;
}

void TopoBizCmd::topoByUnitId(string unitid,string stationid,STRMAP& passNodes)
{
	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitid,unitid));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitid);

	// 3.根据连接点ID在连接关系表查询关联的设备
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// 根据连接点，查找该连接点关联的设备集合
			LISTMAP unitsList = getUnitsByConnId(connIter->second);

			// 遍历该设备集合
			for (int k = 0;k<unitsList.size();k++)
			{
				STRMAP  unitMap = unitsList.at(k);
				MAP_ITERATOR unitIter = unitMap.find("id");
				string unitId ;
				if (unitIter != unitMap.end())
				{
					// 判断是否已经做为起始设备进行搜索，如果是则跳过
					if (passNodes.find(unitIter->second) != passNodes.end())
					{
						continue;
					}
				}

				// 本次查询的元件ID
				unitId = unitIter->second;

				// 查询元件类型
				unitIter = unitMap.find("UnitType");

				int etype ;
				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);
					if (etype == eBreaker || etype == eSwitch)
					{
						// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 1)
							{
								// 更新该设备带电状态为带电
								updateIsElectricByUnitId(unitId,1);
							}
							else
							{
								// 6.如果该设备为开关，且为断开，则不用再遍历该设备的关联设备；
								updateIsElectricByUnitId(unitId,0);

							}
						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为带电；
						updateIsElectricByUnitId(unitId,1);
					}
				}

				// 判断是否为跨站点
				unitIter = unitMap.find("StationId");
				if (unitIter != unitMap.end())
				{

					// 如果该次遍历出的设备站点ID与起始设备的站点ID不相同，且该设备为进出线，则标记该进出线为相对电源点；
					if (str2i(unitIter->second) != str2i(stationid))
					{
						if (etype == eLine)
						{
							updateIsPowerByUnitId(unitid);
						}
					}
				}

				// 站点ID
				string sId = unitIter->second;

				// 递归，以该元件为起点进行重新遍历
				topoByUnitId(unitid,sId,passNodes);
			}

		}

	}
}

void TopoBizCmd::updateIsPowerByUnitId(string unitid)
{
	char* psql = "update UnitStatus set IsPower=1 where UnitId=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid);
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret>0)
	{
		LOG->message("update :%s as power success.",unitid.c_str());
	}
	else
	{
		LOG->message("update :%s as power failed.",unitid.c_str());
	}
	
}

void TopoBizCmd::updateIsElectricByUnitId(string unitid,int state)
{
	char* psql = "update UnitStatus set IsElectric=%d where UnitId=%s";
	string sql = App_Dba::instance()->formatSql(psql,state,unitid);
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret>0)
	{
		LOG->message("update :%s to electric  success.",unitid.c_str());
	}
	else
	{
		LOG->message("update :%s to electric failed.",unitid.c_str());
	}
}