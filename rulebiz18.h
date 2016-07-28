/************************************************************************/
/* 
		DESC:	带电合地刀，禁止操作.
		DATE:	2016-06-28
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ18_H__
#define __RULEBIZ18_H__

#include "topobase.h"

class RuleBiz18		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");
};

#endif