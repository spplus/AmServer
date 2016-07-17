#include "querymgrcmd.h"


void QueryMgrCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_QUERY_CIRCLE_LIST:
		{
			m_isFirst = true;
			m_buslist.clear();
			queryCricleList(msg);
		}
		break;
	case CMD_QUERY_SIGN_LIST:
		querySignList(msg);
		break;
	case CMD_QUERY_GSWITCH_LIST:
		queryGSwitchList(msg);
		break;
	case CMD_QUERY_MSET_LIST:
		queryMsetList(msg);
		break;
	case CMD_QUERY_EVENT_LIST:
		queryEventList(msg);
		break;
	}

}


void QueryMgrCmd::queryCricleList(sClientMsg* msg)
{
	PBNS::CircleListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int nSvid = req.saveid();
	std::string saveid = COM->i2str(nSvid);
	
	//��ʼ��·��ѯ����
	cricleTopo(nSvid);


	PBNS::CircleListMsg_Response resp;

	//���ĸ�߲�ѯ����
	string buss;
	for (int i = 0;i<m_buslist.size();i++)
	{
		buss = "'" + m_buslist.at(i) + "'";
		buss += ",";
	}
	if (buss.length()>2)
	{
		buss = buss.substr(0,buss.length()-1);
	}
	else
	{
		// ���ص��ͻ���
		string data;
		resp.SerializeToString(&data);
		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

		return;
	}

	string sql;
	//��ѯĸ�����ƺͶ�Ӧ��վ����
	char *psql = "select ut.CimId,ut.Name,ut.StationCim,(select stn.Name from stations stn where stn.CimId=ut.StationCim ) as StationName from units ut where ut.UnitType=2 and ut.CimId in (%s);";

	sql = App_Dba::instance()->formatSql(psql,buss.c_str());

	LISTMAP circlelist;
	circlelist = App_Dba::instance()->getList(sql.c_str());

	for (int i=0; i < circlelist.size(); i++)
	{
		STRMAP recode = circlelist.at(i);
		MAP_ITERATOR iter;
		PBNS::CircleQueryBean *crlbean = resp.add_circlelist();

		iter = recode.find("CimId");
		if (iter != recode.end())
		{
			crlbean->set_unitcim(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			crlbean->set_name(iter->second);
		}

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			crlbean->set_stationcim(iter->second);
		}

		iter = recode.find("StationName");
		if (iter != recode.end())
		{
			crlbean->set_stationname(iter->second);
		}

	}

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;

}


void QueryMgrCmd::cricleTopo(int saveid)
{
	// �Ѿ��������������豸ID����
	STRMAP passedNodes;

	string sql;
	//��ѯ���п��ء���բcimid
	char *psql = "select CimId from units where UnitType in (0,1) ;";

	LISTMAP bekswhList = App_Dba::instance()->getList(psql);
	for (int i = 0;i<bekswhList.size();i++)
	{
		STRMAP saveMap = bekswhList.at(i);
		MAP_ITERATOR iter = saveMap.find("CimId");
		if(iter != saveMap.end())
		{
			//ĸ�߼�¼����ʼ����Ϊ0
			m_busCount = 0;

			//������ʼ���ص�բcimid
			m_brkswhCim = iter->second;

			//���ݿ��ء���բԪ�����л�·����
			cricleTopoByUnit(saveid,iter->second,passedNodes);

		}
	}

}

