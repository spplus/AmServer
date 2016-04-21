/************************************************************************/
/* 
	DESC:	������������.
	DATE:	2016-04-20
	AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __DEFINE_H__
#define __DEFINE_H__

#include "ace/Log_Msg.h"
#include "ace/OS_main.h" 
#include "ace/streams.h" 
#include "ace/Log_Msg.h" 
#include "ace/ACE.h"
#include "ace/OS.h"
#include "logger.h"

// �����������˿�
#define		SVR__PORT			45211

// ��ͷ
const short		FRAM_TAG_HEAD = 	0x11;

// ��β
const short		FRAME_TAG_END = 	0x88;

// ����ʶ����
#define		FRAME_TAG__LEN	2

// ��Ϣ����
#define		FRAME_HEAD_LEN	4

// ��Ϣ���ͳ���
#define		DATA_TYPE_LEN		4

// ���յȴ���ʱ
#define		RECV_TIMEOUT		5

// ���͵ȴ���ʱ
#define		SEND_TIMEOUT		5

// ������ݰ�����
#define		MAX_PACKET_LEN		20*1024

// ��־�����ļ�
#define		CONFIG_FILE ACE_TEXT("logconf.conf")

// ��־�ļ�����
#define		LOGNAME "AmServer"

// ������Ϣ���Ͷ���
#define		SYS_MSG_CONNECTED		10		// ���ӽ���

#define		SYS_MSG_CLOSED			11		// ���ӶϿ�

// ��־�����
#define		LOG		App_Logger::instance()	


#endif