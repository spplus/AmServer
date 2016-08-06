/************************************************************************/
/* 
		DESC:	发电机并列业务逻辑.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ1_H__
#define __RULEBIZ1_H__

#include "topobase.h"

class RuleBiz1		:public TopoBase
{
public:
	RuleBiz1();
	bool topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

	//在当前集合查找间隔边界,并针对边界类型做不同处理
	bool topoRange(LISTMAP unitsList, RMAP& ruleMap);

	//第一次执行时检查手车
	bool isInit;
};

#endif