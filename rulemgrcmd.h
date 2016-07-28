/************************************************************************/
/* 
	DESC:		规则编辑管理逻辑.
	DATE:		2016-06-19
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __RULEMGRCMD_H__
#define __RULEMGRCMD_H__
//#pragma once
#include "cmdbase.h"
class RuleMgrCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	void	getRuleList(sClientMsg* msg);	
	void	ruleManager(sClientMsg* msg);

	void	getStationRuleList(sClientMsg* msg);
	void	stationRuleManager(sClientMsg* msg);

};

#endif