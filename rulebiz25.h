/************************************************************************/
/* 
		DESC:	刀闸拉合设备，禁止操作.
		DATE:	2016-06-29
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ25_H__
#define __RULEBIZ25_H__

#include "topobase.h"

class RuleBiz25		:public TopoBase
{
public:
	RuleBiz25();
	void				setOptype(int optype);
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	// 客户端操作类型
	int				m_optype;

	vector<PBNS::StateBean>		m_switchList;
	vector<PBNS::StateBean>		m_breakList;

};

#endif