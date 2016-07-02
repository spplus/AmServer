/************************************************************************/
/* 
		DESC:	断开母线联络刀闸，必须在母连开关断开后才可以操作.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ38_H__
#define __RULEBIZ38_H__

#include "topobase.h"

class RuleBiz38		:public TopoBase
{
public:
	RuleBiz38();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:

	// next cim
	string			m_unitCim;
};

#endif