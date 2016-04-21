/************************************************************************/
/* 
	DESC:	���������.
	DATE:	2016-04-20
	AUTHOR:	YUANLS	
*/
/************************************************************************/
#include "defines.h"
#include "logger.h"
#include "tcpservermgr.h"
#include "scadaclientmgr.h"

int ACE_TMAIN (int, ACE_TCHAR *[]) 
{
	ACE::init();

	// ������־
	App_Logger::instance()->load_config(CONFIG_FILE);
	App_Logger::instance()->set_logname(LOGNAME);
	App_Logger::instance()->open_logger();

	// ����������
	App_TcpServer::instance()->init();
	App_TcpServer::instance()->start();

	// �����ͻ�������
	App_ScadaClient::instance()->init();
	App_ScadaClient::instance()->start();


	// ��ʼ�����ݿ�����

	// �����¼�ѭ��
	ACE_Reactor::instance()->run_reactor_event_loop();
	//App_Reactor::instance()->start();

	ACE::fini();
	return 0;
}