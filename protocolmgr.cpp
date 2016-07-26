#include "protocolmgr.h"
#include "defines.h"


// 本端的发送序号: 每发送一包I格式帧,发送序号+1
unsigned short m_nLocalSendNum = 0x00 ;				// 本端的发送序号
// 本端的接收序号理解: 本端收到了对端发送的I格式帧(其发送序号=本端的接收序号),将序号+1
unsigned short m_nLocalRecvNum = 0x00 ;				// 本端的接收序号
unsigned short m_nRemoteSendNum = 0x00 ;			// 对端的发送序号(也就是本端的接收序号)
unsigned short nRTUaddr=0xFFFF; // 南瑞OPEN3000; 0xFFFF 表示总召; 南京磐能0x03(可配置)

// 用户定义报头
typedef struct User_PackHead
{
	//unsigned char uKey ;				// 同步头 68H
	//unsigned char uLen ;				//（APDU）数据单元长度
	unsigned short uSendNum	;			//发送序号
	unsigned short uRecvNum ;			//接收序号

} USER_PACKHEAD, *LPUSER_PACKHEAD;

// 用户定义报头(104规约中的APCI); I格式
typedef struct Data_PackHead
{
	unsigned char cTypeID;				// 类型标识
	unsigned char cDeterminer;			// 可变结构限定词
	unsigned char cTransReason;			// 传输原因，响应总召唤或变化数据
	unsigned char cSourceAddr;			// 源发地址(源发地址用来标明响应来自那个主站的召唤;
	// 一般情况不使用;规定源发地址不使用时置成0)
	unsigned short sRTUAddr;			// 公共地址即RTU地址

} DATA_PACKHEAD, *LPDATA_PACKHEAD;

/*=======================================================================================================================*/
// 测试帧, 或S格式确认帧
typedef struct Send_Confirm
{
	unsigned char usKey;
	unsigned char usLen;
	unsigned short usSendNum;
	unsigned short usRecvNum;
} SEND_CONFIRM , *LPSEND_CONFIRM;

/*=======================================================================================================================*/


void ProtocolMgr::setClient(TcpClient *client)
{
	m_client = client;
}

void ProtocolMgr::setRtuAddr(unsigned short nRtuAddr)
{
	nRTUaddr = nRtuAddr;
}

void ProtocolMgr::setCallAll(int nAll)
{
	m_CallALL = nAll;
}

int ProtocolMgr::handle_timeout(const ACE_Time_Value &current_time,const void * /* = 0 */)
{
	LOG->debug("=========protocolmgr：send_I_Frame===========");
	//if (brcvflag)
	{
		//定时发送I帧:总召唤
		sendIFrame();
	}

	return 0;
}

void ProtocolMgr::sendUFrame()
{
	char uBuffer[32] = { 0 } ;
	uBuffer [ 0 ] = 0x68 ;				// 起始字符
	uBuffer [ 1 ] = 0x04 ;				// 长度4个字节
	uBuffer [ 2 ] = SEND_U_ACT_FRAME ;	// STARTDT 的ACT=1，CON=0; 0x07
	uBuffer [ 3 ] = 0x00 ;
	uBuffer [ 4 ] = 0x00 ;
	uBuffer [ 5 ] = 0x00 ;

	ACE_Message_Block * mb = new ACE_Message_Block(SHORFRAME_LEN);
	ACE_OS::memcpy(mb->wr_ptr(),uBuffer,SHORFRAME_LEN);
	mb->wr_ptr(SHORFRAME_LEN);

	Sleep(1000);

	m_client->send(mb);

	std::string strsenddata = uBuffer;
	LOG->debug("send sendUFrame data:%s",strsenddata.c_str());
}

