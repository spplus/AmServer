/************************************************************************/
/* 
		DESC:	并列运行变压器，中性点刀闸状态错误.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ35_1_H__
#define __RULEBIZ35_1_H__

#include "topobase.h"

class RuleBiz35_1		:public TopoBase
{
public:
	RuleBiz35_1();
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	
	// 标记是否触发条件3
	bool		m_flag;

};

#endif