#ifndef __RECVTASK_H__
#define __RECVTASK_H__

/************************************************************************/
/* 
	DESC:	��Ϊ�����������յ��Ŀͻ�����Ϣ�������.
	DATE:	2016-04-20
	AUTHOR:	YUANLS
*/
/************************************************************************/


#include "ace/Task.h"
#include "ace/Singleton.h"
#include "packetparser.h"
#include "bizbase.h"

class ClientMsgService :public ACE_Task<ACE_MT_SYNCH>
{
public:
	// ��������
	void		start();
	void		stop();

	// ����Ϣ���뵽��Ϣ����
	int		put(ACE_Message_Block* mb);

protected:
	int		svc();
	
private:

	// ��Ϣ�������ٴν���Э������������������������ҵ����ɽ��д���
	void	parseData(ACE_Message_Block* mb);

private:
	bool m_stop;
	BizBase			m_biz;
	PacketParser	m_pack;
};

typedef ACE_Singleton<ClientMsgService, ACE_Recursive_Thread_Mutex> App_CMService;

#endif