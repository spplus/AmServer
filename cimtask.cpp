#include "cimtask.h"
#include "confmgr.h"
#include "defines.h"
#include "comutils.h"
#include "dbaccess.h"
#include "defines.h"
#include "ace/Time_Value.h"
#include "ace/Date_Time.h"

CimTask::CimTask()
{
	m_stop = false;
}

void CimTask::start()
{
	m_checkHour = COM->str2i(App_Config::instance()->getValue(CIM_ROOT,CIM_CHECKHOUR));
	m_ftpPath = App_Config::instance()->getValue(CIM_ROOT,CIM_FTPPATH);
	m_workPath = App_Config::instance()->getValue(CIM_ROOT,CIM_WORKPATH);
	m_cimName = App_Config::instance()->getValue(CIM_ROOT,CIM_NAME);
	if (m_cimName.length() <= 0)
	{
		LOG->warn("������CIM�ļ�����");
		return;
	}
	if (m_workPath.length() <= 0)
	{
		LOG->warn("������CIM�ļ��Ĺ���Ŀ¼");
		return;
	}
	if (m_ftpPath.length()<=0 )
	{
		LOG->warn("������CIM�ļ���FTPĿ¼");
		return;
	}
	if (m_checkHour < 0)
	{
		LOG->message("δ����CIM�ļ����ʱ�䣬����ÿ�������м��");
		m_checkHour = CIM_DEFAULT_CHECKHOUR;
	}

	activate();

	LOG->message("Cim Task start.");
}

void CimTask::stop()
{
	m_stop = true;
}

int CimTask::svc()
{
	while (!m_stop)
	{
		string curdate = getCurDate();
		if (getHour() == m_checkHour && m_lastCheckDate != curdate)
		{
			// ���ʱ�䵽�������ļ��Ƚ�
			if(checkFile())
			{
				// ��Ҫ����CIM���������ݿ��ֶ�δ������
				updateIsNew();
			}

			m_lastCheckDate = curdate;
		}
		ACE_OS::sleep(5);
	}
	return 0;
}

string CimTask::getCurDate()
{
	ACE_Date_Time tvTime(ACE_OS::gettimeofday());   
	 
	char date[24];
	sprintf(date,"%d-%d-%d",tvTime.year(),tvTime.month(),tvTime.day());

	return string(date);
}

int CimTask::getHour()
{
	ACE_Date_Time tvTime(ACE_OS::gettimeofday());   
	return tvTime.hour();
}

bool CimTask::checkFile()
{
	string ftpfile = m_ftpPath+"/" +m_cimName;
	string workfile = m_workPath + "/" +m_cimName;

	if (ACE_OS::access(workfile.c_str(),0) == -1)
	{
		// ����Ŀ¼�ļ������ڣ����ñȽϣ�ֱ�Ӱ��ļ�����������Ŀ¼
		if(!fileCopy(ftpfile.c_str(),workfile.c_str()))
		{
			return false;
		}
		else
		{
			LOG->message("����CIM�ļ�������Ŀ¼�ɹ�");
			return true;
		}
	}
	else
	{
		 // �������Ŀ¼�Ѿ������ļ���������ļ��Ƚϣ��Ƚ�����޸�ʱ��
		if (needUpdate(workfile.c_str(),ftpfile.c_str()))
		{
			if(fileCopy(workfile.c_str(),ftpfile.c_str()))
			{
				LOG->message("����CIM�ļ�������Ŀ¼�ɹ�");
				return true;
			}
		}
	}
	return false;
}

bool CimTask::fileCopy(const char* output_name,const char* input_name)
{

	 FILE *input, *output;
     unsigned char current_byte;
	 char buff[1024];
    input = fopen(input_name, "rb");
    output = fopen(output_name, "wb");
    if(input==NULL || output==NULL)
    {
		LOG->warn("����cim�ļ�������Ŀ¼ʧ��:%s",output_name);
        return false;
    }
    
    while(1)
    {
        fread(buff, sizeof(unsigned char), 1, input);
        if(feof(input))
        {
            break;
        }
        fwrite(buff, sizeof(unsigned char), 1, output);
    }
    fflush(input);
    fflush(output);
    fclose(input);
    fclose(output);
    return true;

}

bool CimTask::needUpdate(const char* destpath,const char* srcpath)
{
	struct stat destStat,srcState;
	stat(destpath, &destStat);
	stat(srcpath,&srcState);

	if (destStat.st_size != srcState.st_size)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CimTask::updateIsNew()
{
	char* psql = "insert into system_config(isNew,CreateTime)values(1,now())";

	if(DBA->execSql(psql) == 1)
	{
		LOG->message("����CIM���±�־�ɹ�");
	}
	else
	{
		LOG->warn("����CIM���±�־ʧ��");
	}

}