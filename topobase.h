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
	virtual	bool		topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

	// 执行具体的业务逻辑 
	/*
	返回值说明：1 继续拓扑 ，0 不用递归拓扑，2 直接退出，规则不会被触发
	*/
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="") = 0;

	// 设置当前操作的设备
	void				setOpcim(string	cimid);

	// 设置客户端操作设备列表
	void				setReq(PBNS::OprationMsg_Request req);

	// 设置经历过的节点
	void				setPassedNodes(STRMAP &passedNodes);

	//开关变位拓扑，获取整站元件的带电状态
	bool topoOnBreakerChange(PBNS::OprationMsg_Request req);

	//以指定元件进行一轮拓扑
	void topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap,PBNS::OprationMsg_Request req);

	// 判断是否是推车刀闸
	void		checkIsCarSwitch(PBNS::OprationMsg_Request& req);

protected:
	// 通过cimid获取unit信息
	PBNS::StateBean getUnitByCim(int saveid,string unitcim);

	// 根据元件ID查找对应的连接点
	LISTMAP			getConnIdByUnitsId(string unitid);

	// 根据连接点查找关联的设备
	LISTMAP			getUnitsByConnId(string connid,string saveid);

	//根据cimid获取元件对象
	int findUnitByCim(string cim,PBNS::OprationMsg_Request& req,PBNS::StateBean &bean);

protected:
	// 本次操作设备的cim
	string			m_opcim;
	STRMAP			m_passedNodes;
	PBNS::OprationMsg_Request	m_req;
	//开关变位请求拓扑出的预期元件状态集合
	vector<PBNS::StateBean> expectMap;
};

#endif

