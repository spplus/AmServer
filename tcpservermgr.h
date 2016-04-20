#ifndef __TCPSERVERMGR_H__
#define __TCPSERVERMGR_H__

#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"
#include "ClientHandler.h"
#include "ReactorTask.h"

typedef ACE_Acceptor<ClientHandler, ACE_SOCK_ACCEPTOR> SOCKAcceptor;

class TcpServerMgr
{
public:
	bool	start();
	void	stop();

private:
	ReactorTask		m_rtTask;
	SOCKAcceptor	m_connector;
};

#endif