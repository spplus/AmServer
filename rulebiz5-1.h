/************************************************************************/
/* 
		DESC:	ĸ���䵹,Ӧ�������պϵĵ�բ��Ȼ���ٺ�ҵ���߼�.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ5_1_H__
#define __RULEBIZ5_1_H__

#include "topobase.h"

class RuleBiz5_1		:public TopoBase
{
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");
};

#endif