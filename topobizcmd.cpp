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
	case CMD_CHECK_PASS:					// ����У��ͨ��
		topoOnBreakerChange(msg);
		break;
	default:
		break;
	}
}

void TopoBizCmd::topoBySaveId(string saveid,int unittype)
{
	// �Ѿ��������������豸ID����
	STRMAP passedNodes;

	// 1.��ѯ���з������豸
	string sql ;
	char * p = "select Id,StationCim as StationId from units where UnitType=%d";
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

void TopoBizCmd::topoEntire()
{

	// ���״̬����û��ispower����2�ļ�¼������У���ִ�����˷���
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

	// ����У���ִ�����˷���
	if (count>0)
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
			topoBySaveId(iter->second,eGROUNDSWITCH);
		}
	}
}

LISTMAP TopoBizCmd::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str());
	connList = App_Dba::instance()->getList(sql.c_str());
	return connList;
}

LISTMAP TopoBizCmd::getStationIdByLineId(string unitid,string stationid)
{
	LISTMAP stationList;
	char* psql = "select StationCim from related_line where UnitCim=%s and StationCim=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid.c_str(),stationid.c_str());
	stationList = App_Dba::instance()->getList(sql.c_str());
	return stationList;
}

LISTMAP TopoBizCmd::getUnitsByConnId(string connid,string saveid)
{

	// ���⣺������ѯ�豸״̬��ʱ�򣬲��ÿ���saveidô��unit_status���У�ͬһ��unit���ܻ��ж�����¼���������¼Ϊ׼�أ�
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,c.State,d.VolValue " \
		"from Relations a left join Units b on a.UnitCim=b.CimId  "\
		"left join unit_status c on b.UnitCim=b.cimid " \
		"left join voltages d on d.CimId=b.VolCim " \
		"where a.ConnCim=%s and c.saveid=%s";
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
	char* psql = "update unit_status set IsPower=1 where UnitCim=%s and StationCim=%s and SaveId=%s";
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
	char* psql = "update unit_status set IsElectric=%d where UnitCim=%s and saveid=%s";
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
	char* psql = "update unit_status set IsGround=%d where UnitCim=%s and saveid=%s";
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

string TopoBizCmd::execTopoOnBreakerChange(int saveId,string cimid,int state)
{
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
					bean.set_volvalue(iter->second);
				}

				// �Ը��豸Ϊ���������˷���
				topoByUnitIdMem(bean,i2str(saveId),cimid,passedNodes,rsltMap);
			}
		}
	}

	// ���˽�����ؿͻ���
	PBNS::OprationMsg_Response res;
	vector<PBNS::StateBean>::iterator iter = rsltMap.begin();
	for (int i=0;i<rsltMap.size();i++)
	{
		PBNS::StateBean* bean = res.add_devstate();
		
		// ��ѯ���豸�ĵ�ѹ�ȼ�
		bean->CopyFrom(rsltMap.at(i));
	}
	// ���ر��β�����Ŀ���״̬�����ڿͻ���ͼ�α�λ
	res.set_optype(state);

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
	eDeviceType devtype = (eDeviceType)req.unittype();
	int optype = req.type();

	if (req.ischeck())
	{
		// ����У��
		roleCheck(msg->connectId,saveId,cimid,devtype,optype);
	}
	else
	{
		string data = execTopoOnBreakerChange(saveId,cimid,optype);

		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
	}
}

void TopoBizCmd::topoByUnitIdMem(PBNS::StateBean bean,string saveid,string cimid,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap)
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

					// ȡ��ѹ�ȼ�ֵ
					unitIter = unitMap.find("VolValue");
					if (unitIter != unitMap.end())
					{
						cbean.set_volvalue(unitIter->second);
					}

					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.������豸ΪΪ���أ���բ���պϼ�Ϊ���磬����Ϊ�����磻
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							cbean.set_state(state);

							if (cbean.cimid() == cimid && state == 0)
							{
								cbean.set_state(1);
								cbean.set_iselectric(1);
								rsltMap.push_back(cbean);
							}
							else if (state == 1)
							{
								//  �Ѹÿ����豸״̬��Ϊ1�������Ը��豸Ϊ���������ˣ����Ϊ0������������豸
								cbean.set_iselectric(1);
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
					topoByUnitIdMem(cbean,saveid,cimid,passNodes,rsltMap);
				}
			
			}

		}

	}
}