void QueryMgrCmd::cricleTopoByUnit(int saveid,string unitcim,STRMAP& passNodes)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

	// �ѵ�ǰԪ�����뵽�ѷ����б�
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.��Ӧ�ĵ������ӵ㣨���ǲ����е��������ӵ㣩
	for (int i = 0; i < connIds.size();i++)
	{
		STRMAP connMap = connIds.at(i);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

			// �������豸����
			for (int k = 0;k < unitsList.size();k++)
			{
				STRMAP  unitMap = unitsList.at(k);
				MAP_ITERATOR unitIter = unitMap.find("id");
				string unitId ;
				if (unitIter != unitMap.end())
				{
					// �ж��Ƿ��Ѿ���Ϊ��ʼ�豸���������������������
					if (passNodes.find(unitIter->second) != passNodes.end())
					{
						continue;
					}
				}

				// ���β�ѯ��Ԫ��CIMID
				unitId = unitIter->second;

				// Ԫ������
				int unitType = COM->getIval(unitMap,"UnitType");

				// Ԫ��״̬
				int state = COM->getIval(unitMap,"State");

				if (unitType == eTRANSFORMER || ((unitType == eBREAKER||unitType == eSWITCH) && state == 0))
				{
					//����ѭ��ʱĸ�߸�����0
					m_busCount = 0;
					m_buslist.clear();

					// ��ѹ�����Ͽ��Ŀ��غ͵�բΪ����·�������յ�
					continue;
				}
				else if (unitType == eBUS)		//����豸��������ĸ��
				{
					m_busCount++;

					string buscim = unitId;
					//����ĸ�߼�¼
					m_buslist.push_back(unitId);
				}
				else if (m_brkswhCim == unitId && (m_busCount>1))		//������ǰcimid������ʼcimid��ĸ���ۼ�������1��ʾ��·�γ�
				{
					return;
				}
				else
				{
					//�ݹ�
					cricleTopoByUnit(saveid,unitId,passNodes);
				}
			}
		}

		// �״����ˣ�ֻ���˵������ӵ�
		if (m_isFirst)
		{
			break;
		}

		// ���Ϊ��һ�����
		m_isFirst = false;
	}

	return;
}


PBNS::StateBean QueryMgrCmd::getUnitByCim(int saveid,string unitcim)
{
	PBNS::StateBean bean;
	char* psql = "select b.State,a.UnitType,a.StationCim,b.IsElectric,b.IsPower " \
		"from units a left join " \
		"unit_status b on a.CimId=b.UnitCim and b.SaveId=%d " \
		"where a.CimId='%s'";
	string sql = DBA->formatSql(psql,saveid,unitcim.c_str());
	LISTMAP unitList = DBA->getList(sql.c_str());
	if (unitList.size() == 1)
	{
		STRMAP unitMap = unitList.at(0);
		string strval = COM->getVal(unitMap,"State");

		if (strval.length()>0)
		{
			bean.set_state(COM->str2i(strval));
		}
		strval = COM->getVal(unitMap,"UnitType");
		bean.set_unittype(COM->str2i(strval));

		strval = COM->getVal(unitMap,"StationCim");
		bean.set_stationcim(strval);

		strval = COM->getVal(unitMap,"IsElectric");
		if (strval.length()>0)
		{
			bean.set_iselectric(COM->str2i(strval));
		}

		strval = COM->getVal(unitMap,"IsPower");
		if (strval.length()>0)
		{
			bean.set_ispower(COM->str2i(strval));
		}

	}
	else
	{
		LOG->warn("���ݴ���ͬһ�浵�����ж����ͬcim��Ԫ��:%s",unitcim.c_str());
	}

	return bean;
}

LISTMAP QueryMgrCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim='%s'";
	string sql = DBA->formatSql(psql,unitid.c_str());
	connList = DBA->getList(sql.c_str());
	return connList;
}

LISTMAP QueryMgrCmd::getUnitsByConnId(string connid,string saveid)
{
	// ���⣺������ѯ�豸״̬��ʱ�򣬲��ÿ���saveidô��unit_status���У�ͬһ��unit���ܻ��ж�����¼���������¼Ϊ׼�أ�
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,c.State,d.VolValue " \
		"from Relations a left join Units b on a.UnitCim=b.CimId  "\
		"left join unit_status c on c.UnitCim=b.cimid " \
		"left join voltages d on d.CimId=b.VolCim " \
		"where a.ConnCim='%s' and c.saveid=%s";
	string sql = DBA->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = DBA->getList(sql.c_str());
	return unitsList;
}

void QueryMgrCmd::querySignList(sClientMsg* msg)
{
	PBNS::SignListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//��ѯ�����б�
	char *psql = "SELECT us.UnitCim,(SELECT ut.Name from units ut where ut.CimId=us.UnitCim) as unitName,us.StationCim, \
		(SELECT st.Name from stations st WHERE st.CimId=us.StationCim) stationName, \
		(SELECT ut.UnitType from units ut where ut.CimId=us.UnitCim) unitType from unit_status us where us.IsBoard = 1;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP signlist;
	signlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::SignListMsg_Response resp;
	for (int i=0; i < signlist.size(); i++)
	{
		STRMAP recode = signlist.at(i);
		MAP_ITERATOR iter;
		PBNS::SignQueryBean *sgbean = resp.add_signlist();

		iter = recode.find("UnitCim");
		if (iter != recode.end())
		{
			sgbean->set_unitcim(iter->second);
		}

		iter = recode.find("unitName");
		if (iter != recode.end())
		{
			sgbean->set_unitcimname(iter->second);
		}

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			sgbean->set_stationcim(iter->second);
		}

		iter = recode.find("stationName");
		if (iter != recode.end())
		{
			sgbean->set_stationname(iter->second);
		}

		iter = recode.find("unitType");
		if (iter != recode.end())
		{
			sgbean->set_unittype(iter->second);
		}

	}

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;

}

