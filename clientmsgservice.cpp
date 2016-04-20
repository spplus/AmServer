
#include "ace/OS_NS_unistd.h"
#include "clientmsgservice.h"
#include "commands.h"


void ClientMsgService::start()
{
	m_stop = false;
	activate();
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
	while(!m_stop)
	{
		ACE_Message_Block* mb ;
		if(getq(mb) != -1)
		{
			// �Խ��յ������󣬽��д���
			parseData(mb);

			mb->release();
		}
		
	}
	return 0;
}

void ClientMsgService::parseData(ACE_Message_Block* mb)
{
	// ���
	sClientMsg* msg = m_pack.decoder(mb->rd_ptr(),mb->length());

	// ��������ID
	msg.connectId = mb->msg_type();

	// ����ҵ���߼�����
	m_biz.exec(outMsgType,msg,outLength);

	delete []msg;

	
}
