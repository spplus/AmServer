/************************************************************************/
/* 
		DESC:	所连接的开关合，禁止操作刀闸.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ12_H__
#define __RULEBIZ12_H__

#include "topobase.h"

class RuleBiz12		:public TopoBase
{
public:
	RuleBiz12();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

private:
	bool				m_hasBus;
	bool				m_hasSwitch;
};

#endif