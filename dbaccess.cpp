#include "dbaccess.h"
#include "defines.h"
#include "confmgr.h"

DbAccess::DbAccess()
{
	m_dbhost = "127.0.0.1";
	m_dbname = "uc";
	m_dbuser = "root";
	m_dbpwd = "123456";
	m_dbport = 3306;

}

void DbAccess::init()
{
	// �������ݿ�����
	m_dbhost = App_Config::instance()->getValue(DB_ROOT,"DBAddr");
	m_dbname =App_Config::instance()->getValue(DB_ROOT,"DBPort");
	m_dbpwd = App_Config::instance()->getValue(DB_ROOT,"UserPwd");
	m_dbuser = App_Config::instance()->getValue(DB_ROOT,"DBUser");
	m_dbport = ACE_OS::atoi(App_Config::instance()->getValue(DB_ROOT,"DBPort").c_str());

	unsigned long i;

	m_mysql = (MYSQL *)malloc(sizeof(MYSQL));  
	mysql_init(m_mysql);

	if(m_mysql == NULL)
	{
		LOG->error("EROR: MySQL ssock init error. ");
		return ;
	}

	LOG->message("MySQL ssock init OK.");

	//���ӵ�ָ�������ݿ�
	if (conn2db())
	{
		LOG->message("MySQL connnect OK... ");
	}

}

bool DbAccess::conn2db()
{
	//���ӵ�ָ�������ݿ�
	m_mysql =mysql_real_connect(m_mysql, m_dbhost.c_str(),m_dbuser.c_str(),m_dbpwd.c_str(), m_dbname.c_str(),m_dbport, NULL, 0);
	if(!m_mysql)
	{
		LOG->error("conn mysql failed... ");
		unsigned int mtint = mysql_errno(m_mysql);
		return false;
	}

	mysql_set_character_set(m_mysql,"gbk");
	return true;
}

int DbAccess::execSql(char* sql)
{
	if (!conn2db())
	{
		return -1;
	}
	int ret = mysql_query( m_mysql,sql);
	if(ret)  
	{
		LOG->error("mysql_query() Error, %s\n", mysql_error(m_mysql));  
	}

	mysql_close(m_mysql);

	return ret;
}

vector<map<string,string> > DbAccess::getList(char* sql)
{
	vector<map<string,string> > retList;
	if (!conn2db())
	{
		return retList;
	}
	MYSQL_RES *result = NULL;
	MYSQL_FIELD *field = NULL;

	mysql_query(m_mysql, sql);
	result = mysql_store_result(m_mysql);
	int rowcount = mysql_num_rows(result);
	
	int fieldcount = mysql_num_fields(result);
	vector<string> fieldlist;
	for(int i = 0; i < fieldcount; i++)
	{
		field = mysql_fetch_field_direct(result,i);
		fieldlist.push_back(field->name);
	}
	
	MYSQL_ROW row = NULL;
	row = mysql_fetch_row(result);
	while(NULL != row)
	{
		map<string,string> record;
		for(int i=0;i<fieldcount; i++)
		{
			record.insert(map<string,string>::value_type(fieldlist.at(i),row[i]));
			LOG->debug("filedname:%s,value:%s",fieldlist.at(i),row[i]);
		}

		retList.push_back(record);

		row = mysql_fetch_row(result);
	}

	mysql_close(m_mysql);
	
	return retList;
}