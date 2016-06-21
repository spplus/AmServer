#include "topobizcmd.h"

void TopoBizCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TOPO_ENTIRE:		// 整站拓扑
		topoEntire();
		break;
	case CMD_TOPO_BREAKER_CHANGE:		// 开关变位
		topoOnBreakerChange(msg);
		break;
	}
}

void TopoBizCmd::topoBySaveId(string saveid,int unittype)
{
	// 已经做个起点分析的设备ID集合
	STRMAP passedNodes;

	// 1.查询所有发动机设备
	string sql ;
	char * p = "select Id,StationCim as StationId from units where UnitType=%d";
	sql = App_Dba::instance()->formatSql(p,unittype);

	LISTMAP	 powerList;
	MAP_ITERATOR iter;
	powerList = App_Dba::instance()->getList(sql.c_str());

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

		if (unittype == 5)
		{
			// 根据元件进行拓扑带电状态
			topoByUnitId(saveid,powerid,stationid,passedNodes);
		}
		else if (unittype == 10)
		{
			// 拓扑接地
			topoByGround(saveid,powerid,stationid,passedNodes);
		}
		
	}

}

void TopoBizCmd::topoEntire()
{
	// 查询所有断面
	char* psql = "select id from virtual_saves ";
	LISTMAP saveList = App_Dba::instance()->getList(psql);
	for (int i = 0;i<saveList.size();i++)
	{
		STRMAP saveMap = saveList.at(i);
		MAP_ITERATOR iter = saveMap.find("id");
		if(iter != saveMap.end())
		{
			// 拓扑带电状态
			topoBySaveId(iter->second,5);

			// 拓扑接地状态
			topoBySaveId(iter->second,10);
		}
	}
}

LISTMAP TopoBizCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str());
	connList = App_Dba::instance()->getList(sql.c_str());
	return connList;
}

LISTMAP TopoBizCmd::getStationIdByLineId(string unitid,string stationid)
{
	LISTMAP stationList;
	char* psql = "select StationCim from related_line where UnitCim=%s and StationCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str(),stationid.c_str());
	stationList = App_Dba::instance()->getList(sql.c_str());
	return stationList;
}


LISTMAP TopoBizCmd::getUnitsByConnId(string connid,string saveid)
{

	// 问题：关联查询设备状态的时候，不用考虑saveid么？unit_status表中，同一个unit可能会有多条记录，以哪天记录为准呢？
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,c.State,d.VolValue " \
		"from Relations a left join Units b on a.UnitCim=b.CimId  "\
		"left join unit_status c on b.UnitCim=b.cimid " \
		"left join voltages d on d.CimId=b.VolCim " \
		"where a.ConnCim=%s and c.saveid=%s";
	string sql = App_Dba::instance()->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = App_Dba::instance()->getList(sql.c_str());
	return unitsList;
}

void TopoBizCmd::topoByUnitId(string saveid,string unitid,string stationid,STRMAP& passNodes)
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
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

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

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 查询元件类型
				unitIter = unitMap.find("UnitType");

				// 设备类型
				int etype ;

				// 标记时候需要拓扑 0 需要拓扑，1 不需要拓扑
				int flag = 0;

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
								updateIsElectricByUnitId(saveid,unitId,1);
							}
							else
							{
								// 6.如果该设备为开关，且为断开，则不用再遍历该设备的关联设备；
								updateIsElectricByUnitId(saveid,unitId,0);

								// 标记不需要拓扑
								flag = 1;
							}
						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为带电；
						updateIsElectricByUnitId(saveid,unitId,1);
					}
				}

				// 如果当前设备为进出线，则到进出线关联关系表中，查出进出线关联的另一端站点ID，更新状态表中进出线在该站点为相对电源点
				if (etype == eLine)
				{
					LISTMAP stationList = getStationIdByLineId(unitId,stationid);
					if (stationList.size()>0)
					{
						STRMAP stationMap = stationList.at(0);
						unitIter = stationMap.find("StationCim");
						if (unitIter != stationMap.end())
						{
							updateIsPowerByUnitId(unitId,unitIter->second,saveid);
						}
					}

				}

				// 判断是否为跨站点
				unitIter = unitMap.find("StationId");
				//if (unitIter != unitMap.end())
				//{

				//	// 如果该次遍历出的设备站点ID与起始设备的站点ID不相同，且该设备为进出线，则标记该进出线为相对电源点；
				//	if (str2i(unitIter->second) != str2i(stationid))
				//	{
				//		if (etype == eLine)
				//		{
				//			updateIsPowerByUnitId(unitid);
				//		}
				//	}
				//}

				string sId;
				if (unitIter != unitMap.end())
				{
					// 站点ID
					sId = unitIter->second;
				}
				
				if (flag != 1)
				{
					// 递归，以该元件为起点进行重新遍历
					topoByUnitId(saveid,unitid,sId,passNodes);
				}
				
			}

		}

	}
}


void TopoBizCmd::topoByGround(string saveid,string unitid,string stationid,STRMAP& passNodes)
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
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

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

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 查询元件类型
				unitIter = unitMap.find("UnitType");

				// 设备类型
				int etype ;

				// 标记时候需要拓扑 0 需要拓扑，1 不需要拓扑
				int flag = 0;

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
								updateIsGroundByUnitId(saveid,unitId,1);
							}
							else
							{
								// 6.如果该设备为开关，且为断开，则不用再遍历该设备的关联设备；
								updateIsGroundByUnitId(saveid,unitId,0);

								// 标记不需要拓扑
								flag = 1;
							}
						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为接地；
						updateIsGroundByUnitId(saveid,unitId,1);
					}
				}

				unitIter = unitMap.find("StationId");
				
				string sId;
				if (unitIter != unitMap.end())
				{
					// 站点ID
					sId = unitIter->second;
				}

				if (flag != 1)
				{
					// 递归，以该元件为起点进行重新遍历
					topoByGround(saveid,unitid,sId,passNodes);
				}

			}

		}

	}
}




