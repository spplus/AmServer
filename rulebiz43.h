/************************************************************************/
/* 
		DESC:	挂保电牌，禁止操作.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ43_H__
#define __RULEBIZ43_H__

#include "topobase.h"

class RuleBiz43		:public TopoBase
{
public:
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */){return 0;}

};

#endif