
#include "defines.h"
#include "tcpservermgr.h"
#include "clientmsgservice.h"
#include "confmgr.h"

void TcpServerMgr::init()
{
	// ���ط���������
	m_port = ACE_OS::atoi(App_Config::instance()->getValue(SERVER_ROOT,"ListenPort").c_str());
	
	LOG->message("load server config ok.");
}

bool TcpServerMgr::start()
{
	// 1.��������
	ACE_INET_Addr port_to_listen(m_port);
	if (m_connector.open(port_to_listen) == -1)
	{
		return false;
	}
	else
	{
		LOG->message("server started on port:%d",port_to_listen.get_port_number());

		// �����ͻ�����Ϣѭ��
		App_CMService::instance()->start();

		return true;
	}

}

void TcpServerMgr::stop()
{
	// �رնϿ�
	m_connector.close();

	// �رտͻ�����Ϣѭ��
	App_CMService::instance()->stop();
}