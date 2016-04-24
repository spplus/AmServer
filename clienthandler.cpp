
#include "ace/OS_NS_unistd.h"
#include "ace/Time_Value.h"
#include "ace/Date_Time.h"
#include "ace/Addr.h"

#include "defines.h"
#include "clienthandler.h"
#include "clientmgr.h"
#include "clientmsgservice.h"

//#define RECV_TIMEOUT	100
//#define SEND_TIMEOUT	100
//#define FRAME_HEAD_LEN	4
//#define MAX_PACKET_LEN	1024*1024*1024

int ClientHandler::open(void*p)
{
	int nRet = Super::open();
	if(nRet != 0)
	{
		return -1;
	}
	ACE_INET_Addr svraddr;

	//���Զ�����ӵ�ַ�Ͷ˿�
	if(this->peer().get_remote_addr(svraddr) == -1)
	{
		LOG->warn(" get_remote_addr failed!");
		return -1;
	}

	LOG->message("new client connection��%s",svraddr.get_host_addr());

	// ����ͻ�������
	m_connectId = App_ClientMgr::instance()->add(this);

	return 0;
}

int ClientHandler::handle_input(ACE_HANDLE fd )
{
 	ACE_Time_Value     nowait(RECV_TIMEOUT);
	if(fd == ACE_INVALID_HANDLE)
	{
		return -1;
	}
	char buff[FRAME_HEAD_LEN];
	int len = peer().recv(buff,FRAME_HEAD_LEN,&nowait);
	if (len <= 0)
	{
		LOG->message("recv length 0,connection closed.");
		return -1;
	}
	if (len != FRAME_HEAD_LEN)
	{
		LOG->warn("invalid frame head len:%d",len);
		return 0;
	}

	// ��������ݰ��
	int plen = 0;
	ACE_OS::memcpy(&plen,buff,FRAME_HEAD_LEN);
	
	// �ж���ݰ���Ƿ�Ƿ�
	if (plen > MAX_PACKET_LEN || plen <0)
	{
		LOG->warn("invalid packet length:%d",plen);
		return 0;
	}
	// ���հ�������
	ACE_Message_Block* mb = new ACE_Message_Block(plen);

	// �ܽ��ճ���
	int total = 0;

	// �����ȡ��������
	int dlen = peer().recv(mb->wr_ptr(),plen,&nowait);
	if (dlen <= 0)
	{
		LOG->warn("recv packet failed,recived :%d",dlen);
		mb->release();
		return -1;
	}
	total = dlen;
	mb->wr_ptr(dlen);

	// ���̶���������ȡ��
	while(total < plen)
	{
		ACE_DEBUG((LM_DEBUG,"[%D]total:%d,this length:%d,read:%d.\n",plen,dlen,total));
		dlen = peer().recv(mb->wr_ptr(),plen-total,&nowait);
		if (dlen<=0)
		{
			mb->release();
			return -1;
		}
		mb->wr_ptr(dlen);
		total += dlen;
	}

	// ���������Ͷ�ݵ���ݴ������
	if (total == plen)
	{
		// ����Ϣ���������ID,��ҵ��㴦�������ԭ·����
		mb->msg_type(m_connectId);
		if(App_CMService::instance()->putq(mb) != -1)
		{
			LOG->debug("put one packet to message queue.");
		}
	}
	else
	{
		mb->release();

		// ���հ�ȴ���
		LOG->error("Invalid packet length.packet length %d,recive %d.",plen,total);
	}
	return 0;
}

int ClientHandler::handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask)
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
			ACE_DEBUG((LM_DEBUG,"[%D]Connection closed:%s.\n",peer_name));
			LOG->debug("Connection closed:%s.",peer_name);
		}

		// �ͷŶ�����Դ
		this->wait();

		// �ر���·
		this->peer().close_reader();
		this->peer().close_writer();
		this->peer().close();

		this->closing_ = true;

		// ɾ��ͻ������Ӷ���
		App_ClientMgr::instance()->del(m_connectId);

		// ���ӶϿ���֪ͨ�����߳��˳�
		/*ACE_Message_Block* mb = new ACE_Message_Block(1);
		mb->msg_type(ACE_Message_Block::MB_STOP);
		m_checktsk->put(mb);*/
	
		return Super::handle_close (h, mask);
	}
	return 0;
}

bool ClientHandler::SendData(const char* data,int length)
{
	ACE_Time_Value     nowait(SEND_TIMEOUT);

	if(NULL == data )
	{
		return false;
	}

	if(get_handle() == ACE_INVALID_HANDLE)
	{
		delete []data;
		return false;
	}

	const char* pData = data;
	
	//������ݵ��ܳ���
	int nSendLen = length;

	int nIsSendSize = 0;

	//ѭ�����ͣ�ֱ����ݷ�����ɡ�
	while(nIsSendSize < nSendLen)
	{

		int nDataLen = this->peer().send(data+nIsSendSize, nSendLen, &nowait);
		int nErr = ACE_OS::last_error();
		if(nDataLen <= 0)
		{
			if(nErr == EWOULDBLOCK)
			{
				//����Ͷ������10������ٷ��͡�
				ACE_Time_Value tvSleep(0, 10 * 1000);
				ACE_OS::sleep(tvSleep);
				continue;
			}
			delete []data;
			return false;
		}
		// һ���Է�����һ�����
		else if(nDataLen >= nSendLen)   
		{
			delete []data;
			return true;
		}
		// �ֶ�η���
		else
		{
			nIsSendSize  += nDataLen;
			nSendLen -= nDataLen;

			// ��η������
			if (nIsSendSize == nSendLen)
			{
				delete []data;
				return true;
			}
		}
	}
	return true;
}

void ClientHandler::SendBack()
{
	ACE_Date_Time tvTime(ACE_OS::gettimeofday()); 
	std::string ret;
	char temp[32];
	sprintf(temp,"%d-%02d-%02d %d:%d:%d %d",tvTime.year(),tvTime.month(),tvTime.day(),tvTime.hour(),tvTime.minute(),tvTime.second(),tvTime.microsec());
	ret.append(temp);

	int len = ret.length();
	ACE_Message_Block* mb = new ACE_Message_Block(len+4);
	
	ACE_OS::memcpy(mb->wr_ptr(),&len,4);
	mb->wr_ptr(4);

	ACE_OS::memcpy(mb->wr_ptr(),ret.c_str(),len);
	mb->wr_ptr(len);

	SendData(mb->rd_ptr(),mb->length());

}
