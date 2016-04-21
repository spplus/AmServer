/************************************************************************/
/* 
		DESC:	����SCADA�ͻ��˹�����.
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
#include "tcpClient.h"

class ScadaClientMgr
{
public:
	// ��ʼ���ͻ����������
	void	init();

	// �����ͻ�������
	void	start();

	// �ر�����
	void	close();

	// ��������
	int		sendData(char* data,int length);

	void	startTimer();
	void	stopTimer();

private:
	RecvTask*			m_rTask;
	TcpClient			m_tcpClient;
	KeepAliveHandler	m_kpHandler;
	ACE_INET_Addr		m_svrAddr;
};
typedef ACE_Singleton<ScadaClientMgr, ACE_Recursive_Thread_Mutex>  App_ScadaClient;
#endif