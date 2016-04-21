
#include "scadahandler.h"
#include "logger.h"

ScadaHandler::ScadaHandler()
{
	m_isConnected = false;
}

void ScadaHandler::setRecvTask(RecvTask* task)
{
	m_recvTask = task;
}

int ScadaHandler::open(void*p)
{
	int nRet = Super::open();
	if(nRet != 0)
	{
		LOG->error("Clienthandle open() error [%d].",nRet);
		return -1;
	}
	ACE_INET_Addr svraddr;

	//���Զ�����ӵ�ַ�Ͷ˿�
	if(this->peer().get_remote_addr(svraddr) == -1)
	{
		LOG->error("this->peer().get_remote_addr error.");
		return -1;
	}
	m_isConnected = true;

	connectionNotification(SYS_MSG_CONNECTED);

	LOG->message("Connection to [%s:%d] OK.",svraddr.get_host_addr(), svraddr.get_port_number());
	return 0;
}

int ScadaHandler::handle_input(ACE_HANDLE fd )
{
 	ACE_Time_Value     nowait(RECV_TIMEOUT);
	if(fd == ACE_INVALID_HANDLE)
	{
		App_Logger::instance()->error("fd == ACE_INVALID_HANDLE.");
		return -1;
	}
	char buff[FRAME_HEAD_LEN];
	int len = peer().recv(buff,FRAME_HEAD_LEN,&nowait);
	if (len <= 0)
	{
		return -1;
	}
	if (len != FRAME_HEAD_LEN)
	{
		LOG->error("Invalid packet head length:%d.",len);
		return 0;
	}

	// ���������ݰ�����
	int plen = 0;
	ACE_OS::memcpy(&plen,buff,FRAME_HEAD_LEN);
	
	// �ж����ݰ������Ƿ�Ƿ�
	if (plen > MAX_PACKET_LEN || plen <0)
	{
		LOG->error("Invalid packet length:%d.",plen);
		return 0;
	}
	// ���հ�������
	ACE_Message_Block* mb = new ACE_Message_Block(plen);

	// �ܽ��ճ���
	int total = 0;

	// ������ȡ��������
	int dlen = peer().recv(mb->wr_ptr(),plen,&nowait);
	if (dlen <= 0)
	{
		mb->release();
		return -1;
	}
	total = dlen;
	mb->wr_ptr(dlen);

	// ����̶����������ȡ��
	while(total < plen)
	{
		dlen = peer().recv(mb->wr_ptr(),plen-total,&nowait);
		if (dlen<=0)
		{
			mb->release();
			return -1;
		}
		mb->wr_ptr(dlen);
		total += dlen;
	}

	// ������������Ͷ�ݵ����ݴ������
	if (total == plen)
	{
		//string msg = ShowMsg(mb->rd_ptr(),mb->length());
		LOG->debug("Recive data length:%d.",mb->length());
		if (m_recvTask->putq(mb) == -1)
		{
			App_Logger::instance()->error("Put queue error.");
		}
		
	}
	else
	{
		mb->release();
		// ���հ����ȴ���
		LOG->error("Invalid packet length.packet length %d,recive %d.",plen,total);
	}
	return 0;
}

int ScadaHandler::handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask)
{
	if (mask == ACE_Event_Handler::WRITE_MASK)
	{
		return 0;
	}
	else
	{
		ACE_TCHAR peer_name[64];
		ACE_INET_Addr peer_addr;
		if (this->peer().get_remote_addr(peer_addr) == 0 && peer_addr.addr_to_string (peer_name, 64) == 0)
		{
			LOG->message("Connection closed:%s.",peer_name);
		}
		m_isConnected = false;
		connectionNotification(SYS_MSG_CLOSED);

		// �ͷŶ�����Դ
		this->wait();

		// �ر���·
		this->peer().close_reader();
		this->peer().close_writer();
		this->peer().close();

		this->closing_ = true;

		return Super::handle_close (h, mask);
	}
	return 0;
}

bool ScadaHandler::sendData(ACE_Message_Block* pmblk)
{
	ACE_Time_Value     nowait(SEND_TIMEOUT);

	if(NULL == pmblk)
	{
		LOG->warn("Invalid message.");
		return false;
	}

	if(get_handle() == ACE_INVALID_HANDLE)
	{
		LOG->warn("get_handle() == ACE_INVALID_HANDLE.");
		pmblk->release();
		return false;
	}

	char* pData = pmblk->rd_ptr();
	if(NULL == pData)
	{
		LOG->warn("[ClientHandle::SendData]  pData is NULL.");
		pmblk->release();
		return false;
	}
	//�������ݵ��ܳ���
	int nSendLen = (int)pmblk->length();   
	int nIsSendSize = 0;

	//ѭ�����ͣ�ֱ�����ݷ�����ɡ�
	while(nIsSendSize < nSendLen)
	{
		if(nSendLen <= 0)
		{
			LOG->error("nCurrSendSize error is %d.",nSendLen);
			pmblk->release();
			return false;
		}

		int nDataLen = this->peer().send(pmblk->rd_ptr(), nSendLen, &nowait);
		int nErr = ACE_OS::last_error();
		if(nDataLen <= 0)
		{
			if(nErr == EWOULDBLOCK)
			{
				//������Ͷ��������10������ٷ��͡�
				ACE_Time_Value tvSleep(0, 10 * 1000);
				ACE_OS::sleep(tvSleep);
				continue;
			}

			LOG->error("[ClientHandle::SendData]  error = %d.\n",errno);
			pmblk->release();
			return false;
		}
		// һ���Է�����һ������
		else if(nDataLen >= nSendLen)   
		{
			pmblk->release();
			return true;
		}
		// �ֶ�η���
		else
		{
			pmblk->rd_ptr(nDataLen);
			nIsSendSize  += nDataLen;

			// ��η������
			if (nIsSendSize == nSendLen)
			{
				pmblk->release();
				return true;
			}
		}
	}
	return true;
}

void ScadaHandler::connectionNotification(int type)
{
	ACE_Message_Block* mb = new ACE_Message_Block(1);
	mb->msg_type(type);
	if (m_recvTask->putq(mb) == -1)
	{
		LOG->debug("Ͷ����Ϣ�����ն���ʧ��");
	}

}

bool ScadaHandler::isConnected()
{
	return m_isConnected;
}

