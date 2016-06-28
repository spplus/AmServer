/************************************************************************/
/* 
		DESC:	µ¶Õ¢ºÏÕ¢²Ù×÷Ë³Ðò´íÎó£¬½ûÖ¹²Ù×÷.
		DATE:	2016-06-28
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ21_H__
#define __RULEBIZ21_H__

#include "topobase.h"

class RuleBiz21		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif