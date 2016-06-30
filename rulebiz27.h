/************************************************************************/
/* 
		DESC:	刀闸断开操作顺序错误，禁止操作.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ27_H__
#define __RULEBIZ27_H__

#include "topobase.h"

class RuleBiz27		:public TopoBase
{
public:
	RuleBiz27();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:

	// 满足条件一的开关cim
	string			m_breakerCim;
};

#endif