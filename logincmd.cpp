#include "logincmd.h"
#include "buff/msgbody.pb.h"

void LoginCmd::exec(sClientMsg* msg)
{
	// 1.����Ϣ�������л�Ϊ���ڵ�buff �ṹ
	PBNS::UserLoginMsg_Request res;
	res.ParseFromArray(msg->data,msg->length);

	// 2.��ȡ���ݿ�����

	// 3.ִ��SQL

	// 4.��װ���ؽ��

	// 5.�ѽ��buff ���л�Ϊstring
	string data = "hello client";

	// 6.���÷������ݵ��ͻ���
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}