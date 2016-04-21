#include "scadaclientmgr.h"

void ScadaClientMgr::init()
{
	// 读取配置信息
	// ...
	m_svrAddr.set("127.0.0.1",5555);

	// 创建消息接收任务
	m_rTask = new RecvTask();
	m_tcpClient.setRecvTask(m_rTask);

	// 定时器处理器
	m_kpHandler.setClient(&m_tcpClient);

}

void ScadaClientMgr::start()
{
	m_rTask->start();
	m_tcpClient.connect(m_svrAddr);
}

void ScadaClientMgr::close()
{
	m_tcpClient.close();
	m_rTask->close();
}

int ScadaClientMgr::sendData(char* data,int length)
{
	ACE_Message_Block * mb = new ACE_Message_Block(length);
	ACE_OS::memcpy(mb->wr_ptr(),data,length);
	mb->wr_ptr(length);

	return m_tcpClient.send(mb);
}

void ScadaClientMgr::startTimer()
{
	ACE_Time_Value initialDelay (1);
	ACE_Time_Value interval (1000);
	
	ACE_Reactor::instance()->schedule_timer (&m_kpHandler,
		0,
		initialDelay,
		interval);
}

void ScadaClientMgr::stopTimer()
{
	ACE_Reactor::instance()->cancel_timer(&m_kpHandler);
}