#include "querymgrcmd.h"


void QueryMgrCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_QUERY_CIRCLE_LIST:
		{
			m_isFirst = true;
			m_buslist.clear();
			queryCricleList(msg);
		}
		break;
	case CMD_QUERY_SIGN_LIST:
		querySignList(msg);
		break;
	case CMD_QUERY_GSWITCH_LIST:
		queryGSwitchList(msg);
		break;
	case CMD_QUERY_MSET_LIST:
		queryMsetList(msg);
		break;
	case CMD_QUERY_EVENT_LIST:
		queryEventList(msg);
		break;
	}

}


void QueryMgrCmd::queryCricleList(sClientMsg* msg)
{
	PBNS::CircleListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int nSvid = req.saveid();
	std::string saveid = COM->i2str(nSvid);
	
	//开始环路查询拓扑
	cricleTopo(nSvid);


	PBNS::CircleListMsg_Response resp;

	//组合母线查询条件
	string buss;
	for (int i = 0;i<m_buslist.size();i++)
	{
		buss = "'" + m_buslist.at(i) + "'";
		buss += ",";
	}
	if (buss.length()>2)
	{
		buss = buss.substr(0,buss.length()-1);
	}
	else
	{
		// 返回到客户端
		string data;
		resp.SerializeToString(&data);
		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

		return;
	}

	string sql;
	//查询母线名称和对应厂站名称
	char *psql = "select ut.CimId,ut.Name,ut.StationCim,(select stn.Name from stations stn where stn.CimId=ut.StationCim ) as StationName from units ut where ut.UnitType=2 and ut.CimId in (%s);";

	sql = App_Dba::instance()->formatSql(psql,buss.c_str());

	LISTMAP circlelist;
	circlelist = App_Dba::instance()->getList(sql.c_str());

	for (int i=0; i < circlelist.size(); i++)
	{
		STRMAP recode = circlelist.at(i);
		MAP_ITERATOR iter;
		PBNS::CircleQueryBean *crlbean = resp.add_circlelist();

		iter = recode.find("CimId");
		if (iter != recode.end())
		{
			crlbean->set_unitcim(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			crlbean->set_name(iter->second);
		}

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			crlbean->set_stationcim(iter->second);
		}

		iter = recode.find("StationName");
		if (iter != recode.end())
		{
			crlbean->set_stationname(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;

}


void QueryMgrCmd::cricleTopo(int saveid)
{
	// 已经做个起点分析的设备ID集合
	STRMAP passedNodes;

	string sql;
	//查询所有开关、刀闸cimid
	char *psql = "select CimId from units where UnitType in (0,1) ;";

	LISTMAP bekswhList = App_Dba::instance()->getList(psql);
	for (int i = 0;i<bekswhList.size();i++)
	{
		STRMAP saveMap = bekswhList.at(i);
		MAP_ITERATOR iter = saveMap.find("CimId");
		if(iter != saveMap.end())
		{
			//母线记录数初始设置为0
			m_busCount = 0;

			//设置起始开关刀闸cimid
			m_brkswhCim = iter->second;

			//根据开关、刀闸元件进行环路拓扑
			cricleTopoByUnit(saveid,iter->second,passedNodes);

		}
	}

}

void QueryMgrCmd::cricleTopoByUnit(int saveid,string unitcim,STRMAP& passNodes)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.对应的单侧连接点（不是查所有的两个连接点）
	for (int i = 0; i < connIds.size();i++)
	{
		STRMAP connMap = connIds.at(i);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// 根据连接点，查找该连接点关联的设备集合
			LISTMAP unitsList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

			// 遍历该设备集合
			for (int k = 0;k < unitsList.size();k++)
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

				// 元件类型
				int unitType = COM->getIval(unitMap,"UnitType");

				// 元件状态
				int state = COM->getIval(unitMap,"State");

				if (unitType == eTRANSFORMER || ((unitType == eBREAKER||unitType == eSWITCH) && state == 0))
				{
					//重新循环时母线个数清0
					m_busCount = 0;
					m_buslist.clear();

					// 变压器、断开的开关和刀闸为该条路遍历的终点
					continue;
				}
				else if (unitType == eBUS)		//如果设备集合中有母线
				{
					m_busCount++;

					string buscim = unitId;
					//保存母线记录
					m_buslist.push_back(unitId);
				}
				else if (m_brkswhCim == unitId && (m_busCount>1))		//遍历当前cimid等于起始cimid且母线累计数大于1表示环路形成
				{
					return;
				}
				else
				{
					//递归
					cricleTopoByUnit(saveid,unitId,passNodes);
				}
			}
		}

		// 首次拓扑，只拓扑单侧连接点
		if (m_isFirst)
		{
			break;
		}

		// 标记为第一次完成
		m_isFirst = false;
	}

	return;
}


