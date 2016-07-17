#include "bizbase.h"
#include "cmdbase.h"
#include "include/commands.h"
#include "logincmd.h"
#include "usermgrlistcmd.h"
#include "rulemgrcmd.h"
#include "stationmgrcmd.h"
#include "devstatecmd.h"
#include "topobizcmd.h"
#include "querymgrcmd.h"
#include "ticketmsionmgrcmd.h"
#include "ticketmgrcmd.h"

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
	case CMD_RULE_LIST:
	case CMD_RULE_ADD:
	case CMD_RULE_DEL:
	case CMD_RULE_MODIFY:
	case CMD_STATION_RULE_LIST:
	case CMD_STATION_RULE_MANAGER:
	case CMD_STATION_RULE_MGR:
	case CMD_COM_RULE_LIST:
		//对应规则管理涉及的规则列表、增加、删除、修改业务处理命令
		pbase = new RuleMgrCmd;
		break;
	case CMD_QUERY_CIRCLE_LIST:
	case CMD_QUERY_SIGN_LIST:
	case CMD_QUERY_GSWITCH_LIST:
	case CMD_QUERY_MSET_LIST:
	case CMD_QUERY_EVENT_LIST:
		//对应环路、挂牌、接地、人工置数、事件查询处理命令
		pbase = new QueryMgrCmd;
		break;
	case CMD_STATION_TYPE_ADD:
	case CMD_STATION_TYPE_DEL:
	case CMD_STATION_TYPE_MODIFY:
	case CMD_STATION_TYPE_LIST:
	case CMD_STATION_LIST:
	case CMD_STATION_MANAGER:
		//对应厂站和厂站类型管理涉及的厂站站点修改配置，厂站类型增加、修改业务处理命令
		pbase = new StationMgrcmd;
		break;
	case CMD_DEV_STATE:
	case CMD_STATION_TYPE:
	//case CMD_STATION_LIST:
	case CMD_LINE_SET:
	case CMD_POWER_SET:
	case CMD_TAG_OP:
	case CMD_READ_SAVING:
	case CMD_WRITE_SAVING:
		pbase = new DevStateCmd;
		break;

	case CMD_TOPO_BREAKER_CHANGE:		// 开关变位
	case CMD_CHECK_PASS:								// 规则校验通过
	case CMD_TOPO_ENTIRE:							// 整站拓扑
		pbase = new TopoBizCmd;
		break;
	case CMD_TICKETMS_LIST:
	case CMD_TICKETMS_MANAGER:
	case CMD_TICKETMS_ADD:
	case CMD_TICKETMS_DEL:
	case CMD_TICKETMS_MODIFY:
	case CMD_ROLE_USER_LIST:
		//对应操作票任务管理涉及的用户列表、角色、增加、删除、修改业务处理命令
		pbase = new TicketMsionMgrCmd;
		break;
	case CMD_TICKET_LIST:
	case CMD_TICKET_CREATE:
	case CMD_TICKET_QUERY:
	case CMD_TICKET_COMMIT:
		//对应操作票管理涉及的列表、提交、查询业务处理命令
		pbase = new TicketMgrCmd;
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