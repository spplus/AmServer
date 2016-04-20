/************************************************************************/
/* 
		DESC:	业务逻辑处理管理器.根据不同的消息类型，分别创建不同的业务
					处理单元进行处理.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef			__BIZBASE_H__
#define		__BIZBASE_H__

#include "structs.h"

class BizBase
{
public:
	void		exec(sClientMsg* msg);

};

#endif