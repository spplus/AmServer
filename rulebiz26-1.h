/************************************************************************/
/* 
		DESC:	26、先断开负荷侧，再合电源侧.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ26_1_H__
#define __RULEBIZ26_1_H__

#include "topobase.h"

class RuleBiz26_1		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif