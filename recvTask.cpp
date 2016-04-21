#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "recvtask.h"
#include "scadaclientmgr.h"
#include "structs.h"

RecvTask::RecvTask()
{
	m_stop = false;
}

RecvTask::~RecvTask()
{
	
}

void RecvTask::start()
{
	activate();
}


void RecvTask::close()
{
	m_stop = true;

	LOG->message("Close reactor.");

}
int RecvTask::svc()
{
	LOG->message("RecvTask start ok.");
	ACE_Message_Block* mb = NULL;
	ACE_Time_Value tv(0,10);
	while (!m_stop)
	{
		if (getq(mb,&tv) != -1)
		{
			// 判断是否为退出消息
			if (mb->msg_type() == ACE_Message_Block::MB_STOP)
			{
				mb->release();
				break;
			}
	
			ParseData(mb);
			mb->release();
		}
		ACE_OS::sleep(tv);
	}

	LOG->message("RecvTask exit");

	// 系统退出
	close();
	return 0;
}

void RecvTask::ParseData(ACE_Message_Block* mb)
{

	// 1. 数据解包
	switch (mb->msg_type())
	{
	case SYS_MSG_CONNECTED:
		App_ScadaClient::instance()->stopTimer();
		break;
	case SYS_MSG_CLOSED:
		App_ScadaClient::instance()->startTimer();
		break;
	default:
		{
			sClientMsg*	msg = m_pack.decoderS(mb->rd_ptr(),mb->length());

			// 调用业务路径处理
			m_biz.exec(msg);

		}
		
		break;
	}

}