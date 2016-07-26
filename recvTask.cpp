#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
#include "recvTask.h"
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
	ACE_Time_Value tv(0,1);
	while (!m_stop)
	{
		if (getq(mb) != -1)
		{
			ParseData(mb);
			mb->release();
		}
		ACE_OS::sleep(tv);
	}

	LOG->message("RecvTask exit");

	// ϵͳ�˳�
	close();
	return 0;
}

void RecvTask::ParseData(ACE_Message_Block* mb)
{

	// 1. ���ݽ��
	switch (mb->msg_type())
	{
	case SYS_MSG_CONNECTED:
		{
			//tcp���ӽ�����,��������U֡������Լ����
			App_ScadaClient::instance()->sendUFrame();

			App_ScadaClient::instance()->stopTimer();
		}
		break;
	case SYS_MSG_CLOSED:
		{
			//tcp�Ͽ���,�����������ӷ���˶�ʱ�����رն�ʱ����I֡�Ķ�ʱ��
			App_ScadaClient::instance()->startTimer();

			App_ScadaClient::instance()->stopProTimer();
		}
		
		break;
	default:
		{
			//sClientMsg*	msg = m_pack.decoderS(mb->rd_ptr(),mb->length());
			// ����ҵ��·������
			//m_biz.exec(msg);

			//����������
			App_ScadaClient::instance()->parseDataFrame(mb->rd_ptr(),mb->length());

		}
		
		break;
	}

}