/************************************************************************/
/* 
	DESC:	公共变量定义.
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

// 服务器监听端口
#define		SVR__PORT	45211

// 接收数据包固定长度
#define		DECODER_PACKET_FIXLEN 2+4+2+2+4+4+4

// 发送数据包固定长度
#define		ENCODER_PACKET_FIXLEN 2+2+4+4+4

// 单包长度
#define		BUFFER_LEN             1024

// 包体长度
#define		FRAME_HEAD_LEN		4

// 消息类型长度
#define		DATA_TYPE_LEN		4

// 接收等待超时
#define		RECV_TIMEOUT		5

// 发送等待超时
#define		SEND_TIMEOUT		5

// 最大数据包长度
#define		MAX_PACKET_LEN		20*1024

// 日志配置文件
#define		CONFIG_FILE ACE_TEXT("logconf.conf")

// 日志文件名称
#define		LOGNAME "AmServer"

// 保留消息类型定义
#define		SYS_MSG_CONNECTED		10		// 连接建立

#define		SYS_MSG_CLOSED			11		// 连接断开

// 日志对象简化
#define		LOG		App_Logger::instance()	


#endif