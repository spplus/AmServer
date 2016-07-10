#include "querymgrcmd.h"


void QueryMgrCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_QUERY_CIRCLE_LIST:
		queryCricleList(msg);
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

	
	PBNS::EventListMsg_Response resp;
	

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}