PBNS::StateBean QueryMgrCmd::getUnitByCim(int saveid,string unitcim)
{
	PBNS::StateBean bean;
	char* psql = "select b.State,a.UnitType,a.StationCim,b.IsElectric,b.IsPower " \
		"from units a left join " \
		"unit_status b on a.CimId=b.UnitCim and b.SaveId=%d " \
		"where a.CimId='%s'";
	string sql = DBA->formatSql(psql,saveid,unitcim.c_str());
	LISTMAP unitList = DBA->getList(sql.c_str());
	if (unitList.size() == 1)
	{
		STRMAP unitMap = unitList.at(0);
		string strval = COM->getVal(unitMap,"State");

		if (strval.length()>0)
		{
			bean.set_state(COM->str2i(strval));
		}
		strval = COM->getVal(unitMap,"UnitType");
		bean.set_unittype(COM->str2i(strval));

		strval = COM->getVal(unitMap,"StationCim");
		bean.set_stationcim(strval);

		strval = COM->getVal(unitMap,"IsElectric");
		if (strval.length()>0)
		{
			bean.set_iselectric(COM->str2i(strval));
		}

		strval = COM->getVal(unitMap,"IsPower");
		if (strval.length()>0)
		{
			bean.set_ispower(COM->str2i(strval));
		}

	}
	else
	{
		LOG->warn("数据错误，同一存档下面有多个相同cim的元件:%s",unitcim.c_str());
	}

	return bean;
}

LISTMAP QueryMgrCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim='%s'";
	string sql = DBA->formatSql(psql,unitid.c_str());
	connList = DBA->getList(sql.c_str());
	return connList;
}

LISTMAP QueryMgrCmd::getUnitsByConnId(string connid,string saveid)
{

	// 问题：关联查询设备状态的时候，不用考虑saveid么？unit_status表中，同一个unit可能会有多条记录，以哪天记录为准呢？
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,"\
		"c.State,d.VolValue from (select UnitCim from Relations where ConnCim='%s') a left join "\
		"Units b on a.UnitCim=b.CimId  left join (select UnitCim, State from unit_status "\
		"where saveId=%s) c on c.UnitCim=b.cimid left join voltages d on d.CimId=b.VolCim";

	string sql = DBA->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = DBA->getList(sql.c_str());
	return unitsList;
}

