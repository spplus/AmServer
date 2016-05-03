#include "devstatecmd.h"

void DevStateCmd::exec(sClientMsg* msg)
{
	if (msg == NULL || msg->data == NULL)
	{
		LOG->warn("dev state request failed.");
		return;
	}
	PBNS::DevStateMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);	

	// 通过数据库进行查询元件状态
	string sql = "select CimId, State,IsElectric,IsBoard from UnitStatus a left join Units b on a.UnitId=b.id where a.SaveId=";
	sql += req.saveid();
	sql += " and StationId=";
	sql += req.stationid();

	vector<map<string,string> > stateList = App_Dba::instance()->getList(sql.c_str());

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