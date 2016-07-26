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
#include "dbaccess.h"
#include "confmgr.h"
#include "cimtask.h"

int ACE_TMAIN (int, ACE_TCHAR *[]) 
{
	ACE::init();

	// 启动日志
	App_Logger::instance()->load_config(CONFIG_FILE);
	App_Logger::instance()->set_logname(LOGNAME);
	App_Logger::instance()->open_logger();

	// 加载服务器配置
	if (!App_Config::instance()->load(SERVER_CONFIG))
	{
		LOG->error("加载服务器配置失败，服务器自动退出.");
		return 0;
	}
	// 启动CIM 入库检查线程
	App_CIMTask::instance()->start();

	// 初始化数据库连接
	App_Dba::instance()->init();

	// 启动服务器
	App_TcpServer::instance()->init();
	App_TcpServer::instance()->start();

	// 启动客户端连接
	App_ScadaClient::instance()->init();
	App_ScadaClient::instance()->start();

	// 启动事件循环
	ACE_Reactor::instance()->run_reactor_event_loop();
	
	ACE::fini();
	return 0;
}