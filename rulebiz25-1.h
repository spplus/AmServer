/************************************************************************/
/* 
		DESC:	刀闸合闸操作顺序错误，禁止操作.
		DATE:	2016-06-29
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ25_1_H__
#define __RULEBIZ25_1_H__

#include "topobase.h"

class RuleBiz25_1		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif