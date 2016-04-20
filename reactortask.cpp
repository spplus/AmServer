
#include "ace/Reactor.h"
#include "ReactorTask.h"

void ReactorTask::start()
{
	this->activate();
	m_reactor = ACE_Reactor::instance();
}

int ReactorTask::svc()
{
	if (m_reactor != NULL)
	{
		m_run = true;
		m_reactor->run_reactor_event_loop();
	}

	return 0;
}

void ReactorTask::stop()
{
	if (m_run && m_reactor != NULL)
	{
		m_reactor->end_reactor_event_loop();
	}

}