#ifndef __CLIENTHANDLER_H__
#define __CLIENTHANDLER_H__

#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"
#include "RecvTask.h"


class ClientHandler :public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> Super;
public:
	virtual	int		open(void*);
	virtual	int		handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
	virtual	int		handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask);
	bool			SendData(const char* data,int length);

private: 
	void			SendBack();

private:
	RecvTask		m_recvtsk;
};

#endif