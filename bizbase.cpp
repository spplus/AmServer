#include "bizbase.h"
#include "cmdbase.h"
#include "commands.h"

void BizBase::exec(sClientMsg* msg)
{
	CmdBase* pbase = 0;
	switch (msg->type)
	{
	case CMD_USER_LONGIN:

		// ������¼ҵ��������
		
		break;
	default:
		break;
	}
	if (pbase != 0)
	{
		pbase->exec(msg);
	}
}