void ProtocolMgr::sendIFrame()
{
	/**/
	if(m_nLocalSendNum>=65534)
            m_nLocalSendNum=0;

    char IBuffer[64] = { 0 } ;
    IBuffer[0] = 0x68 ;	// 启动字符(68H)
    IBuffer[1] = 0x0E ;	// APDU长度(≤253)

    // 发送序号,设置为0,
    unsigned short sendnumber=m_nLocalSendNum;//
    memcpy(&IBuffer[2],&sendnumber,sizeof(unsigned short));

    // 接收序号,设置为0
    unsigned short recvnumber=m_nLocalRecvNum;
    memcpy(&IBuffer[4],&recvnumber,sizeof(unsigned short));

    // CON<100>: 总召唤命令(ASDU类型标识)
    IBuffer[6]=0x64;
    // 可变结构限定词
    IBuffer[7]=0x01;
    IBuffer[8]=0x06;
	IBuffer[9]=0x00;		// 传送原因
    // 向地址为0的子站发送总召唤命令
    unsigned short RTUaddr=nRTUaddr;		// ASDU公共地址(低字节在前,高字节在后),占两个字节.缺省为0x01; (必须正确)
    memcpy(&IBuffer[10],&RTUaddr,sizeof(unsigned short));

    IBuffer[12] = 0x00;
	IBuffer[13] = 0x00;
	IBuffer[14] = 0x00;	// 信息体地址

    IBuffer[15] = 0x14;	// 信息体元素(召唤限定词QOI=20,站召唤全局)



	ACE_Message_Block * mb = new ACE_Message_Block(LONGFRAME_LEN);
	ACE_OS::memcpy(mb->wr_ptr(),IBuffer,LONGFRAME_LEN);
	mb->wr_ptr(LONGFRAME_LEN);

	m_client->send(mb);

    // 发送方每发送一个I格式报文,其发送序号+1
    m_nLocalSendNum += 1 ;

	std::string strsenddata = IBuffer;
	LOG->debug("send sendIFrame data:%s",strsenddata.c_str());

	return;
}

//发送S帧的策略:每次接收到I帧后发送一个S帧
void ProtocolMgr::sendSFrame()
{
	//S帧：68  04  01  00  02  00
	char sBuffer[32] = { 0 } ;
	sBuffer [ 0 ] = 0x68 ;				// 起始字符
	sBuffer [ 1 ] = 0x04 ;				// 长度4个字节
	sBuffer [ 2 ] = SEND_S_ACT_FRAME ;	// STARTDT 的ACT=1，CON=0; 0x01
	sBuffer [ 3 ] = 0x00 ;
	// 接收序号,设置为0
	unsigned short recvnumber=m_nLocalRecvNum*2;
	//recvnumber<<1;
	memcpy(&sBuffer[4],&recvnumber,sizeof(unsigned short));

	ACE_Message_Block * mb = new ACE_Message_Block(SHORFRAME_LEN);
	ACE_OS::memcpy(mb->wr_ptr(),sBuffer,SHORFRAME_LEN);
	mb->wr_ptr(SHORFRAME_LEN);

	m_client->send(mb);

	std::string strsenddata = sBuffer;
	LOG->debug("send sendSFrame data:%s",strsenddata.c_str());

}

// 发送测试帧（如果主站超过一定时间没有下发报文或者RTU也没有上送任何报文则双方都可以按频率发送U帧，测试帧）
void ProtocolMgr::sendTestFrame()
{
	char testBuffer[32] = { 0 } ;
	testBuffer [ 0 ] = 0x68 ;
	testBuffer [ 1 ] = 0x04 ;
	testBuffer [ 2 ] = SEND_UTEST_CON_FRAME ;		//0x43
	testBuffer [ 3 ] = 0x00 ;
	testBuffer [ 4 ] = 0x00 ;
	testBuffer [ 5 ] = 0x00 ;

	ACE_Message_Block * mb = new ACE_Message_Block(SHORFRAME_LEN);
	ACE_OS::memcpy(mb->wr_ptr(),testBuffer,SHORFRAME_LEN);
	mb->wr_ptr(SHORFRAME_LEN);

	m_client->send(mb);

	std::string strsenddata = testBuffer;
	LOG->debug("send sendTestFrame data:%s",strsenddata.c_str());
}

// 发送测试确认帧
void ProtocolMgr::sendTestConFrame()
{
	char testBuffer[32] = { 0 } ;
	testBuffer [ 0 ] = 0x68 ;
	testBuffer [ 1 ] = 0x04 ;
	testBuffer [ 2 ] = RCV_UTEST_CON_FRAME ;		//0x83
	testBuffer [ 3 ] = 0x00 ;
	testBuffer [ 4 ] = 0x00 ;
	testBuffer [ 5 ] = 0x00 ;

	ACE_Message_Block * mb = new ACE_Message_Block(SHORFRAME_LEN);
	ACE_OS::memcpy(mb->wr_ptr(),testBuffer,SHORFRAME_LEN);
	mb->wr_ptr(SHORFRAME_LEN);

	m_client->send(mb);

	std::string strsenddata = testBuffer;
	LOG->debug("send sendTestConFrame data:%s",strsenddata.c_str());
}

