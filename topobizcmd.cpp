#include "topobizcmd.h"
#include "confmgr.h"
#include "cimloader.h"
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
	case CMD_TOPO_ENTIRE:		// 整站拓扑
		topoEntire(msg);
		break;
	case CMD_TOPO_BREAKER_CHANGE:		// 开关变位
		topoOnBreakerChange(msg);
		break;
	case CMD_CHECK_PASS:					// 规则校验通过
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
	// 已经做过起点分析的设备ID集合
	STRMAP passedNodes;

	// 1.查询所有发电机设备
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
			// 根据元件进行拓扑带电状态
			topoByUnitId(saveid,powerid,stationid,passedNodes);
		}
		else if (unittype == eGROUNDSWITCH)
		{
			// 拓扑接地
			topoByGround(saveid,powerid,stationid,passedNodes);
		}
		
	}

}

void TopoBizCmd::loadCim()
{
	// 判断是否有更新
	char *psql = "select id, count(*) as count from system_config where isnew=1";
	LISTMAP countList = DBA->getList(psql);
	if (countList.size() > 0)
	{
		STRMAP countMap = countList.at(0);
		int count = COM->getIval(countMap,"count");
		if (count > 0)
		{
			int factype = COM->str2i(App_Config::instance()->getValue(CIM_ROOT,CIM_PRODUCTID));
			CimLoader cloader;
			string cimname = App_Config::instance()->getValue(CIM_ROOT,CIM_WORKPATH);
			cimname += "/";
			cimname +=  App_Config::instance()->getValue(CIM_ROOT,CIM_NAME);
			
			// 如果导库成功，则更新系统标志位0
			if(cloader.Load(cimname,factype) == 0)
			{
				psql = "update system_config set IsNew=0,ModifyTime=now() where id=%d";
				string sql = DBA->formatSql(psql,COM->getIval(countMap,"id"));
				if (DBA->execSql(sql.c_str()) <=0 )
				{
					LOG->warn("更新CIM文件变更标志失败");
				}
				else
				{
					LOG->message("更新CIM文件变更标志成功");
				}
			}
		}
	}
}

