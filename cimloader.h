/************************************************************************/
/* 
	DESC:	CIM文件解析，入库.
	DATE:	2016-07-24
	AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __CIMLOADER_H__
#define __CIMLOADER_H__

#include "parsexml.h"

class CimLoader
{
public:
	int		Load(string cimname,int factype);

private:
	int		cim2db();
	void		save_log(char *buf);							//保存程序运行记录日志信息

	int		saveBaseVoltage();								//保存基础电压
	int		saveSubstation();								//保存厂站信息
	int		saveVollevel();									//保存电压等级

	int		saveBreaker();									//保存断路器
	int		saveDisconnector();								//保存刀闸
	int		saveGrdDisconnector();							//保存接地刀闸
	int		saveBusbarSection();							//保存母线
	int		saveCompensator();								//保存电容
	int		saveEnergyConsumer();							//保存负荷
	int		saveSynchronousMachine();						//保存发电机
	int		savePotentialTransformer();						//保存电压互感器
	int		savePowerTransformer();							//保存变压器
	int		saveACLineSegment();							//保存线路

	int		saveTransformerWinding();						//保存变压器绕组
	int		saveRelateLine();								//保存进出线站点关系表
	int		saveConnectivityNode();							//保存连接点表
	int		saveTerminalRelation();							//保存连接关系表
	int		saveUnitStatus();								//设置元件初始状态信息

	// 获取当前时间
private:
	//保存日志路径信息
	char				m_log[10240];										//保存程序中产生的log
	ParseXML	parseXML;									//解析类对象
};


#endif