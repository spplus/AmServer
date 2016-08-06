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

bool TopoBase::topoOnBreakerChange(PBNS::OprationMsg_Request req)
{
	int saveId = req.saveid();
	string cimid = req.unitcim();
	int state = req.type();

	// 设备的cimid,带电状态 1带电，0不带电
	vector<PBNS::StateBean>	rsltMap;

	// 已经做个起点分析的设备ID集合
	STRMAP passedNodes;

	// 1. 查询指定saveid，指定unit对应站点下面的设备状态集合；
	char * psql = "select a.UnitCim,a.StationCim,IsElectric,IsPower,a.State,b.UnitType " \
		"from unit_status a " \
		"left join units b on a.UnitCim=b.CimId " \
		"where a.SaveId=%d and a.StationCim =(select StationCim from units where CimId='%s')";
	string sql = App_Dba::instance()->formatSql(psql,saveId,cimid.c_str());

	LISTMAP unitList = App_Dba::instance()->getList(sql.c_str());

	// 2.对以上结果中的ispower的值等于1或者2的记录作为起点进行拓扑分析。
	for (int i = 0;i<unitList.size();i++)
	{
		STRMAP unitMap = unitList.at(i);
		MAP_ITERATOR iter = unitMap.find("IsPower");

		// 相对电源点或绝对电源电记录
		if (iter != unitMap.end() && (COM->str2i(iter->second) == 1 || COM->str2i(iter->second) == 2))
		{
			iter = unitMap.find("UnitCim");
			if (iter != unitMap.end())
			{
				PBNS::StateBean bean;
				bean.set_cimid(iter->second);

				//保存带电状态
				bean.set_iselectric(1);

				//保存站点cim
				bean.set_stationcim(COM->getVal(unitMap,"StationCim"));

				//将找到的电源点加入带电集合
				rsltMap.push_back(bean);

				// 以该设备为起点进行拓扑分析
				topoByUnitIdMem(bean,passedNodes,rsltMap,req);
			}
		}
	}

	for (int i = 0;i<unitList.size();i++)
	{
		// 把整站的设备默认带电状态设置为0
		STRMAP unitMap = unitList.at(i);
		PBNS::StateBean pbean;

		// 保存CIM
		pbean.set_cimid(COM->getVal(unitMap,"UnitCim"));

		// 保存设备类型
		pbean.set_unittype(COM->getIval(unitMap,"UnitType"));

		// 把整站的设备默认带电状态设置为0
		pbean.set_iselectric(0);
	
		// 遍历拓扑后带电的集合
		for (int j = 0;j<rsltMap.size();j++)
		{
			PBNS::StateBean bean = rsltMap.at(j);
			if (bean.cimid() == COM->getVal(unitMap,"UnitCim"))
			{
				pbean.set_iselectric(1);
			}
		}

		//设置全局预期集合
		expectMap.push_back(pbean);
	}

	if(expectMap.size() > 0)
		return true;
	else
		return false;
}

