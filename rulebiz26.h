/************************************************************************/
/* 
		DESC:	�ȶϿ����ɲ࣬�ٺϵ�Դ��.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ26_H__
#define __RULEBIZ26_H__

#include "topobase.h"

class RuleBiz26		:public TopoBase
{
public:
	RuleBiz26();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:

	// �����������Ŀ���cim
	string			m_breakerCim;
};

#endif