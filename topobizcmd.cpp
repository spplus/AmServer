#include "topobizcmd.h"

void TopoBizCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TOPO_ENTIRE:		// ��վ����
		topoEntire();
		break;
	case CMD_TOPO_BREAKER_CHANGE:		// ���ر�λ
		topoOnBreakerChange(msg);
		break;
	}
}

void TopoBizCmd::topoEntire()
{
	// �Ѿ��������������豸ID����
	STRMAP passedNodes;


	// 1.��ѯ���з������豸
	string sql ;
	char * p = "select Id,StationCim as StationId from units where UnitType=5";
	LISTMAP	 powerList;
	MAP_ITERATOR iter;
	powerList = App_Dba::instance()->getList(p);
	
	for (int i = 0;i<powerList.size();i++)
	{
		STRMAP power = powerList.at(i);
		iter = power.find("Id");
		string powerid ,stationid;
		if (iter != power.end())
		{
			powerid = iter->second;

		}
		iter = power.find("StationId");
		if (iter != power.end())
		{
			stationid = iter->second;
		}
		
		// ����Ԫ����������
		topoByUnitId(powerid,stationid,passedNodes);

	}
	

	// 7.�ظ�����2

	// 8.����ôα��������豸վ��ID����ʼ�豸��վ��ID����ͬ���Ҹ��豸Ϊ�����ߣ����Ǹý�����Ϊ��Ե�Դ�㣻

	// 9.�Ѿ���Ϊ��ʼ����豸��������Ϊ��ʼ����б���
}

LISTMAP TopoBizCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid);
	connList = App_Dba::instance()->getList(sql.c_str());
	return connList;
}


LISTMAP TopoBizCmd::getUnitsByConnId(string connid)
{

	// ���⣺������ѯ�豸״̬��ʱ�򣬲��ÿ���saveidô��unit_status���У�ͬһ��unit���ܻ��ж�����¼���������¼Ϊ׼�أ�
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,c.State,d.VolValue " \
		"from Relations a left join Units b on a.UnitCim=b.CimId  "\
		"left join unit_status c on b.UnitCim=b.cimid " \
		"left join voltages d on d.CimId=b.VolCim " \
		"where a.ConnCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,connid);
	unitsList = App_Dba::instance()->getList(sql.c_str());
	return unitsList;
}

void TopoBizCmd::topoByUnitId(string unitid,string stationid,STRMAP& passNodes)
{
	// �ѵ�ǰԪ�����뵽�ѷ����б�
	passNodes.insert(MAPVAL(unitid,unitid));

	// 2.����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
	LISTMAP connIds = getConnIdByUnitsId(unitid);

	// 3.�������ӵ�ID�����ӹ�ϵ���ѯ�������豸
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second);

			// �������豸����
			for (int k = 0;k<unitsList.size();k++)
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

				// ��ѯԪ������
				unitIter = unitMap.find("UnitType");

				// �豸����
				int etype ;

				// ���ʱ����Ҫ���� 0 ��Ҫ���ˣ�1 ����Ҫ����
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);
					if (etype == eBreaker || etype == eSwitch)
					{
						// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 1)
							{
								// ���¸��豸����״̬Ϊ����
								updateIsElectricByUnitId(unitId,1);
							}
							else
							{
								// 6.������豸Ϊ���أ���Ϊ�Ͽ��������ٱ������豸�Ĺ����豸��
								updateIsElectricByUnitId(unitId,0);

								// ��ǲ���Ҫ����
								flag = 1;
							}
						}

					}
					else
					{
						// 5.������豸���ǿ����豸��������Ϊ���磻
						updateIsElectricByUnitId(unitId,1);
					}
				}

				// �ж��Ƿ�Ϊ��վ��
				unitIter = unitMap.find("StationId");
				if (unitIter != unitMap.end())
				{

					// ����ôα��������豸վ��ID����ʼ�豸��վ��ID����ͬ���Ҹ��豸Ϊ�����ߣ����Ǹý�����Ϊ��Ե�Դ�㣻
					if (str2i(unitIter->second) != str2i(stationid))
					{
						if (etype == eLine)
						{
							updateIsPowerByUnitId(unitid);
						}
					}
				}

				// վ��ID
				string sId = unitIter->second;
				
				if (flag != 1)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					topoByUnitId(unitid,sId,passNodes);
				}
				
			}

		}

	}
}

void TopoBizCmd::updateIsPowerByUnitId(string unitid)
{
	char* psql = "update unit_status set IsPower=1 where UnitCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid);
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret>0)
	{
		LOG->message("update :%s as power success.",unitid.c_str());
	}
	else
	{
		LOG->message("update :%s as power failed.",unitid.c_str());
	}
	
}

