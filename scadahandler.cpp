
#include "scadahandler.h"
#include "logger.h"

#include "scadaclientmgr.h"


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

	//获得远程链接地址和端口
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

	char buff[HEADLEN_LEN];
	int len = peer().recv(buff,HEADLEN_LEN,&nowait);
	if (len <= 0)
	{
		return -1;
	}
	if (len != HEADLEN_LEN)
	{
		LOG->error("Invalid packet head length:%d.",len);
		return 0;
	}

	//取第一个字节内容所有帧首字节为：0x68
	int nhead = 0;
	ACE_OS::memcpy(&nhead,buff,FIRST_LEN);
	if (nhead != HEAD_TAG)
	{
		LOG->error("Invalid packet head :%x.",nhead);
		return 0;
	}
	
	//取第二个字节内容，所有帧第二个字节表示帧的数据长度(不是帧长度，帧长度=帧头+帧长度字节+数据帧长度)
	int plen = 0;
	ACE_OS::memcpy(&plen,buff+FIRST_LEN,SECOND_LEN);
	//数据帧长度用一个字节表示的，最大长度为0xFF，数据帧最小长度为4
	if (plen > DATAFRAME_MAX_LEN || plen < DATAFRAME_MIN_LEN)
	{
		LOG->error("Invalid packet data length:%d.",plen);
		return 0;
	}
	
	// 接收包体内容
	ACE_Message_Block* mb = new ACE_Message_Block(plen);

	// 总接收长度
	int total = 0;

	// 继续读取包体内容
	int dlen = peer().recv(mb->wr_ptr(),plen,&nowait);
	if (dlen <= 0)
	{
		mb->release();
		return -1;
	}
	total = dlen;
	mb->wr_ptr(dlen);

	// 如果短读，则继续读取。
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

	// 接收完整包，投递到数据处理队列
	if (total == plen)
	{
		//string msg = ShowMsg(mb->rd_ptr(),mb->length());
		LOG->debug("Recive data length:%d",mb->length());
		LOG->debug("Recive data :%s",mb->rd_ptr());
		ACE_OS::printf("%s\n",mb->rd_ptr());
		if (m_recvTask->putq(mb) == -1)
		{
			App_Logger::instance()->error("Put queue error.");
		}

		/*
		char *data = mb->rd_ptr();
		// 消息标识头
		unsigned char type;
		unsigned char end;
		int pos = 0;
		ACE_OS::memcpy(&type,data+pos,1);
		//
		if (plen == 4 && type == 0x0B)
		{
			App_ScadaClient::instance()->startProTimer();
		}*/
		

	}
	else
	{
		mb->release();
		// 接收包长度错误
		LOG->error("Invalid packet length.packet length %d,recive %d.",plen,total);
	}

/*

	
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

	// 解析出数据包长度
	int plen = 0;
	ACE_OS::memcpy(&plen,buff,FRAME_HEAD_LEN);
	
	// 判断数据包长度是否非法
	if (plen > MAX_PACKET_LEN || plen <0)
	{
		LOG->error("Invalid packet length:%d.",plen);
		return 0;
	}
	// 接收包体内容
	ACE_Message_Block* mb = new ACE_Message_Block(plen);

	// 总接收长度
	int total = 0;

	// 继续读取包体内容
	int dlen = peer().recv(mb->wr_ptr(),plen,&nowait);
	if (dlen <= 0)
	{
		mb->release();
		return -1;
	}
	total = dlen;
	mb->wr_ptr(dlen);

	// 如果短读，则继续读取。
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

	// 接收完整包，投递到数据处理队列
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
		// 接收包长度错误
		LOG->error("Invalid packet length.packet length %d,recive %d.",plen,total);
	}*/
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

		// 释放队列资源
		this->wait();

		// 关闭链路
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
	//发送数据的总长度
	int nSendLen = (int)pmblk->length();   
	int nIsSendSize = 0;

	//循环发送，直到数据发送完成。
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
				//如果发送堵塞，则等10毫秒后再发送。
				ACE_Time_Value tvSleep(0, 10 * 1000);
				ACE_OS::sleep(tvSleep);
				continue;
			}

			LOG->error("[ClientHandle::SendData]  error = %d.\n",errno);
			pmblk->release();
			return false;
		}
		// 一次性发送完一包数据
		else if(nDataLen >= nSendLen)   
		{
			pmblk->release();
			return true;
		}
		// 分多次发送
		else
		{
			pmblk->rd_ptr(nDataLen);
			nIsSendSize  += nDataLen;

			// 多次发送完毕
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
		LOG->debug("投递消息到接收队列失败");
	}

}

bool ScadaHandler::isConnected()
{
	return m_isConnected;
}

