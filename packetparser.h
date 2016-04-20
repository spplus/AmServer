/************************************************************************/
/* 
		DESC:	客户端和服务器之间协议解析.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/
#include "structs.h"

class PacketParser
{
public:
	// 打包
	sClientMsg*		decoder(char* data,int datalength);

	// 解包
	char*		encoder(char* data,int datalength,int &outlengh);
};