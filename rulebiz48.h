/************************************************************************/
/* 
		DESC:	�˳�����ĸ�ߵ�ѹ����.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ48_H__
#define __RULEBIZ48_H__

#include "topobase.h"

class RuleBiz48		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

};

#endif