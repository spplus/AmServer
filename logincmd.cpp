#include "logincmd.h"
#include "buff/msgbody.pb.h"

void LoginCmd::exec(sClientMsg* msg)
{
	// 1.把消息内容序列化为对于的buff 结构
	PBNS::UserLoginMsg_Request res;
	res.ParseFromArray(msg->data,msg->length);

	string u = res.username();
	string pwd = res.userpwd();


	// 2.获取数据库连接
	// 3.执行SQL
	// 4.组装返回结果
	// 5.把结果buff 序列化为string
	// 6.调用返回内容到客户端
	//string data = "hello client";

	// 通过数据库进行登录用户验证 SELECT ID,RoleId,Name,Password,RealName FROM users where name='admin' and Password='202cb962ac59075b964b07152d234b70'
	string sql ;
	char *psql = "SELECT ID,RoleId,Name,Password,RealName FROM users where name='%s' and Password='%s' ";
	sql = App_Dba::instance()->formatSql(psql,res.username().c_str(),res.userpwd().c_str());
	LISTMAP usersatelist;

	usersatelist = App_Dba::instance()->getList(sql.c_str());

	PBNS::UserListMsg_Response rep;
	for (int i=0; i < usersatelist.size(); i++)
	{
		STRMAP record = usersatelist.at(i);
		MAP_ITERATOR iter;
		PBNS::UserBean *ubean = rep.add_userlist();

		iter = record.find("ID");
		if (iter != record.end())
		{
			ubean->set_userid(iter->second);
		}
		
		iter = record.find("RoleId");
		if (iter != record.end())
		{
			ubean->set_userrole(iter->second);
		}

		iter = record.find("Name");
		if (iter != record.end())
		{
			ubean->set_username(iter->second);
		}

		iter = record.find("Password");
		if (iter != record.end())
		{
			ubean->set_userpwd(iter->second);
		}

		iter = record.find("RealName");
		if (iter != record.end())
		{
			ubean->set_realname(iter->second);
		}

	}

	// 如果登陆成功，则坚持是否有CIM文件需要更新
	if (usersatelist.size() > 0)
	{
		char *psql = "select count(*) as count from system_config where isnew=1";
		LISTMAP countList = DBA->getList(psql);
		if (countList.size() > 0)
		{
			STRMAP countMap = countList.at(0);
			int count = COM->getIval(countMap,"count");
			if (count > 0)
			{
				rep.set_rescode(2);
			}
		}
	}
	

	// 返回到客户端
	string data;
	rep.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}