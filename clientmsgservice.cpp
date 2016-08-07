
#include "ace/OS_NS_unistd.h"
#include "clientmsgservice.h"
#include "include/commands.h"
#include "defines.h"
#include "dbaccess.h"
#include "buff/msgbody.pb.h"
#include "clientmgr.h"

void ClientMsgService::start()
{
	m_stop = false;
	activate();

	LOG->message("client message service started.");
}

void ClientMsgService::stop()
{
	m_stop = true;
}


int ClientMsgService::put(ACE_Message_Block* mb)
{
	return putq(mb);
}

int ClientMsgService::svc()
{
	ACE_Time_Value tv(0,1);
	while(!m_stop)
	{
		ACE_Message_Block* mb ;
		if(getq(mb) != -1)
		{
			// 对接收到的请求，进行处理
			parseData(mb);

			mb->release();
		}
		ACE_OS::sleep(tv);
	}
	return 0;
}

void ClientMsgService::parseData(ACE_Message_Block* mb)
{
	if (mb->length() > MAX_PACKET_LEN)
	{
		LOG->error("Invalid data length %d",mb->length());
		return;
	}

	//检查是否为校验客户端请求校验保持的tcp连接ID
	if (m_connectId == mb->msg_type())
	{

		char *jydata = mb->rd_ptr();
		int datalen = mb->length();

		//解析数据区数据
		YK_QUESTION *pYk = (YK_QUESTION*)&jydata[0];
		//赋值指针供返回发送到客户端(scada服务器)时用
		pykqust = new YK_QUESTION;
		pykqust = pYk;
		
		//遥控操作:遥控：0拉闸，1合闸。升降：0降 1升 2急停
		int nyktype = (int)pYk->cOprateType;
		if (nyktype == 2)	//急停直接返回
		{
			//直接返回中的数据不再这里填充，在发送时再填充
			string strdata="scada";
			App_ClientMgr::instance()->sendData(m_connectId,strdata,CMD_SCADA_QUESTION);
			return;
		}

		//操作对应的设备cimid
		string cimid = pYk->szObjCode;
		//根据设备cimid获取设备类型
		int cimtype = getTypeByCimid(cimid);

		//组合检验服务器需要的数据区数据
		PBNS::OprationMsg_Request req;
		req.set_saveid(0);
		req.set_type(nyktype);
		req.set_unitcim(cimid);
		req.set_unittype(cimtype);
		req.set_ischeck(true);
		string data = req.SerializeAsString();

		
		sClientMsg* msg = new sClientMsg;
		//设置连接ID
		msg->connectId = mb->msg_type();
		msg->length = data.length();
		msg->type = CMD_TOPO_BREAKER_CHANGE;
		//char *msgdata = const_cast<char*>(data.c_str());
		//msg->data = msgdata;
		msg->data = const_cast<char*>(data.c_str());

		// 调用业务逻辑处理
		m_biz.exec(msg);

		msg->data = NULL;
		delete msg;

		int tt = 0;
	}
	else
	{
		// 解包
		sClientMsg* msg = m_pack.decoder(mb->rd_ptr(),mb->length());

		if (msg == NULL)
		{
			return ;
		}

		// 设置连接ID
		msg->connectId = mb->msg_type();

		// 调用业务逻辑处理
		m_biz.exec(msg);

		delete msg;
	}
	
}

int ClientMsgService::getTypeByCimid(string cimid)
{
	// 0开关，1刀闸，2母线，3电容，4负载，5发电机，6电压互感器(PT)，7电抗器，8变压器，9进出线

	char * psql = "SELECT UnitType from units where CimId='%s'";
	string sql = DBA->formatSql(psql,cimid.c_str());
	LISTMAP countlist;

	int unitType = 0;

	countlist = DBA->getList(sql.c_str());
	for (int i=0;i<countlist.size();i++)
	{
		STRMAP record = countlist.at(i);
		MAP_ITERATOR iter;

		iter = record.find("UnitType");
		if (iter != record.end())
		{
			unitType = atoi(iter->second.c_str());
		}
	}

	return unitType;
}
