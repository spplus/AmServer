#include "devstatecmd.h"

void DevStateCmd::exec(sClientMsg* msg)
{
	if (msg == NULL || msg->data == NULL)
	{
		LOG->warn("dev state request failed.");
		return;
	}
	switch (msg->type)
	{
	case CMD_DEV_STATE:
		getDevState(msg);
		break;
	case CMD_STATION_TYPE:
		getStationType(msg);
		break;
	
		// 该命令暂不用了
	case CMD_STATION_LIST:
		getStationList(msg);
		break;
	default:
		break;
	}
}

void DevStateCmd::getDevState(sClientMsg* msg)
{

	PBNS::DevStateMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);	

	// 通过数据库进行查询元件状态
	string sql ;
	char * p = "select b.CimId, State,IsElectric,IsBoard ,c.VolValue " \
		"from Unit_Status a " \
		"left join Units b on a.UnitCim=b.CimId  " \
		"left join voltages c on c.CimId = b.VolCim " \
		"where a.SaveId=%d and a.StationCim=%s";
	sql = App_Dba::instance()->formatSql(p,req.saveid(),req.stationcim().c_str());
	LISTMAP stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	PBNS::DevStateMsg_Response res;
	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		STRMAP record = stateList.at(i);
		MAP_ITERATOR iter;
		PBNS::StateBean* bean = res.add_devstate();
		iter = record.find("CimId");
		if (iter != record.end())
		{
			bean->set_cimid(iter->second);
		}

		iter = record.find("State");
		if (iter != record.end())
		{
			bean->set_state(str2i(iter->second));
		}

		iter = record.find("IsElectric");
		if (iter != record.end())
		{
			bean->set_iselectric(str2i(iter->second));
		}

		iter = record.find("IsBoard");
		if (iter != record.end())
		{
			bean->set_isboard(str2i(iter->second));
		}

		iter = record.find("VolValue");
		if (iter != record.end())
		{
			bean->set_volvalue(iter->second);
		}
	}

	// 返回到客户端
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}

void DevStateCmd::getStationType(sClientMsg* msg)
{

	string sql = "select id, Name from station_category order by OrderNum ";
	
	LISTMAP stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	PBNS::StationTypeMsg_Response res;

	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		STRMAP record = stateList.at(i);
		MAP_ITERATOR iter;
		PBNS::StationTypeBean* bean = res.add_typelist();
		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(str2i(iter->second));
		}

		iter = record.find("Name");
		if (iter != record.end())
		{
			bean->set_name(iter->second);
		}

		// 获取该类别下的站点列表
		getStationByTypeId(bean);
	}

	// 返回到客户端
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}

void DevStateCmd::getStationByTypeId(PBNS::StationTypeBean *typebean)
{
	// 通过数据库进行查询元件状态
	string sql ;
	char * p = "select id, CategoryId, CimId,Name,CurrentName,Path from stations where CategoryId=%d";
	sql = App_Dba::instance()->formatSql(p,typebean->id());
	LISTMAP stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		STRMAP record = stateList.at(i);
		MAP_ITERATOR iter;
		PBNS::StationBean* bean = typebean->add_stationlist();

		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(str2i(iter->second));
		}

		iter = record.find("CategoryId");
		if (iter != record.end())
		{
			bean->set_categoryid(str2i(iter->second));
		}

		iter = record.find("CimId");
		if (iter != record.end())
		{
			bean->set_cimid(iter->second);
		}

		iter = record.find("Name");
		if (iter != record.end())
		{
			bean->set_name(iter->second);
		}

		iter = record.find("CurrentName");
		if (iter != record.end())
		{
			bean->set_currentname(iter->second);
		}

		iter = record.find("Path");
		if (iter != record.end())
		{
			bean->set_path(iter->second);
		}
	}
}

void DevStateCmd::getStationList(sClientMsg* msg)
{
	PBNS::StationListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);	

	// 通过数据库进行查询元件状态
	string sql ;
	char * p = "select id, CategoryId, CimId,Name,CurrentName,Path from stations where CategoryId=%d";
	sql = App_Dba::instance()->formatSql(p,req.stationid());
	LISTMAP stateList;


	stateList = App_Dba::instance()->getList(sql.c_str());

	PBNS::StationListMsg_Response res;
	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		STRMAP record = stateList.at(i);
		MAP_ITERATOR iter;
		PBNS::StationBean* bean = res.add_stationlist();

		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(str2i(iter->second));
		}

		iter = record.find("CategoryId");
		if (iter != record.end())
		{
			bean->set_categoryid(str2i(iter->second));
		}

		iter = record.find("CimId");
		if (iter != record.end())
		{
			bean->set_cimid(iter->second);
		}

		iter = record.find("Name");
		if (iter != record.end())
		{
			bean->set_name(iter->second);
		}

		iter = record.find("CurrentName");
		if (iter != record.end())
		{
			bean->set_currentname(iter->second);
		}

		iter = record.find("Path");
		if (iter != record.end())
		{
			bean->set_path(iter->second);
		}
	}

	// 返回到客户端
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}
