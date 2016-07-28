/************************************************************************/
/* 
		DESC:	电抗器已投运，先退掉电抗器，再投电容器.
		DATE:	2016-07-01
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ32_H__
#define __RULEBIZ32_H__

#include "topobase.h"

class RuleBiz32		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);


};

#endif