#include "topobizcmd.h"

void TopoBizCmd::exec(sClientMsg* msg)
{
	switch (msg->type)
	{
	case CMD_TOPO_ENTIRE:		// ��վ����
		topoEntire();
		break;
	}
}

void TopoBizCmd::topoEntire()
{
	// �Ѿ��������������豸ID����
	STRMAP passedNodes;


	// 1.��ѯ���з������豸
	string sql ;
	char * p = "select Id,StationId from units where UnitType=5";
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
	char* psql = "select connId from Relations where UnitId=%s";
	string sql = App_Dba::instance()->formatSql(psql,unitid);
	connList = App_Dba::instance()->getList(sql.c_str());
	return connList;
}


LISTMAP TopoBizCmd::getUnitsByConnId(string connid)
{
	LISTMAP unitsList ;
	char* psql = "select b.id,b.UnitType,b.StationId from Relations a left join Units b on a.UnitId=b.id  where a.ConnId=%s";
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

				// ���β�ѯ��Ԫ��ID
				unitId = unitIter->second;

				// ��ѯԪ������
				unitIter = unitMap.find("UnitType");

				int etype ;
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

				// �ݹ飬�Ը�Ԫ��Ϊ���������±���
				topoByUnitId(unitid,sId,passNodes);
			}

		}

	}
}

void TopoBizCmd::updateIsPowerByUnitId(string unitid)
{
	char* psql = "update UnitStatus set IsPower=1 where UnitId=%s";
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
	char* psql = "update UnitStatus set IsElectric=%d where UnitId=%s";
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