#ifndef __REACTORTASK_H__
#define __REACTORTASK_H__

#include "ace/Task.h"
#include "ace/Reactor.h"

class ReactorTask :public ACE_Task<ACE_MT_SYNCH>
{
public:
	void	start();
	int		svc();
	void	stop();
private:
	bool			m_run;
	ACE_Reactor*	m_reactor;
};

#endif