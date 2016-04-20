
#include "ClientMgr.h"
ClientMgr::ClientMgr()
{
	m_connectId = 1000;
}

unsigned int ClientMgr::add(ClientHandler* client)
{
	m_connectId++;
	m_clientList.insert( CMAP::value_type(m_connectId, client));
}

void ClientMgr::del(unsigned int connid)
{
	m_clientList.erase(m_clientList.find(connid));
}

ClientHandler* ClientMgr::get(unsigned int connid)
{
	CMAP::iterator iter = m_clientList.find(connid);
	if (iter != m_clientList.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}