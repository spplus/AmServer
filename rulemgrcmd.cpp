#include "rulemgrcmd.h"
#include "buff/msgbody.pb.h"

void RuleMgrCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_RULE_LIST:
	case CMD_COM_RULE_LIST:
		getRuleList(msg);
		break;
	case CMD_RULE_ADD:
	case CMD_RULE_DEL:
	case CMD_RULE_MODIFY:
		ruleManager(msg);
		break;
	case CMD_STATION_RULE_LIST:
		getStationRuleList(msg);
		break;
	case CMD_STATION_RULE_MANAGER:
	case CMD_STATION_RULE_MGR:
		stationRuleManager(msg);
		break;
	}

}

void RuleMgrCmd::getRuleList(sClientMsg* msg)
{
	PBNS::RuleListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//查询规则列表
	char *psql = "SELECT ID,Name,AlarmLevel,Description FROM rules;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP rulelist;
	rulelist = App_Dba::instance()->getList(sql.c_str());

	PBNS::RuleListMsg_Response resp;
	for (int i=0; i < rulelist.size(); i++)
	{
		STRMAP recode = rulelist.at(i);
		MAP_ITERATOR iter;
		PBNS::RuleBean *rlbean = resp.add_rulelist();

		iter = recode.find("ID");
		if (iter != recode.end())
		{
			rlbean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			rlbean->set_name(iter->second);
		}

		iter = recode.find("AlarmLevel");
		if (iter != recode.end())
		{
			rlbean->set_alarmlevel(iter->second);
		}

		iter = recode.find("Description");
		if (iter != recode.end())
		{
			rlbean->set_description(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}


void RuleMgrCmd::ruleManager(sClientMsg* msg)
{
	PBNS::RuleMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rdsql = req.mgrsql();

	int nret = App_Dba::instance()->execSql(rdsql.c_str());

	PBNS::RuleMgrMsg_Response resp;
	resp.set_rescode(nret);

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}


void RuleMgrCmd::getStationRuleList(sClientMsg* msg)
{
	
	//查询站点规则关联关系列表
	string sql = "SELECT sr.StationCim,s.Name, sr.RuleId, r.Name as RName, sr.IsUse ,case WHEN sr.IsUse=0 THEN '否' when sr.IsUse=1 THEN '是' else '未知' end as IsUseName from station_rule sr,stations s,rules r WHERE sr.StationCim=s.CimId and sr.RuleId=r.ID;";
	//string sql = "SELECT sr.StationCim,s.Name, sr.RuleId, r.Name as RName, sr.IsUse  from station_rule sr,stations s,rules r WHERE sr.StationCim=s.CimId and sr.RuleId=r.ID ;";

	LISTMAP srulelist;
	srulelist = App_Dba::instance()->getList(sql.c_str());

	PBNS::StationRuleListMsg_Response resp;
	for (int i=0; i < srulelist.size(); i++)
	{
		STRMAP recode = srulelist.at(i);
		MAP_ITERATOR iter;
		PBNS::StationRuleBean *srlbean = resp.add_starulelist();

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			srlbean->set_stationcim(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			srlbean->set_stationname(iter->second);
		}

		iter = recode.find("RuleId");
		if (iter != recode.end())
		{
			srlbean->set_ruleid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("RName");
		if (iter != recode.end())
		{
			srlbean->set_rulename(iter->second);
		}

		iter = recode.find("IsUse");
		if (iter != recode.end())
		{
			//int iuse = ACE_OS::atoi(iter->second.c_str());
			//srlbean->set_isuse(iuse);
			srlbean->set_isuse(iter->second);
		}

		/**/
		iter = recode.find("IsUseName");
		if (iter != recode.end())
		{
			srlbean->set_isusename(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}


void RuleMgrCmd::stationRuleManager(sClientMsg* msg)
{
	PBNS::StationRuleMgrMsg_Request reqst;
	reqst.ParseFromArray(msg->data,msg->length);

	int ntype = reqst.reqtype();
	if (reqst.reqtype() == 1)
	{
		string sql = reqst.mgrsql();

		int nret = App_Dba::instance()->execSql(sql.c_str());

		PBNS::StationRuleMgrMsg_Response resp;
		resp.set_rescode(nret);

		// 返回到客户端
		string data;
		resp.SerializeToString(&data);
		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

		return;
	}

	//当规则类型修改时只是传递作用
	if (reqst.reqtype() == 2)
	{
		PBNS::StationRuleMgrMsg_Response resp;
		resp.set_rescode(2);

		// 返回到客户端
		string data;
		resp.SerializeToString(&data);
		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

		return;
	}

	//提交修改的厂站规则关联关系列表中启用值
	if (reqst.reqtype() == 3)
	{
		int nsuccess = 0;

		for (int i=0;i<reqst.stationrllist_size();i++)
		{
			PBNS::StationRuleBean strlbean = reqst.stationrllist(i);

			string sql ;
			//UPDATE station_rule SET IsUse=0 WHERE StationCim='%1'
			char *psql = "UPDATE station_rule SET IsUse=%s WHERE StationCim='%s' and RuleId=%d";;
			sql = App_Dba::instance()->formatSql(psql,strlbean.isuse().c_str(),strlbean.stationcim().c_str(),strlbean.ruleid());

			int nret = App_Dba::instance()->execSql(sql.c_str());

			nsuccess = nret;

		}

		PBNS::StationMgrMsg_Response resp;
		resp.set_rescode(nsuccess);

		// 返回到客户端
		string data;
		resp.SerializeToString(&data);
		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

		return;
	}
}

