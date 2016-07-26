#include "protocolmgr.h"
#include "defines.h"


// ���˵ķ������: ÿ����һ��I��ʽ֡,�������+1
unsigned short m_nLocalSendNum = 0x00 ;				// ���˵ķ������
// ���˵Ľ���������: �����յ��˶Զ˷��͵�I��ʽ֡(�䷢�����=���˵Ľ������),�����+1
unsigned short m_nLocalRecvNum = 0x00 ;				// ���˵Ľ������
unsigned short m_nRemoteSendNum = 0x00 ;			// �Զ˵ķ������(Ҳ���Ǳ��˵Ľ������)
unsigned short nRTUaddr=0xFFFF; // ����OPEN3000; 0xFFFF ��ʾ����; �Ͼ�����0x03(������)

// �û����屨ͷ
typedef struct User_PackHead
{
	//unsigned char uKey ;				// ͬ��ͷ 68H
	//unsigned char uLen ;				//��APDU�����ݵ�Ԫ����
	unsigned short uSendNum	;			//�������
	unsigned short uRecvNum ;			//�������

} USER_PACKHEAD, *LPUSER_PACKHEAD;

// �û����屨ͷ(104��Լ�е�APCI); I��ʽ
typedef struct Data_PackHead
{
	unsigned char cTypeID;				// ���ͱ�ʶ
	unsigned char cDeterminer;			// �ɱ�ṹ�޶���
	unsigned char cTransReason;			// ����ԭ����Ӧ���ٻ���仯����
	unsigned char cSourceAddr;			// Դ����ַ(Դ����ַ����������Ӧ�����Ǹ���վ���ٻ�;
	// һ�������ʹ��;�涨Դ����ַ��ʹ��ʱ�ó�0)
	unsigned short sRTUAddr;			// ������ַ��RTU��ַ

} DATA_PACKHEAD, *LPDATA_PACKHEAD;

/*=======================================================================================================================*/
// ����֡, ��S��ʽȷ��֡
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
	LOG->debug("=========protocolmgr��send_I_Frame===========");
	//if (brcvflag)
	{
		//��ʱ����I֡:���ٻ�
		sendIFrame();
	}

	return 0;
}

void ProtocolMgr::sendUFrame()
{
	char uBuffer[32] = { 0 } ;
	uBuffer [ 0 ] = 0x68 ;				// ��ʼ�ַ�
	uBuffer [ 1 ] = 0x04 ;				// ����4���ֽ�
	uBuffer [ 2 ] = SEND_U_ACT_FRAME ;	// STARTDT ��ACT=1��CON=0; 0x07
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
    IBuffer[0] = 0x68 ;	// �����ַ�(68H)
    IBuffer[1] = 0x0E ;	// APDU����(��253)

    // �������,����Ϊ0,
    unsigned short sendnumber=m_nLocalSendNum;//
    memcpy(&IBuffer[2],&sendnumber,sizeof(unsigned short));

    // �������,����Ϊ0
    unsigned short recvnumber=m_nLocalRecvNum;
    memcpy(&IBuffer[4],&recvnumber,sizeof(unsigned short));

    // CON<100>: ���ٻ�����(ASDU���ͱ�ʶ)
    IBuffer[6]=0x64;
    // �ɱ�ṹ�޶���
    IBuffer[7]=0x01;
    IBuffer[8]=0x06;
	IBuffer[9]=0x00;		// ����ԭ��
    // ���ַΪ0����վ�������ٻ�����
    unsigned short RTUaddr=nRTUaddr;		// ASDU������ַ(���ֽ���ǰ,���ֽ��ں�),ռ�����ֽ�.ȱʡΪ0x01; (������ȷ)
    memcpy(&IBuffer[10],&RTUaddr,sizeof(unsigned short));

    IBuffer[12] = 0x00;
	IBuffer[13] = 0x00;
	IBuffer[14] = 0x00;	// ��Ϣ���ַ

    IBuffer[15] = 0x14;	// ��Ϣ��Ԫ��(�ٻ��޶���QOI=20,վ�ٻ�ȫ��)



	ACE_Message_Block * mb = new ACE_Message_Block(LONGFRAME_LEN);
	ACE_OS::memcpy(mb->wr_ptr(),IBuffer,LONGFRAME_LEN);
	mb->wr_ptr(LONGFRAME_LEN);

	m_client->send(mb);

    // ���ͷ�ÿ����һ��I��ʽ����,�䷢�����+1
    m_nLocalSendNum += 1 ;

	std::string strsenddata = IBuffer;
	LOG->debug("send sendIFrame data:%s",strsenddata.c_str());

	return;
}

//����S֡�Ĳ���:ÿ�ν��յ�I֡����һ��S֡
void ProtocolMgr::sendSFrame()
{
	//S֡��68  04  01  00  02  00
	char sBuffer[32] = { 0 } ;
	sBuffer [ 0 ] = 0x68 ;				// ��ʼ�ַ�
	sBuffer [ 1 ] = 0x04 ;				// ����4���ֽ�
	sBuffer [ 2 ] = SEND_S_ACT_FRAME ;	// STARTDT ��ACT=1��CON=0; 0x01
	sBuffer [ 3 ] = 0x00 ;
	// �������,����Ϊ0
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

// ���Ͳ���֡�������վ����һ��ʱ��û���·����Ļ���RTUҲû�������κα�����˫�������԰�Ƶ�ʷ���U֡������֡��
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

// ���Ͳ���ȷ��֡
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
	//ȡ��(ȥ��ǰ�����ֽڵİ�)ͷ
	 USER_PACKHEAD *pHead = ( USER_PACKHEAD* ) &data[0] ;

	//���յ����ݽ���֡��־��Ϊtrue
	brcvflag = true;

	//��ӡ���յ��ı���
	char revdata[1024];
	ACE_OS::memcpy(&revdata,data,datalength);
	std::string strdata = revdata;
	LOG->debug("Recive data:%s",strdata.c_str());

	// ��Ϣ��ʶͷ
	unsigned char type;
	int pos = 0;
	ACE_OS::memcpy(&type,data+pos,THREE_LEN);

	//�����ж��Ƿ��յ�U֡��ȷ��֡,����������ʱ����I֡�Ķ�ʱ��
	if (datalength == SHORFRAME_DATA_LEN && type == RCV_U_ACT_FRAME)
	{
		brcvflag = true;

		startITimer();
		//����֡��û���յ�����ʱ����֡��־��Ϊfalse
		brcvflag = false;
	}
	else if (datalength == SHORFRAME_DATA_LEN && type == RCV_S_ACT_FRAME)			//�����Ƕ�֡��������S֡ʱ������,����S֡�Ĳ���:ÿ�ν��յ�I֡����һ��S֡
	{
		//sendSFrame();
		//����֡��û���յ�����ʱ����֡��־��Ϊfalse
		//brcvflag = false;
	}
	else if (datalength == SHORFRAME_DATA_LEN && type == SEND_UTEST_CON_FRAME)		//�������������Ͳ���֡�������Ƕ�֡��ͬʱ�������ǲ���֡ʱ�����Ͳ���֡����ȷ��
	{
		sendTestConFrame();
		//����֡��û���յ�����ʱ����֡��־��Ϊfalse
		brcvflag = false;
	}
	else if (datalength == SHORFRAME_DATA_LEN && type == RCV_UTEST_CON_FRAME)		//�յ�:���˷��Ͳ���֡�Զ˻صĲ���ȷ��֡,��������
	{
		//���յ��Զ˷��͵Ĳ���ȷ��֡�����˲��ô�����ʾ���˺ͶԶ�������·��ά�ֵ�
		LOG->message("Recv RCV_UTEST_CON_FRAME");
	}
	else if (datalength == LONGFRAME_DATA_LEN && data[4] == C_IC_NA_1 && data[6]==ACTCON)		//���յ�����֡Ϊ���ٻ�ȷ��֡
	{
		//���˽�����ż�1
		if(m_nLocalRecvNum*2 == (pHead->uSendNum&0x7F))
		{
			m_nLocalRecvNum+=1;
		}
			
		m_nRemoteSendNum = pHead->uSendNum ;

		//�յ�I֡����һ֡S֡����ȷ��
		sendSFrame();
	}
	else if (datalength == LONGFRAME_DATA_LEN && data[4] == C_IC_NA_1 && data[6]==ACTTERM)		//���յ�����֡Ϊ���ٻ�����֡
	{
		//���˽�����ż�1
		if(m_nLocalRecvNum*2 == (pHead->uSendNum&0x7F))
		{
			m_nLocalRecvNum+=1;
		}

		m_nRemoteSendNum = pHead->uSendNum ;

		//�յ�I֡����һ֡S֡����ȷ��
		sendSFrame();

		//������յ�yx����
		setDataRelation();

		brcvflag = true;
	}
	else
	{
		// ���շ����յ�һ��I��ʽ����,�жϴ�I��ʽ���ĵķ�������Ƿ�����Լ��Ľ������
		// ��������Լ��Ľ������+1.
		if(m_nLocalRecvNum*2 == (pHead->uSendNum&0x7F))
		{
			m_nLocalRecvNum+=1;
		}	
		else if((pHead->uSendNum&0x7F) > m_nLocalRecvNum*2)
		{
			// I��ʽ�ķ�����Ŵ����Լ��Ľ������,���ͷ����͵�һЩ���Ķ�ʧ
		}
		else
		{
			// ���ͷ��������ظ�����
		}
		// I��ʽ��S��ʽ���ĵĽ�����ű����˷��͸ñ��ĵ�һ�����ѽ��յ���I��ʽ���ĵ�ȷ��
		// �����ͷ����͵�ĳһI��ʽ���ĺ�ʱ���޷��ٶԷ��Ľ��������ȷ��,�����˱��Ķ�ʧ.
		m_nRemoteSendNum = pHead->uSendNum ;
	}

	//ȥ��ǰ�����ֽڵİ�������СΪ10���ֽڲ��ܽ�������֡
	if (datalength < 10)
	{
		return;
	}

	if (!(data[6] == ACTCON || data[6] == ACTTERM))		//ȥ�����ٻ�ȷ��֡�ͽ���֡�Ľ���
	{
		//ȡ��(ȥ��ǰ�����ֽڵİ�)�п���������Ϣ
		DATA_PACKHEAD *pdataHead = ( DATA_PACKHEAD* ) &data[4] ;

		unsigned char uFrameClass = pdataHead->cTypeID;					// ���ͱ�ʶ��
		unsigned short nRTUaddr = (unsigned short)pdataHead->sRTUAddr ;	// RTU��ַ/��վ���(��ַ 2���ֽ�)
		//�ɱ�ṹ�޶��� �ߵ��ֽ�û��ת������������
		m_nRecvDataSum = pdataHead->cDeterminer & 0x7f;					// �ɱ�ṹ�޶��ʵĵ�7λ��ʾ�������ݵĸ���
		int nContType = pdataHead->cDeterminer & 0x80;					// �ɱ�ṹ�޶��ʵĸ�1λ��ʾ�����������������������0��ʾ������,1��ʾ����
		char szCode[32]={0};
		LOG->debug("��ʼ����SCADAʵʱ����,����Ϊ=%u",uFrameClass);
		LOG->debug("����ԭ��=%u",pdataHead->cTransReason);

		//�������ͱ�ʶ��������Ӧ������
		switch ( uFrameClass )
		{
		case 0x00:
			{
				LOG->debug("-----------ʵʱ�������ʹ���--------------");
				break;
			}
		case M_SP_NA_1:			//����ң��:ң��ֵ0��ʾ�֣�1��ʾ��
			{
				LOG->debug("����ȫң�Ÿ���Ϊ=%d",m_nRecvDataSum);

				//��ȡ��ǰʱ��
				time_t ti;
				struct tm *tp;
				ti = time(NULL);
				tp = localtime(&ti);
				char chtime[32];
				//ʱ���ʽ��. 
				sprintf(chtime,"%04d-%02d-%02d %02d:%02d:%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);

				//��ȡ��������ʼ��ַָ��
				char *realdata;
				realdata = &data[10];

				if (pdataHead->cTransReason == SPONT)		//�Զ���������ң��ֵ
				{
					//ң��������
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//ң������ֵ
							pYx->byValue;

							//����������ݵ�map�б���
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

							//����������Ϣ��ַ����
							nAddr++;
						}
					}

					//ң�����ݲ�����
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//ң������ֵ��ÿ��ң�ŵ�ַ��Ҫ����
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;

							//����������ݵ�map�б���
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));
						}
						
					}
				}
				else if (pdataHead->cTransReason == INROGEN )	//��Ӧ���ٻ�,ȫң������
				{
					//ң��������
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//ң������ֵ
							pYx->byValue;

							//����������ݵ�map�б���
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));

							//����������Ϣ��ַ����
							nAddr++;
						}
					}

					//ң�����ݲ�����
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//ң������ֵ��ÿ��ң�ŵ�ַ��Ҫ����
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;

							//����������ݵ�map�б���
							YXPOINT_VAL yxpointv;
							yxpointv.point = nAddr;
							yxpointv.yxVal  = pYx->byValue;
							sprintf(yxpointv.dtime,"%s",chtime);

							m_yxpointval_list.insert(std::pair<int,YXPOINT_VAL>(yxpointv.point,yxpointv));
						}
					}
				}

				//����һ֡����֡��S֡
				sendSFrame();
			}
			break;
		case M_DP_NA_1:		//˫��ң��:ң��ֵ1��ʾ�֣�2��ʾ��,3��ʾδ֪״̬
			{
				LOG->debug("˫��ȫң�Ÿ���Ϊ=%d",m_nRecvDataSum);

				//��ȡ��ǰʱ��
				time_t ti;
				struct tm *tp;
				ti = time(NULL);
				tp = localtime(&ti);
				char chtime[32];
				//ʱ���ʽ��. 
				sprintf(chtime,"%04d-%02d-%02d %02d:%02d:%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);


				//��ȡ��������ʼ��ַָ��
				char *realdata;
				realdata = &data[10];

				if (pdataHead->cTransReason == SPONT)		//�Զ���������ң��ֵ
				{
					//ң��������
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//ң������ֵ
							pYx->byValue;

							//����������ݵ�map�б���
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

							//����������Ϣ��ַ����
							nAddr++;
						}
					}

					//ң�����ݲ�����
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//ң������ֵ��ÿ��ң�ŵ�ַ��Ҫ����
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;

							//����������ݵ�map�б���
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
				else if (pdataHead->cTransReason == INROGEN )	//��Ӧ���ٻ�,ȫң������
				{
					//ң��������
					if (nContType == T_Continuity)
					{
						YX_TO_BS yxtbs;
						memcpy(&yxtbs,realdata,sizeof(YX_TO_BS));
						unsigned int nAddr = getInfoAddr(yxtbs.szAddr);

						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_BS *pYx = (YX_BS*)&realdata[3 + i*sizeof(YX_BS)];

							//ң������ֵ
							pYx->byValue;

							//����������ݵ�map�б���
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

							//����������Ϣ��ַ����
							nAddr++;
						}
					}

					//ң�����ݲ�����
					if (nContType == T_NotContinuity)
					{
						for (int i=0;i<m_nRecvDataSum;i++)
						{
							//ȡÿ��ң��ֵ
							YX_TO_BS *pYx = (YX_TO_BS*)&realdata[i*sizeof(YX_TO_BS)];
							//ң������ֵ��ÿ��ң�ŵ�ַ��Ҫ����
							unsigned int nAddr = getInfoAddr(pYx->szAddr);
							pYx->byValue;
							
							//����������ݵ�map�б���
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

				//����һ֡����֡��S֡
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
	//�ƶ���cimid���ṩ�Ĺ���cimid������ǰ���_Breaker_
	string sql = "select CONCAT('_Breaker_',CimId) as CimId,SerialNum from cim_serial ;";
	
	LISTMAP staList;

	staList = App_Dba::instance()->getList(sql.c_str());


	// ��vectorתbuff
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
			//���������ĵ�Ŷ�Ӧ���豸cimid��Ӧ��yxֵ
			YXCIMID_VAL yxcimidval;
			yxcimidval.cimid = cimniter->second.cimid;
			yxcimidval.yxVal = iter->second.yxVal;
			sprintf(yxcimidval.dtime,"%s",iter->second.dtime);

			m_yxcimidval_list.insert(std::pair<std::string,YXCIMID_VAL>(yxcimidval.cimid,yxcimidval));

		}
	}

	//�������������ݸ��µ����ݿ���
	updateData2DB();
	//�������Ľ�����ź�yxֵ��Ӧ������ϵ�б�
	m_yxpointval_list.clear();
}

void ProtocolMgr::updateData2DB()
{
	//�����¼�ͬ����
	YXCIMID_VAL_LIST::iterator iter;
	for (iter=m_yxcimidval_list.begin();iter!=m_yxcimidval_list.end();iter++)
	{
		string sql ;
		char *psql = "UPDATE syn_events SET EventValue=%d,SynTime='%s' WHERE UnitId='%s' ";
		sql = App_Dba::instance()->formatSql(psql,iter->second.yxVal,iter->second.dtime,iter->second.cimid.c_str());

		int nret = App_Dba::instance()->execSql(sql.c_str());

		//�����豸״̬
		psql = "UPDATE unit_status SET State=%d WHERE UnitCim='%s' ";
		sql = App_Dba::instance()->formatSql(psql,iter->second.yxVal,iter->second.cimid.c_str());

		int mret = App_Dba::instance()->execSql(sql.c_str());
	}

	//�����豸״̬

}





