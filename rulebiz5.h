/************************************************************************/
/* 
		DESC:	ĸ���䵹,Ӧ�������պϵĵ�բ��Ȼ���ٺ�ҵ���߼�.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ5_H__
#define __RULEBIZ5_H__

#include "topobase.h"

class RuleBiz5		:public TopoBase
{
private:
	virtual bool		topoBiz(int saveid,string unitcim,vector<int>& ruleList);
};

#endif