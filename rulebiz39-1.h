/************************************************************************/
/* 
		DESC:	�պ�ĸ�����أ������ڿ��������ĸ�����絶բ���պϺ�ſ��Բ���.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ39_1_H__
#define __RULEBIZ39_1_H__

#include "topobase.h"

class RuleBiz39_1		:public TopoBase
{
public:
	RuleBiz39_1();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	
	// ����Ƿ񴥷�����3
	bool		m_flag;

};

#endif