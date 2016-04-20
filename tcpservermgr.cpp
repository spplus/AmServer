
#include "defines.h"
#include "TcpServerMgr.h"

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
		// 2.启动事件循环
		m_rtTask.start();
		return true;
	}

}

void TcpServerMgr::stop()
{
	// 关闭断口
	m_connector.close();

	// 停止事件循环
	m_rtTask.stop();

}