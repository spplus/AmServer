/************************************************************************/
/* 
		DESC:	造成设备停电.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ44_H__
#define __RULEBIZ44_H__

#include "topobase.h"

class RuleBiz44		:public TopoBase
{
public:
	RuleBiz44();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	// 包含的刀闸数量
	int				m_switchCount;

	// 刀闸状态
	bool				m_switchState;

	// next cim
	string			m_unitCim;

	// 刀闸列表
	vector<string>	m_switchList;
};

#endif