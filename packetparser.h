/************************************************************************/
/* 
		DESC:	�ͻ��˺ͷ�����֮��Э�����.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/
#include "structs.h"

class PacketParser
{
public:
	// ���
	sClientMsg*		decoder(char* data,int datalength);

	// ���
	char*		encoder(char* data,int datalength,int &outlengh);
};