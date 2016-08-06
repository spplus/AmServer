/************************************************************************/
/* 
		DESC:	开关分,不允许合小环.
		DATE:	2016-06-28
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ20_H__
#define __RULEBIZ20_H__

#include "topobase.h"

class RuleBiz20		:public TopoBase
{
public:
	RuleBiz20();
	bool topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	vector<string>	m_stationCim;

	//在当前集合查找间隔边界,并针对边界类型做不同处理
	int topoRange(LISTMAP unitsList, RMAP& ruleMap,string stationcim);

	//检查当前集合带电状态,如果均不带电，跳过该连接点逻辑
	bool checkEcletric(int saveid,LISTMAP unitsList);

	//第一次执行时检查手车
	bool isInit;
};

#endif