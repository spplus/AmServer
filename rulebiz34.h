/************************************************************************/
/* 
		DESC:	��ѹ�����������Ե㵶բ״̬����.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ34_H__
#define __RULEBIZ34_H__

#include "topobase.h"

class RuleBiz34		:public TopoBase
{
public:
	RuleBiz34();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:

	// ��ѹ��cim
	string			m_transCim;
};

#endif