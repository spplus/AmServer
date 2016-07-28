/************************************************************************/
/* 
	DESC:		操作票任务管理逻辑.
	DATE:		2016-07-10
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __TICKETMSIONMGRCMD_H__
#define __TICKETMSIONMGRCMD_H__

//#pragma once
#include "cmdbase.h"

class TicketMsionMgrCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	void	getTicketMsionList(sClientMsg* msg);	
	void	ticketMsionManager(sClientMsg* msg);
	void	getUserListbyRoleid(sClientMsg* msg);

};
#endif
