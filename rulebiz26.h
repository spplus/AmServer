/************************************************************************/
/* 
		DESC:	先断开负荷侧，再合电源侧.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ26_H__
#define __RULEBIZ26_H__

#include "topobase.h"

class RuleBiz26		:public TopoBase
{
public:
	RuleBiz26();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	bool				hasBus;
	// 满足条件二的开关cim
	string			m_breakerCim;
};

#endif