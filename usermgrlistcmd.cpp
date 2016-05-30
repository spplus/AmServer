#include "usermgrlistcmd.h"
#include "buff/msgbody.pb.h"

void UserMgrListCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_USER_MANAGER:
		getUserList(msg);
		break;
	case CMD_USER_ROLE:
		getUserRoleList(msg);
		break;
	case CMD_USER_ADD:
	case CMD_USER_DEL:
	case CMD_USER_MODIFY:
	case CMD_PWD_MODIFY:
		userManager(msg);
		break;
	}

}

void UserMgrListCmd::getUserList(sClientMsg* msg)
{
	PBNS::UserListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//查询用户列表
	char *psql = "select u.ID,u.Name,u.Password,u.RoleId,r.Name as rolename,u.RealName from users u, roles r where u.RoleId=r.ID;";

	sql = App_Dba::instance()->formatSql(psql);

	//vector<map<string,string> > userlist;
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

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
}

void UserMgrListCmd::getUserRoleList(sClientMsg* msg)
{
	PBNS::UserRoleListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string sql;
	//查看用户角色列表
	char *psql = "select id,Name from roles;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP userrolelist;
	userrolelist = App_Dba::instance()->getList(sql.c_str());

	PBNS::UserRoleListMsg_Response resp;
	for (int i=0;i < userrolelist.size(); i++)
	{
		STRMAP	recode = userrolelist.at(i);
		MAP_ITERATOR iter;

		PBNS::UserRoleBean *urBean = resp.add_userrolelist();

		iter = recode.find("id");
		if (iter != recode.end())
		{
			urBean->set_userroleid(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			urBean->set_userrolename(iter->second);
		}

	}

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}

void UserMgrListCmd::userManager(sClientMsg* msg)
{
	
	PBNS::UserMgrMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rdsql = req.mgrsql();

	int nret = App_Dba::instance()->execSql(rdsql.c_str());

	PBNS::UserMgrMsg_Response resp;
	resp.set_rescode(nret);

	// 返回到客户端
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}



