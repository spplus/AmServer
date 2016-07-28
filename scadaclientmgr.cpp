#include "scadaclientmgr.h"
#include "confmgr.h"

void ScadaClientMgr::init()
{
	// 读取配置信息
	int port = ACE_OS::atoi(App_Config::instance()->getValue(SERVER_ROOT,"ScadaPort").c_str());
	string addr = App_Config::instance()->getValue(SERVER_ROOT,"ScadaAddr");

	//加载总召唤发送命令周期
	int	nAllCall =  ACE_OS::atoi(App_Config::instance()->getValue(SERVER_ROOT,"MAllCall").c_str());

	//总召唤RTU地址
	int nrtu = ACE_OS::atoi(App_Config::instance()->getValue(SERVER_ROOT,"RtuAddr").c_str());
	unsigned short nRtuaddr = nrtu;

	m_svrAddr.set(port,addr.c_str());

	// 创建消息接收任务
	m_rTask = new RecvTask();
	m_tcpClient.setRecvTask(m_rTask);

	// 定时器处理器
	m_kpHandler.setClient(&m_tcpClient);

	//总召唤发送命令定时器
	m_proMgr.setClient(&m_tcpClient);

	//设置总召唤RTU地址
	m_proMgr.setRtuAddr(nRtuaddr);

	//设置定时周期
	m_proMgr.setCallAll(nAllCall);

	//初始化数据库中数据取数据
	m_proMgr.initDBCimid();

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


void ScadaClientMgr::sendUFrame()
{
	m_proMgr.sendUFrame();
}

void ScadaClientMgr::parseDataFrame(char *data,int datalength)
{
	m_proMgr.parseDataFrame(data,datalength);
}

void ScadaClientMgr::startProTimer()
{
	m_proMgr.startITimer();
}


void ScadaClientMgr::stopProTimer()
{
	m_proMgr.stopITimer();
}



