/************************************************************************/
/* 
		DESC:	±äÑ¹Æ÷²Ù×÷£¬ÖÐÐÔµãµ¶Õ¢×´Ì¬´íÎó.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ34_1_H__
#define __RULEBIZ34_1_H__

#include "topobase.h"

class RuleBiz34_1		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif