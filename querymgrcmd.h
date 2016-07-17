/************************************************************************/
/* 
	DESC:		查询类管理逻辑.
	DATE:		2016-06-28
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __QUERYMGRCMD_H__
#define __QUERYMGRCMD_H__
#include "comutils.h"
#include "cmdbase.h"
class QueryMgrCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	//环路查询
	void	queryCricleList(sClientMsg* msg);

	//查询类
	void	querySignList(sClientMsg* msg);
	void	queryGSwitchList(sClientMsg* msg);
	void	queryMsetList(sClientMsg* msg);
	void	queryEventList(sClientMsg* msg);

	//环路拓扑
	void	cricleTopo(int saveid);

	//根据开关、刀闸元件进行环路拓扑
	void	cricleTopoByUnit(int saveid,string unitcim,STRMAP& passNodes);

	// 通过cimid获取unit信息
	PBNS::StateBean getUnitByCim(int saveid,string unitcim);

	// 根据元件ID查找对应的连接点
	LISTMAP		getConnIdByUnitsId(string unitid);

	// 根据连接点查找关联的设备
	LISTMAP		getUnitsByConnId(string connid,string saveid);

	// 是否第一次
	bool		m_isFirst;

	// 起始操作的开关，刀闸cim
	string		m_brkswhCim;

	//母线记录数
	int			m_busCount;

	vector<string>		m_buslist;
};

#endif