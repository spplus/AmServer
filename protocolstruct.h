
#ifndef _PROTOCOLSTRUCT_H_
#define _PROTOCOLSTRUCT_H_

#define			HEADLEN_LEN				2					//帧头长度:包含第一个字节关键字(0x68)和第二个字节数据区长度
#define			HEAD_TAG				0x68				//帧头关键字
#define			FIRST_LEN				1					//第一个字节
#define			SECOND_LEN				1					//第二个字节
#define			THREE_LEN				1					//第三个字节
#define			DATAFRAME_MAX_LEN		0xFF				//数据区最大长度
#define			DATAFRAME_MIN_LEN		4					//数据区最小长度

#define			SHORFRAME_LEN			6					//短帧长度
#define			LONGFRAME_LEN			16					//长帧长度
#define			SHORFRAME_DATA_LEN		4					//短帧数据区长度,短帧包含:U帧的发送确认帧，S帧的发送和确认帧，测试帧的发送却帧
#define			LONGFRAME_DATA_LEN		14					//厂帧数据区长度，在这仅表示I帧的数据长度

#define			SEND_U_ACT_FRAME		0x07				//U帧发送激活帧(首次建立连接)
#define			RCV_U_ACT_FRAME			0x0B				//U帧激活确认帧
#define			SEND_UTEST_CON_FRAME	0x43				//U帧发送测试帧
#define			RCV_UTEST_CON_FRAME		0x83				//U帧测试确认帧
#define			SEND_S_ACT_FRAME		0x01				//S帧发送确认帧
#define			RCV_S_ACT_FRAME			0x01				//S帧接收确认帧



/**********************************************************************/
/*
* 数据连续类型标识
*/
enum Enum_TypeContinuity
{
	/**
	* 数据连续
	*/
	T_Continuity = 1,
	/**
	* 数据不连续
	*/
	T_NotContinuity = 0
};

