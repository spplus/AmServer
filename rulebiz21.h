/************************************************************************/
/* 
		DESC:	��բ��բ����˳����󣬽�ֹ����.
		DATE:	2016-06-28
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ21_H__
#define __RULEBIZ21_H__

#include "topobase.h"

class RuleBiz21		:public TopoBase
{
public:
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif