/************************************************************************/
/* 
		DESC:	发电机并列业务逻辑.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ1_H__
#define __RULEBIZ1_H__

#include "topobase.h"

class RuleBiz1		:public TopoBase
{
private:
	virtual bool		topoBiz(int saveid,string unitcim,map<int,int>& ruleMap);
};

#endif