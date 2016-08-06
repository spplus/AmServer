/************************************************************************/
/* 
		DESC:	投入引起母线电压升高.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ45_H__
#define __RULEBIZ45_H__

#include "topobase.h"

class RuleBiz45		:public TopoBase
{
public:
	RuleBiz45();
	bool topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
	bool topoRange(LISTMAP unitsList,RMAP& ruleMap);
	bool isInit;

};

#endif