
#ifndef __RECVTASK_H__
#define __RECVTASK_H__
#include "ace/Task.h"
#include "packetparser.h"
#include "bizbase.h"

class RecvTask	:public ACE_Task<ACE_MT_SYNCH>
{
public:
	// ���캯��
	RecvTask();
	~RecvTask();

	// �����߳�
	void	start();

	// �ر��߳�
	void	close();

	// �߳����
	int		svc();

private:
	// ��������
	void	ParseData(ACE_Message_Block* mb);

private:
	
	BizBase			m_biz;
	bool			m_stop;
	PacketParser	m_pack;
};


#endif