/************************************************************************/
/* 
		DESC:	闭合母联开关，必须在开关两侧的母线联络刀闸均闭合后才可以操作.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ39_1_H__
#define __RULEBIZ39_1_H__

#include "topobase.h"

class RuleBiz39_1		:public TopoBase
{
public:
	RuleBiz39_1();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	
	// 标记是否触发条件3
	bool		m_flag;

};

#endif