/************************************************************************/
/* 
		DESC:	刀闸合闸操作顺序错误，禁止操作.
		DATE:	2016-06-28
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ21_H__
#define __RULEBIZ21_H__

#include "topobase.h"

class RuleBiz21		:public TopoBase
{
public:
	RuleBiz21();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	// 保存第一个遇到的开关cim
	string			m_breakerCim;
};

#endif