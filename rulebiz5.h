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
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

};

#endif