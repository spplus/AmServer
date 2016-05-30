#include "bizbase.h"
#include "cmdbase.h"
#include "include/commands.h"
#include "logincmd.h"
#include "usermgrlistcmd.h"
#include "stationmgrcmd.h"
#include "devstatecmd.h"
#include "topobizcmd.h"

void BizBase::exec(sClientMsg* msg)
{
	CmdBase* pbase = 0;
	switch (msg->type)
	{
	case CMD_USER_LONGIN:
		// 创建登录业务处理命令
		pbase = new LoginCmd;
		break;
	case CMD_USER_MANAGER:
	case CMD_USER_ROLE:
	case CMD_USER_ADD:
	case CMD_USER_DEL:
	case CMD_USER_MODIFY:
	case CMD_PWD_MODIFY:
		//对应用户管理涉及的用户列表、角色、增加、删除、修改业务处理命令
		pbase = new UserMgrListCmd;
		break;
	case CMD_STATION_TYPE_ADD:
	case CMD_STATION_TYPE_DEL:
	case CMD_STATION_TYPE_MODIFY:
	case CMD_STATION_TYPE_LIST:
	case CMD_STATION_MANAGER:
		//对应厂站和厂站类型管理涉及的厂站站点修改配置，厂站类型增加、修改业务处理命令
		pbase = new StationMgrcmd;
		break;
	case CMD_DEV_STATE:
	case CMD_STATION_TYPE:
	case CMD_STATION_LIST:
		pbase = new DevStateCmd;
		break;

	case CMD_TOPO_ENTIRE:
		pbase = new TopoBizCmd;
		break;
	default:
		break;
	}
	if (pbase != 0)
	{
		pbase->exec(msg);
		delete pbase;
		pbase = NULL;
	}
}