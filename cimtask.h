/************************************************************************/
/* 
	DESC:	CIM文件入库任务.
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
	// 启动服务
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
	// 是否启动
	bool			m_stop;

	// 检测时间
	int			m_checkHour;

	// 上次检查日期
	string		m_lastCheckDate;

	// FTP目录
	string		m_ftpPath;

	// Work目录
	string		m_workPath;

	// 文件名称
	string		m_cimName;
};


#endif