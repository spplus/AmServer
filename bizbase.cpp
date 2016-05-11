#include "bizbase.h"
#include "cmdbase.h"
#include "include/commands.h"
#include "logincmd.h"
#include "devstatecmd.h"

void BizBase::exec(sClientMsg* msg)
{
	CmdBase* pbase = 0;
	switch (msg->type)
	{
	case CMD_USER_LONGIN:

		// 创建登录业务处理命令
		pbase = new LoginCmd;
		break;
	case CMD_DEV_STATE:
	case CMD_STATION_TYPE:
	case CMD_STATION_LIST:
		pbase = new DevStateCmd;
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