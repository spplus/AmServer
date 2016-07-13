/************************************************************************/
/* 
	DESC:		���˷�������߼�.
	DATE:		2016-05-24
	AUTHOR:	YUANLS	
*/
/************************************************************************/
#ifndef __TOPOBIZCMD_H__
#define __TOPOBIZCMD_H__

#include "cmdbase.h"
class TopoBizCmd	:public CmdBase
{
public:
	void			exec(sClientMsg* msg);

	// ���浵ID��������
	void			topoBySaveId(string saveid,int unittype=eGENERATOR);
private:

	// ��վ����
	void		topoEntire();

	// ����Ԫ��ID�������ˣ��������ݿ�汾
	void		topoByUnitId(string saveid,string unitid,string stationid,STRMAP& passNodes);

	// ���˽ӵص�բ
	void		topoByGround(string saveid,string unitid,string stationid,STRMAP& passNodes);

	// ����Ԫ��ID���Ҷ�Ӧ�����ӵ�
	LISTMAP		getConnIdByUnitsId(string unitid);

	// ���ݽ�����ID�����ҽ����߹�����վ��ID
	LISTMAP		getStationIdByLineId(string unitid,string stationid);

	// �������ӵ���ҹ������豸
	LISTMAP		getUnitsByConnId(string connid,string saveid);

	// �����豸����״̬
	void		updateIsElectricByUnitId(string saveid,string unitid,int state);

	// ����Ϊ�Ƿ�ӵ�
	void		updateIsGroundByUnitId(string saveid,string unitid,int state);

	// �����豸�Ǿ��Ե�Դ��
	void		updateIsPowerByUnitId(string unitid,string stationid,string saveid);


	// ���ر�λ����
	void		topoOnBreakerChange(sClientMsg *msg);

	// ����Ԫ��CIMID�������ˣ����������ݿ�汾
	void		topoByUnitIdMem(PBNS::StateBean bean,string saveid,string cimid,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap);

	// ִ�п��ر�λ����
	string	execTopoOnBreakerChange(int saveid,string unitcim,int state);

	// ������
	void		roleCheck(int connid,int saveid,string unitcim,eDeviceType devType,int optype);
	void		sendRuleBack(int connid,int optype,vector<int> ruleList);

	// ���ô���״̬
	void		updateIsElectric(string saveid,string unitcim);


	// �������Ƿ���Ч
	bool		checkRuleIsUse(string cimid,int ruleid);

	bool		check1(int saveid,string unitcim);
	bool		check2(int saveid,string unitcim);
	bool		check4(int saveid,string unitcim);
	bool		check5(int saveid,string unitcim);
	bool		check12(int saveid,string unitcim);
	bool		check16(int saveid,string unitcim);
	bool		check18(int saveid,string unitcim);
	bool		check20(int saveid,string unitcim);
	bool		check21(int saveid,string unitcim);
	bool		check22(int saveid,string unitcim);
	bool		check25(int saveid,string unitcim,int optype);
	bool		check26(int saveid,string unitcim);
	bool		check27(int saveid,string unitcim);
	bool		check28(int saveid,string unitcim);
	bool		check29(int saveid,string unitcim);
	bool		check32(int saveid,string unitcim);
	bool		check33(int saveid,string unitcim);
	bool		check34(int saveid,string unitcim);
	bool		check35(int saveid,string unitcim);
	bool		check38(int saveid,string unitcim);
	bool		check39(int saveid,string unitcim);
	bool		check43(int saveid,string unitcim);

	bool		check45(int saveid,string unitcim);
	bool		check46(int saveid,string unitcim);
	bool		check47(int saveid,string unitcim);
	bool		check48(int saveid,string unitcim);
};

#endif