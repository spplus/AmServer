#include "logincmd.h"
#include "buff/msgbody.pb.h"

void LoginCmd::exec(sClientMsg* msg)
{
	// 1.����Ϣ�������л�Ϊ���ڵ�buff �ṹ
	PBNS::UserLoginMsg_Request res;
	res.ParseFromArray(msg->data,msg->length);

	string u = res.username();
	string pwd = res.userpwd();


	// 2.��ȡ���ݿ�����
	// 3.ִ��SQL
	// 4.��װ���ؽ��
	// 5.�ѽ��buff ���л�Ϊstring
	// 6.���÷������ݵ��ͻ���
	//string data = "hello client";

	// ͨ�����ݿ���е�¼�û���֤
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

	// ���ص��ͻ���
	string data;
	rep.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}