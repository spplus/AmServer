/************************************************************************/
/* 
		DESC:	�������б�ѹ�������Ե㵶բ״̬����.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ35_1_H__
#define __RULEBIZ35_1_H__

#include "topobase.h"

class RuleBiz35_1		:public TopoBase
{
public:
	RuleBiz35_1();
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	
	// ����Ƿ񴥷�����3
	bool		m_flag;

};

#endif