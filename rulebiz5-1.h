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
public:
	RuleBiz5_1();

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

private:
	// ���ֵĵ�բ����
	int		count;
};

#endif