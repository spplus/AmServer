/************************************************************************/
/* 
	DESC:		查询类管理逻辑.
	DATE:		2016-06-28
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __QUERYMGRCMD_H__
#define __QUERYMGRCMD_H__

#include "cmdbase.h"
class QueryMgrCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	void	queryCricleList(sClientMsg* msg);

	void	querySignList(sClientMsg* msg);
	void	queryGSwitchList(sClientMsg* msg);
	void	queryMsetList(sClientMsg* msg);
	void	queryEvnetList(sClientMsg* msg);

};

#endif