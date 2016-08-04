#include "topobase.h"

void TopoBase::setReq(PBNS::OprationMsg_Request req)
{
	m_req = req;
}

void TopoBase::setPassedNodes(STRMAP &passedNodes)
{
	m_passedNodes = passedNodes;
}

bool TopoBase::topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap)
{
	PBNS::StateBean beginBean = getUnitByCim(saveid,unitcim);

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
			// 判断是否已经查找过的连接点，如果是则跳出，不是则加入
			if (passNodes.find(connIter->second) != passNodes.end())
			{
				continue;
			}
			else
			{
				passNodes.insert(MAPVAL(connIter->second,connIter->second));
			}

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
					else
					{
						passNodes.insert(MAPVAL(unitIter->second,unitIter->second));
					}
				}

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 判断是否是本次操作的设备，如果是，则跳过
				if (unitcim == unitId)
				{
					continue;
				}

				// 本轮拓扑的业务处理，具体子类实现
				int topoRst = topoBiz(saveid,unitId,ruleMap,beginBean.stationcim());
				bool ret = false;

				if (topoRst == 1)
				{
					// 递归，以该元件为起点进行重新遍历
					ret = topoByUnit(saveid,unitId,passNodes,ruleMap);
				}
				// 判断是否直接退出
				else if (topoRst == 2)
				{
					return false;
				}
				// 跳过该连接点下的所有设备
				else if (topoRst == 3)
				{
					break;
				}
				else if (topoRst == 4)
				{
					// 规则被触发
					return true;
				}

			}

		}

	}

	// 判断是否规则触发
	if (ruleMap.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

PBNS::StateBean TopoBase::getUnitByCim(int saveid,string unitcim)
{
	
	PBNS::StateBean bean;
	
	if (unitcim.length()<=0)
	{
		LOG->warn("参数错误，cim不可以为空");
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
		LOG->warn("数据错误，同一存档下面有多个相同cim的元件:%s",unitcim.c_str());
	}

	// 判断是否在客户端操作列表中，如果在，则用客户端状态
	int flag = 0;
	PBNS::StateBean fbean;
	for (int i = 0;i<m_req.opdevlist_size();i++)
	{
		fbean = m_req.opdevlist(i);
		if (fbean.cimid() == unitcim)
		{
			flag = 1;
			break;
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
	//该连接点连接有变压器时，对应的UnitCim是winding的CimId，不是变压器的CimId，需特殊处理
	//先确定连接点是否连接有变压器
	LISTMAP transList,dataList;

	char* ptranSql = "select a.UnitCim, b.CimId from relations a left join units b on a.UnitCim=b.CimId where ConnCim='%s'";
	string tranSql = App_Dba::instance()->formatSql(ptranSql,connid.c_str());
	dataList = App_Dba::instance()->getList(tranSql.c_str());
	for(int i = 0;i<dataList.size();i++)
	{
		STRMAP unitMap = dataList.at(i);
		MAP_ITERATOR unitIter = unitMap.find("CimId");
		string unitId = unitIter->second;
		if(unitId.empty())//如果连接有winding，则CimId为空，UnitCim为winding的CimId，不为空
		{
			unitId = unitMap.find("UnitCim")->second;
			if(unitId.empty())//如果UnitCim也为空，数据有误不处理
				break;

			//获取变压器
			char* pdataSql = "select CimId as id,UnitType,StationCim as StationId"\
				" from units where "\
				"CimId=(select UnitCim from windings where CimId='%s')";
			string dataSql = App_Dba::instance()->formatSql(pdataSql,unitId.c_str());
			transList = App_Dba::instance()->getList(dataSql.c_str());
			break;
		}
	}
	

	LISTMAP unitsList ;
	char* psql = "select b.CimId as id,b.UnitType,b.StationCim as StationId,"\
		"c.State,d.VolValue from (select UnitCim from Relations where ConnCim='%s') a left join "\
		"Units b on a.UnitCim=b.CimId  left join (select UnitCim, State from unit_status "\
		"where saveId=%s) c on c.UnitCim=b.cimid left join voltages d on d.CimId=b.VolCim order by b.UnitType asc ";

	string sql = DBA->formatSql(psql,connid.c_str(),saveid.c_str());
	unitsList = DBA->getList(sql.c_str());

	//unitsList获取的变压器属性将为空，需将变压器加入集合
	if(transList.size() > 0)
	{
		unitsList.push_back(transList[0]);
	}

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