void TopoBizCmd::updateIsElectricByUnitId(string unitid,int state)
{
	char* psql = "update unit_status set IsElectric=%d where UnitCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,state,unitid);
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret>0)
	{
		LOG->message("update :%s to electric  success.",unitid.c_str());
	}
	else
	{
		LOG->message("update :%s to electric failed.",unitid.c_str());
	}
}


void TopoBizCmd::topoOnBreakerChange(sClientMsg *msg)
{
	/*
	���������	1.�浵ID
						2.Ԫ��CIMID
	��������
					�ҳ���վ�豸�ڴ˴ο��ر�λ��ĵĴ���״̬.
	*/
	// �������˷����Ľ�������豸�Ĵ���״̬
	PBNS::DevStateMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);

	int saveId = req.saveid();
	string cimid = req.unitcim();


	// �豸��cimid,����״̬ 1���磬0������
	vector<PBNS::StateBean>	rsltMap;

	// �Ѿ��������������豸ID����
	STRMAP passedNodes;

	// 1. ��ѯָ��saveid��ָ��unit��Ӧվ��������豸״̬���ϣ�
	char * psql = "select a.UnitCim,a.StationCim,IsElectric,IsPower,VolValue " \
		"from unit_status a " \
		"left join units b on a.UnitCim=b.CimId " \
		"left join voltages c on c.CimId = b.VolCim " \
		"where a.SaveId=%d and a.StationCim in (select d.StationCim from units d where CimId=%s)";
	string sql = App_Dba::instance()->formatSql(psql,saveId,cimid.c_str());

	LISTMAP unitList = App_Dba::instance()->getList(sql.c_str());

	// 2.�����Ͻ���е�ispower��ֵ����1����2�ļ�¼��Ϊ���������˷�����
	for (int i = 0;i<unitList.size();i++)
	{
		STRMAP unitMap = unitList.at(i);
		MAP_ITERATOR iter = unitMap.find("IsPower");

		// ��Ե�Դ�����Ե�Դ���¼
		if (iter != unitMap.end() && (str2i(iter->second) == 1 || str2i(iter->second) == 2))
		{
			iter = unitMap.find("UnitCim");
			if (iter != unitMap.end())
			{
				PBNS::StateBean bean;
				bean.set_cimid(iter->second);
				iter = unitMap.find("VolValue");
				if(iter != unitMap.end())
				{
					bean.set_iselectric(str2i(iter->second));
				}

				// �Ը��豸Ϊ���������˷���
				topoByUnitIdMem(bean,passedNodes,rsltMap);
			}
		}
	}

	// ���˽�����ؿͻ���
	PBNS::DevStateMsg_Response res;
	vector<PBNS::StateBean>::iterator iter = rsltMap.begin();
	for (int i=0;i<rsltMap.size();i++)
	{
		PBNS::StateBean* bean = res.add_devstate();
		// ��ѯ���豸�ĵ�ѹ�ȼ�
		bean->CopyFrom(rsltMap.at(i));
	}
	string data;
	res.SerializeToString(&data);
	App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);

}


void TopoBizCmd::topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap)
{
	string unitid = bean.cimid();

	LOG->debug("topoByUnitIdMem,uinit cimid:%s",unitid.c_str());

	// �ѵ�ǰԪ�����뵽�ѷ����б�
	passNodes.insert(MAPVAL(unitid,unitid));

	// 2.����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
	LISTMAP connIds = getConnIdByUnitsId(unitid);

	// 3.�������ӵ�ID�����ӹ�ϵ���ѯ�������豸
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second);

			// �������豸����
			for (int k = 0;k<unitsList.size();k++)
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

				PBNS::StateBean cbean;
				cbean.set_cimid(unitIter->second);

				// ���β�ѯ��Ԫ��CIMID
				unitId = unitIter->second;

				// ��ѯԪ������
				unitIter = unitMap.find("UnitType");

				// �豸����
				int etype ;

				// ��־�Ƿ���Ҫ�����豸Ϊ���������ˡ�������豸Ϊ���أ���δ�Ͽ�״̬�������Ը��豸Ϊ����������
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);

					// ȡ��ѹ�ȼ�ֵ
					unitIter = unitMap.find("	VolValue");
					if (unitIter != unitMap.end())
					{
						cbean.set_volvalue(unitIter->second);
					}

					if (etype == eBreaker || etype == eSwitch)
					{
						// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 0)
							{
								//  �Ѹÿ����豸״̬��Ϊ1�������Ը��豸Ϊ���������ˣ����Ϊ0������������豸
								rsltMap.push_back(cbean);

							}
							else
							{
								// ���Ϊ����Ҫ��������
								flag = 1;
							}
						}

					}
					else
					{
						// 5.������豸���ǿ����豸��������Ϊ���磻
						rsltMap.push_back(cbean);
					}
				}

				// ���˲���Ҫ�������˵��豸
				if (flag != 1)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					topoByUnitIdMem(cbean,passNodes,rsltMap);
				}
			
			}

		}

	}
}