void TopoBase::topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap,PBNS::OprationMsg_Request req)
{
	string saveid = COM->i2str(req.saveid());
	string cimid = req.unitcim();
	int objState = req.type();

	string unitid = bean.cimid();

	// 把当前元件加入到已分析列表
	passNodes.insert(MAPVAL(unitid,unitid));

	// 2.根据元件ID，查找对应的连接点（可能是两个）
	LISTMAP connIds = getConnIdByUnitsId(unitid);

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
			LISTMAP unitsList = getUnitsByConnId(connIter->second,saveid);

			// 遍历该设备集合
			for (int k = 0;k<unitsList.size();k++)
			{
				STRMAP  unitMap = unitsList.at(k);

				//判断是否为当前站点的元件，不是则跳过
				MAP_ITERATOR unitIter = unitMap.find("StationId");
				if (unitIter != unitMap.end())
				{
					if(unitIter->second != bean.stationcim())
						continue;
				}

				unitIter = unitMap.find("id");
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

				PBNS::StateBean cbean;
				cbean.set_cimid(unitIter->second);

				// 本次查询的元件CIMID
				unitId = unitIter->second;

				// 查询元件类型
				unitIter = unitMap.find("UnitType");
				cbean.set_unittype(COM->str2i(unitIter->second));

				// 设备类型
				int etype ;

				// 标志是否需要按此设备为起点进行拓扑。如果该设备为开关，且未断开状态，则不用以该设备为起点进行拓扑
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = COM->str2i(unitIter->second);

					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = COM->str2i(unitIter->second);

							// 判断是否在客户端操作列表中，如果在用客户端操作列表中的状态
							PBNS::StateBean fbean;
							int idx = findUnitByCim(cbean.cimid(),req,fbean);
							if(idx >= 0)
							{
								state = fbean.state();
							}

							cbean.set_state(state);
							
							// 判断是否为本次操作设备，如果是，则根据本次操作的最终状态进行分析
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

							//  把该开关设备状态变为1，继续以该设备为起点进行拓扑，如果为0，则跳过这个设备
							else if (state == 1 ) 
							{
								// 保持带电状态
								cbean.set_iselectric(1);

								rsltMap.push_back(cbean);
							}
							else
							{
								cbean.set_iselectric(0);

								// 标记为不需要进行拓扑
								flag = 1;
							}

						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为带电；
						cbean.set_iselectric(1);

						rsltMap.push_back(cbean);
					}
				}

				// 过滤不需要进行拓扑的设备
				if (flag != 1)
				{
					// 查询元件站点cim，下一次递归需要用到
					unitIter = unitMap.find("StationId");
					cbean.set_stationcim(unitIter->second);

					// 递归，以该元件为起点进行重新遍历
					topoByUnitIdMem(cbean,passNodes,rsltMap,req);
				}
			
			}

		}

	}
}

int TopoBase::findUnitByCim(string cim,PBNS::OprationMsg_Request& req,PBNS::StateBean &bean)
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

void TopoBase::checkIsCarSwitch(PBNS::OprationMsg_Request& req)
{
	// 判断是否 为开关，如果不是则返回
	if (req.unittype() == eBREAKER)
	{
		// 标记查找到几个刀闸
		int flag = 0;

		string unitcim = "";

		// 保存找到的刀闸cim集合
		vector<string> cimStack;

		// 查询该元件两端的连接点集合
		LISTMAP connList = getConnIdByUnitsId(req.unitcim());

		for (int i = 0;i<connList.size();i++)
		{

			STRMAP connMap = connList.at(i);

			// 标记是否有母线
			bool isBus = false;

			// 本连接点找到的刀闸数量
			int curNodeSwitchCount = 0;

			// 根据连接点查找对应的设备集合
			string connId = COM->getVal(connMap,"connId");

			LISTMAP unitList = getUnitsByConnId(connId,COM->i2str(req.saveid()));

			for (int j = 0;j<unitList.size();j++)
			{
				STRMAP unitMap = unitList.at(j);
				
				int unitType = COM->getIval(unitMap,"UnitType");
				if (unitType == eSWITCH)
				{
					//flag++;
					curNodeSwitchCount++;
					unitcim = COM->getVal(unitMap,"id");
					cimStack.push_back(unitcim);
				}
				else if (unitType == eBUS)
				{
					isBus = true;
					break;
				}

			}

			// 如果找到母线，则该连接点被忽略
			if (isBus)
			{
				
				// 把该连接点找到的刀闸从堆栈中弹出
				for (int k=0;k<curNodeSwitchCount;k++)
				{
					cimStack.pop_back();
				}

				curNodeSwitchCount = 0;
			}
			else
			{
				flag += curNodeSwitchCount;
			}
		}

		// 如果只找到一个刀闸，则标识为推车，把该刀闸的cim加入到已操作列表中，并置为闭合
		if (flag == 1 && cimStack.size() == 1)
		{
			unitcim = cimStack.at(0);
			PBNS::StateBean *bean = req.add_opdevlist();
			bean->set_unittype(eSWITCH);
			bean->set_cimid(unitcim);
			bean->set_state(1);
			bean->set_isop(1);
		}
	}
}