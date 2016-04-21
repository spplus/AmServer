
#include <stdio.h>
#include <stdarg.h>
#include <sys/timeb.h>

#include <ace/OS.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/os_include/os_unistd.h>
#include <ace/os_include/sys/os_stat.h>
#include <ace/os_include/os_time.h>
#include <ace/Time_Value.h>
#include <ace/FILE_Addr.h>
#include <ace/Addr.h>
#include <ace/Message_Block.h>
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>

#include "logger.h"

static void * pthr_cleaner_process(void * arg);

/*-----------------------------------------------------------------------
* name:		��ʼ����־ϵͳ����
* input:	fdir  -- ��־�ļ���Ŀ¼
*			_media  -- ��־ý��
*			_level  -- ��־����
*			_cleaner_switch  -- ��־������
*			_saved_month  -- ��־��������
* output:	NONE
* return:	NONE
*-----------------------------------------------------------------------*/
void Logger::init_logger(const char * _fdir, unsigned int _media, unsigned int _level, int _cleaner_switch, int _saved_month)
{
	this->media_ = _media;
	this->level_ = _level;
	this->cleaner_switch_ = _cleaner_switch;
	this->saved_month_ = _saved_month;

	ACE_OS::strncpy(this->parentdir_, _fdir, sizeof(this->parentdir_));
	if (ACE_OS::strlen(this->parentdir_) <= 0)
	{
		ACE_OS::sprintf(this->parentdir_, "%s",HP_LOG_FILEDIR);
	}
}

/*-----------------------------------------------------------------------
* name:		��ʼ����־ϵͳ
* 			ÿ��ϵͳ���ô���־��ʱ ��Ҫ���ô˺�����ʼ����־ϵͳ
* input:	NONE
* output:	NONE
* return:	-1 -- ʧ��
* 			 1 -- �ɹ�
*-----------------------------------------------------------------------*/
int Logger::open_logger()
{
	ACE_OS::memset(this->filedir_, 0x00, sizeof(this->filedir_));
	ACE_OS::memset(this->filename_, 0x00, sizeof(this->filename_));
	ACE_OS::memset(this->fullpath_, 0x00, sizeof(this->fullpath_));

	/*������Ŀ¼*/
	if (ACE_OS::strlen(this->parentdir_) <= 0)
		ACE_OS::sprintf(this->parentdir_, "%s",HP_LOG_FILEDIR);
	if (this->mkdir(this->parentdir_) < 0)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) Create root dir��'%s' failed\n"), this->parentdir_), -1);
	}
	if (this->parentdir_[ACE_OS::strlen(this->parentdir_)-1] != '/')
	{
		ACE_OS::sprintf(this->parentdir_, "%s/", this->parentdir_);
	}

	/*�����־Ŀ¼��ͬʱ��ʼ��this->filedir_*/
	this->check_subdir(this->parentdir_);

	this->create_filename(this->filename_);

	ACE_OS::sprintf(this->fullpath_, "%s%s", this->filedir_, this->filename_);

	if (this->fp_conn_.connect(this->fp_, ACE_FILE_Addr(ACE_TEXT(this->fullpath_)), 0, ACE_Addr::sap_any, 0, O_RDWR|O_APPEND|O_CREAT, ACE_DEFAULT_FILE_PERMS) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n to %s"), ACE_TEXT("open"), this->fullpath_), -1);
	}
	else
	{
		chmod(this->fullpath_,S_IRWXU|S_IRWXG|S_IRWXO); /*����Ȩ��*/
	}

	/*������־ϵͳ*/
	this->active_ = true;

	/*������־�����߳�*/
	if (this->activate() == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) Logger activate failed \n")), -1);
	}

	if (cleaner_switch_)
	{
		/*������־�����߳�*/
		if (ACE_Thread_Manager::instance()->spawn_n(1,
			(ACE_THR_FUNC)pthr_cleaner_process,/*Execute task one*/
			this, /*arguments*/
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, /*New Light Weight Process*/
			ACE_DEFAULT_THREAD_PRIORITY,
			HP_LOG_GRP_ID)==-1) /*Group ID*/
			ACE_ERROR((LM_ERROR, "Failure to spawn first group of threads: %p \n"));
	}
	
	return 1;
}

/*-----------------------------------------------------------------------
* name:		�ر���־ϵͳ
* input:	NONE
* output:	NONE
* return:	NONE
*-----------------------------------------------------------------------*/
void Logger::close_logger()
{
	this->active_ = false;

	ACE_Thread_Manager::instance()->cancel_grp(HP_LOG_GRP_ID);
	ACE_Thread_Manager::instance()->wait_grp(HP_LOG_GRP_ID);

	this->msg_queue()->close();
	this->wait();

	this->fp_.close();
}

/*-----------------------------------------------------------------------
* name:		���������ļ�
* input:	_config_filename  -- ��־�ļ�·��
* output:	NONE
* return:	-1 -- ʧ��
* 			1 -- �ɹ�
*-----------------------------------------------------------------------*/
int Logger::load_config(const char * _config_filename)
{
	ACE_TString str;
	long num;

	ACE_Configuration_Heap config;
	if (config.open() == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("config.open()")), -1);
	}

	ACE_Ini_ImpExp config_importer(config);
	if (config_importer.import_config(ACE_TEXT(_config_filename)) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), _config_filename), -1);
	}

	ACE_Configuration_Section_Key status_section;
	if (config.open_section (config.root_section(),ACE_TEXT(HP_LOG_CONF_SECTION), 0, status_section) == -1)
	{
		ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT ("Can't open [LOGGER] section")), -1);
	}

	/*��־�����·��*/
	if (config.get_string_value(status_section, ACE_TEXT(HP_LOG_CONF_LOGFILEDIR), str) != -1)
	{
		if (str.length()>0 && str.c_str()[str.length()-1]!='/')
		{
			str.append("/", 1);
		}
		if (ACE_OS::strlen(this->parentdir_)>0 && this->parentdir_[ACE_OS::strlen(this->parentdir_)-1]!='/')
		{
			ACE_OS::strcat(this->parentdir_, "/");
		}
		if (ACE_OS::strcmp(str.c_str(), this->parentdir_) != 0)
		{
			//ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) [%s]->'%s' is updated (%s)->(%s)\n"),HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGFILEDIR, this->parentdir_, str.c_str()));
			ACE_OS::memset(this->parentdir_, 0x00, sizeof(this->parentdir_));
			ACE_OS::strncpy(this->parentdir_, str.c_str(), sizeof(this->parentdir_));
		}
		str.clear();
	}
	else
	{
		ACE_OS::memset(this->parentdir_, 0x00, sizeof(this->parentdir_));
		ACE_OS::strncpy(this->parentdir_, "log", sizeof(this->parentdir_));
		ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' does not exist, default value is (%s)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGFILEDIR, this->parentdir_));
	}

	/*��־���ý��  0����Ļ  1���ļ�  2����Ļ+�ļ�*/
	if (config.get_string_value(status_section, ACE_TEXT(HP_LOG_CONF_LOGMEDIA), str) != -1)
	{
		try
		{
			num = ACE_OS::strtol(str.c_str(), NULL, 10);
			if (num != this->media_)
			{
				//ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) [%s]->'%s' is updated (%d)->(%d)\n"),HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGMEDIA, this->media_, num));
				this->media_ = num;
			}
		}
		catch (...)
		{
			this->media_ = 2;
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' is catch exception, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGMEDIA, this->media_));
		}
		num = 0;
	}
	else
	{
		this->media_ = 2;
		ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' does not exist, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGMEDIA, this->media_));
	}

	/*��־��ӡ����  0��ERROR  1��WARN  2��MESSAGE  3��DEBUG*/
	if (config.get_string_value(status_section, ACE_TEXT(HP_LOG_CONF_LOGLEVEL), str) != -1)
	{
		try
		{
			num = ACE_OS::strtol(str.c_str(), NULL, 10);
			if (num != this->level_)
			{
				//ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) [%s]->'%s' is updated (%d)->(%d)\n"),HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGLEVEL, this->level_, num));
				this->level_ = num;
			}
		}
		catch (...)
		{
			this->level_ = 3;
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' is catch exception, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGLEVEL, this->level_));
		}
		num = 0;
	}
	else
	{
		this->level_ = 3;
		ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' does not exist, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGLEVEL, this->level_));
	}

	/*��־������  1������0����*/
	if (config.get_string_value(status_section, ACE_TEXT(HP_LOG_CONF_LOGCLEANER), str) != -1)
	{
		try
		{
			num = ACE_OS::strtol(str.c_str(), NULL, 10);
			if (num != this->cleaner_switch_)
			{
				//ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) [%s]->'%s' is updated (%d)->(%d)\n"),HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGCLEANER, this->cleaner_switch_, num));
				this->cleaner_switch_ = num;
			}
		}
		catch (...)
		{
			this->cleaner_switch_ = 0;
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' is catch exception, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGCLEANER, this->cleaner_switch_));
		}
		num = 0;
	}
	else
	{
		this->cleaner_switch_ = 0;
		ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' does not exist, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGCLEANER, this->cleaner_switch_));
	}

	/*��־������������־����ʱ��Ч  0����ʾ��ɾ��*/
	if (config.get_string_value(status_section, ACE_TEXT(HP_LOG_CONF_LOGSAVEDMONTH), str) != -1)
	{
		try
		{
			num = ACE_OS::strtol(str.c_str(), NULL, 10);
			if (num != this->saved_month_)
			{
				//ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) [%s]->'%s' is updated (%d)->(%d)\n"),HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGSAVEDMONTH, this->saved_month_, num));
				this->saved_month_ = num;
			}
		}
		catch (...)
		{
			this->saved_month_ = 0;
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' is catch exception, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGSAVEDMONTH, this->saved_month_));
		}
		num = 0;
	}
	else
	{
		this->saved_month_ = 0;
		ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) [%s]->'%s' does not exist, default value is (%d)\n"), HP_LOG_CONF_SECTION, HP_LOG_CONF_LOGSAVEDMONTH, this->saved_month_));
	}

	return 1;
}

