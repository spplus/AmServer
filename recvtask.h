#ifndef __RECVTASK_H__
#define __RECVTASK_H__

#include "ace/Task.h"

class RecvTask :public ACE_Task<ACE_MT_SYNCH>
{
public:
	void start();
	void stop();
	int  svc();
	
private:
	void	parseData(ACE_Message_Block* mb);

private:
	bool m_stop;
};

#endif