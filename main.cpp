/************************************************************************/
/* 
	DESC:	服务器入口.
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

	// 启动日志
	App_Logger::instance()->load_config(CONFIG_FILE);
	App_Logger::instance()->set_logname(LOGNAME);
	App_Logger::instance()->open_logger();

	// 启动服务器
	App_TcpServer::instance()->init();
	App_TcpServer::instance()->start();

	// 启动客户端连接
	App_ScadaClient::instance()->init();
	App_ScadaClient::instance()->start();


	// 初始化数据库连接

	// 启动事件循环
	ACE_Reactor::instance()->run_reactor_event_loop();
	//App_Reactor::instance()->start();

	ACE::fini();
	return 0;
}