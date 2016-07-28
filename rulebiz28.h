/************************************************************************/
/* 
		DESC:	解环，原来在环上，解环后不在环上（设备解出现有环路）.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ28_H__
#define __RULEBIZ28_H__

#include "topobase.h"

class RuleBiz28		:public TopoBase
{
public:
	RuleBiz28();
	void				setBeginCim(string bcim);
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */){return 0;}

private:
	// 是否第一次
	bool				m_isFirst;

	// 起始操作的开关，刀闸cim
	string			m_breakerCim;
};

#endif