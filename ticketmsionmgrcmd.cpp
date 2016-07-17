#include "ticketmsionmgrcmd.h"

void TicketMsionMgrCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TICKETMS_LIST:
		getTicketMsionList(msg);
		break;
	case CMD_ROLE_USER_LIST:
		getUserListbyRoleid(msg);
		break;
	case CMD_TICKETMS_MANAGER:
	case CMD_TICKETMS_ADD:
	case CMD_TICKETMS_DEL:
	case CMD_TICKETMS_MODIFY:
		ticketMsionManager(msg);
		break;
	}

}

void TicketMsionMgrCmd::getUserListbyRoleid(sClientMsg* msg)
{
	PBNS::UserListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//��ѯ��ά��ɫ��Ӧ���û��б�
	char *psql = "select u.ID,u.Name,u.Password,u.RoleId,r.Name as rolename,u.RealName from users u, roles r where u.RoleId=r.ID and u.RoleId=4;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP userlist;
	userlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::UserListMsg_Response resp;
	for (int i=0; i < userlist.size(); i++)
	{
		STRMAP recode = userlist.at(i);
		MAP_ITERATOR iter;
		PBNS::UserBean *ulbean = resp.add_userlist();

		iter = recode.find("ID");
		if (iter != recode.end())
		{
			ulbean->set_userid(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			ulbean->set_username(iter->second);
		}

		iter = recode.find("Password");
		if (iter != recode.end())
		{
			ulbean->set_userpwd(iter->second);
		}

		iter = recode.find("RoleId");
		if (iter != recode.end())
		{
			ulbean->set_userrole(iter->second);
		}

		iter = recode.find("rolename");
		if (iter != recode.end())
		{
			ulbean->set_rolename(iter->second);
		}

		iter = recode.find("RealName");
		if (iter != recode.end())
		{
			ulbean->set_realname(iter->second);
		}

	}

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}

void TicketMsionMgrCmd::getTicketMsionList(sClientMsg* msg)
{
	PBNS::TicketMsionListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int nUserid = req.userid();
	int nRoleid = req.roleid();

	string sql;

	//����û���ɫ����άԱ(id=4)����ѯִ���˶�Ӧ�Ĳ��������б�
	if (nRoleid == 4)
	{
		//��ѯ���������б�
		char *psql = "select tkm.ID,tkm.UserId,(select u.Name FROM users u WHERE tkm.UserId=u.ID) as Usernaem, \
					 tkm.ActionUserId,(select u.Name FROM users u WHERE tkm.ActionUserId=u.ID) as Actuname,tkm.Name,tkm.PublishTime from ticket_missions tkm where tkm.ActionUserId=%d ;";

		sql = App_Dba::instance()->formatSql(psql,nUserid);
	}
	else
	{
		//��ѯ���������б�
		char *psql = "select tkm.ID,tkm.UserId,(select u.Name FROM users u WHERE tkm.UserId=u.ID) as Usernaem, \
					 tkm.ActionUserId,(select u.Name FROM users u WHERE tkm.ActionUserId=u.ID) as Actuname,tkm.Name,tkm.PublishTime from ticket_missions tkm where tkm.UserId=%d ;";

		sql = App_Dba::instance()->formatSql(psql,nUserid);
	}

	LISTMAP tkmslist;
	tkmslist = App_Dba::instance()->getList(sql.c_str());

	PBNS::TicketMsionListMsg_Response resp;
	for (int i=0; i < tkmslist.size(); i++)
	{
		STRMAP recode = tkmslist.at(i);
		MAP_ITERATOR iter;
		PBNS::TicketMsionBean *tkmsbean = resp.add_tktmsionlist();

		iter = recode.find("ID");
		if (iter != recode.end())
		{
			tkmsbean->set_id(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("UserId");
		if (iter != recode.end())
		{
			tkmsbean->set_userid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("Usernaem");
		if (iter != recode.end())
		{
			tkmsbean->set_username(iter->second);
		}

		iter = recode.find("ActionUserId");
		if (iter != recode.end())
		{
			tkmsbean->set_actuserid(ACE_OS::atoi(iter->second.c_str()));
		}

		iter = recode.find("Actuname");
		if (iter != recode.end())
		{
			tkmsbean->set_actusername(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			tkmsbean->set_name(iter->second);
		}

		iter = recode.find("PublishTime");
		if (iter != recode.end())
		{
			tkmsbean->set_publishtime(iter->second);
		}

	}

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}


void TicketMsionMgrCmd::ticketMsionManager(sClientMsg* msg)
{
	PBNS::TicketMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rdsql = req.mgrsql();

	int nret = App_Dba::instance()->execSql(rdsql.c_str());

	PBNS::TicketMgrMsg_Response resp;
	resp.set_rescode(nret);

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}