void TopoBizCmd::roleCheck(int connid,int saveid,string unitcim,eDeviceType devtype,int optype)
{
	// �����Ĺ����б�
	vector<int> ruleList;

	switch (devtype)
	{
	case eBREAKER:

		// ���رպ�
		if (optype == 1)
		{
			if (check1(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_1);
			}

			// ����4�������������Σ����򲻴���������������Σ����򴥷����������ֵ�͹���һ�෴
			if (!check4(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_4);
			}

			if (check20(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_20);
			}

			if (check22(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_22);
			}

			if (check29(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_29);
			}
			if (check32(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_32);
			}
			if (check33(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_33);
			}

			if (check34(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_34);
			}

			if (check35(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_35);
			}
		}

		// ���ضϿ�
		else
		{

			if (check2(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_2);
			}

			if (check28(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_28);
			}
		}
		

		break;
	case eSWITCH:
		if (optype == 1)
		{
			if (check5(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_5);
			}
			if (check21(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_21);
			}
			if (check22(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_22);
			}
			if (check26(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_26);
			}

			if (check29(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_29);
			}
			if (check32(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_32);
			}
			if (check33(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_33);
			}
		}
		else
		{
			if (check12(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_12);
			}
			if (check27(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_27);
			}
			if (check28(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_28);
			}
			if (check38(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_38);
			}
		}
		if (check25(saveid,unitcim,optype))
		{
			ruleList.push_back(R_CHECK_25);
		}
		break;
	case eGROUNDSWITCH:
		if (check16(saveid,unitcim))
		{
			ruleList.push_back(R_CHECK_16);
		}
		if (optype == 1)
		{
			if (check18(saveid,unitcim))
			{
				ruleList.push_back(R_CHECK_18);
			}
		}
		break;
	default:
		break;
	}

	

	// ...

	// �Ѵ����Ĺ��򣬷��ص��ͻ���
	sendRuleBack(connid,optype,ruleList);

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
	string sql = App_Dba::instance()->formatSql(psql,ids);

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

bool TopoBizCmd::check1(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz1 r1;
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r1.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check2(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz1 r1;
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r1.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check4(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz4 r4;
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r4.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check5(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz5 r;
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	ruleMap.insert(RVAL(4,4));
	ruleMap.insert(RVAL(5,5));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check12(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz12 r;
	RMAP ruleMap;

	// ��������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
bool TopoBizCmd::check16(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz16 r;
	RMAP ruleMap;

	// 1������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check18(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz18 r;
	RMAP ruleMap;

	// 1������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check20(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz20 r;
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check21(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz21 r;
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);

}

bool TopoBizCmd::check22(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz22 r;
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check25(int saveid,string unitcim,int optype)
{
	STRMAP passedNodes;
	RuleBiz25 r;
	RMAP ruleMap;

	// 5������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	ruleMap.insert(RVAL(4,4));
	ruleMap.insert(RVAL(5,5));
	r.setOptype(optype);
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check26(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz26 r;
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check27(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz27 r;
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

// 28,29 �߼���ͬ
bool TopoBizCmd::check28(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz28 r;
	r.setBeginCim(unitcim);
	RMAP ruleMap;

	// 1������
	ruleMap.insert(RVAL(1,1));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check29(int saveid,string unitcim)
{
	return check28(saveid,unitcim);
}

bool TopoBizCmd::check32(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz32 r;
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check33(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz33 r;
	RMAP ruleMap;

	// 2������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check34(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz34 r;
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check35(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz35 r;
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	ruleMap.insert(RVAL(4,4));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

bool TopoBizCmd::check38(int saveid,string unitcim)
{
	STRMAP passedNodes;
	RuleBiz38 r;
	RMAP ruleMap;

	// 3������
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}