void ProtocolMgr::parseDataFrame(char *data,int datalength)
{
	//取包(去掉前两个字节的包)头
	 USER_PACKHEAD *pHead = ( USER_PACKHEAD* ) &data[0] ;

	//接收到数据接收帧标志置为true
	brcvflag = true;

	//打印接收到的报文
	char revdata[1024];
	ACE_OS::memcpy(&revdata,data,datalength);
	std::string strdata = revdata;
	LOG->debug("Recive data:%s",strdata.c_str());

	// 消息标识头
	unsigned char type;
	int pos = 0;
	ACE_OS::memcpy(&type,data+pos,THREE_LEN);

	//首先判断是否收到U帧的确认帧,若是启动定时发送I帧的定时器
	if (datalength == SHORFRAME_DATA_LEN && type == RCV_U_ACT_FRAME)
	{
		brcvflag = true;

		startITimer();
		//发送帧后没接收到数据时接收帧标志置为false
		brcvflag = false;
	}
	else if (datalength == SHORFRAME_DATA_LEN && type == RCV_S_ACT_FRAME)			//接收是短帧且类型是S帧时不处理,发送S帧的策略:每次接收到I帧后发送一个S帧
	{
		//sendSFrame();
		//发送帧后没接收到数据时接收帧标志置为false
		//brcvflag = false;
	}
	else if (datalength == SHORFRAME_DATA_LEN && type == SEND_UTEST_CON_FRAME)		//服务器主动发送测试帧：接收是短帧的同时，类型是测试帧时，发送测试帧进行确认
	{
		sendTestConFrame();
		//发送帧后没接收到数据时接收帧标志置为false
		brcvflag = false;
	}
	else if (datalength == SHORFRAME_DATA_LEN && type == RCV_UTEST_CON_FRAME)		//收到:本端发送测试帧对端回的测试确认帧,不做处理
	{
		//接收到对端发送的测试确认帧，本端不用处理，表示本端和对端数据链路是维持的
		LOG->message("Recv RCV_UTEST_CON_FRAME");
	}
	else if (datalength == LONGFRAME_DATA_LEN && data[4] == C_IC_NA_1 && data[6]==ACTCON)		//接收的数据帧为总召唤确认帧
	{
		//本端接收序号加1
		if(m_nLocalRecvNum*2 == (pHead->uSendNum&0x7F))
		{
			m_nLocalRecvNum+=1;
		}
			
		m_nRemoteSendNum = pHead->uSendNum ;

		//收到I帧后发送一帧S帧进行确认
		sendSFrame();
	}
	else if (datalength == LONGFRAME_DATA_LEN && data[4] == C_IC_NA_1 && data[6]==ACTTERM)		//接收的数据帧为总召唤结束帧
	{
		//本端接收序号加1
		if(m_nLocalRecvNum*2 == (pHead->uSendNum&0x7F))
		{
			m_nLocalRecvNum+=1;
		}

		m_nRemoteSendNum = pHead->uSendNum ;

		//收到I帧后发送一帧S帧进行确认
		sendSFrame();

		//处理接收的yx数据
		setDataRelation();

		brcvflag = true;
	}
	else
	{
		// 接收方若收到一个I格式报文,判断此I格式报文的发送序号是否等于自己的接收序号
		// 若相等则将自己的接收序号+1.
		if(m_nLocalRecvNum*2 == (pHead->uSendNum&0x7F))
		{
			m_nLocalRecvNum+=1;
		}	
		else if((pHead->uSendNum&0x7F) > m_nLocalRecvNum*2)
		{
			// I格式的发送序号大于自己的接收序号,发送方发送的一些报文丢失
		}
		else
		{
			// 发送方出现了重复发送
		}
		// I格式和S格式报文的接收序号表明了发送该报文的一方对已接收到的I格式报文的确认
		// 若发送方发送的某一I格式报文后长时间无法再对方的接收序号中确认,发生了报文丢失.
		m_nRemoteSendNum = pHead->uSendNum ;
	}

	//去掉前两个字节的包长度最小为10个字节才能解析数据帧
	if (datalength < 10)
	{
		return;
	}

	if (!(data[6] == ACTCON || data[6] == ACTTERM))		//去掉总召唤确认帧和结束帧的解析
	{
		//取包(去掉前两个字节的包)中控制数据信息
		DATA_PACKHEAD *pdataHead = ( DATA_PACKHEAD* ) &data[4] ;

		unsigned char uFrameClass = pdataHead->cTypeID;					// 类型标识符
		unsigned short nRTUaddr = (unsigned short)pdataHead->sRTUAddr ;	// RTU地址/厂站序号(地址 2个字节)
		//可变结构限定词 高低字节没有转换，后续处理
		m_nRecvDataSum = pdataHead->cDeterminer & 0x7f;					// 可变结构限定词的低7位表示接收数据的个数
		int nContType = pdataHead->cDeterminer & 0x80;					// 可变结构限定词的高1位表示后面的数据是连续非连续，0表示非连续,1表示连续
		char szCode[32]={0};
		LOG->debug("开始解析SCADA实时数据,类型为=%u",uFrameClass);
		LOG->debug("传输原因=%u",pdataHead->cTransReason);

		//根据类型标识符解析对应的数据
		switch ( uFrameClass )
		{
		case 0x00:
			{
				LOG->debug("-----------实时数据类型错误--------------");
				break;
			}
		case M_SP_NA_1:			//单点遥信:遥信值0表示分，1表示合
			{
				LOG->debug("单点全遥信个数为=%d",m_nRecvDataSum);

				//获取当前时间
				time_t ti;
				struct tm *tp;
				ti = time(NULL);
				tp = localtime(&ti);
				char chtime[32];
				//时间格式化. 
				sprintf(chtime,"%04d-%02d-%02d %02d:%02d:%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);

				//获取纯数据起始地址指针
				char *realdata;
				realdata = &data[10];

				if (pdataHead->cTransReason == SPONT)		//对端主动上送遥信值
				{
					//遥信数连续
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//遥信数据值
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

							//数据连续信息地址自增
							nAddr++;
						}
					}

					//遥信数据不连续
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//遥信数据值：每个遥信地址都要解析
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));
						}
						
					}
				}
				else if (pdataHead->cTransReason == INROGEN )	//响应总召唤,全遥信数据
				{
					//遥信数连续
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//遥信数据值
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

							//数据连续信息地址自增
							nAddr++;
						}
					}

					//遥信数据不连续
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//遥信数据值：每个遥信地址都要解析
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));
						}
					}
				}

				//接收一帧数据帧后发S帧
				sendSFrame();
			}
			break;
		case M_DP_NA_1:		//双点遥信:遥信值1表示分，2表示合,3表示未知状态
			{
				LOG->debug("双点全遥信个数为=%d",m_nRecvDataSum);

				//获取当前时间
				time_t ti;
				struct tm *tp;
				ti = time(NULL);
				tp = localtime(&ti);
				char chtime[32];
				//时间格式化. 
				sprintf(chtime,"%04d-%02d-%02d %02d:%02d:%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);


				//获取纯数据起始地址指针
				char *realdata;
				realdata = &data[10];

				if (pdataHead->cTransReason == SPONT)		//对端主动上送遥信值
				{
					//遥信数连续
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//遥信数据值
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							if (pYx->byValue == '1')
							{
								yxpointv.yxVal  = 0;
							}
							else if (pYx->byValue == '2')
							{
								yxpointv.yxVal  = 1;
							}
							else
							{
								yxpointv.yxVal  = 0;
							}
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

							//数据连续信息地址自增
							nAddr++;
						}
					}

					//遥信数据不连续
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//遥信数据值：每个遥信地址都要解析
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							if (pYx->byValue == '1')
							{
								yxpointv.yxVal  = 0;
							}
							else if (pYx->byValue == '2')
							{
								yxpointv.yxVal  = 1;
							}
							else
							{
								yxpointv.yxVal  = 0;
							}
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));
						}

					}
				}
				else if (pdataHead->cTransReason == INROGEN )	//响应总召唤,全遥信数据
				{
					//遥信数连续
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//遥信数据值
							pYx->byValue;

							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							if (pYx->byValue == '1')
							{
								yxpointv.yxVal  = 0;
							}
							else if (pYx->byValue == '2')
							{
								yxpointv.yxVal  = 1;
							}
							else
							{
								yxpointv.yxVal  = 0;
							}
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

							//数据连续信息地址自增
							nAddr++;
						}
					}

					//遥信数据不连续
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//取每个遥信值
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//遥信数据值：每个遥信地址都要解析
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;
							
							//保存解析数据到map列表中
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							if (pYx->byValue == 1)
							{
								yxpointv.yxVal  = 0;
							}
							else if (pYx->byValue == 2)
							{
								yxpointv.yxVal  = 1;
							}
							else
							{
								yxpointv.yxVal  = 0;
							}
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

						}
					}
				}

				//接收一帧数据帧后发S帧
				sendSFrame();
			}
			break;
		}
	}
}


