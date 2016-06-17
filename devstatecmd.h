/************************************************************************/
/* 
	DESC:	获取开关状态.
	DATE:	2016-05-03
	AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __DEVSTATECMD_H__
#define __DEVSTATECMD_H__

#include "cmdbase.h"

class DevStateCmd	:public CmdBase
{
public:
	void		exec(sClientMsg* msg);

private:
	// 获取设备状态
	void		getDevState(sClientMsg* msg);

	// 获取站点分类
	void		getStationType(sClientMsg* msg);

	// 获取站点列表
	void		getStationList(sClientMsg* msg);

	void		getStationByTypeId(PBNS::StationTypeBean *typebean);

	// 挂摘牌
	void		updateIsBoard(sClientMsg* msg);			

	// 设置电源点
	void		updateIsPower(sClientMsg* msg);

	// 设置进出线
	void		updateIsLine(sClientMsg* msg);

	// 获取存档列表
	void		getSavingList(sClientMsg* msg);

	// 保存存档
	void		writeSaving(sClientMsg* msg);
};

#endif

