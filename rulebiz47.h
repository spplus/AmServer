/************************************************************************/
/* 
		DESC:	退出引起母线电压升高.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ47_H__
#define __RULEBIZ47_H__

#include "topobase.h"

class RuleBiz47		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

};

#endif