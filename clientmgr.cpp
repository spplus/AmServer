
#include "defines.h"
#include "clientmgr.h"
#include "buff/msgbody.pb.h"
#include "include/commands.h"
#include "dbaccess.h"
#include "ace/Time_Value.h"
#include "ace/Date_Time.h"

ClientMgr::ClientMgr()
{
	m_connectId = 1000;
}

unsigned int ClientMgr::add(ClientHandler* client)
{
	m_connectId++;
	m_clientList.insert( CMAP::value_type(m_connectId, client));
	return m_connectId;
}

void ClientMgr::del(unsigned int connid)
{
	m_clientList.erase(m_clientList.find(connid));
}

ClientHandler* ClientMgr::get(unsigned int connid)
{
	CMAP::iterator iter = m_clientList.find(connid);
	if (iter != m_clientList.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

int ClientMgr::sendData(unsigned int connid,string data,int msgtype)
{
	// ДђАќЪ§Он
	int outLength = 0;
	
	//当请求校验连接的客户端连接ID和返回连接ID相等时单独解帧处理
	if (App_CMService::instance()->m_connectId == connid)
	{
		//请求遥控为急停,直接返回处理
		if (msgtype == CMD_SCADA_QUESTION)
		{
			//得到scada请求的校验数据区指针
			YK_QUESTION *pYkq = App_CMService::instance()->pykqust;
			//遥控操作
			int coprtype = (int)pYkq->cOprateType; 
			//填充校验结果:不可控
			pYkq->cPermissio = 1;
			//填充遥控信息:可控不可控的原因
			string strinfo = "升降为:急停,不进行校验,直接返回;";
			ACE_OS::memcpy(pYkq->szInfo,strinfo.c_str(),strinfo.length());

			//帧长度
			int framlen = 4+4+sizeof(YK_QUESTION);
			char *repdata = new char[framlen];

			//帧头值
			unsigned long int plen = 0x90eb90eb;
			//数据体长度值
			int datalen = 396;

			int pos = 0;
			// 消息头
			ACE_OS::memcpy(repdata+pos,&plen,4);
			pos += 4;

			// 消息数据区长度
			ACE_OS::memcpy(repdata+pos,&datalen,4);
			pos += 4;

			// 消息数据体
			ACE_OS::memcpy(repdata+pos,pYkq,sizeof(YK_QUESTION));

			//通过连接ID的客户端返回校验结果
			ClientHandler* handler = get(connid);
			if (handler != NULL)
			{
				saveQusetion2DB(pYkq->szObjCode,coprtype,strinfo);
				return handler->SendData(repdata,framlen);
			}
			else
			{
				delete []repdata;
				LOG->error("can not find connection :%d,send failed.",connid);
				return -1;
			}

		} 
		else
		{
			//解析服务器向后发送帧,再重新组帧发送给scada
			PBNS::OprationMsg_Response res;
			res.ParseFromArray(data.c_str(),data.length());

			//规则拼接串
			string strRulelist;
			//得到scada请求的校验数据区指针
			YK_QUESTION *pYkq = App_CMService::instance()->pykqust;
			//遥控操作
			int coprtype = (int)pYkq->cOprateType; 
			//返回0校验成功表示可控，非0表示校验失败表示不可控
			if (res.rescode() == 0)
			{
				pYkq->cPermissio = 0;
			}
			else
			{
				pYkq->cPermissio = 1;
			}

			//
			if (res.rulelist_size()>0)	//触发了规则
			{
				for (int i=0;i<res.rulelist_size();i++)
				{
					PBNS::RuleBean bean = res.rulelist(i);
					strRulelist.append(bean.name().c_str());
					strRulelist.append(";");
				}
			}
			else		//未触发规则
			{
				strRulelist = string("未触发任何规则");
			}

			//对请求校验遥控结果赋值遥信信息
			if (strRulelist.length()<256)
			{
				ACE_OS::memcpy(pYkq->szInfo,strRulelist.c_str(),strRulelist.length());
			}
			else
			{
				ACE_OS::memcpy(pYkq->szInfo,strRulelist.c_str(),256);
			}

			//帧长度
			int framlen = 4+4+sizeof(YK_QUESTION);
			char *repdata = new char[framlen];

			//帧头值
			unsigned long int plen = 0x90eb90eb;
			//数据体长度值
			int datalen = 396;

			int pos = 0;
			// 消息头
			ACE_OS::memcpy(repdata+pos,&plen,4);
			pos += 4;

			// 消息数据区长度
			ACE_OS::memcpy(repdata+pos,&datalen,4);
			pos += 4;

			// 消息数据体
			ACE_OS::memcpy(repdata+pos,pYkq,sizeof(YK_QUESTION));

			//通过连接ID的客户端返回校验结果
			ClientHandler* handler = get(connid);
			if (handler != NULL)
			{
				saveQusetion2DB(pYkq->szObjCode,coprtype,strRulelist);
				return handler->SendData(repdata,framlen);
			}
			else
			{
				delete []repdata;
				LOG->error("can not find connection :%d,send failed.",connid);
				return -1;
			}
		}
		

		int tt = 0;
	}
	else
	{
		char * buff = m_pack.encoder(data,msgtype,outLength);

		ClientHandler* handler = get(connid);
		if (handler != NULL)
		{
			return handler->SendData(buff,outLength);
		}
		else
		{
			delete []buff;
			LOG->error("can not find connection :%d,send failed.",connid);
			return -1;
		}
	}
	
}

void ClientMgr::saveQusetion2DB(string cimid,int action,string rulelist)
{
	int ret = -1;
	//取当前时间
	ACE_Date_Time tvTime(ACE_OS::gettimeofday()); 
	char curtime[32];
	sprintf(curtime,"%d-%02d-%02d %d:%d:%d",tvTime.year(),tvTime.month(),tvTime.day(),tvTime.hour(),tvTime.minute(),tvTime.second());

	//保存校验事件到校验日志表:
	string insertsql = DBA->formatSql("INSERT INTO check_logs(unitid,action,ruleid,checktime) VALUES ('%s',%d,'%s','%s')",cimid.c_str(),action,rulelist.c_str(),curtime);
	ret = DBA->execSql(insertsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveQusetion2DB: insert inot table check_logs error insertsql=%s ",insertsql.c_str());

	}
	else
	{
		LOG->message("saveQusetion2DB: insert inot table check_logs success insertsql=%s ",insertsql.c_str());

	}
}