/**********************************************************************************************************************************/

/*-----------------------------------------------------------------------
* name:		����Ŀ¼
* input:	_fdir  -- Ŀ¼
* output:
* return:	>0  -- �½�Ŀ¼
* 			=0  -- Ŀ¼�Ѵ���
* 			<0  -- ����ʧ��
*-----------------------------------------------------------------------*/
int Logger::mkdir(const char * _fdir)
{
	if (_fdir == NULL || ACE_OS::strlen(_fdir) <= 0) return -1; /*�ļ�������*/
	if (ACE_OS::access(_fdir, F_OK) >= 0) return 0; /*Ŀ¼�Ѵ���*/
	if (ACE_OS::mkdir(_fdir, S_IRWXU) < 0) /*����Ŀ¼ʧ��*/
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n to %s"), ACE_TEXT("mkdir"), _fdir), -1);
	}
	/*����Ŀ¼�ɹ�*/
	chmod(_fdir, S_IRWXU|S_IRWXG|S_IRWXO); /*����Ȩ��*/
	return 1;
}

/*-----------------------------------------------------------------------
* name:		ɾ��Ŀ¼���ļ�
* input:	_fdir  -- Ŀ¼���ļ�·��
* output:
* return:	>0  -- ɾ���ɹ�
* 			=0  -- Ŀ¼������
* 			<0  -- ɾ��ʧ��
*-----------------------------------------------------------------------*/
int Logger::rmdir(const char * _fdir)
{
	if (_fdir == NULL || ACE_OS::strlen(_fdir) <= 0) return -1; /*�ļ�������*/
	if (ACE_OS::access(_fdir, F_OK) < 0) return 0; /*Ŀ¼������*/
	if (ACE_OS::rmdir(_fdir) < 0)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p/n to %s"), ACE_TEXT("rmdir"), _fdir), -1);
	}
	/*ɾ��Ŀ¼�ɹ�*/
	return 1;
}

