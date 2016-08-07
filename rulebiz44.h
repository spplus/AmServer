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
	bool			topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

	//检查集合中有没有间隔边界以及边界是何类型
	int				checkList(int saveid,LISTMAP unitsList);
};

#endif