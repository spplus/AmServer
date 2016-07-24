/************************************************************************/
/* 
	DESC:	CIM�ļ����������.
	DATE:	2016-07-24
	AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __CIMLOADER_H__
#define __CIMLOADER_H__

#include "parsexml.h"

class CimLoader
{
public:
	int		Load(string cimname,int factype);

private:
	int		cim2db();
	void		save_log(char *buf);							//����������м�¼��־��Ϣ

	int		saveBaseVoltage();								//���������ѹ
	int		saveSubstation();								//���泧վ��Ϣ
	int		saveVollevel();									//�����ѹ�ȼ�

	int		saveBreaker();									//�����·��
	int		saveDisconnector();								//���浶բ
	int		saveGrdDisconnector();							//����ӵص�բ
	int		saveBusbarSection();							//����ĸ��
	int		saveCompensator();								//�������
	int		saveEnergyConsumer();							//���渺��
	int		saveSynchronousMachine();						//���淢���
	int		savePotentialTransformer();						//�����ѹ������
	int		savePowerTransformer();							//�����ѹ��
	int		saveACLineSegment();							//������·

	int		saveTransformerWinding();						//�����ѹ������
	int		saveRelateLine();								//���������վ���ϵ��
	int		saveConnectivityNode();							//�������ӵ��
	int		saveTerminalRelation();							//�������ӹ�ϵ��
	int		saveUnitStatus();								//����Ԫ����ʼ״̬��Ϣ

	// ��ȡ��ǰʱ��
private:
	//������־·����Ϣ
	char				m_log[10240];										//��������в�����log
	ParseXML	parseXML;									//���������
};


#endif