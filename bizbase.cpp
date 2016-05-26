#include "bizbase.h"
#include "cmdbase.h"
#include "include/commands.h"
#include "logincmd.h"
#include "usermgrlistcmd.h"
#include "devstatecmd.h"
#include "topobizcmd.h"

void BizBase::exec(sClientMsg* msg)
{
	CmdBase* pbase = 0;
	switch (msg->type)
	{
	case CMD_USER_LONGIN:
		// ������¼ҵ��������
		pbase = new LoginCmd;
		break;
	case CMD_USER_MANAGER:
	case CMD_USER_ROLE:
	case CMD_USER_ADD:
	case CMD_USER_DEL:
	case CMD_USER_MODIFY:
		//��Ӧ�û������漰���û��б���ɫ�����ӡ�ɾ�����޸�ҵ��������
		pbase = new UserMgrListCmd;
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