/*
* 类型标识
*/
enum Enum_TypeIdentification
{
	/**
	* 单点信息
	*/
	M_SP_NA_1 = 0x01,
	/**
	* 带时标的单点信息
	*/
	M_SP_TA_1 = 0x02,
	/**
	* 双点信息
	*/
	M_DP_NA_1 = 0x03,
	/**
	* 带时标的双点信息
	*/
	M_DP_TA_1 = 0x04,
	/**
	* 步位置信息
	*/
	M_ST_NA_1 = 0x05,
	/**
	* 带时标的步位置信息
	*/
	M_ST_TA_1 = 0x06,
	/**
	* 32 比特串
	*/
	M_BO_NA_1 = 0x07,
	/**
	* 带时标的32 比特串
	*/
	M_BO_TA_1 = 0x08,
	/**
	* 测量值，规一化值
	*/
	M_ME_NA_1 = 0x09,
	/**
	* 带时标的测量值,规一化值
	*/
	M_ME_TA_1 = 0x0a,
	/**
	* 测量值，标度化值
	*/
	M_ME_NB_1 = 0x0b,
	/**
	* 带时标的测量值，标度化值
	*/
	M_ME_TB_1 = 0x0c,
	/**
	* 测量值，短浮点数
	*/
	M_ME_NC_1 = 0x0d,
	/**
	* 带时标的测量值，短浮点数
	*/
	M_ME_TC_1 = 0x0e,
	/**
	* 累计量
	*/
	M_IT_NA_1 = 0x0f,
	/**
	* 带时标的累计量
	*/
	M_IT_TA_1 = 0x10,
	/**
	* 带时标的继电保护设备事件
	*/
	M_EP_TA_1 = 0x11,
	/**
	* 带时标的继电保护设备成组启动事件
	*/
	M_EP_TB_1 = 0x12,
	/**
	* 带时标的继电保护设备成组输出电路信息
	*/
	M_EP_TC_1 = 0x13,
	/**
	* 带变位检出的成组单点信息
	*/
	M_PS_NA_1 = 0x14,
	/**
	* 测量值，不带品质描述词的规一化值
	*/
	M_ME_ND_1 = 0x15,
	/**
	* 带CP56Time2a 时标的单点信息
	*/
	M_SP_TB_1 = 0x16,
	/**
	* 带CP56Time2a 时标的双点信息31
	*/
	M_DP_TB_1 = 0x1f,
	/**
	* 带CP56Time2a 时标的步位置信息
	*/
	M_ST_TB_1 = 0x20,
	/**
	* 带CP56Time2a 时标的32 比特串
	*/
	M_BO_TB_1 = 0x21,
	/**
	* 带CP56Time2a 时标的测量值，规一化值
	*/
	M_ME_TD_1 = 0x22,
	/**
	* 带CP56Time2a 时标的测量值，标度化值
	*/
	M_ME_TE_1 = 0x23,
	/**
	* 带CP56Time2a 时标的测量值，短浮点数
	*/
	M_ME_TF_1 = 0x24,
	/**
	* 带CP56Time2a 时标的累计量
	*/
	M_IT_TB_1 = 0x25,
	/**
	* 带CP56Time2a 时标的继电保护设备事件
	*/
	M_EP_TD_1 = 0x26,
	/**
	* 带CP56Time2a 时标的继电保护设备成组启动事件
	*/
	M_EP_TE_1 = 0x27,
	/**
	* 带CP56Time2a 时标的继电保护装置成组输出电路信息
	*/
	M_EP_TF_1 = 0x28,
	/**
	* 单点命令
	*/
	C_SC_NA_1 = 0x2d,
	/**
	* 双点命令
	*/
	C_DC_NA_1 = 0x2e,
	/**
	* 步调节命令
	*/
	C_RC_NA_1 = 0x2f,
	/**
	* 设定值命令、规一化值（直接控制命令）
	*/
	C_SE_NA_1 = 0x30,
	/**
	* 设定值命令、标度化值
	*/
	C_SE_NB_1 = 0x31,
	/**
	* 设定值命令，短浮点数
	*/
	C_SE_NC_1 = 0x32,
	/**
	* 32 比特串
	*/
	C_BO_NC_1 = 0x33,
	/**
	* 初始化结束
	*/
	M_EI_NA_1 = 0x46,
	/**
	* 总召唤命令（包括分组召唤）
	*/
	C_IC_NA_1 = 0x64,
	/**
	* 累计量召唤命令
	*/
	C_CI_NA_1 =  0x65,
	/**
	* 读命令
	*/
	C_RD_NA_1 =  0x66,
	/**
	* 时钟同步命令
	*/
	C_CS_NA_1 =  0x67,
	/**
	* 测试命令
	*/
	C_TS_NA_1 =  0x68,
	/**
	* 复位进程命令
	*/
	C_RP_NA_1 =  0x69,
	/**
	* 延时获得命令
	*/
	C_CD_NA_1 =  0x6a,
	/**
	* 测量值参数，规一化值
	*/
	P_ME_NA_1 =  0x6e,
	/**
	* 测量值参数，标度化值
	*/
	P_ME_NB_1 =  0x6f,
	/**
	* 测量值参数，短浮点数
	*/
	P_ME_NC_1 =  0x70,
	/**
	* 参数激活
	*/
	P_AC_NA_1 = 0x71,
	/**
	* 文件准备就绪
	*/
	F_FR_NA_1 = 0x78,
	/**
	* 节准备就绪
	*/
	F_SR_NA_1 = 0x79,
	/**
	* 召唤目录，选择文件，召唤文件召唤节
	*/
	F_SC_NA_1 = 0x7a,
	/**
	* 最后的节，最后的段
	*/
	F_LS_NA_1 = 0x7b,
	/**
	* 认可文件，认可节
	*/
	F_AF_NA_1 = 0x7c,
	/**
	* 段
	*/
	F_SG_NA_1 = 0x7d,
	/**
	* 目录
	*/
	F_DR_NA_1 = 0x7e
};

