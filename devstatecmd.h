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
	void exec(sClientMsg* msg);

private:
	// ��ȡ�豸״̬
	void	getDevState(sClientMsg* msg);

	// ��ȡվ�����
	void	getStationType(sClientMsg* msg);

	// ��ȡվ���б�
	void	getStationList(sClientMsg* msg);

};

#endif

