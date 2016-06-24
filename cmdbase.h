/************************************************************************/
/* 
		DESC:	业务逻辑处理基类.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __CMDBASE_H__
#define __CMDBASE_H__

#include "defines.h"
#include "structs.h"
#include "clientmgr.h"
#include "buff/msgbody.pb.h"
#include "dbaccess.h"
#include "clientmgr.h"
#include "include/commands.h"
#include "defines.h"

class CmdBase
{
public:
	virtual void		exec(sClientMsg* msg) = 0;

protected:
	int		str2i(string val)
	{
		return ACE_OS::atoi(val.c_str());
	}

	string	i2str(int val)
	{
		char temp[16];
		ACE_OS::itoa(val,temp,10);
		string strval;
		strval.append(temp);
		return strval;
	}
};

#endif