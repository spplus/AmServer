/************************************************************************/
/* 
		DESC:	ҵ���߼����������.���ݲ�ͬ����Ϣ���ͣ��ֱ𴴽���ͬ��ҵ��
					����Ԫ���д���.
		DATE:	2016-04-20
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef			__BIZBASE_H__
#define		__BIZBASE_H__

#include "structs.h"

class BizBase
{
public:
	BizBase();

	// ִ��ҵ���߼�
	void		exec(sClientMsg* msg);
private:
	// ���ط�����æµ
	void		sendBusyBack(sClientMsg* msg);

private:
	
	// �Ƿ��ڴ�����
	bool		m_isBusy;
};

#endif