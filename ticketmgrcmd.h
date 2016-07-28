/************************************************************************/
/* 
	DESC:		操作票管理逻辑.
	DATE:		2016-07-15
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __TICKETMGRCMD_H__
#define __TICKETMGRCMD_H__

#include "cmdbase.h"
class TicketMgrCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	void	getTicketList(sClientMsg* msg);	
	void	queryTicketActions(sClientMsg* msg);
	void	createTicket(sClientMsg* msg);
	void	commitTicket(sClientMsg* msg);

};

#endif