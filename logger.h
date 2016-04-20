
/************************************************************************/
/* 
	DESC:	�ṩ����,ʵ�ָ�ʽ��־��ӡ,��ѡ���ӡ��Ļ,�ļ������߾���,֧���Զ�������־����.
	DATE:	2016-04-20
	AUTHOR:	YUANLS
*/
/************************************************************************/



#ifndef HP_LOGGER_ACE_H_
#define HP_LOGGER_ACE_H_

#include <stdarg.h>

#include <ace/FILE_Connector.h>
#include <ace/FILE_IO.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <ace/Mutex.h>
#include <string>
using namespace std;


/*Ĭ����־�����·�*/
#define DEFAULT_SAVED_MONTH		6

/*��־���� - ������־��ʾ�ļ���*/
#define HP_LOGLEVEL_ERROR		0x00				/*����*/
#define HP_LOGLEVEL_WARN		0x01				/*����*/
#define HP_LOGLEVEL_MESSAGE		0x02				/*��ʾ*/
#define HP_LOGLEVEL_DEBUG		0x03				/*����*/

/*��־ý�� - ������־�����ý��*/
#define HP_LOGMEDIA_SCREEN		0x00				/*��Ļ*/
#define HP_LOGMEDIA_FILE		0x01				/*�ļ�*/
#define HP_LOGMEDIA_ALL			0x02				/*ȫ��*/

/*��־����״̬*/
#define HP_LOGCLEANER_CLOSED	0x00				/*��־�����*/
#define HP_LOGCLEANER_OPEN		0x01				/*��־����*/

/*��־�ļ�*/
#define HP_LOG_MAX_NAMESIZE		255					/*����ļ�������*/

#define HP_LOG_FILEDIR "log"						/*Ĭ���ļ���Ŀ¼*/
#define HP_LOG_FILENAME "syslog"					/*Ĭ����־�ļ���*/
#define HP_LOG_FILETYPE ".log"						/*Ĭ����չ��*/
#define HP_LOG_MAX_FILESIZE (32*1024*1024)			/*�����־�ļ���С32M*/

#define HP_LOG_GRP_ID			96469
#define HP_LOG_INPUT_SIZE		(200*1024)			/*������־����ֽ���*/
#define HP_LOG_SEND_HIGHT_MARK	(1024*1024)			/*��־��������������*/

/*���ò�������*/
#define HP_LOG_CONF_SECTION       "LOGGER"
#define HP_LOG_CONF_LOGFILEDIR    "LOGFILEDIR"
#define HP_LOG_CONF_LOGMEDIA      "LOGMEDIA"
#define HP_LOG_CONF_LOGLEVEL      "LOGLEVEL"
#define HP_LOG_CONF_LOGCLEANER    "LOGCLEANER"
#define HP_LOG_CONF_LOGSAVEDMONTH "LOGSAVEDMONTH"


/********************************************************************************************
* ����ACE_Task���������������ģʽ��������־ϵͳ
********************************************************************************************/
class Logger : public ACE_Task<ACE_MT_SYNCH>
{
public:
	Logger() : active_(false)
	{
		this->media_ = HP_LOGMEDIA_ALL;
		this->level_ = HP_LOGLEVEL_DEBUG;
		this->cleaner_switch_ = HP_LOGCLEANER_OPEN;
		this->saved_month_ = DEFAULT_SAVED_MONTH;
		ACE_OS::memset(this->parentdir_, 0x00, sizeof(this->parentdir_));
		ACE_OS::memset(this->filedir_, 0x00, sizeof(this->filedir_));
		ACE_OS::memset(this->filename_, 0x00, sizeof(this->filename_));
		ACE_OS::memset(this->fullpath_, 0x00, sizeof(this->fullpath_));
		this->msg_queue()->high_water_mark( HP_LOG_SEND_HIGHT_MARK);
	}

	virtual ~Logger()
	{
		this->active_ = false;
		this->msg_queue()->close();
		/*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Logger stop/n")));*/
	}

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
	void	init_logger(const char * _fdir, unsigned int _media = HP_LOGMEDIA_ALL, unsigned int _level = HP_LOGLEVEL_DEBUG, int _cleaner_switch = HP_LOGCLEANER_CLOSED, int _saved_month = DEFAULT_SAVED_MONTH);

	/*-----------------------------------------------------------------------
	* name:		��ʼ����־ϵͳ
	* 			ÿ��ϵͳ���ô���־��ʱ ��Ҫ���ô˺�����ʼ����־ϵͳ
	* input:	NONE
	* output:	NONE
	* return:	RT_NG -- ʧ��
	* 			RT_OK -- �ɹ�
	*-----------------------------------------------------------------------*/
	int		open_logger();

	/*-----------------------------------------------------------------------
	* name:		�ر���־ϵͳ
	* input:	NONE
	* output:	NONE
	* return:	NONE
	*-----------------------------------------------------------------------*/
	void	close_logger();

	/*-----------------------------------------------------------------------
	* name:		���������ļ�
	* input:	_config_filename  -- ��־�ļ�·��
	* output:	NONE
	* return:	RT_NG -- ʧ��
	* 			RT_OK -- �ɹ�
	*-----------------------------------------------------------------------*/
	int		load_config(const char * _config_filename);

	/*-----------------------------------------------------------------------
	* name:		error��־
	* input:	fmt  -- ����ʽ�ַ���
	* 			...  -- ���������б�
	* output:
	* return:
	*-----------------------------------------------------------------------*/
	void	error(const char * fmt, ...);

	/*-----------------------------------------------------------------------
	* name:		warn��־
	* input:	fmt  -- ����ʽ�ַ���
	* 			...  -- ���������б�
	* output:
	* return:
	*-----------------------------------------------------------------------*/
	void	warn(const char * fmt, ...);

