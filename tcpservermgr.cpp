
#include "defines.h"
#include "TcpServerMgr.h"

void TcpServerMgr::init()
{
	// ���ط���������
	// ...
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
		return true;
	}

}

void TcpServerMgr::stop()
{
	// �رնϿ�
	m_connector.close();
}