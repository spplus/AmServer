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

	// 通过数据库进行登录用户验证
	string sql ;
	char *psql = "SELECT COUNT(id) as userstate FROM users where name='%s' and Password='%s' ";
	sql = App_Dba::instance()->formatSql(psql,res.username().c_str(),res.userpwd().c_str());
	//sql = App_Dba::instance()->formatSql(p,req.saveid(),req.stationid());
	vector<map<string,string> > usersatelist;

	usersatelist = App_Dba::instance()->getList(sql.c_str());

	PBNS::UserListMsg_Response rep;
	for (int i=0; i < usersatelist.size(); i++)
	{
		map<string,string> record = usersatelist.at(i);
		map<string,string>::iterator iter;
		iter = record.find("userstate");
		if (iter != record.end())
		{
			string t = iter->first;
			int it = -1;
			it = ACE_OS::atoi(iter->first.c_str());
			rep.set_rescode(ACE_OS::atoi(iter->second.c_str()));
		}
	}

	// 返回到客户端
	string data;
	rep.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}