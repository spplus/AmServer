/************************************************************************/
/* 
		DESC:	�Ͽ�ĸ�����絶բ��������ĸ�����ضϿ���ſ��Բ���.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ38_1_H__
#define __RULEBIZ38_1_H__

#include "topobase.h"

class RuleBiz38_1		:public TopoBase
{
public:
	RuleBiz38_1();
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	
	// ����Ƿ񴥷�����3
	bool		m_flag;

};

#endif