/*-----------------------------------------------------------------------
* name:		���Ŀ¼
* input:	_fdir  -- ��־��Ŀ¼
* output:
* return:	>0  -- �½�Ŀ¼
* 			=0  -- Ŀ¼�ޱ仯
* 			<0  -- ʧ��
*-----------------------------------------------------------------------*/
int Logger::check_subdir(const char * _fdir)
{
	int ret;
	char _subdir[ HP_LOG_MAX_NAMESIZE];

	if (_fdir == NULL) return -1;

	ACE_OS::memset(this->filedir_, 0x00, sizeof(this->filedir_));
	ACE_OS::sprintf(this->filedir_, "%s", _fdir);
	if (this->filedir_[ACE_OS::strlen(this->filedir_)-1] != '/')
	{
		ACE_OS::sprintf(this->filedir_, "%s/", this->filedir_);
	}

	ACE_OS::memset(_subdir, 0x00, sizeof(_subdir));
	this->create_subdirname(_subdir, 1);
	ACE_OS::strncat(this->filedir_, _subdir, sizeof(this->filedir_));
	if ((ret = this->mkdir(this->filedir_)) < 0)
	{
		ACE_OS::memset(this->filedir_, 0x00, sizeof(this->filedir_));
		ACE_OS::sprintf(this->filedir_, "%s", _fdir);
		return -1;
	}
	ACE_OS::sprintf(this->filedir_, "%s/", this->filedir_);

	ACE_OS::memset(_subdir, 0x00, sizeof(_subdir));
	this->create_subdirname(_subdir, 2);
	ACE_OS::strncat(this->filedir_, _subdir, sizeof(this->filedir_));
	if ((ret = this->mkdir(this->filedir_)) < 0)
	{
		ACE_OS::memset(this->filedir_, 0x00, sizeof(this->filedir_));
		ACE_OS::sprintf(this->filedir_, "%s", _fdir);
		return -1;
	}
	ACE_OS::sprintf(this->filedir_, "%s/", this->filedir_);

	/*ACE_OS::memset(_subdir, 0x00, sizeof(_subdir));
	this->create_subdirname(_subdir, 3);
	ACE_OS::strncat(this->filedir_, _subdir, sizeof(this->filedir_));
	if ((ret = this->mkdir(this->filedir_)) < 0)
	{
		ACE_OS::memset(this->filedir_, 0x00, sizeof(this->filedir_));
		ACE_OS::sprintf(this->filedir_, "%s", _fdir);
		return -1;
	}
	ACE_OS::sprintf(this->filedir_, "%s/", this->filedir_);*/

	return ret;
}

/*-----------------------------------------------------------------------
* name:		����һ����־�ļ���Ŀ¼��
* input:	_level  -- Ŀ¼����
* output:	_subdir  -- ��Ŀ¼���ַ���
* return:
*-----------------------------------------------------------------------*/
void Logger::create_subdirname(char * _subdir, int _level)
{
	char _str[20] = {0};

	time_t timep;
	struct tm * p;
	ACE_Time_Value now(ACE_OS::gettimeofday());
	timep = now.sec();
	p = ACE_OS::localtime(&timep);

	switch (_level)
	{
	case 1:
		ACE_OS::sprintf(_str, "%4d", (1900+p->tm_year));
		ACE_OS::strncpy(_subdir, _str, 4);
		break;
	case 2:
		ACE_OS::sprintf(_str, "%02d", (1+p->tm_mon));
		ACE_OS::strncpy(_subdir, _str, 2);
		break;
	case 3:
		ACE_OS::sprintf(_str, "%02d", p->tm_mday);
		ACE_OS::strncpy(_subdir, _str, 2);
		break;
	default:
		ACE_OS::sprintf(_str, "%02d", 0);
		ACE_OS::strncpy(_subdir, _str, 2);
		break;
	}
}

/*-----------------------------------------------------------------------
* name:		������־�ļ�������ʱ���ַ���
* input:
* output:	_filename  -- ��־�ļ���+����ʱ���ַ���
* return:
*-----------------------------------------------------------------------*/
void Logger::create_filename(char * _filename)
{
	char _str[20] = {0};

	time_t timep;
	struct tm * p;
	ACE_Time_Value now(ACE_OS::gettimeofday());
	timep = now.sec();
	p = ACE_OS::localtime(&timep);

	const char* plogname = NULL;
	if (logname_.length()>0)
	{
		plogname = logname_.c_str();
	}
	else
	{
		plogname = HP_LOG_FILENAME;
	}
	
	ACE_OS::sprintf(_str, "%02d_%02d_%02d", p->tm_year+1900, p->tm_mon+1, p->tm_mday);
	ACE_OS::sprintf(_filename, "%s_%s%s",plogname, _str,HP_LOG_FILETYPE);
}

