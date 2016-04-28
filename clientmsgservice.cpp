
#include "ace/OS_NS_unistd.h"
#include "clientmsgservice.h"
#include "include/commands.h"
#include "defines.h"

void ClientMsgService::start()
{
	m_stop = false;
	activate();

	LOG->message("client message service started.");
}

void ClientMsgService::stop()
{
	m_stop = true;
}


int ClientMsgService::put(ACE_Message_Block* mb)
{
	return putq(mb);
}

int ClientMsgService::svc()
{
	ACE_Time_Value tv(0,1);
	while(!m_stop)
	{
		ACE_Message_Block* mb ;
		if(getq(mb) != -1)
		{
			// �Խ��յ������󣬽��д���
			parseData(mb);

			mb->release();
		}
		ACE_OS::sleep(tv);
	}
	return 0;
}

void ClientMsgService::parseData(ACE_Message_Block* mb)
{
	if (mb->length() > MAX_PACKET_LEN)
	{
		LOG->error("Invalid data length %d",mb->length());
		return;
	}
	// ���
	sClientMsg* msg = m_pack.decoder(mb->rd_ptr(),mb->length());

	if (msg == NULL)
	{
		return ;
	}
	
	// ��������ID
	msg->connectId = mb->msg_type();

	// ����ҵ���߼�����
	m_biz.exec(msg);

	delete msg;
}
