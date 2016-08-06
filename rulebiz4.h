/************************************************************************/
/* 
		DESC:	开关两侧刀闸状态分，禁止进行合开关操作业务逻辑.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ4_H__
#define __RULEBIZ4_H__

#include "topobase.h"

class RuleBiz4		:public TopoBase
{
public:
	RuleBiz4();
	// 拓扑分析框架算法
	virtual	bool		topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

	//在当前集合查找间隔边界
	bool topoRange(LISTMAP unitsList);

	//第一次执行时检查手车
	bool isInit;
};

#endif