#ifndef __CLIENTMGR_H__
#define __CLIENTMGR_H__
#include <list>
#include <map>

#include "ace/Singleton.h"
#include "ClientHandler.h"

using namespace std;

class ClientMgr
{
public:
	void add(ClientHandler* client);
	void del(ClientHandler* client);
	ClientHandler* get();
private:
	list<ClientHandler*>	m_clientList;
};

typedef ACE_Singleton<ClientMgr, ACE_Recursive_Thread_Mutex>  App_ClientMgr;

#endif