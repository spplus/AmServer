/************************************************************************/
/* 
		DESC:	开关分,不允许合小环.
		DATE:	2016-06-28
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ20_H__
#define __RULEBIZ20_H__

#include "topobase.h"

class RuleBiz20		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	vector<string>	m_stationCim;
};

#endif