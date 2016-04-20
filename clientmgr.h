/************************************************************************/
/* 
	DESC:	保持客户端连接列表.
	DATE:	2016-04-20
	AUTHOR:	YUANLS
*/
/************************************************************************/


#ifndef __CLIENTMGR_H__
#define __CLIENTMGR_H__
#include <list>
#include <map>

#include "ace/Singleton.h"
#include "ClientHandler.h"

using namespace std;

typedef 	map<unsigned int,ClientHandler*>	CMAP;

class ClientMgr
{
public:
	ClientMgr();
	unsigned int add(ClientHandler* client);
	void del(unsigned int connid);
	ClientHandler* get(unsigned int connid);

private:

	// 连接ID，递增
	unsigned int m_connectId;
	CMAP	m_clientList;
};

typedef ACE_Singleton<ClientMgr, ACE_Recursive_Thread_Mutex>  App_ClientMgr;

#endif