	/*-----------------------------------------------------------------------
	* name:		message��־
	* input:	fmt  -- ����ʽ�ַ���
	* 			...  -- ���������б�
	* output:
	* return:
	*-----------------------------------------------------------------------*/
	void	message(const char * fmt, ...);

	/*-----------------------------------------------------------------------
	* name:		debug��־
	* input:	fmt  -- ����ʽ�ַ���
	* 			...  -- ���������б�
	* output:
	* return:
	*-----------------------------------------------------------------------*/
	void	debug(const char * fmt, ...);

	/*-----------------------------------------------------------------------
	* name:		ɾ��Ŀ¼���ļ�
	* input:	_fdir  -- Ŀ¼���ļ�·��
	* output:
	* return:	>0  -- ɾ���ɹ�
	* 			=0  -- Ŀ¼������
	* 			<0  -- ɾ��ʧ��
	*-----------------------------------------------------------------------*/
	int		rmdir(const char * _fdir);

	// ������־�ļ�����
	void	set_logname(const char* logname);


	void	set_active(bool b);
	bool	get_active();



private:
	private:
	/*-----------------------------------------------------------------------
	* name:		��ָ����ʽ��ӡ��������������Ϣ
	* input:	fmt  -- ����ʽ�ַ���
	* 			...  -- ���������б�
	* output:
	* return:
	*-----------------------------------------------------------------------*/

	void printf(const char * fmt, ...);

	/*-----------------------------------------------------------------------
	* name:		ö�ٻ���
	* input:	_buff  -- ����
	* 			_size  -- �����С
	*			_type  -- ������ͣ�1�����գ�2�����ͣ���������
	* output:
	* return:
	*-----------------------------------------------------------------------*/
	void dump(const char * _buf, size_t _size, int _type);

	/*-----------------------------------------------------------------------
	* name:		����Ŀ¼
	* input:	_fdir  -- Ŀ¼
	* output:
	* return:	>0  -- �½�Ŀ¼
	* 			=0  -- Ŀ¼�Ѵ���
	* 			<0  -- ����ʧ��
	*-----------------------------------------------------------------------*/
	int mkdir(const char * _fdir);
	virtual int svc(void);

	/*-----------------------------------------------------------------------
	* name:		���Ŀ¼
	* input:	_fdir  -- ��־��Ŀ¼
	* output:
	* return:	>0  -- �½�Ŀ¼
	* 			=0  -- Ŀ¼�ޱ仯
	* 			<0  -- ʧ��
	*-----------------------------------------------------------------------*/
	int check_subdir(const char * _fdir);

	/*-----------------------------------------------------------------------
	* name:		����һ����־�ļ���Ŀ¼��
	* input:	_level  -- Ŀ¼����
	* output:	_subdir  -- ��Ŀ¼���ַ���
	* return:
	*-----------------------------------------------------------------------*/
	void create_subdirname(char * _subdir, int _level);

	/*-----------------------------------------------------------------------
	* name:		������־�ļ�������ʱ���ַ���
	* input:
	* output:	_filename  -- ��־�ļ���+����ʱ���ַ���
	* return:
	*-----------------------------------------------------------------------*/
	void create_filename(char * _filename);

	/*-----------------------------------------------------------------------
	* name:		ȡ��־����
	* input:	_var_log_level  -- ��־����
	* output:	_level  -- ��־����
	* return:
	*-----------------------------------------------------------------------*/
	void getloglevel(int _var_log_level, char * _level);

	/*-----------------------------------------------------------------------
	* name:		ȡ��ǰʱ�䣬�������23
	* input:
	* output:	_stime  -- ʱ���ַ���
	* return:
	*-----------------------------------------------------------------------*/
	void currenttime(char * _stime);

	/*-----------------------------------------------------------------------
	* name:		��Ļ��ӡ
	* input:	fmt  -- ����ʽ�ַ���
	* 			
	* output:
	* return:	 -1 -- ʧ��
	* 			>=0 -- ����ַ���
	*-----------------------------------------------------------------------*/
	int to_screen(const char * fmt);

	/*-----------------------------------------------------------------------
	* name:		�ļ���ӡ
	* input:	fmt  -- ����ʽ�ַ���
	* 			
	* output:
	* return:	 -1 -- ʧ��
	* 			>=0 -- ����ַ���
	*-----------------------------------------------------------------------*/
	int to_file(const char * fmt);

public:
	int media_; /*��־ý�� Ĭ��= HP_LOGMEDIA_SCREEN*/
	int level_; /*��־���� Ĭ��= HP_LOGLEVEL_MESSAGE*/

	int cleaner_switch_; /*��־�����أ�=1ʱ��Ч*/
	int saved_month_;    /*��־����������=0��ʾ��ɾ����Ҫ��С��120*/

	char parentdir_[HP_LOG_MAX_NAMESIZE];   /*��־�ļ���Ŀ¼*/
	char filedir_  [HP_LOG_MAX_NAMESIZE];   /*��־�ļ�Ŀ¼, ������Ŀ¼����Ŀ¼*/
	char filename_ [HP_LOG_MAX_NAMESIZE];   /*��־�ļ���,������Ŀ¼*/
	char fullpath_ [HP_LOG_MAX_NAMESIZE*2]; /*��־�ļ�ȫ·����������Ŀ¼���ļ���*/

	ACE_FILE_IO fp_;

private:
	bool		active_;
	string		logname_;

	ACE_FILE_Connector fp_conn_;
};

typedef ACE_Singleton<Logger, ACE_Mutex> App_Logger;
#endif
