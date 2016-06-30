/************************************************************************/
/* 
		DESC:	µ¶Õ¢ºÏÕ¢²Ù×÷Ë³Ðò´íÎó£¬½ûÖ¹²Ù×÷.
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