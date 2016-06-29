/************************************************************************/
/* 
		DESC:	µ¶Õ¢ºÏÕ¢²Ù×÷Ë³Ðò´íÎó£¬½ûÖ¹²Ù×÷.
		DATE:	2016-06-29
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ21_1_H__
#define __RULEBIZ21_1_H__

#include "topobase.h"

class RuleBiz21_1		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif