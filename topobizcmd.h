/************************************************************************/
/* 
	DESC:		拓扑分析相关逻辑.
	DATE:		2016-05-24
	AUTHOR:	YUANLS	
*/
/************************************************************************/
#ifndef __TOPOBIZCMD_H__
#define __TOPOBIZCMD_H__

#include "cmdbase.h"
class TopoBizCmd	:public CmdBase
{
public:
	void				exec(sClientMsg* msg);

private:
	// 整站拓扑
	void				topoEntire();

	// 根据元件ID查找对应的连接点
	LISTMAP	getConnIdByUnitsId(string unitid);

	// 根据连接点查找关联的设备
	LISTMAP		getUnitsByConnId(string connid);

};

#endif