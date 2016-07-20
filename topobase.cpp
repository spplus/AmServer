#include "topobase.h"

void TopoBase::setReq(PBNS::OprationMsg_Request req)
{
	m_req = req;
}

bool TopoBase::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
					}
				}

				// ���β�ѯ��Ԫ��CIMID
				unitId = unitIter->second;

				// �������˵�ҵ������������ʵ��
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
				bool ret = false;

				if (topoRst == 1)
				{
					// �ݹ飬�Ը�Ԫ��Ϊ���������±���
					ret = topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
				// �ж��Ƿ�ֱ���˳�
				else if (topoRst == 2)
				{
					return false;
				}
				// ���������ӵ��µ������豸
				else if (topoRst == 3)
				{
					break;
				}
				else if (topoRst == 4)
				{
					// ���򱻴���
					return true;
				}

				// �ж��Ƿ���򴥷�
				if (ruleMap.size() == 0)
				{
					return true;
				}
				else
				{
					return ret;
				}
			}

		}

	}
	return false;
}

PBNS::StateBean TopoBase::getUnitByCim(int saveid,string unitcim)
{
	
	PBNS::StateBean bean;
	
	if (unitcim.length()<=0)
	{
		LOG->warn("��������cim������Ϊ��");
		return bean;
	}

	bean.set_cimid(unitcim);

	char* psql = "select b.State,a.UnitType,a.StationCim,b.IsElectric,b.IsPower,b.IsBoard " \
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

		bean.set_isboard(COM->getIval(unitMap,"IsBoard"));
		
	}
	else
	{
		LOG->warn("���ݴ���ͬһ�浵�����ж����ͬcim��Ԫ��:%s",unitcim.c_str());
	}

	// �ж��Ƿ��ڿͻ��˲����б��У�����ڣ����ÿͻ���״̬
	int flag = 0;
	PBNS::StateBean fbean;
	for (int i = 0;i<m_req.opdevlist_size();i++)
	{
		fbean = m_req.opdevlist(i);
		if (fbean.cimid() == unitcim)
		{
			flag = 1;
		}
	}

	if (flag == 1)
	{
		bean.set_state(fbean.state());
	}
	

	return bean;
}

LISTMAP TopoBase::getUnitsByConnId(string connid,string saveid)
{

	// 
	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,"\
		"c.State,d.VolValue from (select UnitCim from Relations where ConnCim='%s') a left join "\
		"Units b on a.UnitCim=b.CimId  left join (select UnitCim, State from unit_status "\
		"where saveId=%s) c on c.UnitCim=b.cimid left join voltages d on d.CimId=b.VolCim order by b.UnitType asc ";

	string sql = DBA->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = DBA->getList(sql.c_str());
	return unitsList;
}


LISTMAP TopoBase::getConnIdByUnitsId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select ConnCim as connId from Relations where UnitCim='%s'";
	string sql = DBA->formatSql(psql,unitid.c_str());
	connList = DBA->getList(sql.c_str());
	return connList;
}

void TopoBase::setOpcim(string cimid)
{
	m_opcim = cimid;
}