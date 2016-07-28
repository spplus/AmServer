/************************************************************************/
/* 
		DESC:	电容器已投运，先退掉电容器，再投电抗器.
		DATE:	2016-07-01
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ33_H__
#define __RULEBIZ33_H__

#include "topobase.h"

class RuleBiz33		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif