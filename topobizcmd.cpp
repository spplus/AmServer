#include "topobizcmd.h"
#include "rulebiz1.h"
#include "rulebiz4.h"
#include "rulebiz5.h"
#include "rulebiz12.h"
#include "rulebiz16.h"
#include "rulebiz18.h"
#include "rulebiz20.h"
#include "rulebiz21.h"
#include "rulebiz22.h"
#include "rulebiz25.h"
#include "rulebiz26.h"
#include "rulebiz27.h"
#include "rulebiz28.h"
#include "rulebiz32.h"
#include "rulebiz33.h"
#include "rulebiz34.h"
#include "rulebiz35.h"
#include "rulebiz38.h"
#include "rulebiz39.h"
#include "rulebiz43.h"
#include "rulebiz45.h"
#include "rulebiz46.h"
#include "rulebiz47.h"
#include "rulebiz48.h"


void TopoBizCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TOPO_ENTIRE:		// ��վ����
		topoEntire(msg);
		break;
	case CMD_TOPO_BREAKER_CHANGE:		// ���ر�λ
		topoOnBreakerChange(msg);
		break;
	case CMD_CHECK_PASS:					// ����У��ͨ��
		topoOnBreakerChange(msg);
		break;
	default:
		break;
	}
}

void TopoBizCmd::topoBySaveId(string saveid,int unittype)
{
	if (saveid.length() <= 0 || unittype <= 0)
	{
		return;
	}
	// �Ѿ��������������豸ID����
	STRMAP passedNodes;

	// 1.��ѯ���з�����豸
	string sql ;
	char * p = "select CimId,StationCim as StationId from units where UnitType=%d union all select "\
		"UnitCim as CimId,StationCim as StationId  from related_line where IsPower=1";

	sql = App_Dba::instance()->formatSql(p,unittype);

	LISTMAP	 powerList;
	MAP_ITERATOR iter;
	powerList = App_Dba::instance()->getList(sql.c_str());

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

		if (unittype ==eGENERATOR)
		{
			// ����Ԫ���������˴���״̬
			topoByUnitId(saveid,powerid,stationid,passedNodes);
		}
		else if (unittype == eGROUNDSWITCH)
		{
			// ���˽ӵ�
			topoByGround(saveid,powerid,stationid,passedNodes);
		}
		
	}

}

void TopoBizCmd::topoEntire(sClientMsg *msg)
{

	// ���״̬����û��ispower����2�ļ�¼�����û��˵����δ����cim���ݣ���ִ�����˷���
	int count = 0;
	char* psql = "select count(*) as count from unit_status where IsPower=2 ";
	LISTMAP countList = App_Dba::instance()->getList(psql);
	if (countList.size()>0)
	{
		STRMAP countMap = countList.at(0);
		MAP_ITERATOR countIter = countMap.find("count");
		if (countIter != countMap.end())
		{
			count = str2i(countIter->second);
		}
	}

	// ���û�У���ִ�����˷���
	if (count<=0)
	{
		return;
	}

	// ��ѯ���ж���
	psql = "select id from virtual_saves ";
	LISTMAP saveList = App_Dba::instance()->getList(psql);
	for (int i = 0;i<saveList.size();i++)
	{
		STRMAP saveMap = saveList.at(i);
		MAP_ITERATOR iter = saveMap.find("id");
		if(iter != saveMap.end())
		{
			// ���˴���״̬
			topoBySaveId(iter->second,eGENERATOR);

			// ���˽ӵ�״̬
			//topoBySaveId(iter->second,eGROUNDSWITCH);
		}
	}
	PBNS::OprationMsg_Response res;
	res.set_rescode(1);
	App_ClientMgr::instance()->sendData(msg->connectId,res.SerializeAsString(),msg->type);
}

LISTMAP TopoBizCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim='%s'";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str());
	connList = App_Dba::instance()->getList(sql.c_str());
	return connList;
}

LISTMAP TopoBizCmd::getStationIdByLineId(string unitid,string stationid)
{
	LISTMAP stationList;
	char* psql = "select StationCim from related_line where UnitCim='%s' and StationCim='%s'";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str(),stationid.c_str());
	stationList = App_Dba::instance()->getList(sql.c_str());
	return stationList;
}

LISTMAP TopoBizCmd::getUnitsByConnId(string connid,string saveid)
{

	// ���⣺������ѯ�豸״̬��ʱ�򣬲��ÿ���saveidô��unit_status���У�ͬһ��unit���ܻ��ж�����¼���������¼Ϊ׼�أ�
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,"\
		"c.State,d.VolValue,d.Color from (select UnitCim from Relations where ConnCim='%s') a left join "\
		"Units b on a.UnitCim=b.CimId  left join (select UnitCim, State from unit_status "\
		"where saveId=%s) c on c.UnitCim=b.cimid left join voltages d on d.CimId=b.VolCim";

	string sql = App_Dba::instance()->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = App_Dba::instance()->getList(sql.c_str());
	return unitsList;
}

void TopoBizCmd::topoByUnitId(string saveid,string unitid,string stationid,STRMAP& passNodes)
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
			// �ж��Ƿ��Ѿ����ҹ������ӵ㣬����������������������
			if (passNodes.find(connIter->second) != passNodes.end())
			{
				continue;
			}
			else
			{
				passNodes.insert(MAPVAL(connIter->second,connIter->second));
			}


			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
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
					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 1)
							{
								// ���¸��豸����״̬Ϊ����
								updateIsElectricByUnitId(saveid,unitId,1);
							}
							else
							{
								// 6.������豸Ϊ���أ���Ϊ�Ͽ��������ٱ������豸�Ĺ����豸��
								updateIsElectricByUnitId(saveid,unitId,0);

								// ��ǲ���Ҫ����
								flag = 1;
							}
						}

					}
					else
					{
						// 5.������豸���ǿ����豸��������Ϊ���磻
						updateIsElectricByUnitId(saveid,unitId,1);
					}
				}

				// �����ǰ�豸Ϊ�����ߣ��򵽽����߹�����ϵ���У���������߹�������һ��վ��ID������״̬���н������ڸ�վ��Ϊ��Ե�Դ��
				if (etype == eLINE)
				{
					LISTMAP stationList = getStationIdByLineId(unitId,stationid);
					if (stationList.size()>0)
					{
						STRMAP stationMap = stationList.at(0);
						unitIter = stationMap.find("StationCim");
						if (unitIter != stationMap.end())
						{
							updateIsPowerByUnitId(unitId,unitIter->second,saveid);
						}
					}

				}

				// �ж��Ƿ�Ϊ��վ��
				unitIter = unitMap.find("StationId");
				//if (unitIter != unitMap.end())
				//{

				//	// ����ôα��������豸վ��ID����ʼ�豸��վ��ID����ͬ���Ҹ��豸Ϊ�����ߣ����Ǹý�����Ϊ��Ե�Դ�㣻
				//	if (str2i(unitIter->second) != str2i(stationid))
				//	{
				//		if (etype == eLine)
				//		{
				//			updateIsPowerByUnitId(unitid);
				//		}
				//	}
				//}

				string sId;
				if (unitIter != unitMap.end())
				{
					// վ��ID
					sId = unitIter->second;
				}
				
				if (flag != 1)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					topoByUnitId(saveid,unitid,sId,passNodes);
				}
				
			}

		}

	}
}

void TopoBizCmd::topoByGround(string saveid,string unitid,string stationid,STRMAP& passNodes)
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
			// �ж��Ƿ��Ѿ����ҹ������ӵ㣬����������������������
			if (passNodes.find(connIter->second) != passNodes.end())
			{
				continue;
			}
			else
			{
				passNodes.insert(MAPVAL(connIter->second,connIter->second));
			}


			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
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
					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 1)
							{
								// ���¸��豸����״̬Ϊ����
								updateIsGroundByUnitId(saveid,unitId,1);
							}
							else
							{
								// 6.������豸Ϊ���أ���Ϊ�Ͽ��������ٱ������豸�Ĺ����豸��
								updateIsGroundByUnitId(saveid,unitId,0);

								// ��ǲ���Ҫ����
								flag = 1;
							}
						}

					}
					else
					{
						// 5.������豸���ǿ����豸��������Ϊ�ӵأ�
						updateIsGroundByUnitId(saveid,unitId,1);
					}
				}

				unitIter = unitMap.find("StationId");
				
				string sId;
				if (unitIter != unitMap.end())
				{
					// վ��ID
					sId = unitIter->second;
				}

				if (flag != 1)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					topoByGround(saveid,unitid,sId,passNodes);
				}

			}

		}

	}
}

void TopoBizCmd::updateIsPowerByUnitId(string unitid,string stationid,string saveid)
{
	char* psql = "update unit_status set IsPower=1 where UnitCim='%s' and StationCim='%s' and SaveId='%s' ";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str(),stationid.c_str(),saveid.c_str());
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

void TopoBizCmd::updateIsElectricByUnitId(string saveid,string unitid,int state)
{
	char* psql = "update unit_status set IsElectric=%d where UnitCim='%s' and saveid=%s";
	string sql = App_Dba::instance()->formatSql(psql,state,unitid.c_str(),saveid.c_str());
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

void TopoBizCmd::updateIsGroundByUnitId(string saveid,string unitid,int state)
{
	char* psql = "update unit_status set IsGround=%d where UnitCim='%s' and saveid=%s";
	string sql = App_Dba::instance()->formatSql(psql,state,unitid.c_str(),saveid.c_str());
	int ret = App_Dba::instance()->execSql(sql.c_str());
	if (ret>0)
	{
		LOG->message("update :%s to IsGround  success.",unitid.c_str());
	}
	else
	{
		LOG->message("update :%s to IsGround failed.",unitid.c_str());
	}
}

string TopoBizCmd::execTopoOnBreakerChange(PBNS::OprationMsg_Request req)
{
	int saveId = req.saveid();
	string cimid = req.unitcim();
	int state = req.type();

	// �豸��cimid,����״̬ 1���磬0������
	vector<PBNS::StateBean>	rsltMap;

	// �Ѿ��������������豸ID����
	STRMAP passedNodes;

	// 1. ��ѯָ��saveid��ָ��unit��Ӧվ��������豸״̬���ϣ�
	char * psql = "select a.UnitCim,a.StationCim,IsElectric,IsPower,c.Color,a.State " \
		"from unit_status a " \
		"left join units b on a.UnitCim=b.CimId " \
		"left join voltages c on c.CimId = b.VolCim " \
		"where a.SaveId=%d and a.StationCim =(select StationCim from units where CimId='%s')";
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
				
				// �����ѹ�ȼ���ɫ
				bean.set_volcolor(COM->getVal(unitMap,"Color"));

				// �Ը��豸Ϊ���������˷���
				topoByUnitIdMem(bean,passedNodes,rsltMap,req);
			}
		}
	}

	// ���˽�����ؿͻ���
	PBNS::OprationMsg_Response res;
	res.set_optype(state);

	for (int i = 0;i<unitList.size();i++)
	{
		// ����վ���豸Ĭ�ϴ���״̬����Ϊ0
		STRMAP unitMap = unitList.at(i);
		PBNS::StateBean* pbean = res.add_devstate();

		// ����CIM
		pbean->set_cimid(COM->getVal(unitMap,"UnitCim"));

		// ���Ϊ���β����豸�����״̬����Ϊ�������״̬
		if (pbean->cimid() == cimid)
		{
			// �����豸��״̬
			pbean->set_state(state);
		}
		else
		{
			// �������ݿ��豸��״̬
			pbean->set_state(COM->getIval(unitMap,"State"));
		}

		// ����վ���豸Ĭ�ϴ���״̬����Ϊ0
		pbean->set_iselectric(0);

		// �����豸�ĵ�ѹ�ȼ�
		pbean->set_volcolor(COM->getVal(unitMap,"Color"));

	
		// �������˺����ļ���
		for (int j = 0;j<rsltMap.size();j++)
		{
			PBNS::StateBean bean = rsltMap.at(j);
			if (bean.cimid() == COM->getVal(unitMap,"UnitCim"))
			{
				pbean->set_iselectric(1);
			}
		}
	}
	
	return res.SerializeAsString();

}

void TopoBizCmd::topoOnBreakerChange(sClientMsg *msg)
{
	/*
	���������	1.�浵ID
						2.Ԫ��CIMID
						3.�Ƿ�����У��
	��������
					�ҳ���վ�豸�ڴ˴ο��ر�λ��ĵĴ���״̬.
	*/
	// �������˷����Ľ�������豸�Ĵ���״̬
	PBNS::OprationMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);
	int saveId = req.saveid();
	string cimid = req.unitcim();
	if (saveId < 0 || cimid.length()<=0)
	{
		return;
	}
	/*eDeviceType devtype = (eDeviceType)req.unittype();
	int optype = req.type();*/

	if (req.ischeck())
	{
		// ����У��
		roleCheck(msg->connectId,req);
	}
	else
	{
		string data = execTopoOnBreakerChange(req);

		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
	}
}

void TopoBizCmd::topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap,PBNS::OprationMsg_Request req)
{
	string saveid = COM->i2str(req.saveid());
	string cimid = req.unitcim();
	int objState = req.type();

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

			// �ж��Ƿ��Ѿ����ҹ������ӵ㣬����������������������
			if (passNodes.find(connIter->second) != passNodes.end())
			{
				continue;
			}
			else
			{
				passNodes.insert(MAPVAL(connIter->second,connIter->second));
			}

			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
					}
				}

				PBNS::StateBean cbean;
				cbean.set_cimid(unitIter->second);

				// ���β�ѯ��Ԫ��CIMID
				unitId = unitIter->second;

				// ��ѯԪ������
				unitIter = unitMap.find("UnitType");
				cbean.set_unittype(COM->str2i(unitIter->second));

				// �豸����
				int etype ;

				// ��־�Ƿ���Ҫ�����豸Ϊ���������ˡ�������豸Ϊ���أ���δ�Ͽ�״̬�������Ը��豸Ϊ����������
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);

					// ȡ��ѹ�ȼ���ɫ
					cbean.set_volcolor(COM->getVal(unitMap,"Color"));

					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);

							// �ж��Ƿ��ڿͻ��˲����б��У�������ÿͻ��˲����б��е�״̬
							PBNS::StateBean fbean;
							int idx = findUnitByCim(cbean.cimid(),req,fbean);
							if(idx >= 0)
							{
								state = fbean.state();
							}

							cbean.set_state(state);
							
							// �ж��Ƿ�Ϊ���β����豸������ǣ�����ݱ��β���������״̬���з���
							if (cbean.cimid() == cimid)
							{
								if (objState == 0)
								{
									cbean.set_iselectric(0);
									cbean.set_state(0);
									flag = 1;
								}
								else
								{
									cbean.set_iselectric(1);
									cbean.set_state(1);
									rsltMap.push_back(cbean);
								}
							}

							//  �Ѹÿ����豸״̬��Ϊ1�������Ը��豸Ϊ���������ˣ����Ϊ0������������豸
							else if (state == 1 ) 
							{
								// ���ִ���״̬
								cbean.set_iselectric(1);

								rsltMap.push_back(cbean);
							}
							else
							{
								cbean.set_iselectric(0);

								// ���Ϊ����Ҫ��������
								flag = 1;
							}

						}

					}
					else
					{
						// 5.������豸���ǿ����豸��������Ϊ���磻
						cbean.set_iselectric(1);

						rsltMap.push_back(cbean);
					}
				}

				// ���˲���Ҫ�������˵��豸
				if (flag != 1)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					topoByUnitIdMem(cbean,passNodes,rsltMap,req);
				}
			
			}

		}

	}
}

int TopoBizCmd::findUnitByCim(string cim,PBNS::OprationMsg_Request& req,PBNS::StateBean &bean)
{
	for (int i = 0;i<req.opdevlist_size();i++)
	{
		bean = req.opdevlist(i);
		if (bean.cimid() == cim)
		{
			return i;
		}
	}
	return -1;
}

void TopoBizCmd::updateIsElectric(string saveid,string unitcim,int state)
{
	char* psql = "update unit_status set IsElectric=%d where UnitCim = '%s' and SaveId='%s' ";
	string sql = App_Dba::instance()->formatSql(psql,state,unitcim.c_str(),saveid.c_str());
	if(App_Dba::instance()->execSql(sql.c_str()) > 0)
	{
		LOG->message("update is electric ok :%s",unitcim.c_str());
	}
	else
	{
		LOG->message("update is electric failed :%s",unitcim.c_str());
	}
}

bool TopoBizCmd::checkRuleIsUse(string cimid,int ruleid)
{
	if (cimid.length()<=0 || ruleid<= 0)
	{
		return false;
	}
	char *psql = "select count(*) as count from station_rule a " \
						"where a.IsUse=1 and a.RuleId=%d  " \
						"and a.StationCim=(select b.StationCim from units b where b.CimId='%s');";
	string sql = DBA->formatSql(psql,ruleid,cimid.c_str());
	LISTMAP ruleList = DBA->getList(sql.c_str());
	if (ruleList.size()>0)
	{
		STRMAP ruleMap = ruleList.at(0);
		int count = COM->getIval(ruleMap,"count");
		if (count>0)
		{
			return true;
		}
	}
	return false;
}

void TopoBizCmd::roleCheck(int connid,PBNS::OprationMsg_Request req)
{
	int saveid = req.saveid();
		
	string unitcim = req.unitcim();
	eDeviceType devtype = (eDeviceType)req.unittype();
	int optype = req.type();

	
	// �����Ĺ����б�
	vector<int> ruleList;

	switch (devtype)
	{
	case eBREAKER:

		// ���رպ�
		if (optype == 1)
		{
			if (check1(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_1);
			}

			if (check4(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_4);
			}

			if (check20(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_20);
			}

			if (check22(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_22);
			}

			if (check29(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_29);
			}
			if (check32(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_32);
			}
			if (check33(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_33);
			}

			if (check34(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_34);
			}

			if (check35(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_35);
			}

			if (check39(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_39);
			}
			if (check45(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_45);
			}

			if (check46(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_46);
			}
		}

		// ���ضϿ�
		else
		{

			if (check2(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_2);
			}

			if (check28(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_28);
			}
			if (check47(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_47);
			}
			if (check48(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_48);
			}
		}
		
		if (check43(saveid,unitcim,req))
		{
			ruleList.push_back(R_CHECK_43);
		}
		break;
	case eSWITCH:
		if (optype == 1)
		{
			if (check5(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_5);
			}
			if (check21(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_21);
			}
			if (check22(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_22);
			}
			if (check26(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_26);
			}

			if (check29(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_29);
			}
			if (check32(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_32);
			}
			if (check33(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_33);
			}
		}
		else
		{
			if (check12(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_12);
			}
			if (check27(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_27);
			}
			if (check28(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_28);
			}
			if (check38(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_38);
			}
		}
		if (check25(saveid,unitcim,optype,req))
		{
			ruleList.push_back(R_CHECK_25);
		}

		if (check43(saveid,unitcim,req))
		{
			ruleList.push_back(R_CHECK_43);
		}
		break;
	case eGROUNDSWITCH:
		if (check16(saveid,unitcim,req))
		{
			ruleList.push_back(R_CHECK_16);
		}
		if (optype == 1)
		{
			if (check18(saveid,unitcim,req))
			{
				ruleList.push_back(R_CHECK_18);
			}
		}
		break;
	default:
		break;
	}

	// ���û�д��������򷵻ؿͻ���ִ�б�λ����
	if (ruleList.size() == 0)
	{
		string data = execTopoOnBreakerChange(req);
		App_ClientMgr::instance()->sendData(connid,data,CMD_TOPO_BREAKER_CHANGE);
	}
	else
	{
		// �Ѵ����Ĺ��򣬷��ص��ͻ���
		sendRuleBack(connid,optype,ruleList);
	}
}

void TopoBizCmd::sendRuleBack(int connid,int optype,vector<int> ruleList)
{
	string ids;
	for (int i = 0;i<ruleList.size();i++)
	{
		ids += i2str(ruleList.at(i));
		ids += ",";
	}
	if (ids.length()>0)
	{
		ids = ids.substr(0,ids.length()-1);
	}
	else
	{
		return;
	}
	PBNS::OprationMsg_Response res;

	// ��ѯ�����б�
	char * psql = "select id, name,AlarmLevel,Description from rules where id in(%s)";
	string sql = App_Dba::instance()->formatSql(psql,ids.c_str());

	LISTMAP mapList = App_Dba::instance()->getList(sql.c_str());
	for(int i = 0;i<mapList.size();i++)
	{
		PBNS::RuleBean* bean = res.add_rulelist();

		STRMAP ruleMap = mapList.at(i);
		MAP_ITERATOR iter = ruleMap.find("id");
		if (iter != ruleMap.end())
		{
			bean->set_id(str2i(iter->second));
		}

		iter = ruleMap.find("name");
		if (iter != ruleMap.end())
		{
			bean->set_name(iter->second);
		}

		iter = ruleMap.find("AlarmLevel");
		if (iter != ruleMap.end())
		{
			bean->set_alarmlevel(iter->second);
		}

		iter = ruleMap.find("Description");
		if (iter != ruleMap.end())
		{
			bean->set_description(iter->second);
		}

	}
	res.set_optype(optype);
	string data = res.SerializeAsString();
	App_ClientMgr::instance()->sendData(connid,data,CMD_TRIGGER_RULES);
}

bool TopoBizCmd::check1(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_1))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz1 r1;
	r1.setReq(req);

	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r1.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check2(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_2))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz1 r1;
	r1.setReq(req);
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r1.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check4(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_4))
	{
		return true;
	}
	STRMAP passedNodes;
	RuleBiz4 r4;
	r4.setReq(req);
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r4.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check5(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_5))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz5 r;
	r.setReq(req);
	r.setOpcim(unitcim);
	r.setPassedNodes(passedNodes);
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	ruleMap.insert(RVAL(4,4));
	ruleMap.insert(RVAL(5,5));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check12(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_12))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz12 r;
	r.setReq(req);
	r.setOpcim(unitcim);
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
bool TopoBizCmd::check16(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_16))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz16 r;
	r.setReq(req);
	RMAP ruleMap;

	// 1������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check18(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_18))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz18 r;
	r.setReq(req);
	RMAP ruleMap;

	// 1������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check20(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_20))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz20 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check21(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_21))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz21 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);

}

bool TopoBizCmd::check22(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_22))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz22 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check25(int saveid,string unitcim,int optype,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_25))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz25 r;
	r.setReq(req);
	RMAP ruleMap;

	//3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));

	r.setOptype(optype);
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check26(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_26))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz26 r;
	r.setReq(req);
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check27(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_27))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz27 r;
	r.setReq(req);
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

// 28,29 �߼���ͬ
bool TopoBizCmd::check28(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_28))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz28 r;
	r.setReq(req);
	r.setBeginCim(unitcim);
	RMAP ruleMap;

	// 1������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check29(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_29))
	{
		return false;
	}
	return check28(saveid,unitcim,req);
}

bool TopoBizCmd::check32(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_32))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz32 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check33(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_33))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz33 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check34(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_34))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz34 r;
	r.setReq(req);
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check35(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_35))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz35 r;
	r.setReq(req);
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	ruleMap.insert(RVAL(4,4));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check38(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_38))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz38 r;
	r.setReq(req);
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
bool TopoBizCmd::check39(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_39))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz39 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check43(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_43))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz43 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check45(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_45))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz45 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
bool TopoBizCmd::check46(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_46))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz46 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
bool TopoBizCmd::check47(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_47))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz47 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check48(int saveid,string unitcim,PBNS::OprationMsg_Request req)
{
	if (!checkRuleIsUse(unitcim,R_CHECK_48))
	{
		return false;
	}
	STRMAP passedNodes;
	RuleBiz48 r;
	r.setReq(req);
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
