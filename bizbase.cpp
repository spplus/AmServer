#include "bizbase.h"
#include "cmdbase.h"
#include "commands.h"

void BizBase::exec(sClientMsg* msg)
{
	CmdBase* pbase = 0;
	switch (msg->type)
	{
	case CMD_USER_LONGIN:

		// 创建登录业务处理命令
		
		break;
	default:
		break;
	}
	if (pbase != 0)
	{
		pbase->exec(msg);
	}
}