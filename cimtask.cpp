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
		LOG->warn("请配置CIM文件名称");
		return;
	}
	if (m_workPath.length() <= 0)
	{
		LOG->warn("请配置CIM文件的工作目录");
		return;
	}
	if (m_ftpPath.length()<=0 )
	{
		LOG->warn("请配置CIM文件的FTP目录");
		return;
	}
	if (m_checkHour < 0)
	{
		LOG->message("未配置CIM文件检查时间，将于每天零点进行检查");
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
			// 检测时间到，进行文件比较
			if(checkFile())
			{
				// 需要更新CIM，更新数据库字段未待导库
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
		// 工作目录文件不存在，不用比较，直接把文件拷贝到工作目录
		if(!fileCopy(ftpfile.c_str(),workfile.c_str()))
		{
			return false;
		}
		else
		{
			LOG->message("拷贝CIM文件到工作目录成功");
			return true;
		}
	}
	else
	{
		 // 如果工作目录已经存在文件，则进行文件比较，比较最后修改时间
		if (needUpdate(workfile.c_str(),ftpfile.c_str()))
		{
			if(fileCopy(workfile.c_str(),ftpfile.c_str()))
			{
				LOG->message("拷贝CIM文件到工作目录成功");
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
		LOG->warn("拷贝cim文件到工作目录失败:%s",output_name);
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
		LOG->message("新增CIM更新标志成功");
	}
	else
	{
		LOG->warn("新增CIM更新标志失败");
	}

}