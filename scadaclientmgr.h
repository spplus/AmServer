/************************************************************************/
/* 
		DESC:	连接SCADA客户端管理器.
		DATE:	2016-04-21
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __SCADACLIENTMGR_H__
#define __SCADACLIENTMGR_H__

#include "ace/Connector.h"
#include "ace/SOCK_Connector.h"
#include "ace/Singleton.h"
#include "scadahandler.h"
#include "recvTask.h"
#include "keepAliveHandler.h"
#include "protocolmgr.h"
#include "tcpClient.h"


class ScadaClientMgr
{
public:
	// 初始化客户端相关配置
	void	init();

	// 启动客户端连接
	void	start();

	// 关闭连接
	void	close();

	// 发送数据
	int		sendData(char* data,int length);

	void	startTimer();
	void	stopTimer();

	//发送总召唤定时器启动、停止
	void	startProTimer();
	void	stopProTimer();

	//发送U帧
	void	sendUFrame();
	//数据区数据解析
	void	parseDataFrame(char *data,int datalength);

private:
	RecvTask*			m_rTask;
	TcpClient			m_tcpClient;
	KeepAliveHandler	m_kpHandler;
	ProtocolMgr			m_proMgr;
	ACE_INET_Addr		m_svrAddr;
};
typedef ACE_Singleton<ScadaClientMgr, ACE_Recursive_Thread_Mutex>  App_ScadaClient;
#endif