/*-----------------------------------------------------------------------
* name:		ȡ��־����
* input:	_var_log_level  -- ��־����
* output:	_level  -- ��־����
* return:
*-----------------------------------------------------------------------*/
void Logger::getloglevel(int _var_log_level, char * _level)
{
	switch (_var_log_level)
	{
	case HP_LOGLEVEL_ERROR:
		ACE_OS::sprintf(_level, "ERROR");
		break;
	case HP_LOGLEVEL_WARN:
		ACE_OS::sprintf(_level, "WARN ");
		break;
	case HP_LOGLEVEL_DEBUG:
		ACE_OS::sprintf(_level, "DEBUG");
		break;
	case HP_LOGLEVEL_MESSAGE:
		ACE_OS::sprintf(_level, "MSG  ");
		break;
	default:
		ACE_OS::sprintf(_level, "*****");
		break;
	}
}

/*-----------------------------------------------------------------------
* name:		ȡ��ǰʱ�䣬�������23
* input:
* output:	_stime  -- ʱ���ַ���
* return:
*-----------------------------------------------------------------------*/
void Logger::currenttime(char * _stime)
{
	struct timeb tp;
	struct tm * p;
	ftime(&tp);
	p = ACE_OS::localtime(&tp.time);

	ACE_OS::sprintf(_stime, "%4d-%02d-%02d %02d:%02d:%02d.%03u", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tp.millitm);
}

/**********************************************************************************************************************************/

/*-----------------------------------------------------------------------
* name:		��Ļ��ӡ
* input:	fmt  -- ����ʽ�ַ���
* 			
* output:
* return:	 -1 -- ʧ��
* 			>=0 -- ����ַ���
*-----------------------------------------------------------------------*/
int Logger::to_screen(const char * fmt)
{
	char _buff[HP_LOG_INPUT_SIZE] = {0};
	ACE_OS::memset(_buff, 0, sizeof(_buff));
	ACE_OS::sprintf(_buff, "%s\n",fmt);

	int r = ACE_OS::fprintf(stderr,_buff);
	ACE_OS::fflush(stderr);
	return r;
}

/*-----------------------------------------------------------------------
* name:		�ļ���ӡ
* input:	fmt  -- ����ʽ�ַ���
* 			
* output:
* return:	 -1 -- ʧ��
* 			>=0 -- ����ַ���
*-----------------------------------------------------------------------*/
int Logger::to_file(const char * fmt)
{
	if (this->active_ != true)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("logger not started.\n")), -1);
		return -1;
	}
	char _buff[HP_LOG_INPUT_SIZE] = {0};
	ACE_OS::memset(_buff, 0, sizeof(_buff));
	ACE_OS::sprintf(_buff, "%s\n",fmt);

	const char * buffer = _buff;
	int recv_cnt = ACE_OS::strlen(buffer);

	ACE_Message_Block *mb = new ACE_Message_Block(recv_cnt+1);
	ACE_OS::memcpy(mb->wr_ptr(), buffer, recv_cnt);
	mb->wr_ptr(recv_cnt);

	if (this->putq(mb) == -1)
	{
		ACE_DEBUG((LM_ERROR, ACE_TEXT ("(%P|%t) %p;\n"), ACE_TEXT ("logger input queue failed.")));
		mb->release();
		return -1;
	}

	return recv_cnt;
}

/**********************************************************************************************************************************/

/*-----------------------------------------------------------------------
* name:		error��־
* input:	fmt  -- ����ʽ�ַ���
* 			...  -- ���������б�
* output:
* return:
*-----------------------------------------------------------------------*/
void Logger::error(const char * fmt, ...)
{
	va_list ap;
	char _level[8];
	char _stime[24];
	char _buff[ HP_LOG_INPUT_SIZE] = {0};

	if (this->level_ < HP_LOGLEVEL_ERROR) return;

	va_start(ap, fmt);

	ACE_OS::memset(_level, 0, sizeof(_level));
	ACE_OS::memset(_stime, 0, sizeof(_stime));
	ACE_OS::memset(_buff, 0, sizeof(_buff));

	this->getloglevel(HP_LOGLEVEL_ERROR, _level);
	this->currenttime(_stime);

	ACE_OS::sprintf(_buff, "[%s] [%s]: ", _stime, _level);
	ACE_OS::vsnprintf(_buff+ACE_OS::strlen(_buff), sizeof(_buff)-ACE_OS::strlen(_buff), fmt, ap);
	
	switch (this->media_)
	{
	case HP_LOGMEDIA_ALL:
		this->to_screen(_buff);
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_FILE:
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_SCREEN:
	default:
		this->to_screen(_buff);
		break;
	}
	
	va_end(ap);
}

