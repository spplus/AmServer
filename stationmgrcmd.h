/************************************************************************/
/* 
	DESC:		≥ß’æπ‹¿Ì¬ﬂº≠.
	DATE:		2016-05-25
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __STATIONMGRCMD_H__
#define __STATIONMGRCMD_H__
//#pragma once

#include "cmdbase.h"

class StationMgrcmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);
private:
	void	getStationTypeList(sClientMsg* msg);
	void	stationTypeManager(sClientMsg* msg);
	void	stationManager(sClientMsg* msg);
};

#endif