/*
* 传输原因
*/
enum Enum_CauseOfTransmission
{
	/**
	 * 周期、循环
	 */
	PER_CYC = 1,
	/**
	 * 背景扫描
	 */
	BACK = 2,
	/**
	 * 突发(自发)
	 */
	SPONT = 3,
	/**
	 * 初始化完成
	 */
	INIT = 4,
	/**
	 * 请求或者被请求
	 */
	REQ = 5,
	/**
	 * 激活
	 */
	ACT = 6,
	/**
	 * 激活确认
	 */
	ACTCON = 7,
	/**
	 * 停止激活
	 */
	DEACT = 8,
	/**
	 * 停止激活确认
	 */
	DEACTCON = 9,
	/**
	 * 激活终止
	 */
	ACTTERM = 10,
	/**
	 * 远方命令引起的返送信息
	 */
	RETREM = 11,
	/**
	 * 当地命令引起的返送信息
	 */
	RETLOC = 12,
	/**
	 * 文件传输
	 */
	FILE_FILE = 13,//FILE宏冲突，改为FILE_FILE
	/**
	 * 响应站召唤
	 */
	INROGEN = 20,
	/**
	 * 响应第1组召唤
	 */
	INRO1 = 21,
	/**
	 * 响应第2组召唤
	 */
	INRO2 = 22,
	/**
	 * 响应第3组召唤
	 */
	INRO3 = 23,
	/**
	 * 响应第4组召唤
	 */
	INRO4 = 24,
	/**
	 * 响应第5组召唤
	 */
	INRO5 = 25,
	/**
	 * 响应第6组召唤
	 */
	INRO6 = 26,
	/**
	 * 响应第7组召唤
	 */
	INRO7 = 27,
	/**
	 * 响应第8组召唤
	 */
	INRO8 = 28,
	/**
	 * 响应第9组召唤
	 */
	INRO9 = 29,
	/**
	 * 响应第10组召唤
	 */
	INRO10 = 30,
	/**
	 * 响应第11组召唤
	 */
	INRO11 = 31,
	/**
	 * 响应第12组召唤
	 */
	INRO12 = 32,
	/**
	 * 响应第13组召唤
	 */
	INRO13 = 33,
	/**
	 * 响应第14组召唤
	 */
	INRO14 = 34,
	/**
	 * 响应第15组召唤
	 */
	INRO15 = 35,
	/**
	 * 响应第16组召唤
	 */
	INRO16 = 36,
	/**
	 * 响应计数量站召唤
	 */
	REQCOGEN = 37,
	/**
	 * 响应第1组计数量召唤
	 */
	REQCO1 = 38,
	/**
	 * 响应第2组计数量召唤
	 */
	REQCO2 = 39,
	/**
	 * 响应第3组计数量召唤
	 */
	REQCO3 = 40,
	/**
	 * 响应第4组计数量召唤
	 */
	REQCO4 = 41,
	/**
	 * 未知的类型标识
	 */
	UNKNOWNTYPEID = 44,
	/**
	 * 未知的传送原因
	 */
	UNKNOWNCOT = 45,
	/**
	 * 未知的应用服务数据单元公共地址
	 */
	UNKNOWNASDUCOMADDR = 46,
	/**
	 * 未知的信息对象地址
	 */
	UNKNOWNINFOOBJADDR = 47
};


/*帧头*/
typedef struct FRAME_HEAD_tag{
	unsigned char start;//启动字节0x68
	unsigned char len;//应用规约数据单元的长度
}FRAME_HEAD,*P_FRAME_HEAD;

/*====================================================================*/
// 遥信数据结构
typedef struct _YX_TO_BS
{
	unsigned char szAddr[3];		// 信息体地址(3个字节表示的)
	unsigned char byValue ;			// 量测值
}  YX_TO_BS , *LPYX_TO_BS ;

/*====================================================================*/

// 遥信数据结构
typedef struct _YX_BS
{
	unsigned char byValue ;			// 量测值
}  YX_BS, *LPYX_BS ;

/*====================================================================*/

//设备cimid与点号对应关系
typedef struct _CIMID_NUM
{
	std::string cimid;				//设备cimid
	int			num;				//点号(数据库中)
} CIMID_NUM;

typedef std::map<int,CIMID_NUM> CIMID_NUM_N_LIST;

typedef std::map<std::string,CIMID_NUM> CIMID_NUM_C_LIST;

//规约解析出的YX点号和YX值对应关系
typedef struct _YXPOINT_VAL
{
	int  point;						//规约解析出的点号
	int  yxVal;						//点号对应解析出的遥信值
	char dtime[32];					//时间
} YXPOINT_VAL;

typedef std::map<int,YXPOINT_VAL> YXPOINT_VAL_LIST;

//规约解析出的YX点号关联系统中的设备cimid和YX值对应关系
typedef struct _YXCIMID_VAL
{
	std::string  cimid;				//规约解析出点号对应的cimid
	int  yxVal;						//点号对应解析出的遥信值
	char dtime[32];					//时间
} YXCIMID_VAL;

typedef std::map<std::string,YXCIMID_VAL> YXCIMID_VAL_LIST;

#endif;