void TopoBizCmd::topoEntire(sClientMsg *msg)
{

	// 导CIM
	loadCim();

	// 检查状态表有没有ispower等于2的记录，如果没有说明尚未导入cim数据，则不执行拓扑分析
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

	// 如果没有，则不执行拓扑分析
	if (count<=0)
	{
		return;
	}

	// 查询所有断面
	psql = "select id from virtual_saves ";
	LISTMAP saveList = App_Dba::instance()->getList(psql);
	for (int i = 0;i<saveList.size();i++)
	{
		STRMAP saveMap = saveList.at(i);
		MAP_ITERATOR iter = saveMap.find("id");
		if(iter != saveMap.end())
		{
			// 拓扑带电状态
			topoBySaveId(iter->second,eGENERATOR);

			// 拓扑接地状态
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

LISTMAP TopoBizCmd::getConnIdByTransId(string unitid)
{
	LISTMAP connList ;
	char* psql = "select a.ConnCim as connId from Relations a left join windings b on a.UnitCim = b.CimId where b.UnitCim='%s'";
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
	LISTMAP transList,unitsList;

	//该连接点连接有变压器时，对应的UnitCim是winding的CimId，不是变压器的CimId，需特殊处理
	//先确定连接点是否连接有变压器

	char* ptranSql = "select a.UnitCim, b.CimId from relations a left join units b on a.UnitCim=b.CimId where ConnCim='%s'";
	string tranSql = App_Dba::instance()->formatSql(ptranSql,connid.c_str());
	transList = App_Dba::instance()->getList(tranSql.c_str());
	for(int i = 0;i<transList.size();i++)
	{
		STRMAP unitMap = transList.at(i);
		MAP_ITERATOR unitIter = unitMap.find("CimId");
		string unitId = unitIter->second;
		if(unitId.empty())//如果连接有winding，则CimId为空，UnitCim为winding的CimId，不为空
		{
			unitId = unitMap.find("UnitCim")->second;
			if(unitId.empty())//如果UnitCim也为空，数据有误不处理
				break;

			//设置变压器cim
			char* pdataSql = "select a.CimId,a.StationCim,"\
				"c.Color from units a left join voltages c on c.CimId=a.VolCim where "\
				"a.CimId=(select UnitCim from windings where CimId='%s')";
			string dataSql = App_Dba::instance()->formatSql(pdataSql,unitId.c_str());
			LISTMAP dataList = App_Dba::instance()->getList(dataSql.c_str());
			if(dataList.size() > 0)
			{
				transBean.set_cimid(dataList.at(0).find("CimId")->second);
				
				// 保存电压等级颜色
				transBean.set_volcolor(dataList.at(0).find("Color")->second);

				//保存带电状态
				transBean.set_iselectric(1);

				//保存站点cim
				transBean.set_stationcim(dataList.at(0).find("StationCim")->second);
			}

			transList.clear();
			//通过winding查询变压器所有winding对应的连接点，对应的所有元件集合
			char* ptranListSql = "select d.CimId as id,d.UnitType,d.StationCim as StationId,e.State,"\
				"f.VolValue,f.Color from (select x.UnitCim from Relations x right join "\
"(select a.ConnCim from relations a right join (select CimId from windings where UnitCim = "\
"(select UnitCim from windings where CimId='%s')) b on a.UnitCim=b.CimId) y "\
"on x.ConnCim=y.ConnCim) c left join Units d on c.UnitCim=d.CimId left join "\
"(select UnitCim, State from unit_status where saveId=%s) e on e.UnitCim=d.CimId left join "\
"voltages f on f.CimId=d.VolCim";
			string tranListSql = App_Dba::instance()->formatSql(ptranListSql,unitId.c_str(),saveid.c_str());
			transList =App_Dba::instance()->getList(tranListSql.c_str());
			return transList;
		}
	}


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

				// 查询元件类型
				unitIter = unitMap.find("UnitType");

				// 设备类型
				int etype ;

				// 标记时候需要拓扑 0 需要拓扑，1 不需要拓扑
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);
					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 1)
							{
								// 更新该设备带电状态为带电
								updateIsElectricByUnitId(saveid,unitId,1);
							}
							else
							{
								// 6.如果该设备为开关，且为断开，则不用再遍历该设备的关联设备；
								updateIsElectricByUnitId(saveid,unitId,0);

								// 标记不需要拓扑
								flag = 1;
							}
						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为带电；
						updateIsElectricByUnitId(saveid,unitId,1);
					}
				}

				// 如果当前设备为进出线，则到进出线关联关系表中，查出进出线关联的另一端站点ID，更新状态表中进出线在该站点为相对电源点
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

				// 判断是否为跨站点
				unitIter = unitMap.find("StationId");
				//if (unitIter != unitMap.end())
				//{

				//	// 如果该次遍历出的设备站点ID与起始设备的站点ID不相同，且该设备为进出线，则标记该进出线为相对电源点；
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
					// 站点ID
					sId = unitIter->second;
				}
				
				if (flag != 1)
				{
					// 递归，以该元件为起点进行重新遍历
					topoByUnitId(saveid,unitid,sId,passNodes);
				}
				
			}

		}

	}
}

