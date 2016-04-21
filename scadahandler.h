
#ifndef __CLIENTHANDLE_H__
#define __CLIENTHANDLE_H__


#include "ace/Reactor.h"
#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include "defines.h"
#include "recvTask.h"

using namespace std;

class ScadaHandler	:public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> Super;
public:
	ScadaHandler();
	virtual	int		open(void*);
	virtual	int		handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
	virtual	int		handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask);
	
	void			setRecvTask(RecvTask*);
	bool			sendData(ACE_Message_Block* pmblk);
	bool			isConnected();
private:
	void			connectionNotification(int type);

private:

	RecvTask*	m_recvTask;
	bool		m_isConnected;

};

#endif