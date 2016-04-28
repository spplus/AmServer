#include "logincmd.h"
#include "buff/msgbody.pb.h"

void LoginCmd::exec(sClientMsg* msg)
{
	// 1.把消息内容序列化为对于的buff 结构
	PBNS::UserLoginMsg_Request res;
	res.ParseFromArray(msg->data,msg->length);

	// 2.获取数据库连接

	// 3.执行SQL

	// 4.组装返回结果

	// 5.把结果buff 序列化为string
	string data = "hello client";

	// 6.调用返回内容到客户端
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}