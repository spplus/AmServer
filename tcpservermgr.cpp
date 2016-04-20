
#include "defines.h"
#include "TcpServerMgr.h"

void TcpServerMgr::init()
{
	// 加载服务器配置
	// ...
}

bool TcpServerMgr::start()
{
	// 1.启动侦听
	ACE_INET_Addr port_to_listen(SVR__PORT);
	if (m_connector.open(port_to_listen) == -1)
	{
		return false;
	}
	else
	{
		return true;
	}

}

void TcpServerMgr::stop()
{
	// 关闭断口
	m_connector.close();
}