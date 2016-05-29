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
	void				exec(sClientMsg* msg);

private:

	// ��վ����
	void		topoEntire();

	// ����Ԫ��ID�������ˣ��������ݿ�汾
	void		topoByUnitId(string unitid,string stationid,STRMAP& passNodes);

	// ����Ԫ��ID���Ҷ�Ӧ�����ӵ�
	LISTMAP		getConnIdByUnitsId(string unitid);

	// �������ӵ���ҹ������豸
	LISTMAP		getUnitsByConnId(string connid);

	// �����豸����״̬
	void		updateIsElectricByUnitId(string unitid,int state);

	// �����豸�Ǿ��Ե�Դ��
	void		updateIsPowerByUnitId(string unitid);

	// ���ر�λ����
	void		topoOnBreakerChange(sClientMsg *msg);

	// ����Ԫ��CIMID�������ˣ����������ݿ�汾
	void		topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap);


};

#endif