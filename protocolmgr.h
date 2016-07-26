/************************************************************************/
/* 
		DESC:	������Լ������.
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

	//����RTU��ַ
	void	setRtuAddr(unsigned short nRtuAddr);
	//���ö�ʱ����
	void	setCallAll(int nAll);
	//����U֡��������
	void	sendUFrame();
	//�������ٻ�I֡
	void	sendIFrame();
	//����ȷ��S֡
	void	sendSFrame();
	//���Ͳ���֡
	void	sendTestFrame();
	//����ȷ�ϲ���֡
	void	sendTestConFrame();

	//���������ݽ���
	void	parseDataFrame(char *data,int datalength);

	//��ʱ����I֡��ʱ��
	void	startITimer();
	void	stopITimer();

	//��ʼ�����ݿ�ȡ����
	void	initDBCimid();

	//ȡ���ݿ���cimid����Ŷ�Ӧ��ϵ
	void	getCimidSerialnum();

	//�������ݺ����Ӧ��ϵ
	void	setDataRelation();

	//�������ݵ����ݿ�
	void	updateData2DB();

private:
	TcpClient *m_client;

	unsigned short m_RtuAddr;
	//��ʱ����
	int m_CallALL;
	//����֡��׼
	bool brcvflag;
	//���յ����������
	int m_nRecvDataSum;

	//�������ݿ���cimid����Ŷ�Ӧ��ϵ
	CIMID_NUM_N_LIST	m_cimid_num_n_list;
	CIMID_NUM_C_LIST	m_cimid_num_c_list;

	YXPOINT_VAL_LIST	m_yxpointval_list;
	YXCIMID_VAL_LIST    m_yxcimidval_list;

};

// ��ȡ��Ϣ���ַ
inline unsigned int getInfoAddr(unsigned char *pAddr)
{
	unsigned int nAddr=0;
	unsigned char szAddr[4]={0};
	szAddr[0]=pAddr[0];szAddr[1]=pAddr[1];szAddr[2]=pAddr[2];
	memcpy(&nAddr,szAddr,sizeof(unsigned int));

	return nAddr;
}

#endif
