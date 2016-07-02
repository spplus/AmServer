/************************************************************************/
/* 
		DESC:	并列运行变压器，中性点刀闸状态错误.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ35_H__
#define __RULEBIZ35_H__

#include "topobase.h"

class RuleBiz35		:public TopoBase
{
public:
	RuleBiz35();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:

	// 母线cim
	string			m_busCim;
};

#endif