
#include "ClientMgr.h"

void ClientMgr::add(ClientHandler* client)
{
	m_clientList.push_back(client);
}

void ClientMgr::del(ClientHandler* client)
{
	m_clientList.remove(client);
}

ClientHandler* ClientMgr::get()
{
	return m_clientList.back();
}