void ProtocolMgr::startITimer()
{
	ACE_Time_Value initialDelay (1);
	ACE_Time_Value interval (m_CallALL*60*1000);

	ACE_Reactor::instance()->schedule_timer (this,0,initialDelay,interval);

}

void ProtocolMgr::stopITimer()
{
	ACE_Reactor::instance()->cancel_timer(this);
}

void ProtocolMgr::initDBCimid()
{
	m_cimid_num_n_list.clear();
	m_cimid_num_c_list.clear();

	getCimidSerialnum();

}

void ProtocolMgr::getCimidSerialnum()
{
	//string sql = "select CimId,SerialNum from cim_serial ;";
	//科东的cimid和提供的关联cimid有区别前面加_Breaker_
	string sql = "select CONCAT('_Breaker_',CimId) as CimId,SerialNum from cim_serial ;";
	
	LISTMAP staList;

	staList = App_Dba::instance()->getList(sql.c_str());


	// 把vector转buff
	for (int i=0;i<staList.size();i++)
	{
		STRMAP record = staList.at(i);
		MAP_ITERATOR iter;

		CIMID_NUM cimid_num;
		iter = record.find("CimId");
		if (iter != record.end())
		{
			cimid_num.cimid = iter->second;
		}

		iter = record.find("SerialNum");
		if (iter != record.end())
		{
			cimid_num.num = ACE_OS::atoi(iter->second.c_str());
		}

		m_cimid_num_n_list.insert(std::pair<int,CIMID_NUM>(cimid_num.num,cimid_num));

		m_cimid_num_c_list.insert(std::pair<std::string,CIMID_NUM>(cimid_num.cimid,cimid_num));

	}
}

