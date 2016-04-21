/************************************************************************/
/* 
		DESC:	�ͻ��˺ͷ�����֮��Э�����.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/
#ifndef __PACKETPARSER_H__
#define __PACKETPARSER_H__

#include <string>
#include "structs.h"
using namespace std;

class PacketParser
{
public:
	
	// �ͻ���  <--> ������ ���
	sClientMsg*		decoder(char* data,int datalength);

	// �ͻ���  <--> ������ ���
	char*				encoder(string data,int msgtype,int &outlengh);

	// ������  <--> SCADA ���
	sClientMsg*		decoderS(char* data,int datalength);

	// ������  <--> SCADA ���
	char*				encoderS(char* data,int datalength,int &outlengh);

};

#endif