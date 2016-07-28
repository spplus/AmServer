/************************************************************************/
/* 
	DESC:		用户管理逻辑.
	DATE:		2016-05-12
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __USERMGRLISTCMD_H__
#define __USERMGRLISTCMD_H__

//#pragma once
#include "cmdbase.h"

class UserMgrListCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	void	getUserList(sClientMsg* msg);	
	void	getUserRoleList(sClientMsg* msg);
	void	userManager(sClientMsg* msg);

};

#endif