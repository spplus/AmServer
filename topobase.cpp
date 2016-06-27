#include "topobase.h"


bool TopoBase::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	// �ѵ�ǰԪ�����뵽�ѷ����б�
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.����Ԫ��ID�����Ҷ�Ӧ�����ӵ㣨������������
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.�������ӵ�ID�����ӹ�ϵ���ѯ�������豸
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// �������ӵ㣬���Ҹ����ӵ�������豸����
			LISTMAP unitsList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

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

				// �������˵�ҵ������������ʵ��
				if (topoBiz(saveid,unitId,ruleMap))
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}

				// �ж��Ƿ���򴥷�
				if (ruleMap.size() == 0)
				{
					return true;
				}
			}

		}

	}
	return false;
}

PBNS::StateBean TopoBase::getUnitByCim(int saveid,string unitcim)
{
	PBNS::StateBean bean;
	char* psql = "select b.State,a.UnitType,a.StationCim,b.IsElectric " \
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

	}
	else
	{
		LOG->warn("���ݴ���ͬһ�浵�����ж����ͬcim��Ԫ��:%s",unitcim.c_str());
	}

	return bean;
}

LISTMAP TopoBase::getUnitsByConnId(string connid,string saveid)
{

	// ���⣺������ѯ�豸״̬��ʱ�򣬲��ÿ���saveidô��unit_status���У�ͬһ��unit���ܻ��ж�����¼���������¼Ϊ׼�أ�
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,c.State,d.VolValue " \
		"from Relations a left join Units b on a.UnitCim=b.CimId  "\
		"left join unit_status c on b.UnitCim=b.cimid " \
		"left join voltages d on d.CimId=b.VolCim " \
		"where a.ConnCim=%s and c.saveid=%s";
	string sql = DBA->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = DBA->getList(sql.c_str());
	return unitsList;
}


LISTMAP TopoBase::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim=%s";
	string sql = DBA->formatSql(psql,unitid.c_str());
	connList = DBA->getList(sql.c_str());
	return connList;
}
