/************************************************************************/
/* 
		DESC:	Ͷ������ĸ�ߵ�ѹ����.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ45_H__
#define __RULEBIZ45_H__

#include "topobase.h"

class RuleBiz45		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

};

#endif