void QueryMgrCmd::queryGSwitchList(sClientMsg* msg)
{
	PBNS::GswitchListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	string sql;
	//��ѯ�ӵ��б�
	char *psql = "SELECT us.UnitCim,(SELECT ut.Name from units ut where ut.CimId=us.UnitCim) as unitName,us.StationCim, \
				 (SELECT st.Name from stations st WHERE st.CimId=us.StationCim) stationName, \
				 (SELECT ut.UnitType from units ut where ut.CimId=us.UnitCim) unitType from unit_status us where us.IsGround = 1;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP gswitchlist;
	gswitchlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::GswitchListMsg_Response resp;
	for (int i=0; i < gswitchlist.size(); i++)
	{
		STRMAP recode = gswitchlist.at(i);
		MAP_ITERATOR iter;
		PBNS::GswitchQueryBean *gsbean = resp.add_gswitchlist();

		iter = recode.find("UnitCim");
		if (iter != recode.end())
		{
			gsbean->set_unitcim(iter->second);
		}

		iter = recode.find("unitName");
		if (iter != recode.end())
		{
			gsbean->set_unitcimname(iter->second);
		}

		iter = recode.find("StationCim");
		if (iter != recode.end())
		{
			gsbean->set_stationcim(iter->second);
		}

		iter = recode.find("stationName");
		if (iter != recode.end())
		{
			gsbean->set_stationname(iter->second);
		}

		iter = recode.find("unitType");
		if (iter != recode.end())
		{
			gsbean->set_unittype(iter->second);
		}

	}

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}

void QueryMgrCmd::queryMsetList(sClientMsg* msg)
{

}

void QueryMgrCmd::queryEventList(sClientMsg* msg)
{
	PBNS::EventListMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	string rd = req.reqdate();

	//select sye.UnitId,sye.EventValue,sye.SynTime,us.Name,us.UnitType,st.CimId as stioncim,st.Name as stionname from syn_events sye,units us,stations st where sye.UnitId=us.CimId and us.StationCim=st.CimId
	string sql;
	//��ѯ�����б�
	char *psql = "select sye.UnitId,sye.EventValue,sye.SynTime,us.Name,us.UnitType,st.CimId as StionCim,st.Name as StionName \
		from syn_events sye,units us,stations st where sye.UnitId=us.CimId and us.StationCim=st.CimId ;";

	sql = App_Dba::instance()->formatSql(psql);

	LISTMAP eventlist;
	eventlist = App_Dba::instance()->getList(sql.c_str());

	PBNS::EventListMsg_Response resp;
	for (int i=0; i < eventlist.size(); i++)
	{
		STRMAP recode = eventlist.at(i);
		MAP_ITERATOR iter;
		PBNS::EventQueryBean *evtbean = resp.add_eventlist();

		iter = recode.find("UnitId");
		if (iter != recode.end())
		{
			evtbean->set_unitcim(iter->second);
		}

		iter = recode.find("Name");
		if (iter != recode.end())
		{
			evtbean->set_unitcimname(iter->second);
		}

		iter = recode.find("StionCim");
		if (iter != recode.end())
		{
			evtbean->set_stationcim(iter->second);
		}

		iter = recode.find("StionName");
		if (iter != recode.end())
		{
			evtbean->set_stationname(iter->second);
		}

		iter = recode.find("UnitType");
		if (iter != recode.end())
		{
			evtbean->set_unittype(iter->second);
		}

		iter = recode.find("EventValue");
		if (iter != recode.end())
		{
			evtbean->set_eventvalue(iter->second);
		}

		iter = recode.find("SynTime");
		if (iter != recode.end())
		{
			evtbean->set_eventtime(iter->second);
		}
	}	

	// ���ص��ͻ���
	string data;
	resp.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

	return;
}


