/************************************************************************/
/* 
	DESC:		´¦ÀíµÇÂ¼Âß¼­.
	DATE:		2016-04-21
	AUTHOR:	YUANLS	
*/
/************************************************************************/

#ifndef __LOGINCMD_H__
#define __LOGINCMD_H__

#include "cmdbase.h"

class LoginCmd	:public CmdBase
{
public:
		void		exec(sClientMsg* msg);
};

#endif