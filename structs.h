/************************************************************************/
/* 
	DESC:	定义结构体.
	DATE:	2016-04-20
	AUTHOR:YUANLS
*/
/************************************************************************/

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

struct sClientMsg 
{
	unsigned int	connectId;		// 连接ID
	unsigned int	length;				// 消息长度
	unsigned int	type;				// 消息类型
	char*				data;					// 消息内容

	sClientMsg()
	{
		connectId = 0;
		length = 0;
		type = 0;
		data = 0;
	}
	~sClientMsg()
	{
		if (data != 0)
		{
			delete []data;
		}
	}
};

//遥控数据本体
typedef struct _YK_QUESTION
{
	unsigned char cType;		    // 遥控类型（0:遥控,   1:升降）
	char   szOperator[16];			// 遥控操作人
	char   szKeeper[16];		    // 遥控监护人
	char   szObjCode[40];			// 遥控设备ID，开关/刀闸或变压器
	char   szObjName[64];			// 遥控设备名称
	unsigned char cOprateType;	    // 遥控：0拉闸，1合闸。升降：0降 1升 2急停
	unsigned char cCheckMode;	    // 校验模式（0,表示紧急操作；1，表示带令操作，2，表示强制遥控）
	unsigned char cPermissio;    	// 0可控　1不可控　2确认可控
	char   szInfo[256];				// 可控不可控的原因
}YK_QUESTION, *LPYKQUESTION;


#endif