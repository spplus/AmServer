#include "packetparser.h"
#include "defines.h"

sClientMsg* PacketParser::decoder(char* data,int datalength)
{
	// ��Ϣ��ʶͷ
	unsigned short head;
	unsigned short end;
	int pos = 0;
	ACE_OS::memcpy(&head,data+pos,FRAME_TAG__LEN);
	pos += FRAME_TAG__LEN;

	// �ж���Ϣͷʱ����ȷ
	if (head != FRAM_TAG_HEAD)
	{
		LOG->warn("Invalid frame head tag :%d",head);
		return NULL;
	}

	sClientMsg* pmsg = new sClientMsg;

	// ������Ϣ����
	ACE_OS::memcpy(&pmsg->type,data+pos,DATA_TYPE_LEN);
	pos += DATA_TYPE_LEN;

	// ��������������
	// ���������� 
	int datalen = datalength - FRAME_TAG__LEN-DATA_TYPE_LEN-FRAME_TAG__LEN;
	pmsg->data = new char[datalen];
	
	// ������Ϣ����
	ACE_OS::memcpy(pmsg->data,data+pos,datalen);
	pos += datalen;

	pmsg->length = datalen;

	// ��Ϣβ
	ACE_OS::memcpy(&end,data+pos,FRAME_TAG__LEN);
	pos += FRAME_TAG__LEN;

	return pmsg;
}

char* PacketParser::encoder(string data,int msgtype,int &outlengh)
{
	
	// ��������ܳ��� = �������ܳ���+��Ϣͷ����+��Ϣ���ͳ���+��Ϣβ����
	outlengh = data.length()+FRAME_HEAD_LEN+FRAME_TAG__LEN+DATA_TYPE_LEN+FRAME_TAG__LEN;

	// �����������Ϣ������
	char * buff = new char[outlengh];
	int pos = 0;
	
	// ��Ϣ�ܳ���
	ACE_OS::memcpy(buff+pos,&outlengh,FRAME_HEAD_LEN);
	pos += FRAME_HEAD_LEN;

	// ��Ϣͷ
	ACE_OS::memcpy(buff+pos,&FRAM_TAG_HEAD,FRAME_TAG__LEN);
	pos += FRAME_TAG__LEN;

	// ��Ϣ����
	ACE_OS::memcpy(buff+pos,&msgtype,DATA_TYPE_LEN);
	pos += DATA_TYPE_LEN;

	// ��Ϣ����
	ACE_OS::memcpy(buff+pos,data.c_str(),data.length());
	pos += data.length();

	// ��Ϣβ
	ACE_OS::memcpy(buff+pos,&FRAME_TAG_END,FRAME_TAG__LEN);
	pos += FRAME_TAG__LEN;

	return buff;
}

sClientMsg* PacketParser::decoderS(char* data,int datalength)
{
	return 0;
}

char* PacketParser::encoderS(char* data,int datalength,int &outlengh)
{
	return 0;
}