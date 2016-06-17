/************************************************************************/
/* 
	DESC:	��ȡ����״̬.
	DATE:	2016-05-03
	AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __DEVSTATECMD_H__
#define __DEVSTATECMD_H__

#include "cmdbase.h"

class DevStateCmd	:public CmdBase
{
public:
	void		exec(sClientMsg* msg);

private:
	// ��ȡ�豸״̬
	void		getDevState(sClientMsg* msg);

	// ��ȡվ�����
	void		getStationType(sClientMsg* msg);

	// ��ȡվ���б�
	void		getStationList(sClientMsg* msg);

	void		getStationByTypeId(PBNS::StationTypeBean *typebean);

	// ��ժ��
	void		updateIsBoard(sClientMsg* msg);			

	// ���õ�Դ��
	void		updateIsPower(sClientMsg* msg);

	// ���ý�����
	void		updateIsLine(sClientMsg* msg);

	// ��ȡ�浵�б�
	void		getSavingList(sClientMsg* msg);

	// ����浵
	void		writeSaving(sClientMsg* msg);
};

#endif