/*-----------------------------------------------------------------------
* name:		warn��־
* input:	fmt  -- ����ʽ�ַ���
* 			...  -- ���������б�
* output:
* return:
*-----------------------------------------------------------------------*/
void Logger::warn(const char * fmt, ...)
{
	va_list ap;
	char _level[8];
	char _stime[24];
	char _buff[HP_LOG_INPUT_SIZE] = {0};

	if (this->level_ < HP_LOGLEVEL_WARN) return;

	va_start(ap, fmt);

	ACE_OS::memset(_level, 0, sizeof(_level));
	ACE_OS::memset(_stime, 0, sizeof(_stime));
	ACE_OS::memset(_buff, 0, sizeof(_buff));

	this->getloglevel(HP_LOGLEVEL_WARN, _level);
	this->currenttime(_stime);

	ACE_OS::sprintf(_buff, "[%s] [%s]: ", _stime, _level);
	ACE_OS::vsnprintf(_buff+ACE_OS::strlen(_buff), sizeof(_buff)-ACE_OS::strlen(_buff), fmt, ap);

	switch (this->media_)
	{
	case HP_LOGMEDIA_ALL:
		this->to_screen(_buff);
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_FILE:
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_SCREEN:
	default:
		this->to_screen(_buff);
		break;
	}

	va_end(ap);

}

/*-----------------------------------------------------------------------
* name:		message��־
* input:	fmt  -- ����ʽ�ַ���
* 			...  -- ���������б�
* output:
* return:
*-----------------------------------------------------------------------*/
void Logger::message(const char * fmt, ...)
{
	va_list ap;
	char _level[8];
	char _stime[24];
	char _buff[HP_LOG_INPUT_SIZE] = {0};

	if (this->level_ < HP_LOGLEVEL_MESSAGE) return;

	va_start(ap, fmt);

	ACE_OS::memset(_level, 0, sizeof(_level));
	ACE_OS::memset(_stime, 0, sizeof(_stime));
	ACE_OS::memset(_buff, 0, sizeof(_buff));

	this->getloglevel(HP_LOGLEVEL_MESSAGE, _level);
	this->currenttime(_stime);

	ACE_OS::sprintf(_buff, "[%s] [%s]: ", _stime, _level);
	ACE_OS::vsnprintf(_buff+ACE_OS::strlen(_buff), sizeof(_buff)-ACE_OS::strlen(_buff), fmt, ap);

	switch (this->media_)
	{
	case HP_LOGMEDIA_ALL:
		this->to_screen(_buff);
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_FILE:
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_SCREEN:
	default:
		this->to_screen(_buff);
		break;
	}

	va_end(ap);
}

/*-----------------------------------------------------------------------
* name:		debug��־
* input:	fmt  -- ����ʽ�ַ���
* 			...  -- ���������б�
* output:
* return:
*-----------------------------------------------------------------------*/
void Logger::debug(const char * fmt, ...)
{
	va_list ap;
	char _level[8];
	char _stime[24];
	char _buff[HP_LOG_INPUT_SIZE] = {0};

	if (this->level_ < HP_LOGLEVEL_DEBUG) return;

	va_start(ap, fmt);

	ACE_OS::memset(_level, 0, sizeof(_level));
	ACE_OS::memset(_stime, 0, sizeof(_stime));
	ACE_OS::memset(_buff, 0, sizeof(_buff));

	this->getloglevel(HP_LOGLEVEL_DEBUG, _level);
	this->currenttime(_stime);

	ACE_OS::sprintf(_buff, "[%s] [%s]: ", _stime, _level);
	ACE_OS::vsnprintf(_buff+ACE_OS::strlen(_buff), sizeof(_buff)-ACE_OS::strlen(_buff), fmt, ap);

	switch (this->media_)
	{
	case HP_LOGMEDIA_ALL:
		this->to_screen(_buff);
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_FILE:
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_SCREEN:
	default:
		this->to_screen(_buff);
		break;
	}

	va_end(ap);
}