void TopoBizCmd::updateIsPowerByUnitId(string unitid,string stationid,string saveid)
{
	char* psql = "update unit_status set IsPower=1 where UnitCim=%s and StationCim=%s and SaveId=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str(),stationid.c_str(),saveid.c_str());
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

void TopoBizCmd::updateIsElectricByUnitId(string saveid,string unitid,int state)
{
	char* psql = "update unit_status set IsElectric=%d where UnitCim=%s and saveid=%s";
	string sql = App_Dba::instance()->formatSql(psql,state,unitid.c_str(),saveid.c_str());
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

void TopoBizCmd::updateIsGroundByUnitId(string saveid,string unitid,int state)
{
	char* psql = "update unit_status set IsGround=%d where UnitCim=%s and saveid=%s";
	string sql = App_Dba::instance()->formatSql(psql,state,unitid.c_str(),saveid.c_str());
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret>0)
	{
		LOG->message("update :%s to IsGround  success.",unitid.c_str());
	}
	else
	{
		LOG->message("update :%s to IsGround failed.",unitid.c_str());
	}
}



void TopoBizCmd::topoOnBreakerChange(sClientMsg *msg)
{
	/*
	输入参数：	1.存档ID
						2.元件CIMID
	运算结果：
					找出本站设备在此次开关变位后的的带电状态.
	*/
	// 保存拓扑分析的结果，即设备的带电状态
	PBNS::DevStateMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);
	
	int saveId = req.saveid();
	string cimid = req.unitcim();


	// 设备的cimid,带电状态 1带电，0不带电
	vector<PBNS::StateBean>	rsltMap;

	// 已经做个起点分析的设备ID集合
	STRMAP passedNodes;

	// 1. 查询指定saveid，指定unit对应站点下面的设备状态集合；
	char * psql = "select a.UnitCim,a.StationCim,IsElectric,IsPower,VolValue " \
		"from unit_status a " \
		"left join units b on a.UnitCim=b.CimId " \
		"left join voltages c on c.CimId = b.VolCim " \
		"where a.SaveId=%d and a.StationCim in (select d.StationCim from units d where CimId=%s)";
	string sql = App_Dba::instance()->formatSql(psql,saveId,cimid.c_str());

	LISTMAP unitList = App_Dba::instance()->getList(sql.c_str());

	// 2.对以上结果中的ispower的值等于1或者2的记录作为起点进行拓扑分析。
	for (int i = 0;i<unitList.size();i++)
	{
		STRMAP unitMap = unitList.at(i);
		MAP_ITERATOR iter = unitMap.find("IsPower");

		// 相对电源点或绝对电源电记录
		if (iter != unitMap.end() && (str2i(iter->second) == 1 || str2i(iter->second) == 2))
		{
			iter = unitMap.find("UnitCim");
			if (iter != unitMap.end())
			{
				PBNS::StateBean bean;
				bean.set_cimid(iter->second);
				iter = unitMap.find("VolValue");
				if(iter != unitMap.end())
				{
					bean.set_iselectric(str2i(iter->second));
				}
				char temp[16];
				ACE_OS::itoa(req.saveid(),temp,10);
				string saveid;
				saveid.append(temp);

				// 以该设备为起点进行拓扑分析
				topoByUnitIdMem(bean,saveid,passedNodes,rsltMap);
			}
		}
	}

	// 拓扑结果返回客户端
	PBNS::DevStateMsg_Response res;
	vector<PBNS::StateBean>::iterator iter = rsltMap.begin();
	for (int i=0;i<rsltMap.size();i++)
	{
		PBNS::StateBean* bean = res.add_devstate();
		// 查询该设备的电压等级
		bean->CopyFrom(rsltMap.at(i));
	}
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}


void TopoBizCmd::topoByUnitIdMem(PBNS::StateBean bean,string saveid,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap)
{
	string unitid = bean.cimid();
	
	LOG->debug("topoByUnitIdMem,uinit cimid:%s",unitid.c_str());

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
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

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

				PBNS::StateBean cbean;
				cbean.set_cimid(unitIter->second);

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 查询元件类型
				unitIter = unitMap.find("UnitType");

				// 设备类型
				int etype ;

				// 标志是否需要按此设备为起点进行拓扑。如果该设备为开关，且未断开状态，则不用以该设备为起点进行拓扑
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);

					// 取电压等级值
					unitIter = unitMap.find("	VolValue");
					if (unitIter != unitMap.end())
					{
						cbean.set_volvalue(unitIter->second);
					}

					if (etype == eBreaker || etype == eSwitch)
					{
						// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 0)
							{
								//  把该开关设备状态变为1，继续以该设备为起点进行拓扑，如果为0，则跳过这个设备
								rsltMap.push_back(cbean);

							}
							else
							{
								// 标记为不需要进行拓扑
								flag = 1;
							}
						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为带电；
						rsltMap.push_back(cbean);
					}
				}

				// 过滤不需要进行拓扑的设备
				if (flag != 1)
				{
					// 递归，以该元件为起点进行重新遍历
					topoByUnitIdMem(cbean,saveid,passNodes,rsltMap);
				}
			
			}

		}

	}
}
