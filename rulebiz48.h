/************************************************************************/
/* 
		DESC:	退出引起母线电压降低.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ48_H__
#define __RULEBIZ48_H__

#include "topobase.h"

class RuleBiz48		:public TopoBase
{
public:
	bool topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
	bool topoRange(LISTMAP unitsList,RMAP& ruleMap);

};

#endif