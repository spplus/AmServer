/************************************************************************/
/* 
		DESC:	刀闸断开操作顺序错误，禁止操作.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ27_1_H__
#define __RULEBIZ27_1_H__

#include "topobase.h"

class RuleBiz27_1		:public TopoBase
{
public:
	RuleBiz27_1();
	bool topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
	string curUnit;//当前操作的元件Cim

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
	bool flag;//判断已找到的开关与母刀连接点，是否有其他闭合的刀闸，之后的递归不再进行此逻辑
	bool flag2;//已找到闭合刀闸，再次以此遍历时找到的元件集合，特殊处理的标志
};

#endif