void ProtocolMgr::setDataRelation()
{
	m_yxcimidval_list.clear();

	YXPOINT_VAL_LIST::iterator iter;
	CIMID_NUM_N_LIST::iterator cimniter;

	for (iter=m_yxpointval_list.begin();iter!=m_yxpointval_list.end();iter++)
	{
		cimniter = m_cimid_num_n_list.find(iter->first);
		if (cimniter!=m_cimid_num_n_list.end())
		{
			//填充解析到的点号对应的设备cimid对应的yx值
			YXCIMID_VAL yxcimidval;
			yxcimidval.cimid = cimniter->second.cimid;
			yxcimidval.yxVal = iter->second.yxVal;
			sprintf(yxcimidval.dtime,"%s",iter->second.dtime);

			m_yxcimidval_list.insert(std::pair<std::string,YXCIMID_VAL>(yxcimidval.cimid,yxcimidval));

		}
	}

	//将解析到的数据更新到数据库中
	updateData2DB();
	//清除保存的解析点号和yx值对应关联关系列表
	m_yxpointval_list.clear();
}

void ProtocolMgr::updateData2DB()
{
	//更新事件同步表
	YXCIMID_VAL_LIST::iterator iter;
	for (iter=m_yxcimidval_list.begin();iter!=m_yxcimidval_list.end();iter++)
	{
		string sql ;
		char *psql = "UPDATE syn_events SET EventValue=%d,SynTime='%s' WHERE UnitId='%s' ";
		sql = App_Dba::instance()->formatSql(psql,iter->second.yxVal,iter->second.dtime,iter->second.cimid.c_str());

		int nret = App_Dba::instance()->execSql(sql.c_str());

		//更新设备状态
		psql = "UPDATE unit_status SET State=%d WHERE UnitCim='%s' ";
		sql = App_Dba::instance()->formatSql(psql,iter->second.yxVal,iter->second.cimid.c_str());

		int mret = App_Dba::instance()->execSql(sql.c_str());
	}

	//更新设备状态

}