void TopoBizCmd::topoByGround(string saveid,string unitid,string stationid,STRMAP& passNodes)
{
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

				// 查询元件类型
				unitIter = unitMap.find("UnitType");

				// 设备类型
				int etype ;

				// 标记时候需要拓扑 0 需要拓扑，1 不需要拓扑
				int flag = 0;

				if (unitIter != unitMap.end())
				{
					etype = str2i(unitIter->second);
					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);
							if (state == 1)
							{
								// 更新该设备带电状态为带电
								updateIsGroundByUnitId(saveid,unitId,1);
							}
							else
							{
								// 6.如果该设备为开关，且为断开，则不用再遍历该设备的关联设备；
								updateIsGroundByUnitId(saveid,unitId,0);

								// 标记不需要拓扑
								flag = 1;
							}
						}

					}
					else
					{
						// 5.如果该设备不是开关设备，则设置为接地；
						updateIsGroundByUnitId(saveid,unitId,1);
					}
				}

				unitIter = unitMap.find("StationId");
				
				string sId;
				if (unitIter != unitMap.end())
				{
					// 站点ID
					sId = unitIter->second;
				}

				if (flag != 1)
				{
					// 递归，以该元件为起点进行重新遍历
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

	// 设备的cimid,带电状态 1带电，0不带电
	vector<PBNS::StateBean>	rsltMap;

	// 已经做个起点分析的设备ID集合
	STRMAP passedNodes;

	// 1. 查询指定saveid，指定unit对应站点下面的设备状态集合；
	char * psql = "select a.UnitCim,a.StationCim,IsElectric,IsPower,c.Color,a.State,b.UnitType " \
		"from unit_status a " \
		"left join units b on a.UnitCim=b.CimId " \
		"left join voltages c on c.CimId = b.VolCim " \
		"where a.SaveId=%d and a.StationCim =(select StationCim from units where CimId='%s')";
	string sql = App_Dba::instance()->formatSql(psql,saveId,cimid.c_str());

	LISTMAP unitList = App_Dba::instance()->getList(sql.c_str());

	// 2.对以上结果中的ispower的值等于1或者2的记录作为起点进行拓扑分析。
	for (int i = 0;i<unitList.size();i++)
	{
		STRMAP unitMap = unitList.at(i);
		MAP_ITERATOR iter = unitMap.find("IsPower");

		// 相对电源点或绝对电源电记录
		if (iter != unitMap.end() && (str2i(iter->second) == 1 || str2i(iter->second) == 2))
		{
			iter = unitMap.find("UnitCim");
			if (iter != unitMap.end())
			{
				PBNS::StateBean bean;
				bean.set_cimid(iter->second);
				
				// 保存电压等级颜色
				bean.set_volcolor(COM->getVal(unitMap,"Color"));

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

	// 拓扑结果返回客户端
	PBNS::OprationMsg_Response res;
	res.set_optype(state);

	for (int i = 0;i<unitList.size();i++)
	{
		// 把整站的设备默认带电状态设置为0
		STRMAP unitMap = unitList.at(i);
		PBNS::StateBean* pbean = res.add_devstate();

		// 保存CIM
		pbean->set_cimid(COM->getVal(unitMap,"UnitCim"));

		// 保存设备类型
		pbean->set_unittype(COM->getIval(unitMap,"UnitType"));

		// 如果为本次操作设备，则把状态更新为操作后的状态
		if (pbean->cimid() == cimid)
		{
			// 返回设备的状态
			pbean->set_state(state);
		}
		else
		{
			// 返回数据库设备的状态
			pbean->set_state(COM->getIval(unitMap,"State"));
		}

		// 把整站的设备默认带电状态设置为0
		pbean->set_iselectric(0);

		// 返回设备的电压等级
		pbean->set_volcolor(COM->getVal(unitMap,"Color"));

	
		// 遍历拓扑后带电的集合
		for (int j = 0;j<rsltMap.size();j++)
		{
			PBNS::StateBean bean = rsltMap.at(j);
			if (bean.cimid() == COM->getVal(unitMap,"UnitCim"))
			{
				pbean->set_iselectric(1);
			}
		}
	}
	LOG->debug("返回设备总数量:%d,带电设备数量：%d",res.devstate_size(),rsltMap.size());
	return res.SerializeAsString();

}

void TopoBizCmd::topoOnBreakerChange(sClientMsg *msg)
{
	/*
	输入参数：	1.存档ID
						2.元件CIMID
						3.是否允许校验
	运算结果：
					找出本站设备在此次开关变位后的的带电状态.
	*/
	// 保存拓扑分析的结果，即设备的带电状态
	PBNS::OprationMsg_Request req;
	req.ParseFromArray(msg->data,msg->length);
	int saveId = req.saveid();
	string cimid = req.unitcim();
	if (saveId < 0 || cimid.length()<=0)
	{
		return;
	}
	
	// 仅对科动的厂家生效
	int factype = COM->str2i(App_Config::instance()->getValue(CIM_ROOT,CIM_PRODUCTID));
	if (factype == 2)
	{
		// 检查是否为推车刀闸，如果是则把推车刀闸加入到已操作列表
		checkIsCarSwitch(req);
	}
	
	if (req.ischeck())
	{
		// 规则校验
		roleCheck(msg->connectId,req);
	}
	else
	{
		string data = execTopoOnBreakerChange(req);

		App_ClientMgr::instance()->sendData(msg->connectId,data,msg->type);
	}
}

void TopoBizCmd::checkIsCarSwitch(PBNS::OprationMsg_Request& req)
{
	// 判断是否 为开关，如果不是则返回
	if (req.unittype() == eBREAKER)
	{
		// 标记查找到几个刀闸
		int flag = 0;

		string unitcim = "";

		// 查询该元件两端的连接点集合
		LISTMAP connList = getConnIdByUnitsId(req.unitcim());

		for (int i = 0;i<connList.size();i++)
		{
			STRMAP connMap = connList.at(i);

			// 根据连接点查找对应的设备集合
			string connId = COM->getVal(connMap,"connId");

			LISTMAP unitList = getUnitsByConnId(connId,COM->i2str(req.saveid()));

			for (int j = 0;j<unitList.size();j++)
			{
				STRMAP unitMap = unitList.at(j);
				
				int unitType = COM->getIval(unitMap,"UnitType");
				if (unitType == eSWITCH)
				{
					flag++;
					unitcim = COM->getVal(unitMap,"id");
				}

			}

		}

		// 如果只找到一个刀闸，则标识为推车，把该刀闸的cim加入到已操作列表中，并置为闭合
		if (flag == 1)
		{
			PBNS::StateBean *bean = req.add_opdevlist();
			bean->set_unittype(eSWITCH);
			bean->set_cimid(unitcim);
			bean->set_state(1);

		}
	}
}

void TopoBizCmd::topoByUnitIdMem(PBNS::StateBean bean,STRMAP& passNodes,vector<PBNS::StateBean>& rsltMap,PBNS::OprationMsg_Request req)
{
	string saveid = COM->i2str(req.saveid());
	string cimid = req.unitcim();
	int objState = req.type();

	string unitid = bean.cimid();
	
	LOG->debug("topoByUnitIdMem,uinit cimid:%s",unitid.c_str());

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

			//将找到的变压器加入带电集合
			if(!transBean.cimid().empty())
			{
				rsltMap.push_back(transBean);
				transBean.Clear();
			}

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
					etype = str2i(unitIter->second);

					// 取电压等级颜色
					cbean.set_volcolor(COM->getVal(unitMap,"Color"));

					if (etype == eBREAKER || etype == eSWITCH)
					{
						// 4.如果该设备为为开关，刀闸，闭合即为带电，否则为不带电；
						unitIter = unitMap.find("State");
						if (unitIter != unitMap.end())
						{
							int state = str2i(unitIter->second);

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

	
	// 触发的规则列表
	vector<int> ruleList;

	switch (devtype)
	{
	case eBREAKER:

		// 开关闭合
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

		// 开关断开
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

	// 如果没有触发规则，则返回客户端执行变位操作
	if (ruleList.size() == 0)
	{
		string data = execTopoOnBreakerChange(req);
		App_ClientMgr::instance()->sendData(connid,data,CMD_TOPO_BREAKER_CHANGE);
	}
	else
	{
		// 把触发的规则，返回到客户端
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

	// 查询规则列表
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

	// 两个条件
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

	// 两个条件
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

	// 两个条件
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

	// 两个条件
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
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

	// 两个条件
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

	// 1个条件
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

	// 1个条件
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

	// 2个条件
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

	// 2个条件
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

	// 2个条件
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

	//3个条件
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

	// 3个条件
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

	// 3个条件
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));
	ruleMap.insert(RVAL(3,3));
	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}

// 28,29 逻辑相同
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

	// 1个条件
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

	// 2个条件
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

	// 2个条件
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

	// 3个条件
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

	// 3个条件
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

	// 3个条件
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

	// 2个条件
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

	// 2个条件
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

	// 2个条件
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

	// 2个条件
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

	// 2个条件
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

	// 2个条件
	ruleMap.insert(RVAL(1,1));
	ruleMap.insert(RVAL(2,2));

	return r.topoByUnit(saveid,unitcim,passedNodes,ruleMap);
}
