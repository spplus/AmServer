/************************************************************************/
/* 
		DESC:	�������൶բ״̬�֣���ֹ���кϿ��ز���ҵ���߼�.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ4_H__
#define __RULEBIZ4_H__

#include "topobase.h"

class RuleBiz4		:public TopoBase
{
private:
	virtual bool		topoBiz(int saveid,string unitcim,vector<int>& ruleList);
};

#endif