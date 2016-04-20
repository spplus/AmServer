
#include "ace/OS_NS_unistd.h"
#include "RecvTask.h"
#include "MsgHandler.h"
#include "commands.h"


void RecvTask::start()
{
	m_stop = false;
	activate();
}

void RecvTask::stop()
{
	m_stop = true;
}

int RecvTask::svc()
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

void RecvTask::parseData(ACE_Message_Block* mb)
{
	int outMsgType = 0;
	int outLength = 0;
	char* msg = HPCoder::instance()->decoderC(mb->rd_ptr(),mb->length(),outMsgType,outLength);
	switch (outMsgType)
	{
	case CMD_USER_LONGIN:		
		
		break;

	default:
		break;
	}

	delete []msg;

	
}
