/************************************************************************/
/* 
	DESC:	CIM�ļ��������.
	DATE:	2016-07-23
	AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __CIMTASK_H__
#define __CIMTASK_H__

#include <string>
#include "ace/Task.h"


using namespace std;

class CimTask		:public ACE_Task<ACE_MT_SYNCH>
{
public:
	CimTask();

public:
	// ��������
	void		start();
	void		stop();
	bool		fileCopy(const char* destpath,const char* srcpath);
	bool		checkFile();
protected:
	int		svc();
	int		getHour();
	string	getCurDate();
	void		updateIsNew();
	bool		needUpdate(const char* destpath,const char* srcpath);
private:
	// �Ƿ�����
	bool			m_stop;

	// ���ʱ��
	int			m_checkHour;

	// �ϴμ������
	string		m_lastCheckDate;

	// FTPĿ¼
	string		m_ftpPath;

	// WorkĿ¼
	string		m_workPath;

	// �ļ�����
	string		m_cimName;
};


#endif