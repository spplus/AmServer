﻿#ifndef __RECVTASK_H__
#define __RECVTASK_H__

/************************************************************************/
/* 
	DESC:	作为服务器，接收到的客户端消息处理服务.
	DATE:	2016-04-20
	AUTHOR:	YUANLS
*/
/************************************************************************/


#include "ace/Task.h"
#include "ace/Singleton.h"
#include "packetparser.h"
#include "bizbase.h"

class ClientMsgService :public ACE_Task<ACE_MT_SYNCH>
{
public:
	// 启动服务
	void		start();
	void		stop();

	// 把消息加入到消息队列
	int		put(ACE_Message_Block* mb);

	//连接id
	unsigned int	m_connectId;
	//scada请求校验数据区指针
	YK_QUESTION     *pykqust;

protected:
	int		svc();
	
private:

	// 消息解析，再次进行协议解析，分离出数据区，交由业务处理成进行处理
	void	parseData(ACE_Message_Block* mb);

	//获取cimid对应的设备类型
	int		getTypeByCimid(string cimid);

private:
	bool m_stop;
	BizBase			m_biz;
	PacketParser	m_pack;
};

typedef ACE_Singleton<ClientMsgService, ACE_Recursive_Thread_Mutex> App_CMService;

#endif