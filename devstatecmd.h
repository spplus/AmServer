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
	void exec(sClientMsg* msg);
};

#endif

