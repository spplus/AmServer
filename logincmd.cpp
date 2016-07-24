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

	// ͨ�����ݿ���е�¼�û���֤ SELECT ID,RoleId,Name,Password,RealName FROM users where name='admin' and Password='202cb962ac59075b964b07152d234b70'
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

	// �����½�ɹ��������Ƿ���CIM�ļ���Ҫ����
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
	

	// ���ص��ͻ���
	string data;
	rep.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}