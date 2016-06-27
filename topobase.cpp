#include "topobase.h"


bool TopoBase::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitcim,unitcim));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitcim);

	// 3.根据连接点ID在连接关系表查询关联的设备
	for (int j = 0;j<connIds.size();j++)
	{
		STRMAP connMap = connIds.at(j);
		MAP_ITERATOR connIter = connMap.find("connId");
		if (connIter != connMap.end())
		{
			// 根据连接点，查找该连接点关联的设备集合
			LISTMAP unitsList = getUnitsByConnId(connIter->second,COM->i2str(saveid));

			// 遍历该设备集合
			for (int k = 0;k<unitsList.size();k++)
			{
				STRMAP  unitMap = unitsList.at(k);
				MAP_ITERATOR unitIter = unitMap.find("id");
				string unitId ;
				if (unitIter != unitMap.end())
				{
					// 判断是否已经做为起始设备进行搜索，如果是则跳过
					if (passNodes.find(unitIter->second) != passNodes.end())
					{
						continue;
					}
				}

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 本轮拓扑的业务处理，具体子类实现
				if (topoBiz(saveid,unitId,ruleMap))
				{
					// 递归，以该元件为起点进行重新遍历
					topoByUnit(saveid,unitId,passNodes,ruleMap);
				}

				// 判断是否规则触发
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
		LOG->warn("数据错误，同一存档下面有多个相同cim的元件:%s",unitcim.c_str());
	}

	return bean;
}

LISTMAP TopoBase::getUnitsByConnId(string connid,string saveid)
{

	// 问题：关联查询设备状态的时候，不用考虑saveid么？unit_status表中，同一个unit可能会有多条记录，以哪天记录为准呢？
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
