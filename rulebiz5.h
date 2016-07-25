/************************************************************************/
/* 
		DESC:	母线冷倒,应先拉开闭合的刀闸，然后再合业务逻辑.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ5_H__
#define __RULEBIZ5_H__

#include "topobase.h"

class RuleBiz5		:public TopoBase
{
public:
	RuleBiz5();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

private:
	// 保存第一个遇到的开关cim
	string			m_breakerCim;
	//记录遇到非母线、开关、刀闸、地刀的间隔边界
	bool range;
};

#endif