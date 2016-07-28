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
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif