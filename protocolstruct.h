
#ifndef _PROTOCOLSTRUCT_H_
#define _PROTOCOLSTRUCT_H_

#define			HEADLEN_LEN				2					//֡ͷ����:������һ���ֽڹؼ���(0x68)�͵ڶ����ֽ�����������
#define			HEAD_TAG				0x68				//֡ͷ�ؼ���
#define			FIRST_LEN				1					//��һ���ֽ�
#define			SECOND_LEN				1					//�ڶ����ֽ�
#define			THREE_LEN				1					//�������ֽ�
#define			DATAFRAME_MAX_LEN		0xFF				//��������󳤶�
#define			DATAFRAME_MIN_LEN		4					//��������С����

#define			SHORFRAME_LEN			6					//��֡����
#define			LONGFRAME_LEN			16					//��֡����
#define			SHORFRAME_DATA_LEN		4					//��֡����������,��֡����:U֡�ķ���ȷ��֡��S֡�ķ��ͺ�ȷ��֡������֡�ķ���ȴ֡
#define			LONGFRAME_DATA_LEN		14					//��֡���������ȣ��������ʾI֡�����ݳ���

#define			SEND_U_ACT_FRAME		0x07				//U֡���ͼ���֡(�״ν�������)
#define			RCV_U_ACT_FRAME			0x0B				//U֡����ȷ��֡
#define			SEND_UTEST_CON_FRAME	0x43				//U֡���Ͳ���֡
#define			RCV_UTEST_CON_FRAME		0x83				//U֡����ȷ��֡
#define			SEND_S_ACT_FRAME		0x01				//S֡����ȷ��֡
#define			RCV_S_ACT_FRAME			0x01				//S֡����ȷ��֡



/**********************************************************************/
/*
* �����������ͱ�ʶ
*/
enum Enum_TypeContinuity
{
	/**
	* ��������
	*/
	T_Continuity = 1,
	/**
	* ���ݲ�����
	*/
	T_NotContinuity = 0
};

/*
* ���ͱ�ʶ
*/
enum Enum_TypeIdentification
{
	/**
	* ������Ϣ
	*/
	M_SP_NA_1 = 0x01,
	/**
	* ��ʱ��ĵ�����Ϣ
	*/
	M_SP_TA_1 = 0x02,
	/**
	* ˫����Ϣ
	*/
	M_DP_NA_1 = 0x03,
	/**
	* ��ʱ���˫����Ϣ
	*/
	M_DP_TA_1 = 0x04,
	/**
	* ��λ����Ϣ
	*/
	M_ST_NA_1 = 0x05,
	/**
	* ��ʱ��Ĳ�λ����Ϣ
	*/
	M_ST_TA_1 = 0x06,
	/**
	* 32 ���ش�
	*/
	M_BO_NA_1 = 0x07,
	/**
	* ��ʱ���32 ���ش�
	*/
	M_BO_TA_1 = 0x08,
	/**
	* ����ֵ����һ��ֵ
	*/
	M_ME_NA_1 = 0x09,
	/**
	* ��ʱ��Ĳ���ֵ,��һ��ֵ
	*/
	M_ME_TA_1 = 0x0a,
	/**
	* ����ֵ����Ȼ�ֵ
	*/
	M_ME_NB_1 = 0x0b,
	/**
	* ��ʱ��Ĳ���ֵ����Ȼ�ֵ
	*/
	M_ME_TB_1 = 0x0c,
	/**
	* ����ֵ���̸�����
	*/
	M_ME_NC_1 = 0x0d,
	/**
	* ��ʱ��Ĳ���ֵ���̸�����
	*/
	M_ME_TC_1 = 0x0e,
	/**
	* �ۼ���
	*/
	M_IT_NA_1 = 0x0f,
	/**
	* ��ʱ����ۼ���
	*/
	M_IT_TA_1 = 0x10,
	/**
	* ��ʱ��ļ̵籣���豸�¼�
	*/
	M_EP_TA_1 = 0x11,
	/**
	* ��ʱ��ļ̵籣���豸���������¼�
	*/
	M_EP_TB_1 = 0x12,
	/**
	* ��ʱ��ļ̵籣���豸���������·��Ϣ
	*/
	M_EP_TC_1 = 0x13,
	/**
	* ����λ����ĳ��鵥����Ϣ
	*/
	M_PS_NA_1 = 0x14,
	/**
	* ����ֵ������Ʒ�������ʵĹ�һ��ֵ
	*/
	M_ME_ND_1 = 0x15,
	/**
	* ��CP56Time2a ʱ��ĵ�����Ϣ
	*/
	M_SP_TB_1 = 0x16,
	/**
	* ��CP56Time2a ʱ���˫����Ϣ31
	*/
	M_DP_TB_1 = 0x1f,
	/**
	* ��CP56Time2a ʱ��Ĳ�λ����Ϣ
	*/
	M_ST_TB_1 = 0x20,
	/**
	* ��CP56Time2a ʱ���32 ���ش�
	*/
	M_BO_TB_1 = 0x21,
	/**
	* ��CP56Time2a ʱ��Ĳ���ֵ����һ��ֵ
	*/
	M_ME_TD_1 = 0x22,
	/**
	* ��CP56Time2a ʱ��Ĳ���ֵ����Ȼ�ֵ
	*/
	M_ME_TE_1 = 0x23,
	/**
	* ��CP56Time2a ʱ��Ĳ���ֵ���̸�����
	*/
	M_ME_TF_1 = 0x24,
	/**
	* ��CP56Time2a ʱ����ۼ���
	*/
	M_IT_TB_1 = 0x25,
	/**
	* ��CP56Time2a ʱ��ļ̵籣���豸�¼�
	*/
	M_EP_TD_1 = 0x26,
	/**
	* ��CP56Time2a ʱ��ļ̵籣���豸���������¼�
	*/
	M_EP_TE_1 = 0x27,
	/**
	* ��CP56Time2a ʱ��ļ̵籣��װ�ó��������·��Ϣ
	*/
	M_EP_TF_1 = 0x28,
	/**
	* ��������
	*/
	C_SC_NA_1 = 0x2d,
	/**
	* ˫������
	*/
	C_DC_NA_1 = 0x2e,
	/**
	* ����������
	*/
	C_RC_NA_1 = 0x2f,
	/**
	* �趨ֵ�����һ��ֵ��ֱ�ӿ������
	*/
	C_SE_NA_1 = 0x30,
	/**
	* �趨ֵ�����Ȼ�ֵ
	*/
	C_SE_NB_1 = 0x31,
	/**
	* �趨ֵ����̸�����
	*/
	C_SE_NC_1 = 0x32,
	/**
	* 32 ���ش�
	*/
	C_BO_NC_1 = 0x33,
	/**
	* ��ʼ������
	*/
	M_EI_NA_1 = 0x46,
	/**
	* ���ٻ�������������ٻ���
	*/
	C_IC_NA_1 = 0x64,
	/**
	* �ۼ����ٻ�����
	*/
	C_CI_NA_1 =  0x65,
	/**
	* ������
	*/
	C_RD_NA_1 =  0x66,
	/**
	* ʱ��ͬ������
	*/
	C_CS_NA_1 =  0x67,
	/**
	* ��������
	*/
	C_TS_NA_1 =  0x68,
	/**
	* ��λ��������
	*/
	C_RP_NA_1 =  0x69,
	/**
	* ��ʱ�������
	*/
	C_CD_NA_1 =  0x6a,
	/**
	* ����ֵ��������һ��ֵ
	*/
	P_ME_NA_1 =  0x6e,
	/**
	* ����ֵ��������Ȼ�ֵ
	*/
	P_ME_NB_1 =  0x6f,
	/**
	* ����ֵ�������̸�����
	*/
	P_ME_NC_1 =  0x70,
	/**
	* ��������
	*/
	P_AC_NA_1 = 0x71,
	/**
	* �ļ�׼������
	*/
	F_FR_NA_1 = 0x78,
	/**
	* ��׼������
	*/
	F_SR_NA_1 = 0x79,
	/**
	* �ٻ�Ŀ¼��ѡ���ļ����ٻ��ļ��ٻ���
	*/
	F_SC_NA_1 = 0x7a,
	/**
	* ���Ľڣ����Ķ�
	*/
	F_LS_NA_1 = 0x7b,
	/**
	* �Ͽ��ļ����Ͽɽ�
	*/
	F_AF_NA_1 = 0x7c,
	/**
	* ��
	*/
	F_SG_NA_1 = 0x7d,
	/**
	* Ŀ¼
	*/
	F_DR_NA_1 = 0x7e
};

/*
* ����ԭ��
*/
enum Enum_CauseOfTransmission
{
	/**
	 * ���ڡ�ѭ��
	 */
	PER_CYC = 1,
	/**
	 * ����ɨ��
	 */
	BACK = 2,
	/**
	 * ͻ��(�Է�)
	 */
	SPONT = 3,
	/**
	 * ��ʼ�����
	 */
	INIT = 4,
	/**
	 * ������߱�����
	 */
	REQ = 5,
	/**
	 * ����
	 */
	ACT = 6,
	/**
	 * ����ȷ��
	 */
	ACTCON = 7,
	/**
	 * ֹͣ����
	 */
	DEACT = 8,
	/**
	 * ֹͣ����ȷ��
	 */
	DEACTCON = 9,
	/**
	 * ������ֹ
	 */
	ACTTERM = 10,
	/**
	 * Զ����������ķ�����Ϣ
	 */
	RETREM = 11,
	/**
	 * ������������ķ�����Ϣ
	 */
	RETLOC = 12,
	/**
	 * �ļ�����
	 */
	FILE_FILE = 13,//FILE���ͻ����ΪFILE_FILE
	/**
	 * ��Ӧվ�ٻ�
	 */
	INROGEN = 20,
	/**
	 * ��Ӧ��1���ٻ�
	 */
	INRO1 = 21,
	/**
	 * ��Ӧ��2���ٻ�
	 */
	INRO2 = 22,
	/**
	 * ��Ӧ��3���ٻ�
	 */
	INRO3 = 23,
	/**
	 * ��Ӧ��4���ٻ�
	 */
	INRO4 = 24,
	/**
	 * ��Ӧ��5���ٻ�
	 */
	INRO5 = 25,
	/**
	 * ��Ӧ��6���ٻ�
	 */
	INRO6 = 26,
	/**
	 * ��Ӧ��7���ٻ�
	 */
	INRO7 = 27,
	/**
	 * ��Ӧ��8���ٻ�
	 */
	INRO8 = 28,
	/**
	 * ��Ӧ��9���ٻ�
	 */
	INRO9 = 29,
	/**
	 * ��Ӧ��10���ٻ�
	 */
	INRO10 = 30,
	/**
	 * ��Ӧ��11���ٻ�
	 */
	INRO11 = 31,
	/**
	 * ��Ӧ��12���ٻ�
	 */
	INRO12 = 32,
	/**
	 * ��Ӧ��13���ٻ�
	 */
	INRO13 = 33,
	/**
	 * ��Ӧ��14���ٻ�
	 */
	INRO14 = 34,
	/**
	 * ��Ӧ��15���ٻ�
	 */
	INRO15 = 35,
	/**
	 * ��Ӧ��16���ٻ�
	 */
	INRO16 = 36,
	/**
	 * ��Ӧ������վ�ٻ�
	 */
	REQCOGEN = 37,
	/**
	 * ��Ӧ��1��������ٻ�
	 */
	REQCO1 = 38,
	/**
	 * ��Ӧ��2��������ٻ�
	 */
	REQCO2 = 39,
	/**
	 * ��Ӧ��3��������ٻ�
	 */
	REQCO3 = 40,
	/**
	 * ��Ӧ��4��������ٻ�
	 */
	REQCO4 = 41,
	/**
	 * δ֪�����ͱ�ʶ
	 */
	UNKNOWNTYPEID = 44,
	/**
	 * δ֪�Ĵ���ԭ��
	 */
	UNKNOWNCOT = 45,
	/**
	 * δ֪��Ӧ�÷������ݵ�Ԫ������ַ
	 */
	UNKNOWNASDUCOMADDR = 46,
	/**
	 * δ֪����Ϣ�����ַ
	 */
	UNKNOWNINFOOBJADDR = 47
};


/*֡ͷ*/
typedef struct FRAME_HEAD_tag{
	unsigned char start;//�����ֽ�0x68
	unsigned char len;//Ӧ�ù�Լ���ݵ�Ԫ�ĳ���
}FRAME_HEAD,*P_FRAME_HEAD;

/*====================================================================*/
// ң�����ݽṹ
typedef struct _YX_TO_BS
{
	unsigned char szAddr[3];		// ��Ϣ���ַ(3���ֽڱ�ʾ��)
	unsigned char byValue ;			// ����ֵ
}  YX_TO_BS , *LPYX_TO_BS ;

/*====================================================================*/

// ң�����ݽṹ
typedef struct _YX_BS
{
	unsigned char byValue ;			// ����ֵ
}  YX_BS, *LPYX_BS ;

/*====================================================================*/

//�豸cimid���Ŷ�Ӧ��ϵ
typedef struct _CIMID_NUM
{
	std::string cimid;				//�豸cimid
	int			num;				//���(���ݿ���)
} CIMID_NUM;

typedef std::map<int,CIMID_NUM> CIMID_NUM_N_LIST;

typedef std::map<std::string,CIMID_NUM> CIMID_NUM_C_LIST;

//��Լ��������YX��ź�YXֵ��Ӧ��ϵ
typedef struct _YXPOINT_VAL
{
	int  point;						//��Լ�������ĵ��
	int  yxVal;						//��Ŷ�Ӧ��������ң��ֵ
	char dtime[32];					//ʱ��
} YXPOINT_VAL;

typedef std::map<int,YXPOINT_VAL> YXPOINT_VAL_LIST;

//��Լ��������YX��Ź���ϵͳ�е��豸cimid��YXֵ��Ӧ��ϵ
typedef struct _YXCIMID_VAL
{
	std::string  cimid;				//��Լ��������Ŷ�Ӧ��cimid
	int  yxVal;						//��Ŷ�Ӧ��������ң��ֵ
	char dtime[32];					//ʱ��
} YXCIMID_VAL;

typedef std::map<std::string,YXCIMID_VAL> YXCIMID_VAL_LIST;

#endif;