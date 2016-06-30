/************************************************************************/
/* 
		DESC:	µ¶Õ¢¶Ï¿ª²Ù×÷Ë³Ðò´íÎó£¬½ûÖ¹²Ù×÷.
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