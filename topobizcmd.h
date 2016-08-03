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
	void			topoBySaveId(string saveid,int unittype=eGENERATOR);
private:

	// CIM入库
	void		loadCim();

	// 整站拓扑
	void		topoEntire(sClientMsg *msg);

	// 根据元件ID进行拓扑，更新数据库版本
	void		topoByUnitId(string saveid,string unitid,string stationid,STRMAP& passNodes);

	// 拓扑接地刀闸
	void		topoByGround(string saveid,string unitid,string stationid,STRMAP& passNodes);

	// 根据元件ID查找对应的连接点
	LISTMAP		getConnIdByUnitsId(string unitid);

	//根据变压器ID查找对应的连接点
	LISTMAP		getConnIdByTransId(string unitid);

	// 根据进出线ID，查找进出线关联的站点ID
	LISTMAP		getStationIdByLineId(string unitid,string stationid);

	// 根据连接点查找关联的设备
	LISTMAP		getUnitsByConnId(string connid,string saveid);

	// 更新设备带电状态
	void		updateIsElectricByUnitId(string saveid,string unitid,int state);

	// 更新为是否接地
	void		updateIsGroundByUnitId(string saveid,string unitid,int state);

	// 更新设备是绝对电源点
	void		updateIsPowerByUnitId(string unitid,string stationid,string saveid);


	// 开关变位拓扑
	void		topoOnBreakerChange(sClientMsg *msg);

	// 根据元件CIMID进行拓扑，不更新数据库版本
	void		topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap,PBNS::OprationMsg_Request req);

	// 执行开关变位拓扑
	string	execTopoOnBreakerChange(PBNS::OprationMsg_Request req);

	// 规则检查
	void		roleCheck(int connid,PBNS::OprationMsg_Request req);
	void		sendRuleBack(int connid,int optype,vector<int> ruleList);

	// 设置带电状态
	void		updateIsElectric(string saveid,string unitcim,int state);

	// 从客户端操作列表中查找bean
	int		findUnitByCim(string cim,PBNS::OprationMsg_Request& req,PBNS::StateBean &bean);

	// 检测规则是否生效
	bool		checkRuleIsUse(string cimid,int ruleid);

	// 判断是否是推车刀闸
	void		checkIsCarSwitch(PBNS::OprationMsg_Request& req);

	bool		check1(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check2(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check4(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check5(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check12(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check16(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check18(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check20(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check21(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check22(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check25(int saveid,string unitcim,int optype,PBNS::OprationMsg_Request req);
	bool		check26(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check27(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check28(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check29(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check32(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check33(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check34(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check35(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check38(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check39(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check43(int saveid,string unitcim,PBNS::OprationMsg_Request req);

	bool		check45(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check46(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check47(int saveid,string unitcim,PBNS::OprationMsg_Request req);
	bool		check48(int saveid,string unitcim,PBNS::OprationMsg_Request req);

	//拓扑中需要着色的变压器cim（拓扑中只会拓扑到winding，所以变压器本身需单独处理）
	PBNS::StateBean transBean;
};

#endif