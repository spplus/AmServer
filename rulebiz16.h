/************************************************************************/
/* 
		DESC:	�豸δ����բ��Ч���룬��ֹ�����ص�.
		DATE:	2016-06-27
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ16_H__
#define __RULEBIZ16_H__

#include "topobase.h"

class RuleBiz16		:public TopoBase
{
private:
	virtual bool		topoBiz(int saveid,string unitcim,RMAP& ruleMap);
};

#endif