void QueryMgrCmd::querySignList(sClientMsg* msg)
{
	PBNS::SignListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//查询挂牌列表
	char *psql = "SELECT us.UnitCim,(SELECT ut.Name from units ut where ut.CimId=us.UnitCim) as unitName,us.StationCim, \
		(SELECT st.Name from stations st WHERE st.CimId=us.StationCim) stationName, \
		(SELECT ut.UnitType from units ut where ut.CimId=us.UnitCim) unitType from unit_status us where us.IsBoard = 1;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP signlist;
	signlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::SignListMsg_Response resp;
	for (int i=0; i < signlist.size(); i++)
	{
		STRMAP recode = signlist.at(i);
		MAP_ITERATOR iter;
		PBNS::SignQueryBean *sgbean = resp.add_signlist();

		iter = recode.find("UnitCim");
		if (iter != recode.end())
		{
			sgbean->set_unitcim(iter->second);
		}

		iter = recode.find("unitName");
		if (iter != recode.end())
		{
			sgbean->set_unitcimname(iter->second);
		}

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			sgbean->set_stationcim(iter->second);
		}

		iter = recode.find("stationName");
		if (iter != recode.end())
		{
			sgbean->set_stationname(iter->second);
		}

		iter = recode.find("unitType");
		if (iter != recode.end())
		{
			sgbean->set_unittype(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;

}

void QueryMgrCmd::queryGSwitchList(sClientMsg* msg)
{
	PBNS::GswitchListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//查询接地列表
	char *psql = "SELECT us.UnitCim,(SELECT ut.Name from units ut where ut.CimId=us.UnitCim) as unitName,us.StationCim, \
				 (SELECT st.Name from stations st WHERE st.CimId=us.StationCim) stationName, \
				 (SELECT ut.UnitType from units ut where ut.CimId=us.UnitCim) unitType from unit_status us where us.IsGround = 1;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP gswitchlist;
	gswitchlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::GswitchListMsg_Response resp;
	for (int i=0; i < gswitchlist.size(); i++)
	{
		STRMAP recode = gswitchlist.at(i);
		MAP_ITERATOR iter;
		PBNS::GswitchQueryBean *gsbean = resp.add_gswitchlist();

		iter = recode.find("UnitCim");
		if (iter != recode.end())
		{
			gsbean->set_unitcim(iter->second);
		}

		iter = recode.find("unitName");
		if (iter != recode.end())
		{
			gsbean->set_unitcimname(iter->second);
		}

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			gsbean->set_stationcim(iter->second);
		}

		iter = recode.find("stationName");
		if (iter != recode.end())
		{
			gsbean->set_stationname(iter->second);
		}

		iter = recode.find("unitType");
		if (iter != recode.end())
		{
			gsbean->set_unittype(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}

void QueryMgrCmd::queryMsetList(sClientMsg* msg)
{

}

void QueryMgrCmd::queryEventList(sClientMsg* msg)
{
	PBNS::EventListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	//select sye.UnitId,sye.EventValue,sye.SynTime,us.Name,us.UnitType,st.CimId as stioncim,st.Name as stionname from syn_events sye,units us,stations st where sye.UnitId=us.CimId and us.StationCim=st.CimId
	string sql;
	//查询挂牌列表
	char *psql = "select sye.UnitId,sye.EventValue,sye.SynTime,us.Name,us.UnitType,st.CimId as StionCim,st.Name as StionName \
		from syn_events sye,units us,stations st where sye.UnitId=us.CimId and us.StationCim=st.CimId ;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP eventlist;
	eventlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::EventListMsg_Response resp;
	for (int i=0; i < eventlist.size(); i++)
	{
		STRMAP recode = eventlist.at(i);
		MAP_ITERATOR iter;
		PBNS::EventQueryBean *evtbean = resp.add_eventlist();

		iter = recode.find("UnitId");
		if (iter != recode.end())
		{
			evtbean->set_unitcim(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			evtbean->set_unitcimname(iter->second);
		}

		iter = recode.find("StionCim");
		if (iter != recode.end())
		{
			evtbean->set_stationcim(iter->second);
		}

		iter = recode.find("StionName");
		if (iter != recode.end())
		{
			evtbean->set_stationname(iter->second);
		}

		iter = recode.find("UnitType");
		if (iter != recode.end())
		{
			evtbean->set_unittype(iter->second);
		}

		iter = recode.find("EventValue");
		if (iter != recode.end())
		{
			evtbean->set_eventvalue(iter->second);
		}

		iter = recode.find("SynTime");
		if (iter != recode.end())
		{
			evtbean->set_eventtime(iter->second);
		}
	}	

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}


