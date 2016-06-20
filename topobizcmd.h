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
	void			exec(sClientMsg* msg);

	// 按存档ID进行拓扑
	void			topoBySaveId(string saveid);
private:

	// 整站拓扑
	void		topoEntire();

	// 根据元件ID进行拓扑，更新数据库版本
	void		topoByUnitId(string saveid,string unitid,string stationid,STRMAP& passNodes);

	// 根据元件ID查找对应的连接点
	LISTMAP		getConnIdByUnitsId(string unitid);

	// 根据进出线ID，查找进出线关联的站点ID
	LISTMAP		getStationIdByLineId(string unitid,string stationid);

	// 根据连接点查找关联的设备
	LISTMAP		getUnitsByConnId(string connid,string saveid);

	// 更新设备带电状态
	void		updateIsElectricByUnitId(string saveid,string unitid,int state);

	// 更新设备是绝对电源点
	void		updateIsPowerByUnitId(string unitid,string stationid,string saveid);


	// 开关变位拓扑
	void		topoOnBreakerChange(sClientMsg *msg);

	// 根据元件CIMID进行拓扑，不更新数据库版本
	void		topoByUnitIdMem(PBNS::StateBean bean,string saveid,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap);


};

#endif