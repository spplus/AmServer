/************************************************************************/
/* 
		DESC:	��բ�Ͽ�����˳����󣬽�ֹ����.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ27_H__
#define __RULEBIZ27_H__

#include "topobase.h"

class RuleBiz27		:public TopoBase
{
public:
	RuleBiz27();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:

	// ��������һ�Ŀ���cim
	string			m_breakerCim;
};

#endif