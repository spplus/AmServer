
#include "defines.h"
#include "comutils.h"
#include "cimloader.h"
#include "definetag.h"
#include "dbaccess.h"

int CimLoader::Load(string cimname,int factype)
{
	string strxml = cimname;
	LOG->message("loadFile: parsexml  begin");
	parseXML.loadXML(strxml,factype);
	LOG->message("loadFile: parsexml  end");

	LOG->message("cim2db  begin");
	int ret = cim2db();
	LOG->message("cim2db  end");

	return ret;
}

void CimLoader::save_log(char *buf)
{
	LOG->message(buf);
}

int CimLoader::cim2db()
{
	int ret;

	sprintf(m_log,"saveBaseVoltage: BaseVoltage to db begin ");
	save_log(m_log);

	ret = saveBaseVoltage();

	sprintf(m_log,"saveBaseVoltage: BaseVoltage to db end ");
	save_log(m_log);

	sprintf(m_log,"saveSubstation: Substation to db start ............................ ");
	save_log(m_log);

	ret = saveSubstation();

	sprintf(m_log,"saveSubstation: Substation to db end ............................ ");
	save_log(m_log);


	sprintf(m_log,"saveVollevel: Vollevel to db start ............................ ");
	save_log(m_log);

	ret = saveVollevel();

	sprintf(m_log,"saveVollevel: Vollevel to db end ............................ ");
	save_log(m_log);
	/**/
	sprintf(m_log,"saveBreaker: Breaker to db start ............................ ");
	save_log(m_log);

	ret = saveBreaker();

	sprintf(m_log,"saveBreaker: Breaker to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"saveDisconnector: Disconnector to db start ............................ ");
	save_log(m_log);

	ret = saveDisconnector();

	sprintf(m_log,"saveDisconnector: Disconnector to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"saveGrdDisconnector: GrdDisconnector to db start ............................ ");
	save_log(m_log);

	ret = saveGrdDisconnector();

	sprintf(m_log,"saveGrdDisconnector: GrdDisconnector to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"saveBusbarSection: BusbarSection to db start ............................ ");
	save_log(m_log);

	ret = saveBusbarSection();

	sprintf(m_log,"saveBusbarSection: BusbarSection to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"saveCompensator: sCompensator to db start ............................ ");
	save_log(m_log);

	ret = saveCompensator();

	sprintf(m_log,"saveCompensator: Compensator to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"saveEnergyConsumer: EnergyConsumer to db start ............................ ");
	save_log(m_log);

	ret = saveEnergyConsumer();

	sprintf(m_log,"saveEnergyConsumer: EnergyConsumer to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"saveSynchronousMachine: SynchronousMachine to db start ............................ ");
	save_log(m_log);

	ret = saveSynchronousMachine();

	sprintf(m_log,"saveSynchronousMachine: SynchronousMachine to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"savePotentialTransformer: PotentialTransformer to db start ............................ ");
	save_log(m_log);

	ret = savePotentialTransformer();

	sprintf(m_log,"savePotentialTransformer: PotentialTransformer to db end ............................ ");
	save_log(m_log);

	sprintf(m_log,"savePowerTransformer: PowerTransformer to db start ............................ ");
	save_log(m_log);

	ret = savePowerTransformer();

	sprintf(m_log,"savePowerTransformer: PowerTransformer to db end ............................ ");
	save_log(m_log);


	sprintf(m_log,"saveACLineSegment: ACLineSegment to db start ............................ ");
	save_log(m_log);

	ret = saveACLineSegment();

	sprintf(m_log,"saveACLineSegment: ACLineSegment to db end ............................ ");
	save_log(m_log);



	sprintf(m_log,"saveTransformerWinding: TransformerWinding to db start ............................ ");
	save_log(m_log);

	ret = saveTransformerWinding();

	sprintf(m_log,"saveTransformerWinding: TransformerWinding to db end ............................ ");
	save_log(m_log);


	sprintf(m_log,"saveRelateLine: RelateLine to db start ............................ ");
	save_log(m_log);

	ret = saveRelateLine();

	sprintf(m_log,"saveRelateLine: RelateLine to db end ............................ ");
	save_log(m_log);


	sprintf(m_log,"saveConnectivityNode: ConnectivityNode to db start ............................ ");
	save_log(m_log);

	ret = saveConnectivityNode();

	sprintf(m_log,"saveConnectivityNode: ConnectivityNode to db end ............................ ");
	save_log(m_log);


	sprintf(m_log,"saveTerminalRelation: TerminalRelation to db start ............................ ");
	save_log(m_log);

	ret = saveTerminalRelation();

	sprintf(m_log,"saveTerminalRelation: TerminalRelation to db end ............................ ");
	save_log(m_log);


	sprintf(m_log,"saveUnitStatus: UnitStatus to db start ............................ ");
	save_log(m_log);

	ret = saveUnitStatus();

	sprintf(m_log,"saveUnitStatus: UnitStatus to db end ............................ ");
	save_log(m_log);

	return ret;
}



int CimLoader::saveBaseVoltage()
{
	int ret;
	MAPBVOL mapBVol = parseXML.getBaseVoltage();
	MAPBVOL::iterator bviter;

	/*
	//创建临时表
	QString dropsql = QString("DROP TABLE IF EXISTS `voltages_temp`;");
	int ret = execSQL(dropsql.toStdString().c_str());

	QString createsql = QString("CREATE TABLE `voltages_temp` ( \
		`ID` int(11) NOT NULL AUTO_INCREMENT, \
		`CimId` varchar(100) NOT NULL,\
		`Name` varchar(100) NOT NULL,\
		`VolValue` varchar(50) NOT NULL,\
		PRIMARY KEY (`ID`)\
		) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	ret = execSQL(createsql.toStdString().c_str());


	//INSERT INTO voltages (cimid,name,volvalue) VALUES ('200000003','110kv','110'),('200000002','220kv','220')
	QString sqlInsert = QString("INSERT INTO voltages_temp (cimid,name,volvalue) VALUES ");
	QString sqlVal = "";
	QString sql = "";

	//组合插入语句值部分
	for (iter=mapBVol.begin();iter!=mapBVol.end();iter++)
	{
		QString sqlval = QString("('%1','%2','%3'),").arg(iter->second.bvcimid.c_str()).arg(iter->second.bvname.c_str()).arg(iter->second.bvval.c_str());

		sqlVal.append(sqlval);
	}

	//去除最后一个逗号
	sqlVal = sqlVal.left(sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = execSQL(sql.toStdString().c_str());
*/

	//组合插入语句值部分
	for (bviter=mapBVol.begin();bviter!=mapBVol.end();bviter++)
	{
		//SELECT COUNT(CimId) AS count FROM voltages WHERE CimId='200000003'
		char * psql = "SELECT COUNT(CimId) AS count FROM voltages WHERE CimId='%s'";
		string qsql = DBA->formatSql(psql,bviter->second.bvcimid.c_str());
		//string qsql = c("SELECT COUNT(CimId) AS count FROM voltages WHERE CimId='%1'").arg(bviter->second.bvcimid.c_str());
		LISTMAP countlist;
		
		int nCount = -1;

		countlist = DBA->getList(qsql.c_str());
		for (int i=0;i<countlist.size();i++)
		{
			STRMAP record = countlist.at(i);
			MAP_ITERATOR iter;

			iter = record.find("count");
			if (iter != record.end())
			{
				nCount = atoi(iter->second.c_str());
			}
		}

		if (nCount == 0)
		{
			psql = "INSERT INTO voltages (cimid,name,volvalue) VALUES ('%s','%s','%s')";
			qsql = DBA->formatSql(psql,bviter->second.bvcimid.c_str(),bviter->second.bvname.c_str(),bviter->second.bvval.c_str());

			//insersql = QString("INSERT INTO voltages (cimid,name,volvalue) VALUES ('%1','%2','%3')").arg(bviter->second.bvcimid.c_str()).arg(bviter->second.bvname.c_str()).arg(bviter->second.bvval.c_str());;
			//执行插入语句
			ret = DBA->execSql(qsql.c_str());
			if (ret <= 0)
			{
				sprintf(m_log,"saveBaseVoltage: BaseVoltage to db error insersql=%s ",qsql.c_str());
				save_log(m_log);
			}
		}
		else if (nCount == 1)
		{
			//UPDATE voltages SET Name='',VolValue='' WHERE CimId='20000003'
			
			psql = "UPDATE voltages SET Name='%s',VolValue='%s' WHERE CimId='%s'";
			qsql = DBA->formatSql(psql,bviter->second.bvname.c_str(),bviter->second.bvval.c_str(),bviter->second.bvcimid.c_str());
			//QString updatesql = QString("UPDATE voltages SET Name='%1',VolValue='%2' WHERE CimId='%3'").arg(bviter->second.bvname.c_str()).arg(bviter->second.bvval.c_str()).arg(bviter->second.bvcimid.c_str());
			//执行插入语句
			ret = DBA->execSql(qsql.c_str());
			if (ret <= 0)
			{
				sprintf(m_log,"saveBaseVoltage: BaseVoltage to db error updatesql=%s ",qsql.c_str());
				save_log(m_log);
			}
		}
		else
		{
			
			return RC_FAILURE;
		}

	}

	return RC_SUCCESS;
}


int CimLoader::saveSubstation()
{
	int ret;
	MAPSUBSTATION mapSubstion = parseXML.getSubstation();
	MAPSUBSTATION::iterator subiter;

	//组合插入语句值部分
	for (subiter=mapSubstion.begin();subiter!=mapSubstion.end();subiter++)
	{
		//SELECT COUNT(CimId) AS count FROM stations WHERE CimId='210000201'
		char * psql = "SELECT COUNT(CimId) AS count FROM stations WHERE CimId='%s'";
		string sql = DBA->formatSql(psql,subiter->second.faccimid.c_str());
		//QString qsql = QString("SELECT COUNT(CimId) AS count FROM stations WHERE CimId='%1'").arg(subiter->second.faccimid.c_str());
		LISTMAP countlist;

		int nCount = -1;

		countlist = DBA->getList(sql.c_str());
		for (int i=0;i<countlist.size();i++)
		{
			STRMAP record = countlist.at(i);
			MAP_ITERATOR iter;

			iter = record.find("count");
			if (iter != record.end())
			{
				nCount = atoi(iter->second.c_str());
			}
		}

		if (nCount == 0)
		{
			psql = "INSERT INTO stations (categoryid,cimid,name,currentname) VALUES (0,'%s','%s','%s')";
			
			sql = DBA->formatSql(psql,subiter->second.faccimid.c_str(),subiter->second.facname.c_str(),subiter->second.faccrname.c_str());
			//QString insersql = QString("INSERT INTO stations (categoryid,cimid,name,currentname) VALUES (0,'%1','%2','%3')").arg(subiter->second.faccimid.c_str()).arg(subiter->second.facname.c_str()).arg(subiter->second.faccrname.c_str());
			//执行插入语句
			ret = DBA->execSql(sql.c_str());
			if (ret <= 0)
			{
				sprintf(m_log,"saveSubstation: Substation to db error insersql=%s ",sql.c_str());
				save_log(m_log);
			}
		}
		else if (nCount == 1)
		{
			//UPDATE stations SET Name='test',currentname='test2' WHERE CimId='2100012'
			psql = "UPDATE stations SET Name='%s',currentname='%s' WHERE CimId='%s'";
			sql = DBA->formatSql(psql,subiter->second.facname.c_str(),subiter->second.faccrname.c_str(),subiter->second.faccimid.c_str());
			//QString updatesql = QString("UPDATE stations SET Name='%1',currentname='%2' WHERE CimId='%3'").arg(subiter->second.facname.c_str()).arg(subiter->second.faccrname.c_str()).arg(subiter->second.faccimid.c_str());
			//执行插入语句
			ret = DBA->execSql(sql.c_str());
			if (ret <= 0)
			{
				sprintf(m_log,"saveSubstation: Substation to db error updatesql=%s ",sql.c_str());
				save_log(m_log);
			}
		}
		else
		{
			sprintf(m_log,"saveSubstation: saveSubstation to db error nCount=%d ",nCount);
			save_log(m_log);

			return RC_FAILURE;
		}

	}

	return RC_SUCCESS;
}

int CimLoader::saveVollevel()
{
	int ret;
	MAPVOLEVEL mapVolel = parseXML.getVollevel();
	MAPVOLEVEL::iterator vliter;

	//创建临时表:创建之前删除临时表


	string dropsql = ("DROP TABLE IF EXISTS `station_vol_temp`;");
	ret = DBA->execSql(dropsql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveVollevel: drop table station_vol_temp error dropsql=%s ",dropsql.c_str());
		save_log(m_log);
	}

	//创建临时表

	string sql= ("CREATE TABLE `station_vol_temp` (\
		`StationId` varchar(100) NOT NULL,\
		`VolId` varchar(100) NOT NULL,\
		PRIMARY KEY (`StationId`,`VolId`)\
		) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveVollevel: create table station_vol_temp error createsql=%s ",sql.c_str());
		save_log(m_log);
	}

	string sqlInsert = ("INSERT INTO station_vol_temp (StationId,VolId) VALUES ");
	string sqlVal = "";
	//string sql = "";

	//组合插入语句值部分
	for (vliter=mapVolel.begin();vliter!=mapVolel.end();vliter++)
	{
		string sqlval = DBA->formatSql("('%s','%s'),",vliter->second.faccimid.c_str(),vliter->second.bvcimid.c_str());
		sqlVal.append(sqlval);
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	
	if (ret <= 0)
	{
		LOG->message("saveVollevel: insert data to table station_vol_temp error sql=%s",sql.c_str());
		return RC_FAILURE;
	}

	//删除station_vol表中内容
	string delsql = ("DELETE FROM station_vol;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveVollevel: delete table station_vol data error delsql=%s ",delsql.c_str());
		save_log(m_log);
	}


	//将临时表中的内容插入到station_vol中
	string selupsql = ("INSERT INTO station_vol(stationid,volid) \
		SELECT (SELECT id from stations where CimId = svt.stationid ) AS stationid,(SELECT id FROM voltages WHERE CimId = svt.volid) AS volid from station_vol_temp svt");
	ret = DBA->execSql(selupsql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveVollevel: station_vol_temp data to station_vol data error selupsql=%s ",selupsql.c_str());
		save_log(m_log);
	}

	//最后删除临时表
	ret = DBA->execSql(dropsql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveVollevel: drop table station_vol_temp error dropsql=%s ",dropsql.c_str());
		save_log(m_log);
	}

	return RC_SUCCESS;
}

int CimLoader::saveBreaker()
{
	int ret;
	int ncount=0;
	MAPBREAKER mapBreaker = parseXML.getBreaker();
	MAPBREAKER::iterator brkiter;


	//删除Units表中内容
	string delsql = ("DELETE FROM Units;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveBreaker: delete table Units data error delsql=%s ",delsql.c_str());
		save_log(m_log);
	}

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (brkiter=mapBreaker.begin();brkiter!=mapBreaker.end();brkiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",brkiter->second.brkcimid.c_str(),brkiter->second.brkname.c_str(),brkiter->second.faccimid.c_str(),brkiter->second.bvcimid.c_str(),eBreaker);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveBreaker: insert Breaker data to table Units error sql=%s ",sql.c_str());
		//save_log(m_log);

		return RC_FAILURE;
	}
	else
	{
		sprintf(m_log,"saveBreaker: insert Breaker data to table Counts = %d ",ncount);
		save_log(m_log);
	}


	return RC_SUCCESS;
}

int CimLoader::saveDisconnector()
{
	int ret;
	int ncount=0;
	MAPDISCTOR mapDisctor = parseXML.getDisconnector();
	MAPDISCTOR::iterator distoriter;

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (distoriter=mapDisctor.begin();distoriter!=mapDisctor.end();distoriter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",distoriter->second.distcimid.c_str(),distoriter->second.distname.c_str(),distoriter->second.faccimid.c_str(),distoriter->second.bvcimid.c_str(),eSwitch);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		
		LOG->message("saveDisconnector: insert Disconnector data to table Units error sql=%s ",sql.c_str());

		return RC_FAILURE;
	}
	else
	{
		sprintf(m_log,"saveDisconnector: insert Disconnector data to table Counts = %d ",ncount);
		save_log(m_log);
	}

	return RC_SUCCESS;
}

int CimLoader::saveGrdDisconnector()
{
	int ret;
	int ncount=0;
	MAPGRDDISCTOR mapGrdDisctor = parseXML.getGroundDisconnector();
	MAPGRDDISCTOR::iterator grditer;

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (grditer=mapGrdDisctor.begin();grditer!=mapGrdDisctor.end();grditer++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",grditer->second.gdistcimid.c_str(),grditer->second.gdistname.c_str(),grditer->second.faccimid.c_str(),grditer->second.bvcimid.c_str(),eGrdSwitch);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveGrdDisconnector: insert GrdDisconnector data to table Units error sql=%s ",sql.c_str());
		save_log(m_log);

		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveGrdDisconnector: insert GrdDisconnector data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::saveBusbarSection()
{
	int ret;
	int ncount=0;
	MAPBUS mapBus = parseXML.getBusbarSection();
	MAPBUS::iterator busiter;

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (busiter=mapBus.begin();busiter!=mapBus.end();busiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",busiter->second.buscimid.c_str(),busiter->second.busname.c_str(),busiter->second.faccimid.c_str(),busiter->second.bvcimid.c_str(),eBus);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveBusbarSection: insert BusbarSection data to table Units error sql=%s ",sql.c_str());
		
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveBusbarSection: insert BusbarSection data to table Counts = %d ",ncount);
		save_log(m_log);
	}

	return RC_SUCCESS;
}

int CimLoader::saveCompensator()
{
	int ret;
	int ncount=0;
	MAPCOMPTOR mapCmptor = parseXML.getCompensator();
	MAPCOMPTOR::iterator cptiter;

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (cptiter=mapCmptor.begin();cptiter!=mapCmptor.end();cptiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",cptiter->second.cptorcimid.c_str(),cptiter->second.cptorname.c_str(),cptiter->second.faccimid.c_str(),cptiter->second.bvcimid.c_str(),eCapacitor);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveCompensator: insert Compensator data to table Units error sql=%s ",sql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveCompensator: insert Compensator data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::saveEnergyConsumer()
{
	int ret;
	int ncount=0;
	MAPEYCONMER mapEyer = parseXML.getEnergyConsumer();
	MAPEYCONMER::iterator eyeriter;

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (eyeriter=mapEyer.begin();eyeriter!=mapEyer.end();eyeriter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",eyeriter->second.eyccimid.c_str(),eyeriter->second.eycname.c_str(),eyeriter->second.faccimid.c_str(),eyeriter->second.bvcimid.c_str(),eLoad);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveEnergyConsumer: insert EnergyConsumer data to table Units error sql=%s ",sql.c_str());
		
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveEnergyConsumer: insert EnergyConsumer data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::saveSynchronousMachine()
{
	int ret;
	int ncount=0;
	MAPSYNCMCHINE mapMch = parseXML.getSynchronousMachine();
	MAPSYNCMCHINE::iterator mchiter;

	string sqlInsert = ("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (mchiter=mapMch.begin();mchiter!=mapMch.end();mchiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",mchiter->second.symhcimid.c_str(),mchiter->second.symhname.c_str(),mchiter->second.faccimid.c_str(),mchiter->second.bvcimid.c_str(),eGenerator);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveSynchronousMachine: insert SynchronousMachine data to table Units error sql=%s ",sql.c_str());
		

		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveSynchronousMachine: insert SynchronousMachine data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::savePotentialTransformer()
{
	int ret;
	int ncount=0;
	MAPPT mapPT = parseXML.getPT();
	MAPPT::iterator ptiter;

	string sqlInsert = string("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (ptiter=mapPT.begin();ptiter!=mapPT.end();ptiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",ptiter->second.ptcimid.c_str(),ptiter->second.ptname.c_str(),ptiter->second.faccimid.c_str(),ptiter->second.bvcimid.c_str(),ePt);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("savePotentialTransformer: insert PotentialTransformer data to table Units error sql=%s ",sql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("savePotentialTransformer: insert PotentialTransformer data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::savePowerTransformer()
{
	int ret;
	int ncount=0;
	MAPPOWERTFUT mapPtfut = parseXML.getPowerTransformerUnt();
	MAPPOWERTFUT::iterator ptiter;

	string sqlInsert = string("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (ptiter=mapPtfut.begin();ptiter!=mapPtfut.end();ptiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",ptiter->second.ptfcimid.c_str(),ptiter->second.ptfname.c_str(),ptiter->second.faccimid.c_str(),ptiter->second.bvcimid.c_str(),eTransformer);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("savePowerTransformer: insert PowerTransformer data to table Units error sql=%s ",sql.c_str());
		
		return RC_FAILURE;
	}
	else
	{
		LOG->message("savePowerTransformer: insert PowerTransformer data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::saveACLineSegment()
{
	int ret;
	int ncount=0;
	MAPACLINE mapPtfut = parseXML.getACLineSegment();
	MAPACLINE::iterator ptiter;

	string sqlInsert = string("INSERT INTO Units (cimid,name,stationcim,volcim,unittype) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (ptiter=mapPtfut.begin();ptiter!=mapPtfut.end();ptiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d),",ptiter->second.aclcimid.c_str(),ptiter->second.aclname.c_str(),ptiter->second.fac1cimid.c_str(),ptiter->second.bvcimid.c_str(),eLINE);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveACLineSegment: insert ACLineSegment data to table Units error sql=%s ",sql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveACLineSegment: insert ACLineSegment data to table Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}

int CimLoader::saveTransformerWinding()
{
	int ret;
	int ncount=0;
	MAPTRANSFWUT mapTsfw = parseXML.getTransformerWindingUnt();
	MAPTRANSFWUT::iterator tsfwiter;

	//创建临时表:创建之前删除临时表
	string dropsql = string("DROP TABLE IF EXISTS `Windings_temp`;");
	ret = DBA->execSql(dropsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTransformerWinding: drop table Windings_temp error dropsql=%s ",dropsql.c_str());
		
	}
	else
	{
		LOG->message("saveTransformerWinding: drop table Windings_temp success dropsql=%s ",dropsql.c_str());
	
	}

	//创建临时表
	string createsql = string("CREATE TABLE `Windings_temp` (\
		`UnitCim` varchar(100) NOT NULL,\
		`CimId` varchar(100) DEFAULT NULL,\
		`VolId` varchar(100) NOT NULL,\
		`WindingIndex` tinyint(4) NOT NULL,\
		`WindingGrade` tinyint(4) NOT NULL\
		) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	ret = DBA->execSql(createsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTransformerWinding: create table Windings_temp error createsql=%s ",createsql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveTransformerWinding: create table Windings_temp success createsql=%s ",createsql.c_str());
	
	}

	string sqlInsert = string("INSERT INTO Windings_temp (UnitCim,CimId,VolId,WindingIndex,WindingGrade) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (tsfwiter=mapTsfw.begin();tsfwiter!=mapTsfw.end();tsfwiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s',%d,%d),",tsfwiter->second.ptfcimid.c_str(),tsfwiter->second.tfwcimid.c_str(),tsfwiter->second.bvcimid.c_str(),tsfwiter->second.wdindex,tsfwiter->second.wdgrade);

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTransformerWinding: insert data to table Windings_temp error sql=%s ",sql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveTransformerWinding: insert Windings_temp data to table Counts = %d ",ncount);
	
	}

	//删除Windings表中内容
	string delsql = string("DELETE FROM Windings;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTransformerWinding: delete table Windings data error delsql=%s ",delsql.c_str());
	
	}
	else
	{
		LOG->message("saveTransformerWinding: delete table Windings data success delsql=%s ",delsql.c_str());

	}


	//将临时表中的内容插入到Windings中
	string selupsql = string("INSERT INTO Windings(UnitCim,CimId,VolId,WindingIndex,WindingGrade) \
							   SELECT UnitCim,CimId,(SELECT id FROM voltages WHERE CimId = wdt.volid) AS volid ,WindingIndex,WindingGrade from Windings_temp wdt");
	ret = DBA->execSql(selupsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTransformerWinding: Windings_temp data to Windings data error selupsql=%s ",selupsql.c_str());
	}
	else
	{
		LOG->message("saveTransformerWinding: Windings_temp data to Windings data success selupsql=%s ",selupsql.c_str());
		
	}

	//最后删除临时表
	ret = DBA->execSql(dropsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTransformerWinding: drop table Windings_temp error dropsql=%s ",dropsql.c_str());
		
	}
	else
	{
		LOG->message("saveTransformerWinding: drop table Windings_temp success dropsql=%s ",dropsql.c_str());
		
	}

	return RC_SUCCESS;
}

int CimLoader::saveRelateLine()
{
	int ret;
	int ncount=0;
	MAPACLINE mapACline = parseXML.getACLineSegment();
	MAPACLINE::iterator aciter;

	//删除related_line表中内容
	string delsql = string("DELETE FROM related_line;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveRelateLine: delete table related_line data error delsql=%s ",delsql.c_str());

	}
	else
	{
		LOG->message("saveRelateLine: delete table related_line data success delsql=%s ",delsql.c_str());

	}

	string sqlInsert = string("INSERT INTO related_line (UnitCim,StationCim) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (aciter=mapACline.begin();aciter!=mapACline.end();aciter++)
	{
		string sqlval = DBA->formatSql("('%s','%s'),",aciter->second.aclcimid.c_str(),aciter->second.fac1cimid.c_str());

		sqlVal.append(sqlval);

		ncount++;

		sqlval =  DBA->formatSql("('%s','%s'),",aciter->second.aclcimid.c_str(),aciter->second.fac2cimid.c_str());

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveRelateLine: insert ACLineSegment data to table RelateLine error sql=%s ",sql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveRelateLine: insert ACLineSegment data to table  RelateLine Counts = %d ",ncount);
	
	}

	return RC_SUCCESS;
}

int CimLoader::saveConnectivityNode()
{
	int ret;
	int ncount=0;
	MAPCNODE mapCNode = parseXML.getConnectivityNode();
	MAPCNODE::iterator cnoditer;

	//创建临时表:创建之前删除临时表
	string dropsql = string("DROP TABLE IF EXISTS `connections_temp`;");
	ret = DBA->execSql(dropsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveConnectivityNode: drop table connections_temp error dropsql=%s ",dropsql.c_str());
		
	}
	else
	{
		LOG->message("saveConnectivityNode: drop table connections_temp success dropsql=%s ",dropsql.c_str());
	
	}

	//创建临时表
	string createsql = string("CREATE TABLE `connections_temp` (\
		`CimId` varchar(100) NOT NULL,\
		`Name` varchar(100) NOT NULL,\
		`StationId` varchar(100) NOT NULL\
		) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
	ret = DBA->execSql(createsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveConnectivityNode: create table connections_temp error createsql=%s ",createsql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveConnectivityNode: create table connections_temp success createsql=%s ",createsql.c_str());
		
	}

	string sqlInsert = string("INSERT INTO connections_temp (CimId,Name,StationId) VALUES ");
	string sqlVal = "";
	string  sql = "";

	//组合插入语句值部分
	for (cnoditer=mapCNode.begin();cnoditer!=mapCNode.end();cnoditer++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%d'),",cnoditer->second.nodecimid.c_str(),cnoditer->second.nodename.c_str(),cnoditer->second.faccimid.c_str());

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveConnectivityNode: insert data to table connections_temp error sql=%s ",sql.c_str());
	
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveConnectivityNode: insert connections_temp data to table Counts = %d ",ncount);
		
	}

	//删除connections表中内容
	string delsql = string("DELETE FROM connections;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveConnectivityNode: delete table connections data error delsql=%s ",delsql.c_str());
		
	}
	else
	{
		LOG->message("saveConnectivityNode: delete table connections data success delsql=%s ",delsql.c_str());
		
	}


	//将临时表中的内容插入到connections中
	string selupsql = string("INSERT INTO connections(CimId,Name,StationId) \
							   SELECT CimId,Name,(SELECT id from stations where CimId = stnt.stationid ) AS stationid  from connections_temp stnt");
	ret = DBA->execSql(selupsql.c_str());
	if (ret <= 0)
	{
		sprintf(m_log,"saveConnectivityNode: Windings_temp data to connections data error selupsql=%s ",selupsql.c_str());
		save_log(m_log);
	}
	else
	{
		LOG->message("saveConnectivityNode: Windings_temp data to connections data success selupsql=%s ",selupsql.c_str());
		
	}

	//最后删除临时表
	ret = DBA->execSql(dropsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveConnectivityNode: drop table connections_temp error dropsql=%s ",dropsql.c_str());
	
	}
	else
	{
		LOG->message("saveConnectivityNode: drop table connections_temp success dropsql=%s ",dropsql.c_str());
	
	}

	return RC_SUCCESS;
}

int CimLoader::saveTerminalRelation()
{
	int ret;
	int ncount=0;
	MAPTMLRAL mapTmla = parseXML.getTerminalRelation();
	MAPTMLRAL::iterator tmlaiter;

	//删除relations表中内容
	string delsql = string("DELETE FROM relations;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTerminalRelation: delete table relations data error delsql=%s ",delsql.c_str());

	}
	else
	{
		LOG->message("saveTerminalRelation: delete table relations data success delsql=%s ",delsql.c_str());

	}

	string sqlInsert = string("INSERT INTO relations (CimId,Name,StationCim,UnitCim,Port,ConnCim) VALUES ");
	string sqlVal = "";
	string sql = "";

	//组合插入语句值部分
	for (tmlaiter=mapTmla.begin();tmlaiter!=mapTmla.end();tmlaiter++)
	{
		string sqlval = DBA->formatSql("('%s','%s','%s','%s',%d,'%s'),",tmlaiter->second.tmlcimid.c_str(),tmlaiter->second.tmlname.c_str(),tmlaiter->second.faccimid.c_str(),tmlaiter->second.unitcimid.c_str(),tmlaiter->second.tmlport,tmlaiter->second.nodecimid.c_str());

		sqlVal.append(sqlval);

		ncount++;
	}

	//去除最后一个逗号
	sqlVal = sqlVal.substr(0,sqlVal.length()-1);

	//拼接完整插入sql语句
	sql.append(sqlInsert).append(sqlVal);

	//执行插入语句
	ret = DBA->execSql(sql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveTerminalRelation: insert TerminalRelation data to table relations error sql=%s ",sql.c_str());
		
		return RC_FAILURE;
	}
	else
	{
		LOG->message("saveTerminalRelation: insert TerminalRelation data to table  relations Counts = %d ",ncount);
		
	}

	return RC_SUCCESS;
}
int CimLoader::saveUnitStatus()
{
	int ret;

	//删除unit_status表中内容
	string delsql = string("DELETE FROM unit_status;");
	ret = DBA->execSql(delsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveUnitStatus: delete table unit_status data error delsql=%s ",delsql.c_str());
		
	}
	else
	{
		LOG->message("saveUnitStatus: delete table unit_status data success delsql=%s ",delsql.c_str());
		
	}

	//填充unit_status表:实时态Saveid=0
	string selupsql = string("INSERT INTO unit_status(UnitCim,StationCim) SELECT CimId,StationCim from units");
	ret = DBA->execSql(selupsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveUnitStatus: units data Saveid=0 to unit_status data error selupsql=%s ",selupsql.c_str());
		
	}
	else
	{
		LOG->message("saveUnitStatus: units data Saveid=0 to unit_status data success selupsql=%s ",selupsql.c_str());
		
	}

	//填充unit_status表:模拟态Saveid=1
	string selupdesql = string("INSERT INTO unit_status(Saveid,UnitCim,StationCim) SELECT 1 as saveid,CimId,StationCim from units");
	ret = DBA->execSql(selupdesql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveUnitStatus: units data Saveid=1 to unit_status data error selupsql=%s ",selupdesql.c_str());

	}
	else
	{
		LOG->message("saveUnitStatus: units data Saveid=1 to unit_status data success selupsql=%s ",selupdesql.c_str());

	}

	//更新unit_status表中设备为发电机的带电和是否电源点信息
	string selupdsql = string("UPDATE unit_status us,units u set us.IsElectric=1,us.IsPower=2 where u.CimId=us.UnitCim and u.UnitType=5");
	ret = DBA->execSql(selupdsql.c_str());
	if (ret <= 0)
	{
		LOG->message("saveUnitStatus: update unit_status eGenerator data error selupsql=%s ",selupdsql.c_str());
		
	}
	else
	{
		LOG->message("saveUnitStatus: update unit_status eGenerator data success selupsql=%s ",selupdsql.c_str());
		
	}

	return RC_SUCCESS;
}