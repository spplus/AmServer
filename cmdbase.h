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

class CmdBase
{
public:
	virtual void		exec(sClientMsg* msg) = 0;
};

#endif