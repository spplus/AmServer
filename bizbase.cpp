#include "bizbase.h"
#include "cmdbase.h"
#include "include/commands.h"
#include "logincmd.h"

void BizBase::exec(sClientMsg* msg)
{
	CmdBase* pbase = 0;
	switch (msg->type)
	{
	case CMD_USER_LONGIN:

		// 创建登录业务处理命令
		pbase = new LoginCmd;
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