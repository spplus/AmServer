#include "ticketmgrcmd.h"


void TicketMgrCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TICKET_LIST:
		getTicketList(msg);
		break;
	case CMD_TICKET_CREATE:
		createTicket(msg);
		break;
	case CMD_TICKET_QUERY:
		queryTicketActions(msg);
		break;
	case CMD_TICKET_COMMIT:
		commitTicket(msg);
		break;
	}

}

void TicketMgrCmd::getTicketList(sClientMsg* msg)
{
	PBNS::TicketListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int nType = req.reqtype();
	int nId = req.reqid();

	string sql;


	//查询操作任务列表
	char *psql = "select tk.ID,tkm.UserId,(select u.Name from users u where u.ID=tkm.UserId) as UserName, \
		tkm.ActionUserId,(select u.Name from users u where u.ID=tkm.ActionUserId) as ActionUserName, \
		tk.MissionId,tkm.Name as MissionName,tk.No,tk.ActionType,tk.Info,tk.ActionPerson,tk.ProtectPerson,tk.ChargePerson,tk.StartTime,tk.EndTime,tkm.PublishTime \
		from tickets tk,ticket_missions tkm where tk.MissionId=tkm.ID and tkm.ActionUserId=%d ;";

	sql = App_Dba::instance()->formatSql(psql,nId);


	LISTMAP tklist;
	tklist = App_Dba::instance()->getList(sql.c_str());

	PBNS::TicketListMsg_Response resp;
	for (int i=0; i < tklist.size(); i++)
	{
		STRMAP recode = tklist.at(i);
		MAP_ITERATOR iter;
		PBNS::TicketBean *tkbean = resp.add_ticketlist();

		iter = recode.find("ID");
		if (iter != recode.end())
		{
			tkbean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("UserId");
		if (iter != recode.end())
		{
			tkbean->set_cuserid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("UserName");
		if (iter != recode.end())
		{
			tkbean->set_cusername(iter->second);
		}

		iter = recode.find("ActionUserId");
		if (iter != recode.end())
		{
			tkbean->set_auserid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("ActionUserName");
		if (iter != recode.end())
		{
			tkbean->set_ausername(iter->second);
		}

		iter = recode.find("MissionId");
		if (iter != recode.end())
		{
			tkbean->set_missionid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("MissionName");
		if (iter != recode.end())
		{
			tkbean->set_missionname(iter->second);
		}

		iter = recode.find("No");
		if (iter != recode.end())
		{
			tkbean->set_no(iter->second);
		}

		iter = recode.find("ActionType");
		if (iter != recode.end())
		{
			tkbean->set_actiontype(iter->second);
		}

		iter = recode.find("Info");
		if (iter != recode.end())
		{
			tkbean->set_info(iter->second);
		}

		iter = recode.find("ActionPerson");
		if (iter != recode.end())
		{
			tkbean->set_actionperson(iter->second);
		}

		iter = recode.find("ProtectPerson");
		if (iter != recode.end())
		{
			tkbean->set_protectperson(iter->second);
		}

		iter = recode.find("ChargePerson");
		if (iter != recode.end())
		{
			tkbean->set_chargeperson(iter->second);
		}

		iter = recode.find("StartTime");
		if (iter != recode.end())
		{
			tkbean->set_starttime(iter->second);
		}

		iter = recode.find("EndTime");
		if (iter != recode.end())
		{
			tkbean->set_endtime(iter->second);
		}

		iter = recode.find("PublishTime");
		if (iter != recode.end())
		{
			tkbean->set_publishtime(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}


void TicketMgrCmd::createTicket(sClientMsg* msg)
{
	PBNS::TicketActMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rdsql = req.mgrsql();

	int nret = App_Dba::instance()->execSql(rdsql.c_str());

	int ticketid = 0;

	if (nret > 0)
	{
		string sql ;
		//select ID from tickets t where t.UserId=8 and t.MissionId=4 and t.StartTime='2016-07-17 10:42:26' and t.EndTime='2016-07-17 10:42:26'
		char *psql = "select ID from tickets t where t.UserId=%d and t.MissionId=%d and t.StartTime='%s' and t.EndTime='%s' ";
		sql = App_Dba::instance()->formatSql(psql,req.requid(),req.reqmid(),req.startt().c_str(),req.endt().c_str());

		LISTMAP ticketlist;
		ticketlist = App_Dba::instance()->getList(sql.c_str());

		for (int i=0; i < ticketlist.size(); i++)
		{
			STRMAP record = ticketlist.at(i);
			MAP_ITERATOR iter;

			iter = record.find("ID");
			if (iter != record.end())
			{
				ticketid = ACE_OS::atoi(iter->second.c_str());
			}
		}


		int nsuccess = 0;

		for (int i=0;i<req.ticketactlist_size();i++)
		{
			PBNS::TicketActBean tketactbean = req.ticketactlist(i);

			string sql ;
			char *psql = "insert into ticket_actions (TicketId,OrderNum,SystemContent,Content) VALUES (%d,%d,'%s','%s');";
			sql = App_Dba::instance()->formatSql(psql,ticketid,tketactbean.ordernum(),tketactbean.systemcontent().c_str(),tketactbean.content().c_str());

			int nret = App_Dba::instance()->execSql(sql.c_str());

			nsuccess = nret;

		}
	}


	PBNS::TicketActMgrMsg_Response resp;
	resp.set_rescode(nret);

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}

void TicketMgrCmd::queryTicketActions(sClientMsg* msg)
{
	PBNS::TicketActListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int nId = req.reqid();

	string sql;

	//查询操作任务列表
	char *psql = "select tka.TicketId,tka.OrderNum,tka.SystemContent,tka.Content from ticket_actions tka where tka.TicketId=%d ;";

	sql = App_Dba::instance()->formatSql(psql,nId);


	LISTMAP tkaclist;
	tkaclist = App_Dba::instance()->getList(sql.c_str());

	PBNS::TicketActListMsg_Response resp;
	for (int i=0; i < tkaclist.size(); i++)
	{
		STRMAP recode = tkaclist.at(i);
		MAP_ITERATOR iter;
		PBNS::TicketActBean *tkacbean = resp.add_ticketactlist();

		iter = recode.find("TicketId");
		if (iter != recode.end())
		{
			tkacbean->set_ticketid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("OrderNum");
		if (iter != recode.end())
		{
			tkacbean->set_ordernum(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("SystemContent");
		if (iter != recode.end())
		{
			tkacbean->set_systemcontent(iter->second);
		}

		iter = recode.find("Content");
		if (iter != recode.end())
		{
			tkacbean->set_content(iter->second.c_str());
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}

void TicketMgrCmd::commitTicket(sClientMsg* msg)
{
	PBNS::TicketMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rdsql = req.mgrsql();

	int nret = App_Dba::instance()->execSql(rdsql.c_str());

	PBNS::TicketMgrMsg_Response resp;
	resp.set_rescode(nret);

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}