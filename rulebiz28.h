/************************************************************************/
/* 
		DESC:	�⻷��ԭ���ڻ��ϣ��⻷���ڻ��ϣ��豸������л�·��.
		DATE:	2016-06-30
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ28_H__
#define __RULEBIZ28_H__

#include "topobase.h"

class RuleBiz28		:public TopoBase
{
public:
	RuleBiz28();
	void				setBeginCim(string bcim);
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */){return 0;}

private:
	// �Ƿ��һ��
	bool				m_isFirst;

	// ��ʼ�����Ŀ��أ���բcim
	string			m_breakerCim;
};

#endif