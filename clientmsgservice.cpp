
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
			// 对接收到的请求，进行处理
			parseData(mb);

			mb->release();
		}
		
	}
	return 0;
}

void ClientMsgService::parseData(ACE_Message_Block* mb)
{
	// 解包
	sClientMsg* msg = m_pack.decoder(mb->rd_ptr(),mb->length());

	// 设置连接ID
	msg.connectId = mb->msg_type();

	// 调用业务逻辑处理
	m_biz.exec(outMsgType,msg,outLength);

	delete []msg;

	
}
