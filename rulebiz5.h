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
public:
	RuleBiz5();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="");

private:
	// �����һ�������Ŀ���cim
	string			m_breakerCim;
	//��¼������ĸ�ߡ����ء���բ���ص��ļ���߽�
	bool range;
};

#endif