
#include "defines.h"
#include "TcpServerMgr.h"

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
		// 2.�����¼�ѭ��
		m_rtTask.start();
		return true;
	}

}

void TcpServerMgr::stop()
{
	// �رնϿ�
	m_connector.close();

	// ֹͣ�¼�ѭ��
	m_rtTask.stop();

}