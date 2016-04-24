
#include "defines.h"
#include "tcpservermgr.h"
#include "clientmsgservice.h"

void TcpServerMgr::init()
{
	// ���ط���������
	// ...
	LOG->message("load server config ok.");
}

bool TcpServerMgr::start()
{
	// 1.��������
	ACE_INET_Addr port_to_listen(SVR__PORT);
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
