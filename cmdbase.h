/************************************************************************/
/* 
		DESC:	ҵ���߼��������.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __CMDBASE_H__
#define __CMDBASE_H__

#include "structs.h"

class CmdBase
{
public:
	virtual void		exec(sClientMsg* msg) = 0;
};

#endif