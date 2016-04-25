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
#define		SVR__PORT			45211

// 包头
const short		FRAM_TAG_HEAD = 	0x11;

// 包尾
const short		FRAME_TAG_END = 	0x88;

// 包标识长度
#define		FRAME_TAG__LEN	2

// 消息长度
#define		FRAME_HEAD_LEN	4

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

// 服务器配置文件
#define		SERVER_CONFIG "server.conf"

// 服务器配置根KEY
#define		SERVER_ROOT		"SERVER"

// 数据库配置根KEY
#define		DB_ROOT			"DB"



// 日志文件名称
#define		LOGNAME "AmServer"

// 保留消息类型定义
#define		SYS_MSG_CONNECTED		10		// 连接建立

#define		SYS_MSG_CLOSED			11		// 连接断开

// 日志对象简化
#define		LOG		App_Logger::instance()	

#define		PBNS	com::spplus::buff

#endif