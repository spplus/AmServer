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
	void				topoEntire();

	// ����Ԫ��ID���Ҷ�Ӧ�����ӵ�
	LISTMAP	getConnIdByUnitsId(string unitid);

	// �������ӵ���ҹ������豸
	LISTMAP		getUnitsByConnId(string connid);

};

#endif