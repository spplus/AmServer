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
#include "dbaccess.h"
#include "confmgr.h"

int ACE_TMAIN (int, ACE_TCHAR *[]) 
{
	ACE::init();

	// ������־
	App_Logger::instance()->load_config(CONFIG_FILE);
	App_Logger::instance()->set_logname(LOGNAME);
	App_Logger::instance()->open_logger();

	// ���ط���������
	if (!App_Config::instance()->load(SERVER_CONFIG))
	{
		LOG->error("���ط���������ʧ�ܣ��������Զ��˳�.");
		return 0;
	}


	// ����������
	App_TcpServer::instance()->init();
	App_TcpServer::instance()->start();

	// �����ͻ�������
	App_ScadaClient::instance()->init();
	App_ScadaClient::instance()->start();


	// ��ʼ�����ݿ�����
	App_Dba::instance()->init();

	// �����¼�ѭ��
	ACE_Reactor::instance()->run_reactor_event_loop();
	
	ACE::fini();
	return 0;
}