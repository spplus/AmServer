/************************************************************************/
/* 
		DESC:	��բ�ڽӵ�ϵͳ����ֹ����.
		DATE:	2016-06-29
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ22_H__
#define __RULEBIZ22_H__

#include "topobase.h"

class RuleBiz22		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif