#include "devstatecmd.h"
#include "topobizcmd.h"

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
	//case CMD_STATION_LIST:
		//getStationList(msg);
		//break;

	case CMD_TAG_OP:
		updateIsBoard(msg);
		break;

	case CMD_POWER_SET:
		updateIsPower(msg);
		break;

	case CMD_LINE_SET:
		updateIsLine(msg);
		break;

	case CMD_READ_SAVING:
		getSavingList(msg);
		break;

	case CMD_WRITE_SAVING:
		writeSaving(msg);
		break;
	default:
		break;
	}
}

void DevStateCmd::getDevState(sClientMsg* msg)
{

	PBNS::DevStateMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);	

	if (req.stationcim().length() <=0 )
	{
		return;
	}
	// 通过数据库进行查询元件状态
	string sql ;
	char * p = "select b.CimId, State,IsElectric,IsBoard ,c.VolValue,b.unitType " \
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

		iter = record.find("unitType");
		if (iter != record.end())
		{
			bean->set_unittype(str2i(iter->second));
		}
	}

	// 返回到客户端
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}

void DevStateCmd::getStationType(sClientMsg* msg)
{

	string sql = "select id,OrderNum,Name from station_category order by OrderNum ";
	
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

		iter = record.find("OrderNum");
		if (iter != record.end())
		{
			bean->set_ordernum(str2i(iter->second));
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

void DevStateCmd::updateIsBoard(sClientMsg* msg)
{
	PBNS::TagMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int tag ;
	if (req.type() == 1)
	{
		tag = 0;
	}
	else
	{
		tag = 1;
	}
	// 返回内容
	PBNS::TagMsg_Response res;

	// 是否允许挂牌，摘牌
	int flag = 0;

	// 1. 检测是否已经挂牌或已经摘牌
	char * psql = "select count(*) as count  from unit_status   where UnitCim='%s' and SaveId=%d and IsBoard=%d";
	string sql = App_Dba::instance()->formatSql(psql,req.unitcim().c_str(),req.saveid(),tag);
	LISTMAP countMap = App_Dba::instance()->getList(sql.c_str());
	if (countMap.size()>0)
	{
		STRMAP strMap = countMap.at(0);
		MAP_ITERATOR iter = strMap.find("count");
		if (iter != strMap.end())
		{
			// 允许操作
			if (str2i(iter->second) ==0)
			{
				flag = 1;
			}
		}
	}
	if (flag == 1)
	{
		// 2.更新挂摘牌标志 
		psql = "update unit_status set IsBoard=%d where UnitCim='%s' and SaveId=%d ";
		sql = App_Dba::instance()->formatSql(psql,req.type(),req.unitcim().c_str(),req.saveid());
		int ret = App_Dba::instance()->execSql(sql.c_str());
		if (ret == 1)
		{
			// 执行成功
			res.set_rescode(0);
			res.set_resmsg("操作成功");
		}
		else
		{
			res.set_resmsg("更新失败");
			res.set_rescode(1);
		}
	}
	else
	{
		// 操作失败
		res.set_rescode(2);
		res.set_resmsg("未查找到相应的记录");
	}

	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}

void DevStateCmd::updateIsLine(sClientMsg* msg)
{

	PBNS::LineSetMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	// 1.删除 related_line表中 unitcim等于该cimid的记录
	char * psql = "delete from related_line where UnitCim='%s'";
	string sql = App_Dba::instance()->formatSql(psql,req.unitcim().c_str());
	App_Dba::instance()->execSql(sql.c_str());

	// 2.插入两条记录
	psql = "insert into related_line(UnitCim,StationCim) values(%s,%s)";
	sql = App_Dba::instance()->formatSql(psql,req.unitcim().c_str(),req.stationonecim().c_str());
	int ret =App_Dba::instance()->execSql(sql.c_str());

	sql = App_Dba::instance()->formatSql(psql,req.unitcim().c_str(),req.stationothercim().c_str());
	ret += App_Dba::instance()->execSql(sql.c_str());

	PBNS::LineSetMsg_Response res;

	if (ret == 2)
	{
		// 操作成功
		res.set_rescode(0);
	}
	else
	{
		// 操作失败
		res.set_rescode(1);
	}
	
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}

void DevStateCmd::updateIsPower(sClientMsg* msg)
{
	PBNS::PowerSetMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	// 更新RelatedLine表UnitCim字段等于CimId，StationCim等于StationId的IsPower字段；
	char* psql = "update related_line set IsPower=1 where UnitCim='%s' and StationCim='%s'";
	string sql = App_Dba::instance()->formatSql(psql,req.unitcim().c_str(),req.stationcim().c_str());
	int ret = App_Dba::instance()->execSql(sql.c_str());
	PBNS::PowerSetMsg_Response res;
	if (ret == 1)
	{
		res.set_rescode(0);
		LOG->message("设置电源点成功");
	}
	else
	{
		res.set_rescode(1);
		LOG->message("设置电源点失败");
	}
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}

void DevStateCmd::getSavingList(sClientMsg* msg)
{
	PBNS::SavingListMsg_Response res;
	char* psql = "select id,name,savetime from virtual_saves";
	LISTMAP saveList = App_Dba::instance()->getList(psql);
	for (int i = 0;i<saveList.size();i++)
	{
		PBNS::SavingBean *bean = res.add_savelist();
		STRMAP saveMap = saveList.at(i);
		MAP_ITERATOR iter = saveMap.find("id");
		if (iter != saveMap.end())
		{
			bean->set_id(str2i(iter->second));
		}

		iter = saveMap.find("name");
		if (iter != saveMap.end())
		{
			bean->set_name(iter->second);
		}

		iter = saveMap.find("savetime");
		if (iter != saveMap.end())
		{
			bean->set_savetime(iter->second);
		}
	}

	App_ClientMgr::instance()->sendData(msg->connectId,res.SerializeAsString(),msg->type);
}

void DevStateCmd::writeSaving(sClientMsg* msg)
{
	PBNS::WriteSavingMsg_Request req;
	PBNS::WriteSavingMsg_Response res;
	req.ParseFromArray(msg->data,msg->length);
	
	char *psql = "insert into virtual_saves(name,savetime) values('%s',now())";
	string sql = App_Dba::instance()->formatSql(psql,req.savename().c_str());
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret == 1)
	{
		res.set_rescode(0);
		LOG->message("保持存档成功,存档名称：%s",req.savename().c_str());
	}
	else
	{
		res.set_rescode(1);
		LOG->message("保持存档失败,存档名称：%s",req.savename().c_str());
	}

	// 查询新生成的存档ID
	string nSaveId ;
	psql = "select max(id) as id from virtual_saves";
	LISTMAP idList = App_Dba::instance()->getList(psql);
	if (idList.size()>0)
	{
		STRMAP idMap = idList.at(0);
		MAP_ITERATOR iter = idMap.find("id");
		if (iter != idMap.end())
		{
			nSaveId = iter->second;
		}
	}

	// 查询原SaveID下面存档的状态数据
	psql = "insert into unit_status(SaveId,UnitCim,StationCim,State,IsElectric,IsPower,IsBoard) " \
		"select %s, UnitCim,StationCim,State,IsElectric,IsPower,IsBoard from unit_status where saveid=%d";
	sql = App_Dba::instance()->formatSql(psql,nSaveId.c_str(),req.saveid());
	App_Dba::instance()->execSql(sql.c_str());

	// 更新客户端操作后的设备状态，包括开关，刀闸的状态，挂牌状态
	for (int i=0;i<req.statelist_size();i++)
	{
		PBNS::StateBean bean = req.statelist(i);
		psql = "update unit_status set IsBoard=%d,State=%d where SaveId='%s' and UnitCim='%s' ";
		sql = App_Dba::instance()->formatSql(psql,bean.isboard(),bean.state(),nSaveId.c_str(),bean.cimid().c_str());
		App_Dba::instance()->execSql(sql.c_str());
	}

	// 对新存档进行整站拓扑
	TopoBizCmd topo;
	topo.topoBySaveId(nSaveId,eGENERATOR);

	App_ClientMgr::instance()->sendData(msg->connectId,res.SerializeAsString(),msg->type);

}