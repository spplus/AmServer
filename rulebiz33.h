/************************************************************************/
/* 
		DESC:	��������Ͷ�ˣ����˵�����������Ͷ�翹��.
		DATE:	2016-07-01
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ32_H__
#define __RULEBIZ33_H__

#include "topobase.h"

class RuleBiz33		:public TopoBase
{

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);
};

#endif