/*-----------------------------------------------------------------------
* name:		��ָ����ʽ��ӡ��������������Ϣ
* input:	fmt  -- ����ʽ�ַ���
* 			...  -- ���������б�
* output:
* return:
*-----------------------------------------------------------------------*/
void Logger::printf(const char * fmt, ...)
{
	va_list ap;
	char _buff[HP_LOG_INPUT_SIZE] = {0};

	va_start(ap, fmt);

	ACE_OS::memset(_buff, 0, sizeof(_buff));

	ACE_OS::vsnprintf(_buff, sizeof(_buff), fmt, ap);

	switch (this->media_)
	{
	case HP_LOGMEDIA_ALL:
		this->to_screen(_buff);
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_FILE:
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_SCREEN:
	default:
		this->to_screen(_buff);
		break;
	}

	va_end(ap);
}

/*-----------------------------------------------------------------------
* name:		ö�ٻ���
* input:	_buff  -- ����
* 			_size  -- �����С
*			_type  -- ������ͣ�1�����գ�2�����ͣ���������
* output:
* return:
*-----------------------------------------------------------------------*/
void Logger::dump(const char * _buf, size_t _size, int _type)
{
	char _level[24];
	char _stime[24];
	char _buff[HP_LOG_INPUT_SIZE] = {0};

	ACE_OS::memset(_level, 0, sizeof(_level));
	ACE_OS::memset(_stime, 0, sizeof(_stime));
	ACE_OS::memset(_buff, 0, sizeof(_buff));

	switch (_type)
	{
	case 1:
		ACE_OS::sprintf(_level, "����(%d)��[", _size);
		break;

	case 2:
		ACE_OS::sprintf(_level, "����(%d)��[", _size);
		break;

	default:
		ACE_OS::sprintf(_level, "����(%d)��[", _size);
		break;
	}

	this->currenttime(_stime);

	ACE_OS::sprintf(_buff, "[%s] %s", _stime, _level);

	for (size_t i=0; i<_size; i++)
	{
		ACE_OS::snprintf(_buff+ACE_OS::strlen(_buff), sizeof(_buff)-ACE_OS::strlen(_buff), "%02X ", _buf[i]&0xFF);
	}

	ACE_OS::snprintf(_buff+ACE_OS::strlen(_buff), sizeof(_buff)-ACE_OS::strlen(_buff), "]/n");

	switch (this->media_)
	{
	case HP_LOGMEDIA_ALL:
		this->to_screen(_buff);
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_FILE:
		this->to_file(_buff);
		break;
	case HP_LOGMEDIA_SCREEN:
	default:
		this->to_screen(_buff);
		break;
	}
}

/**********************************************************************************************************************************/

void Logger::set_active(bool b)
{
	this->active_ = b;
}

bool Logger::get_active()
{
	return this->active_;
}

int Logger::svc(void)
{
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) *** logger running,file dir [%s],logger level [%d] ***\n"), this->parentdir_, this->level_));

	while (this->active_ == true)
	{
		ACE_Message_Block *mb = NULL;
		if (this->getq(mb) == -1)
		{
			if (mb) mb->release();
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) dequeue failed.\n")));
			continue;
		}
		if (this->active_ == false)
		{
			if (mb) mb->release();
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) logger closed.\n")));
			break;
		}

		char * data = mb->rd_ptr();
		size_t data_size = mb->length();

		ssize_t send_cnt = this->fp_.send(data, data_size);
		if (send_cnt == -1)
		{
			ACE_DEBUG((LM_ERROR, ACE_TEXT("%p\n"), "write()-1"));
		}
		else
		{
			mb->rd_ptr(send_cnt);
			if (send_cnt != data_size)
			{
				ACE_DEBUG((LM_ERROR, ACE_TEXT("%p\n"), "write()-2"));
			}
		}

		/*���ݴ�����������ͷ��ڴ�*/
		mb->release();

		/*�жϵ�ǰ·�����ж��ļ���С*/
		ACE_FILE_Info finfo;
		this->fp_.get_info(finfo);
		if (this->check_subdir(this->parentdir_) > 0 || finfo.size_ > HP_LOG_MAX_FILESIZE)
		{
			this->fp_.close();
			this->create_filename(this->filename_);
			ACE_OS::memset(this->fullpath_, 0x00, sizeof(this->fullpath_));
			ACE_OS::sprintf(this->fullpath_, "%s%s", this->filedir_, this->filename_);
			if (this->fp_conn_.connect(this->fp_, ACE_FILE_Addr(ACE_TEXT(this->fullpath_)), 0, ACE_Addr::sap_any, 0, O_RDWR|O_APPEND|O_CREAT, ACE_DEFAULT_FILE_PERMS) == -1)
			{
				ACE_ERROR_RETURN((LM_ERROR, "%p\n to %s", "open", this->fullpath_), -1);
			}
			else
			{
				chmod(this->fullpath_, S_IRWXU|S_IRWXG|S_IRWXO); /*����Ȩ��*/
			}
		}
	}

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) *** logger exit. ***\n")));
	return 0;
}

