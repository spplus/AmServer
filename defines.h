﻿/************************************************************************/
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
#include <map>
#include <vector>
using namespace std;

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

// CIM文件配置KEY
#define		CIM_ROOT						"CIM"
#define		CIM_NAME						"CimName"
#define		CIM_FTPPATH					"FtpPath"
#define		CIM_WORKPATH				"WorkPath"
#define		CIM_CHECKHOUR			"CheckHour"
#define		CIM_PRODUCTID			"ProductId"

// CIM文件默认检查时间
#define		CIM_DEFAULT_CHECKHOUR		23

// 日志文件名称
#define		LOGNAME "AmServer"

// 保留消息类型定义
#define		SYS_MSG_CONNECTED		10		// 连接建立

#define		SYS_MSG_CLOSED			11		// 连接断开

// 日志对象简化
#define		LOG		App_Logger::instance()	

// 公共方法
#define		COM		App_Com::instance()

// 数据库对象
#define		DBA		App_Dba::instance()

#define		PBNS	com::spplus::buff
//
//// 重定义查询结果数据类型
typedef		map<string,string>				STRMAP;
typedef		STRMAP::value_type				MAPVAL;
typedef		STRMAP::iterator					MAP_ITERATOR;
typedef   	vector<STRMAP>					LISTMAP	;		
typedef		map<int,int>						RMAP;
typedef		RMAP::value_type					RVAL;
typedef		RMAP::iterator					R_ITERATOR;

// 定义设备类型
//enum eUnitType{eBreaker,eSwitch,eBus,eCapacitor,eLoad,eGenerator,ePt,eTransformer,eLine};
#endif