#include "stationmgrcmd.h"


void StationMgrcmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_STATION_TYPE_ADD:
	case CMD_STATION_TYPE_DEL:
	case CMD_STATION_TYPE_MODIFY:
		stationTypeManager(msg);
		break;
	case CMD_STATION_TYPE_LIST:
		getStationTypeList(msg);
		break;
	case CMD_STATION_MANAGER:
		stationManager(msg);
		break;
	}

}

void StationMgrcmd::getStationTypeList(sClientMsg* msg)
{
	string sql = "select id,OrderNum, Name from station_category order by OrderNum ";

	LISTMAP staList;

	staList = App_Dba::instance()->getList(sql.c_str());

	PBNS::StationTypeMsg_Response res;

	// 把vector转buff
	for (int i=0;i<staList.size();i++)
	{
		STRMAP record = staList.at(i);
		MAP_ITERATOR iter;
		PBNS::StationTypeBean* bean = res.add_typelist();
		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("OrderNum");
		if (iter != record.end())
		{
			bean->set_ordernum(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("Name");
		if (iter != record.end())
		{
			bean->set_name(iter->second.c_str());
		}

	}

	// 返回到客户端
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}

void StationMgrcmd::stationTypeManager(sClientMsg* msg)
{
	PBNS::StationTypeMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rdsql = req.mgrsql();

	int nret = App_Dba::instance()->execSql(rdsql.c_str());

	PBNS::StationTypeMgrMsg_Response resp;
	resp.set_rescode(nret);

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}

void StationMgrcmd::stationManager(sClientMsg* msg)
{
	PBNS::StationMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int nsuccess = 0;

	for (int i=0;i<req.stationlist_size();i++)
	{
		PBNS::StationBean stabean = req.stationlist(i);
		
		string sql ;
		char *psql = "UPDATE stations SET CategoryId=%d , CurrentName='%s' , Path='%s' WHERE ID=%d ";
		sql = App_Dba::instance()->formatSql(psql,stabean.categoryid(),stabean.currentname().c_str(),stabean.path().c_str(),stabean.id());

		int nret = App_Dba::instance()->execSql(sql.c_str());

		nsuccess = nsuccess || nret;

	}

	PBNS::StationMgrMsg_Response resp;
	resp.set_rescode(nsuccess);

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}












