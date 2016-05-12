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
	char * p = "select CimId, State,IsElectric,IsBoard from Unit_Status a left join Units b on a.UnitId=b.id where a.SaveId=%d and a.StationId=%d";
	sql = App_Dba::instance()->formatSql(p,req.saveid(),req.stationid());
	vector<map<string,string> > stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	PBNS::DevStateMsg_Response res;
	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		map<string,string> record = stateList.at(i);
		map<string,string>::iterator iter;
		PBNS::StateBean* bean = res.add_devstate();
		iter = record.find("CimId");
		if (iter != record.end())
		{
			bean->set_cimid(iter->second);
		}

		iter = record.find("State");
		if (iter != record.end())
		{
			bean->set_state(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("IsElectric");
		if (iter != record.end())
		{
			bean->set_iselectric(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("IsBoard");
		if (iter != record.end())
		{
			bean->set_isboard(ACE_OS::atoi(iter->second.c_str()));
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
	
	vector<map<string,string> > stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	PBNS::StationTypeMsg_Response res;

	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		map<string,string> record = stateList.at(i);
		map<string,string>::iterator iter;
		PBNS::StationTypeBean* bean = res.add_typelist();
		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("Name");
		if (iter != record.end())
		{
			bean->set_name(iter->second.c_str());
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
	vector<map<string,string> > stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		map<string,string> record = stateList.at(i);
		map<string,string>::iterator iter;
		PBNS::StationBean* bean = typebean->add_stationlist();

		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("CategoryId");
		if (iter != record.end())
		{
			bean->set_categoryid(ACE_OS::atoi(iter->second.c_str()));
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
	vector<map<string,string> > stateList;

	stateList = App_Dba::instance()->getList(sql.c_str());

	PBNS::StationListMsg_Response res;
	// 把vector转buff
	for (int i=0;i<stateList.size();i++)
	{
		map<string,string> record = stateList.at(i);
		map<string,string>::iterator iter;
		PBNS::StationBean* bean = res.add_stationlist();

		iter = record.find("id");
		if (iter != record.end())
		{
			bean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = record.find("CategoryId");
		if (iter != record.end())
		{
			bean->set_categoryid(ACE_OS::atoi(iter->second.c_str()));
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
