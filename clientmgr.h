/************************************************************************/
/* 
	DESC:	���ֿͻ��������б�.
	DATE:	2016-04-20
	AUTHOR:	YUANLS
*/
/************************************************************************/


#ifndef __CLIENTMGR_H__
#define __CLIENTMGR_H__
#include <list>
#include <map>

#include "ace/Singleton.h"
#include "clienthandler.h"
#include "packetparser.h"

using namespace std;

typedef 	map<unsigned int,ClientHandler*>	CMAP;

class ClientMgr
{
public:
	ClientMgr();

	// ���ӹ���
	unsigned int add(ClientHandler* client);
	void del(unsigned int connid);
	ClientHandler* get(unsigned int connid);

	// �������
	int			sendData(unsigned int connid,string data,int msgtype);

private:

	// ��ݴ��
	PacketParser	m_pack;

	// ����ID������
	unsigned int	m_connectId;
	CMAP			m_clientList;
};

typedef ACE_Singleton<ClientMgr, ACE_Recursive_Thread_Mutex>  App_ClientMgr;

#endif
