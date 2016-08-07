/************************************************************************/
/* 
		DESC:	闭合母联开关，必须在开关两侧的母线联络刀闸均闭合后才可以操作.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ39_H__
#define __RULEBIZ39_H__

#include "topobase.h"

class RuleBiz39		:public TopoBase
{
public:
	RuleBiz39();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
	int topoRange(int saveid,LISTMAP unitsList,RMAP& ruleMap);

private:
	// 遍历到的刀闸
	LISTMAP  switchList;

	//当前连接点cim
	string curConn;

	//找到母线次数
	int busNum;

	//第一次执行该方法，非递归
	bool isInit;

	//开关两侧的连接点
	STRMAP connBreaker;
};

#endif