/*-----------------------------------------------------------------------
* name:		��־�����߳�
* input:
* output:	arg  -- void *
* return:
*-----------------------------------------------------------------------*/
static void * pthr_cleaner_process(void * arg)
{
	struct timespec rqt;
	time_t timep;
	struct tm * p;

	time_t curr_timep; /*��ǰ���ʱ��*/
	time_t last_timep; /*��һ�μ��ʱ��*/

	char rm_year[4+1], rm_month[2+1]; /*��ɾ������*/
	char rm_dir[HP_LOG_MAX_NAMESIZE]; /*��ɾ��Ŀ¼·��*/
	int c_year, c_month; /*��ǰ����*/

	Logger * logger_0 = (Logger *)arg;
	if (logger_0 == NULL)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) logger clear error��param error,clear thread exit.\n")));
		return 0;
	}

	last_timep = ACE_OS::gettimeofday().sec();

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) *** logger clear thread running,file dir:[%s]��saving month:[%d] ***\n"), logger_0->parentdir_, logger_0->saved_month_));

	while (logger_0->get_active() == true)
	{
		if (logger_0->cleaner_switch_ != HP_LOGCLEANER_OPEN || logger_0->saved_month_ <= 0 || logger_0->saved_month_ >= 120)
		{
			if (logger_0->saved_month_ <= 0 || logger_0->saved_month_ >= 120)
			{
				logger_0->saved_month_ = 0;
			}

			rqt.tv_sec = 1;
			rqt.tv_nsec = 0;
			ACE_OS::nanosleep(&rqt);
			continue;
		}

		curr_timep = ACE_OS::gettimeofday().sec();
		if ((curr_timep-last_timep) < 300) /*300�� = 5����*/
		{
			rqt.tv_sec = 1;
			rqt.tv_nsec = 0;
			ACE_OS::nanosleep(&rqt);
			continue;
		}
		last_timep = curr_timep;

		if (ACE_OS::strlen(logger_0->parentdir_) <= 0)
		{
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) logger clear thread exption��invalid dir.\n")));
			break;
		}
		if (logger_0->parentdir_[ACE_OS::strlen(logger_0->parentdir_)-1] != '/')
		{
			ACE_OS::sprintf(logger_0->parentdir_, "%s/", logger_0->parentdir_);
		}

		ACE_OS::memset(rm_year, 0x00, sizeof(rm_year));
		ACE_OS::memset(rm_month, 0x00, sizeof(rm_month));
		ACE_OS::memset(rm_dir, 0x00, sizeof(rm_dir));

		/*ȡ��ǰ����*/
		timep = ACE_OS::gettimeofday().sec();
		p = ACE_OS::localtime(&timep);
		c_year = (1900+p->tm_year);
		c_month = (1+p->tm_mon);

		/*ȡ��ɾ������*/
		if (c_month <= (logger_0->saved_month_+1))
		{
			ACE_OS::sprintf(rm_year, "%04d", c_year-1);
			ACE_OS::sprintf(rm_month, "%02d", c_month+12-(logger_0->saved_month_+1));
		}
		else
		{
			ACE_OS::sprintf(rm_year, "%04d", c_year);
			ACE_OS::sprintf(rm_month, "%02d", c_month-(logger_0->saved_month_+1));
		}

		/*ɾ��*/
		if (ACE_OS::strcmp(rm_month, "12") == 0)
		{
			ACE_OS::sprintf(rm_dir, "%s%s", logger_0->parentdir_, rm_year);
		}
		else
		{
			ACE_OS::sprintf(rm_dir, "%s%s/%s", logger_0->parentdir_, rm_year, rm_month);
		}

		if (logger_0->rmdir(rm_dir) > 0)
		{
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) *** clear expire logs.[%s] ***\n"), rm_dir));
		}
	}

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) *** logger clear thread exit. ***\n")));
	return 0;
}

void Logger::set_logname(const char* logname)
{
	logname_ = logname;
}
