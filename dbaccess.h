/************************************************************************/
/* 
	DESC:	数据库操作接口.
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
	// 加载数据库配置
	void			init();

	// 创建数据库连接
	bool			createConnection();

	// 获取数据库连接
	// ...
private:
};

#endif