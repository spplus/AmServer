/************************************************************************/
/* 
		DESC:	拓扑分析基类，提供拓扑分析的框架逻辑，具体业务逻辑，通过虚函数，在子类实现.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __TOPOBASE_H__
#define __TOPOBASE_H__

#include "comutils.h"
#include <string>

using namespace std;

class TopoBase
{
public:

	// 拓扑分析框架算法
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,vector<int>& ruleList);

	// 执行具体的业务逻辑
	virtual bool		topoBiz(int saveid,string unitcim,vector<int>& ruleList) = 0;

protected:
	// 通过cimid获取unit信息
	PBNS::StateBean getUnitByCim(int saveid,string unitcim);

private:

	// 根据元件ID查找对应的连接点
	LISTMAP			getConnIdByUnitsId(string unitid);

	// 根据连接点查找关联的设备
	LISTMAP			getUnitsByConnId(string connid,string saveid);
};

#endif

