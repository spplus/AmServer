/************************************************************************/
/* 
	DESC:	���ݿ�����ӿ�.
	DATE:	2016-04-21
	AUTHOR:	YUANLS	
*/
/************************************************************************/

#ifndef __DBACCESS_H__
#define __DBACCESS_H__
#include "ace/Singleton.h"

class DbAccess
{
public:
	// �������ݿ�����
	void			init();

	// �������ݿ�����
	bool			createConnection();

	// ��ȡ���ݿ�����
	// ...
private:
};

#endif