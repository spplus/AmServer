/************************************************************************/
/* 
		DESC:	解析规约管理类.
		DATE:	2016-07-03
		AUTHOR:	ABELSKY
*/
/************************************************************************/

#ifndef __PROTOCLMGR_H__
#define __PROTOCLMGR_H__

#include "ace/Event_Handler.h"
#include "tcpClient.h"
#include "dbaccess.h"

class ProtocolMgr: public ACE_Event_Handler
{
public:
	void	setClient(TcpClient *client);
	int		handle_timeout (const ACE_Time_Value &current_time,const void * = 0);

	//设置RTU地址
	void	setRtuAddr(unsigned short nRtuAddr);
	//设置定时周期
	void	setCallAll(int nAll);
	//发送U帧建立链接
	void	sendUFrame();
	//发送总召唤I帧
	void	sendIFrame();
	//发送确认S帧
	void	sendSFrame();
	//发送测试帧
	void	sendTestFrame();
	//发送确认测试帧
	void	sendTestConFrame();

	//数据区数据解析
	void	parseDataFrame(char *data,int datalength);

	//定时发送I帧定时器
	void	startITimer();
	void	stopITimer();

	//初始化数据库取数据
	void	initDBCimid();

	//取数据库中cimid和序号对应关系
	void	getCimidSerialnum();

	//解析数据后处理对应关系
	void	setDataRelation();

	//更新数据到数据库
	void	updateData2DB();

private:
	TcpClient *m_client;

	unsigned short m_RtuAddr;
	//定时周期
	int m_CallALL;
	//接收帧标准
	bool brcvflag;
	//接收到数据项个数
	int m_nRecvDataSum;

	//保存数据库中cimid和序号对应关系
	CIMID_NUM_N_LIST	m_cimid_num_n_list;
	CIMID_NUM_C_LIST	m_cimid_num_c_list;

	YXPOINT_VAL_LIST	m_yxpointval_list;
	YXCIMID_VAL_LIST    m_yxcimidval_list;

};

// 获取信息体地址
inline unsigned int getInfoAddr(unsigned char *pAddr)
{
	unsigned int nAddr=0;
	unsigned char szAddr[4]={0};
	szAddr[0]=pAddr[0];szAddr[1]=pAddr[1];szAddr[2]=pAddr[2];
	memcpy(&nAddr,szAddr,sizeof(unsigned int));

	return nAddr;
}

#endif
