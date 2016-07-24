#include "parsexml.h"


ParseXML::ParseXML(void)
{
}


ParseXML::~ParseXML(void)
{
}


void ParseXML::loadXML(std::string filename,int factype)
{
	std::string strxml = filename;

	file<> fdoc(strxml.c_str());
	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	//��ȡ���ڵ�
	xml_node<>* root = doc.first_node();


	//����������ѹ��Ϣ
	parseBaseVoltages(root,factype);

	//������վ��Ϣ
	parseSubStation(root,factype);

	//������ѹ�ȼ���Ϣ:������վ��Ӧ�Ļ�����ѹ
	parseVoltagLevel(root,factype);
	
	//���������Ϣ
	parseBay(root,factype);

	//�����˵���Ϣ
	parseTerminal(root,factype);

	//�������ӵ���Ϣ
	parseConnectivityNode(root,factype);

	//������·����Ϣ
	parseBreaker(root,factype);

	//������բ��Ϣ
	parseDisconnector(root,factype);

	//�����ӵص�բ��Ϣ
	parseGrdDisconnector(root,factype);

	//����ĸ����Ϣ
	parseBusbarSection(root,factype);

	//������������Ϣ
	parseCompensator(root,factype);

	//����������Ϣ
	parseEnergyConsumer(root,factype);

	//�����������Ϣ
	parseSynchronousMachine(root,factype);

	//������ѹ��������Ϣ
	parsePotentialTransformer(root,factype);

	//������ѹ����Ϣ
	parsePowerTransformer(root,factype);

	//������ѹ��������Ϣ
	parseTransformerWinding(root,factype);

	//�����ѹ��
	dealPowerTransformer();

	//�����ѹ������
	dealTransformerWinding();

	//������·��Ϣ
	parseACLineSegment(root,factype);

	//����˵������ϵ��
	dealTerminalRelation();
/**/
	return;
}


int ParseXML::parseBaseVoltages(xml_node<>* parnode,int factype)
{

	xml_node<>* pBaseVoltage = parnode->first_node(CIM_BVOL);			//��ȡ<cim:BaseVoltage>�ڵ�
	if (!pBaseVoltage)
	{
		return RC_FAILURE;
	}

	mapBaseVol.clear();

	for (xml_node<>* pBVol = parnode->first_node(CIM_BVOL);pBVol != NULL; pBVol=pBVol->next_sibling(CIM_BVOL))
	{
		//ȡ������ѹCIMID
		std::string strid = pBVol->first_attribute(RDFID)->value();

		std::string strbvname = "";
		//��ѹ�ȼ�cim�ļ���û�������ֶ�,ֻ�����𳧼���
		if (factype == FACNARI)
		{
			strbvname = pBVol->first_node(CIM_NARI_BVOL_NAME)->value();
		}

		if (factype == FACJICHENGDZ)
		{
			strbvname = pBVol->first_node(CIM_IES_BVOL_NAME)->value();
		}

		//ȡ������ѹ��Ӧֵ
		xml_node<>* pNalVol = pBVol->first_node(CIM_BVOL_NALVOL);
		std::string strnval = pNalVol->value();

		BaseVoltage bvol;
		bvol.bvcimid = strid;
		bvol.bvname = strbvname;
		bvol.bvval = strnval;

		mapBaseVol.insert(std::pair<std::string,BaseVoltage>(strid,bvol));

	}


	return RC_SUCCESS;
}


int ParseXML::parseSubStation(xml_node<>* parnode,int factype)
{
	xml_node<>* pSubStation = parnode->first_node(CIM_SUBSTATION);			//��ȡ<cim:Substation>�ڵ�
	if (!pSubStation)
	{
		return RC_FAILURE;
	}

	mapSubstation.clear();

	for (xml_node<>* pStation = parnode->first_node(CIM_SUBSTATION);pStation != NULL; pStation=pStation->next_sibling(CIM_SUBSTATION))
	{
		//ȡ��վCIMID
		std::string strid = pStation->first_attribute(RDFID)->value();

		//ȡ��վ����
		xml_node<>* pStaname = pStation->first_node(CIM_NAMING_NAME);
		std::string strname = pStaname->value();

		//ȡ��վ����
		std::string stralsname = "";
		if (factype == FACNARI || factype == FACKEDONG || factype == FACDONGFANG)
		{
			stralsname = pStation->first_node(CIM_NAMING_ALSNAME)->value();
		}

		if (factype == FACJICHENGDZ)
		{
			stralsname = pStation->first_node(CIM_NAMING_DESCRIPTION)->value();
		}
		

		Substation subst;
		subst.faccimid = strid;
		subst.faccrname = stralsname;
		subst.facname = strname;

		mapSubstation.insert(std::pair<std::string,Substation>(strid,subst));
	}

	return RC_SUCCESS;
}


int ParseXML::parseVoltagLevel(xml_node<>* parnode,int factype)
{
	xml_node<>* pVoltagLevel = parnode->first_node(CIM_VOLEVEL);			//��ȡ<cim:VoltageLevel>�ڵ�
	if (!pVoltagLevel)
	{
		return RC_FAILURE;
	}

	mapVollevel.clear();

	for (xml_node<>* pVolevel = parnode->first_node(CIM_VOLEVEL);pVolevel != NULL; pVolevel=pVolevel->next_sibling(CIM_VOLEVEL))
	{
		//ȡ��ѹ�ȼ�CIMID
		std::string strid = pVolevel->first_attribute(RDFID)->value();

		std::string strname = "";
		std::string strstationid = "";
		std::string strbvid = "";

		if (factype == FACNARI || factype == FACKEDONG || factype == FACDONGFANG || factype == FACJICHENGDZ)
		{
			//ȡ��ѹ�ȼ�����
			xml_node<>* pVolevelname = pVolevel->first_node(CIM_NAMING_NAME);
			strname = pVolevelname->value();

			//ȡ������վCIMID
			strstationid = pVolevel->first_node(CIM_VOLEVEL_STATION)->first_attribute(RDFRESOURCE)->value();
			strstationid = strstationid.substr(1,strstationid.length()-1);

			//ȡ����������ѹ
			strbvid = pVolevel->first_node(CIM_VOLEVEL_BVOL)->first_attribute(RDFRESOURCE)->value();
			strbvid = strbvid.substr(1,strbvid.length()-1);
		}

		VoltageLevel volel;
		volel.vlcimid = strid;
		volel.vlname = strname;
		volel.faccimid = strstationid;
		volel.bvcimid = strbvid;

		mapVollevel.insert(std::pair<std::string,VoltageLevel>(strid,volel));
	}

	return RC_SUCCESS;
}

int ParseXML::parseBay(xml_node<>* parnode,int factype)
{
	xml_node<>* pBayy = parnode->first_node(CIM_BAY);			//��ȡ<cim:Bay>�ڵ�
	if (!pBayy)
	{
		return RC_FAILURE;
	}

	mapBay.clear();

	for (xml_node<>* pBay = parnode->first_node(CIM_BAY);pBay != NULL; pBay=pBay->next_sibling(CIM_BAY))
	{
		//ȡ���CIMID
		std::string strid = pBay->first_attribute(RDFID)->value();

		//ȡ�������
		std::string strname = pBay->first_node(CIM_NAMING_NAME)->value();

		//ȡ�����Ӧ�����ĳ�վCIMID
		std::string strfacid = pBay->first_node(CIM_BAY_MOF_STATION)->first_attribute(RDFRESOURCE)->value();
		strfacid = strfacid.substr(1,strfacid.length()-1);

		Bay bay;
		bay.baycimid = strid;
		bay.bayname = strname;
		bay.faccimid = strfacid;

		mapBay.insert(std::pair<std::string,Bay>(strid,bay));
	}

	return RC_SUCCESS;
}

int ParseXML::parseTerminal(xml_node<>* parnode,int factype)
{
	xml_node<>* pTerminal = parnode->first_node(CIM_TERMINAL);			//��ȡ<cim:Terminal>�ڵ�
	if (!pTerminal)
	{
		return RC_FAILURE;
	}

	mapTerminal.clear();

	for (xml_node<>* pTminal = parnode->first_node(CIM_TERMINAL);pTminal != NULL; pTminal=pTminal->next_sibling(CIM_TERMINAL))
	{
		//ȡ�˵�CIMID
		std::string strid = pTminal->first_attribute(RDFID)->value();

		//ȡ�˵�����
		std::string strname = pTminal->first_node(CIM_NAMING_NAME)->value();

		std::string strcdeqment = "";
		std::string strnode = "";

		//ȡ�˵���������ӵ�CIMID
		xml_node<>* pCNode = pTminal->first_node(CIM_TERMINAL_CONDNODE);
		if (pCNode)
		{
			strnode = pCNode->first_attribute(RDFRESOURCE)->value();
			strnode = strnode.substr(1,strnode.length()-1);
		}
		
		//ȡ�˵������Ԫ��CIMID
		xml_node<>* pCdeqment = pTminal->first_node(CIM_TERMINAL_CONDEQMENT);
		if (pCdeqment)
		{
			strcdeqment = pCdeqment->first_attribute(RDFRESOURCE)->value();
			strcdeqment = strcdeqment.substr(1,strcdeqment.length()-1);
		}
	
		Terminal terml;
		terml.tmlcimid = strid;
		terml.tmlname = strname;
		terml.nodecimid = strnode;
		terml.cdcimid = strcdeqment;
		
		mapTerminal.insert(std::pair<std::string,Terminal>(strid,terml));
	}

	return RC_SUCCESS;
}


int ParseXML::parseConnectivityNode(xml_node<>* parnode,int factype)
{
	xml_node<>* pConnectivityNode = parnode->first_node(CIM_CONNECT_NODE);			//��ȡ<cim:ConnectivityNode>�ڵ�
	if (!pConnectivityNode)
	{
		return RC_FAILURE;
	}

	mapConnectivityNode.clear();

	for (xml_node<>* pCNode = parnode->first_node(CIM_CONNECT_NODE);pCNode != NULL; pCNode=pCNode->next_sibling(CIM_CONNECT_NODE))
	{
		//ȡ���ӵ�CIMID
		std::string strid = pCNode->first_attribute(RDFID)->value();

		//ȡ���ӵ�����
		std::string strname = pCNode->first_node(CIM_NAMING_NAME)->value();

		std::string stvlcimid = "";

		//ȡ���ӵ�����ĵ�ѹ�ȼ�cimid
		stvlcimid = pCNode->first_node(CIM_CONNECT_NODE_MOFEQMNET)->first_attribute(RDFRESOURCE)->value();
		stvlcimid = stvlcimid.substr(1,stvlcimid.length()-1);

		std::map<std::string,VoltageLevel>::iterator vliter;
		vliter = mapVollevel.find(stvlcimid);
		if (vliter != mapVollevel.end())
		{
			ConnectivityNode CNode;
			CNode.nodecimid = strid;
			CNode.nodename = strname;
			CNode.vlcimid = stvlcimid;
			CNode.faccimid = vliter->second.faccimid;
			CNode.bvcimid = vliter->second.bvcimid;

			mapConnectivityNode.insert(std::pair<std::string,ConnectivityNode>(strid,CNode));
		}
	}

	return RC_SUCCESS;
}



int ParseXML::parseBreaker(xml_node<>* parnode,int factype)
{
	xml_node<>* pBreaker = parnode->first_node(CIM_BREAKER);			//��ȡ<cim:Breaker>�ڵ�
	if (!pBreaker)
	{
		return RC_FAILURE;
	}

	mapBreaker.clear();

	for (xml_node<>* pBker = parnode->first_node(CIM_BREAKER);pBker != NULL; pBker=pBker->next_sibling(CIM_BREAKER))
	{
		//ȡ��·��CIMID
		std::string strid = pBker->first_attribute(RDFID)->value();

		//ȡ��·������
		std::string strname = pBker->first_node(CIM_NAMING_NAME)->value();

		std::string strbvid = "";

		std::string strbayid = "";
		std::string strvolevid = "";

		if (factype == FACNARI)
		{
			//ȡ����������ѹ
			strbvid = pBker->first_node(CIM_CONDTEQPMENT_BVAL)->first_attribute(RDFRESOURCE)->value();
			strbvid = strbvid.substr(1,strbvid.length()-1);

			//ȡ��·������BAY��cimid
			strbayid = pBker->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
			strbayid = strbayid.substr(1,strbayid.length()-1);

			std::map<std::string,Bay>::iterator bayiter;
			bayiter = mapBay.find(strbayid);
			if (bayiter != mapBay.end())
			{
				Breaker brker;
				brker.brkcimid = strid;
				brker.brkname = strname;
				brker.faccimid = bayiter->second.faccimid;
				brker.bvcimid = strbvid;

				mapBreaker.insert(std::pair<std::string,Breaker>(strid,brker));
			}
		}

		if (factype == FACKEDONG || factype == FACDONGFANG || factype == FACJICHENGDZ)
		{
			//ȡ��·��������ѹ�ȼ���cimid
			strvolevid = pBker->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
			strvolevid = strvolevid.substr(1,strvolevid.length()-1);

			std::map<std::string,VoltageLevel>::iterator vliter;
			vliter = mapVollevel.find(strvolevid);
			if (vliter != mapVollevel.end())
			{
				Breaker brker;
				brker.brkcimid = strid;
				brker.brkname = strname;
				brker.faccimid = vliter->second.faccimid;
				brker.bvcimid = vliter->second.bvcimid;

				mapBreaker.insert(std::pair<std::string,Breaker>(strid,brker));
			}		

		}
	}

	return RC_SUCCESS;
}


int ParseXML::parseDisconnector(xml_node<>* parnode,int factype)
{
	xml_node<>* pDiscnntor = parnode->first_node(CIM_DISCTOR);			//��ȡ<cim:Disconnector>�ڵ�
	if (!pDiscnntor)
	{
		return RC_FAILURE;
	}

	mapDisconnector.clear();

	for (xml_node<>* pDisctor = parnode->first_node(CIM_DISCTOR);pDisctor != NULL; pDisctor=pDisctor->next_sibling(CIM_DISCTOR))
	{
		//ȡ��բCIMID
		std::string strid = pDisctor->first_attribute(RDFID)->value();

		//ȡ��բ����
		std::string strname = pDisctor->first_node(CIM_NAMING_NAME)->value();

		std::string strbvid = "";

		std::string strbayid = "";
		std::string strvolevid = "";

		if (factype == FACNARI)
		{
			//ȡ����������ѹ
			strbvid = pDisctor->first_node(CIM_CONDTEQPMENT_BVAL)->first_attribute(RDFRESOURCE)->value();
			strbvid = strbvid.substr(1,strbvid.length()-1);

			//ȡ��բ����BAY��cimid
			strbayid = pDisctor->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
			strbayid = strbayid.substr(1,strbayid.length()-1);

			std::map<std::string,Bay>::iterator bayiter;
			bayiter = mapBay.find(strbayid);
			if (bayiter != mapBay.end())
			{
				Disconnector Discntor;
				Discntor.distcimid = strid;
				Discntor.distname = strname;
				Discntor.faccimid = bayiter->second.faccimid;
				Discntor.bvcimid = strbvid;

				mapDisconnector.insert(std::pair<std::string,Disconnector>(strid,Discntor));
			}
		}

		if (factype == FACKEDONG || factype == FACDONGFANG || factype == FACJICHENGDZ)
		{
			//ȡ������ѹ�ȼ�
			strvolevid = pDisctor->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
			strvolevid = strvolevid.substr(1,strvolevid.length()-1);

			std::map<std::string,VoltageLevel>::iterator vliter;
			vliter = mapVollevel.find(strvolevid);
			if (vliter != mapVollevel.end())
			{
				Disconnector Discntor;
				Discntor.distcimid = strid;
				Discntor.distname = strname;
				Discntor.faccimid = vliter->second.faccimid;
				Discntor.bvcimid = vliter->second.bvcimid;

				mapDisconnector.insert(std::pair<std::string,Disconnector>(strid,Discntor));
			}		
		}
	}

	return RC_SUCCESS;
}


int ParseXML::parseGrdDisconnector(xml_node<>* parnode,int factype)
{
	xml_node<>* pGrdDiscnntor = parnode->first_node(CIM_GRD_DISCTOR);			//��ȡ<cim:GroundDisconnector>�ڵ�
	if (!pGrdDiscnntor)
	{
		return RC_FAILURE;
	}

	mapGroundDisconnector.clear();

	for (xml_node<>* pGrdDisctor = parnode->first_node(CIM_GRD_DISCTOR);pGrdDisctor != NULL; pGrdDisctor=pGrdDisctor->next_sibling(CIM_GRD_DISCTOR))
	{
		//ȡ��բCIMID
		std::string strid = pGrdDisctor->first_attribute(RDFID)->value();

		//ȡ��բ����
		std::string strname = pGrdDisctor->first_node(CIM_NAMING_NAME)->value();

		std::string strbvid = "";

		std::string strbayid = "";
		std::string strvolevid = "";

		if (factype == FACNARI)
		{
			//ȡ����������ѹ
			strbvid = pGrdDisctor->first_node(CIM_CONDTEQPMENT_BVAL)->first_attribute(RDFRESOURCE)->value();
			strbvid = strbvid.substr(1,strbvid.length()-1);

			//ȡ��·������BAY��cimid
			strbayid = pGrdDisctor->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
			strbayid = strbayid.substr(1,strbayid.length()-1);

			std::map<std::string,Bay>::iterator bayiter;
			bayiter = mapBay.find(strbayid);
			if (bayiter != mapBay.end())
			{
				GroundDisconnector GDiscntor;
				GDiscntor.gdistcimid = strid;
				GDiscntor.gdistname = strname;
				GDiscntor.faccimid = bayiter->second.faccimid;
				GDiscntor.bvcimid = strbvid;

				mapGroundDisconnector.insert(std::pair<std::string,GroundDisconnector>(strid,GDiscntor));
			}
		}

		if (factype == FACKEDONG || factype == FACDONGFANG || factype == FACJICHENGDZ)
		{
			//ȡ������ѹ�ȼ�
			strvolevid = pGrdDisctor->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
			strvolevid = strvolevid.substr(1,strvolevid.length()-1);

			std::map<std::string,VoltageLevel>::iterator vliter;
			vliter = mapVollevel.find(strvolevid);
			if (vliter != mapVollevel.end())
			{
				GroundDisconnector GDiscntor;
				GDiscntor.gdistcimid = strid;
				GDiscntor.gdistname = strname;
				GDiscntor.faccimid = vliter->second.faccimid;
				GDiscntor.bvcimid = vliter->second.bvcimid;

				mapGroundDisconnector.insert(std::pair<std::string,GroundDisconnector>(strid,GDiscntor));
			}		
		}
	}


	return RC_SUCCESS;
}


int ParseXML::parseBusbarSection(xml_node<>* parnode,int factype)
{
	xml_node<>* pBusbar = parnode->first_node(CIM_BUS);			//��ȡ<cim:BusbarSection>�ڵ�
	if (!pBusbar)
	{
		return RC_FAILURE;
	}

	mapBusbarSection.clear();

	for (xml_node<>* pBus = parnode->first_node(CIM_BUS);pBus != NULL; pBus=pBus->next_sibling(CIM_BUS))
	{
		//ȡĸ��CIMID
		std::string strid = pBus->first_attribute(RDFID)->value();

		//ȡĸ������
		std::string strname = pBus->first_node(CIM_NAMING_NAME)->value();

		std::string strvolevid = "";

		//ȡ������ѹ�ȼ�
		strvolevid = pBus->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
		strvolevid = strvolevid.substr(1,strvolevid.length()-1);

		std::map<std::string,VoltageLevel>::iterator vliter;
		vliter = mapVollevel.find(strvolevid);
		if (vliter != mapVollevel.end())
		{
			BusbarSection Busbar;
			Busbar.buscimid = strid;
			Busbar.busname = strname;
			Busbar.faccimid = vliter->second.faccimid;
			Busbar.bvcimid = vliter->second.bvcimid;

			mapBusbarSection.insert(std::pair<std::string,BusbarSection>(strid,Busbar));
		}
	}

	return RC_SUCCESS;
}


int ParseXML::parseCompensator(xml_node<>* parnode,int factype)
{
	xml_node<>* pCompensator = parnode->first_node(CIM_COMPENSATOR);			//��ȡ<cim:Compensator>�ڵ�
	if (!pCompensator)
	{
		return RC_FAILURE;
	}

	mapCompensator.clear();

	for (xml_node<>* pCompstor = parnode->first_node(CIM_COMPENSATOR);pCompstor != NULL; pCompstor=pCompstor->next_sibling(CIM_COMPENSATOR))
	{
		//ȡĸ��CIMID
		std::string strid = pCompstor->first_attribute(RDFID)->value();

		//ȡĸ������
		std::string strname = pCompstor->first_node(CIM_NAMING_NAME)->value();

		std::string strvolevid = "";

		//ȡ������ѹ�ȼ�
		strvolevid = pCompstor->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
		strvolevid = strvolevid.substr(1,strvolevid.length()-1);

		std::map<std::string,VoltageLevel>::iterator vliter;
		vliter = mapVollevel.find(strvolevid);
		if (vliter != mapVollevel.end())
		{
			Compensator Comptor;
			Comptor.cptorcimid = strid;
			Comptor.cptorname = strname;
			Comptor.faccimid = vliter->second.faccimid;
			Comptor.bvcimid = vliter->second.bvcimid;

			mapCompensator.insert(std::pair<std::string,Compensator>(strid,Comptor));
		}
	}

	return RC_SUCCESS;
}


int ParseXML::parseEnergyConsumer(xml_node<>* parnode,int factype)
{
	xml_node<>* pEnergy = parnode->first_node(CIM_ENERGY_CONSUMER);			//��ȡ<cim:EnergyConsumer>�ڵ�
	if (!pEnergy)
	{
		return RC_FAILURE;
	}

	mapEnergyConsumer.clear();

	for (xml_node<>* pEngy = parnode->first_node(CIM_ENERGY_CONSUMER);pEngy != NULL; pEngy=pEngy->next_sibling(CIM_ENERGY_CONSUMER))
	{
		//ȡ�����CIMID
		std::string strid = pEngy->first_attribute(RDFID)->value();

		//ȡ���������
		std::string strname = pEngy->first_node(CIM_NAMING_NAME)->value();

		std::string strvolevid = "";

		//ȡ������ѹ�ȼ�
		strvolevid = pEngy->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
		strvolevid = strvolevid.substr(1,strvolevid.length()-1);

		std::map<std::string,VoltageLevel>::iterator vliter;
		vliter = mapVollevel.find(strvolevid);
		if (vliter != mapVollevel.end())
		{
			EnergyConsumer Energycmer;
			Energycmer.eyccimid = strid;
			Energycmer.eycname = strname;
			Energycmer.faccimid = vliter->second.faccimid;
			Energycmer.bvcimid = vliter->second.bvcimid;

			mapEnergyConsumer.insert(std::pair<std::string,EnergyConsumer>(strid,Energycmer));
		}
	}

	return RC_SUCCESS;
}


int ParseXML::parseSynchronousMachine(xml_node<>* parnode,int factype)
{
	xml_node<>* pSynchronousMachine = parnode->first_node(CIM_SYNC_HRUS_MACHINE);			//��ȡ<cim:SynchronousMachine>�ڵ�
	if (!pSynchronousMachine)
	{
		return RC_FAILURE;
	}

	mapSynchronousMachine.clear();

	for (xml_node<>* pSyncMachine = parnode->first_node(CIM_SYNC_HRUS_MACHINE);pSyncMachine != NULL; pSyncMachine=pSyncMachine->next_sibling(CIM_SYNC_HRUS_MACHINE))
	{
		//ȡ�����CIMID
		std::string strid = pSyncMachine->first_attribute(RDFID)->value();

		//ȡ���������
		std::string strname = pSyncMachine->first_node(CIM_NAMING_NAME)->value();

		std::string strvolevid = "";

		//ȡ������ѹ�ȼ�
		xml_node<>* pVolev = pSyncMachine->first_node(CIM_EQPMENT_MOF_QPTCONT);
		if (pVolev)
		{
			strvolevid = pVolev->first_attribute(RDFRESOURCE)->value();
			strvolevid = strvolevid.substr(1,strvolevid.length()-1);
		}

		std::map<std::string,VoltageLevel>::iterator vliter;
		vliter = mapVollevel.find(strvolevid);
		if (vliter != mapVollevel.end())
		{
			SynchronousMachine Syncmachine;
			Syncmachine.symhcimid = strid;
			Syncmachine.symhname = strname;
			Syncmachine.faccimid = vliter->second.faccimid;
			Syncmachine.bvcimid = vliter->second.bvcimid;

			mapSynchronousMachine.insert(std::pair<std::string,SynchronousMachine>(strid,Syncmachine));
		}
	}

	return RC_SUCCESS;
}

int ParseXML::parsePotentialTransformer(xml_node<>* parnode,int factype)
{
	xml_node<>* pPtrans = parnode->first_node(CIM_PT);			//��ȡ<cim:PotentialTransformer>�ڵ�
	if (!pPtrans)
	{
		return RC_FAILURE;
	}

	mapPT.clear();

	for (xml_node<>* pPT = parnode->first_node(CIM_PT);pPT != NULL; pPT=pPT->next_sibling(CIM_PT))
	{
		//ȡ��ѹ������CIMID
		std::string strid = pPT->first_attribute(RDFID)->value();

		//ȡ��ѹ����������
		std::string strname = pPT->first_node(CIM_NAMING_NAME)->value();

		std::string strvolevid = "";

		//ȡ������ѹ�ȼ�
		strvolevid = pPT->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
		strvolevid = strvolevid.substr(1,strvolevid.length()-1);

		std::map<std::string,VoltageLevel>::iterator vliter;
		vliter = mapVollevel.find(strvolevid);
		if (vliter != mapVollevel.end())
		{
			PT Pt;
			Pt.ptcimid = strid;
			Pt.ptname = strname;
			Pt.faccimid = vliter->second.faccimid;
			Pt.bvcimid = vliter->second.bvcimid;

			mapPT.insert(std::pair<std::string,PT>(strid,Pt));
		}
	}

	return RC_SUCCESS;
}


int ParseXML::parsePowerTransformer(xml_node<>* parnode,int factype)
{
	xml_node<>* pPowerTrasf = parnode->first_node(CIM_POWER_TRANSFMER);			//��ȡ<cim:PowerTransformer>�ڵ�
	if (!pPowerTrasf)
	{
		return RC_FAILURE;
	}

	mapPowerTransformer.clear();

	for (xml_node<>* pPowertf = parnode->first_node(CIM_POWER_TRANSFMER);pPowertf != NULL; pPowertf=pPowertf->next_sibling(CIM_POWER_TRANSFMER))
	{
		//ȡ��ѹ��CIMID
		std::string strid = pPowertf->first_attribute(RDFID)->value();

		//ȡ��ѹ������
		std::string strname = pPowertf->first_node(CIM_NAMING_NAME)->value();

		std::string strstationid = "";

		//ȡ��ѹ����Ӧ��վcimid
		strstationid = pPowertf->first_node(CIM_EQPMENT_MOF_QPTCONT)->first_attribute(RDFRESOURCE)->value();
		strstationid = strstationid.substr(1,strstationid.length()-1);

		PowerTransformer ptformer;
		ptformer.ptfcimid = strid;
		ptformer.ptfname = strname;
		ptformer.faccimid = strstationid;

		mapPowerTransformer.insert(std::pair<std::string,PowerTransformer>(strid,ptformer));

	}

	return RC_SUCCESS;
}


int ParseXML::parseTransformerWinding(xml_node<>* parnode,int factype)
{
	xml_node<>* pTransfwind = parnode->first_node(CIM_TRANSFMER_WINDING);			//��ȡ<cim:TransformerWinding>�ڵ�
	if (!pTransfwind)
	{
		return RC_FAILURE;
	}

	mapTransformerWinding.clear();

	for (xml_node<>* pTransfw = parnode->first_node(CIM_TRANSFMER_WINDING);pTransfw != NULL; pTransfw=pTransfw->next_sibling(CIM_TRANSFMER_WINDING))
	{
		//ȡ��ѹ������CIMID
		std::string strid = pTransfw->first_attribute(RDFID)->value();

		//ȡ��ѹ����������
		std::string strname = pTransfw->first_node(CIM_NAMING_NAME)->value();

		std::string strtype = "";
		std::string strstationid = "";
		std::string strbvid = "";
		std::string strptf = "";
		std::string strvolevid = "";

		//����ѹ�������Ӧ�ĵ�ǰ��(�ߡ��С���)
		xml_node<>* pType = pTransfw->first_node(CIM_TRANSFMER_WINDING_TYPE);
		if (pType)
		{
			strtype = pType->first_attribute(RDFRESOURCE)->value();
			std::size_t nsize = strtype.find_last_of(".");
			strtype = strtype.substr(nsize+1,strtype.length()-nsize-1);
		}	

		//ȡ��ѹ�������Ӧ������ѹ��cimid
		xml_node<>* pPtformer = pTransfw->first_node(CIM_TRANSFW_MOF_POWERTF);
		if (pPtformer)
		{
			strptf = pPtformer->first_attribute(RDFRESOURCE)->value();
			strptf = strptf.substr(1,strptf.length()-1);
		}

		//ȡ��ѹ�������Ӧ��ѹ�ȼ�
		xml_node<>* pVolev = pTransfw->first_node(CIM_EQPMENT_MOF_QPTCONT);
		if (pVolev)
		{
			strvolevid = pVolev->first_attribute(RDFRESOURCE)->value();
			strvolevid = strvolevid.substr(1,strvolevid.length()-1);
		}

		std::map<std::string,VoltageLevel>::iterator voliter = mapVollevel.find(strvolevid);
		if (voliter != mapVollevel.end())
		{
			strstationid = voliter->second.faccimid;
			strbvid = voliter->second.bvcimid;
		}
		
		TransformerWinding tfwinding;
		tfwinding.tfwcimid = strid;
		tfwinding.tfwname = strname;
		tfwinding.tfwtype = strtype;
		tfwinding.ptfcimid = strptf;
		tfwinding.vlcimid = strvolevid;
		tfwinding.bvcimid = strbvid;

		mapTransformerWinding.insert(std::pair<std::string,TransformerWinding>(strid,tfwinding));

	}

	return RC_SUCCESS;
}


int ParseXML::dealPowerTransformer()
{
	mapPowerTransformerUnt.clear();

	for (std::map<std::string,TransformerWinding>::iterator iter = mapTransformerWinding.begin();iter != mapTransformerWinding.end();iter++)
	{
		iter->second.tfwcimid;

		if (iter->second.tfwtype == STR_PRIMARY)
		{
			std::map<std::string,PowerTransformer>::iterator ptfiter = mapPowerTransformer.find(iter->second.ptfcimid);
			if (ptfiter != mapPowerTransformer.end())
			{
				PowerTransformer_Units ptformerunt;
				ptformerunt.ptfcimid = ptfiter->second.ptfcimid;
				ptformerunt.ptfname = ptfiter->second.ptfname;
				ptformerunt.faccimid = ptfiter->second.faccimid;
				ptformerunt.bvcimid = iter->second.bvcimid;

				mapPowerTransformerUnt.insert(std::pair<std::string,PowerTransformer_Units>(ptfiter->second.ptfcimid,ptformerunt));
			}
		}
		
	}

	return RC_SUCCESS;
}


int ParseXML::dealTransformerWinding()
{
	mapTransformerWindingUnt.clear();

	std::map<std::string,PowerTransformer>::iterator ptfiter;
	std::map<std::string,TransformerWinding>::iterator tfwiter;

	for (ptfiter = mapPowerTransformer.begin(); ptfiter != mapPowerTransformer.end(); ptfiter++)
	{
		std::string strprimcimid = "";
		std::string strpscndcimid = "";
		std::string strptertcimid = "";

		std::string strprimbvid = "";
		std::string strpscndbvid = "";
		std::string strptertbvid = "";
		int nCont = 0;

		for (tfwiter=mapTransformerWinding.begin(); tfwiter != mapTransformerWinding.end(); tfwiter++)
		{
			if (ptfiter->first == tfwiter->second.ptfcimid)
			{
				//��ѹ
				if (tfwiter->second.tfwtype == STR_PRIMARY)
				{
					strprimcimid = tfwiter->second.tfwcimid;
					strprimbvid = tfwiter->second.bvcimid;
				}

				//��ѹ
				if (tfwiter->second.tfwtype == STR_SECONDARY)
				{
					strpscndcimid = tfwiter->second.tfwcimid;
					strpscndbvid = tfwiter->second.bvcimid;
				}

				//��ѹ
				if (tfwiter->second.tfwtype == STR_TERTIARY)
				{
					strptertcimid = tfwiter->second.tfwcimid;
					strptertbvid = tfwiter->second.bvcimid;
				}

				//�Ա�ѹ�����м�����ѹ��������м���
				nCont++;
			}
		}

		//��ѹ��Ϊ��Ȧ��
		if (nCont == 2)
		{
			//��ѹ���Ӧֵ
			TransformerWinding_Units tfwdingg;
			tfwdingg.tfwcimid = strprimcimid;
			tfwdingg.ptfcimid = ptfiter->first;
			tfwdingg.bvcimid = strprimbvid;
			tfwdingg.wdindex = 0;
			tfwdingg.wdgrade = 1;
			mapTransformerWindingUnt.insert(std::pair<std::string,TransformerWinding_Units>(strprimcimid,tfwdingg));

			//��ѹ���Ӧֵ
			TransformerWinding_Units tfwdingd;
			tfwdingd.tfwcimid = strpscndcimid;
			tfwdingd.ptfcimid = ptfiter->first;
			tfwdingd.bvcimid = strpscndbvid;
			tfwdingd.wdindex = 1;
			tfwdingd.wdgrade = 3;
			mapTransformerWindingUnt.insert(std::pair<std::string,TransformerWinding_Units>(strpscndcimid,tfwdingd));
		}

		//��ѹ��Ϊ��Ȧ��
		if (nCont == 3)
		{
			//��ѹ���Ӧֵ
			TransformerWinding_Units tfwdingg;
			tfwdingg.tfwcimid = strprimcimid;
			tfwdingg.ptfcimid = ptfiter->first;
			tfwdingg.bvcimid = strprimbvid;
			tfwdingg.wdindex = 0;
			tfwdingg.wdgrade = 1;
			mapTransformerWindingUnt.insert(std::pair<std::string,TransformerWinding_Units>(strprimcimid,tfwdingg));

			//��ѹ���Ӧֵ
			TransformerWinding_Units tfwdingz;
			tfwdingz.tfwcimid = strpscndcimid;
			tfwdingz.ptfcimid = ptfiter->first;
			tfwdingz.bvcimid = strpscndbvid;
			tfwdingz.wdindex = 1;
			tfwdingz.wdgrade = 2;
			mapTransformerWindingUnt.insert(std::pair<std::string,TransformerWinding_Units>(strpscndcimid,tfwdingz));

			//��ѹ���Ӧֵ
			TransformerWinding_Units tfwdingd;
			tfwdingd.tfwcimid = strptertcimid;
			tfwdingd.ptfcimid = ptfiter->first;
			tfwdingd.bvcimid = strptertbvid;
			tfwdingd.wdindex = 2;
			tfwdingd.wdgrade = 3;
			mapTransformerWindingUnt.insert(std::pair<std::string,TransformerWinding_Units>(strptertcimid,tfwdingd));
		}

	}

	return RC_SUCCESS;
}


int ParseXML::parseACLineSegment(xml_node<>* parnode,int factype)
{
	xml_node<>* pACLineSegment = parnode->first_node(CIM_ACLINESGT);			//��ȡ<cim:ACLineSegment>�ڵ�
	if (!pACLineSegment)
	{
		return RC_FAILURE;
	}

	mapACLineSegment.clear();

	for (xml_node<>* pACLine = parnode->first_node(CIM_ACLINESGT);pACLine != NULL; pACLine=pACLine->next_sibling(CIM_ACLINESGT))
	{
		//ȡ�����߶�CIMID
		std::string strid = pACLine->first_attribute(RDFID)->value();

		//ȡ�����߶�����
		std::string strname = pACLine->first_node(CIM_NAMING_NAME)->value();

		std::string strbvid = "";

		std::string strscimid = "";
		std::string strecimid = "";

		//ȡ�����߶ι����Ļ�����ѹcimid
		strbvid = pACLine->first_node(CIM_CONDTEQPMENT_BVAL)->first_attribute(RDFRESOURCE)->value();
		strbvid = strbvid.substr(1,strbvid.length()-1);

		if (factype == FACNARI)
		{
			//ȡ�����߶�һ�˶�Ӧ��վcimid
			strscimid = pACLine->first_node(CIM_NARI_ACLINSGT_ST)->first_attribute(RDFRESOURCE)->value();
			strscimid = strscimid.substr(1,strscimid.length()-1);

			//ȡ�����߶���һ�˶�Ӧ��վcimid
			strecimid = pACLine->first_node(CIM_NARI_ACLINSGT_END)->first_attribute(RDFRESOURCE)->value();
			strecimid = strecimid.substr(1,strecimid.length()-1);

			ACLineSegment acline;
			acline.aclcimid = strid;
			acline.aclname = strname;
			acline.bvcimid = strbvid;
			acline.fac1cimid = strscimid;
			acline.fac2cimid = strecimid;

			mapACLineSegment.insert(std::pair<std::string,ACLineSegment>(strid,acline));
		}

		if (factype == FACJICHENGDZ)
		{
			//ȡ�����߶�һ�˶�Ӧ��վcimid
			strscimid = pACLine->first_node(CIM_ACLINSGT_FROM)->first_attribute(RDFRESOURCE)->value();
			strscimid = strscimid.substr(1,strscimid.length()-1);

			//ȡ�����߶���һ�˶�Ӧ��վcimid
			strecimid = pACLine->first_node(CIM_ACLINSGT_TO)->first_attribute(RDFRESOURCE)->value();
			strecimid = strecimid.substr(1,strecimid.length()-1);

			ACLineSegment acline;
			acline.aclcimid = strid;
			acline.aclname = strname;
			acline.bvcimid = strbvid;
			acline.fac1cimid = strscimid;
			acline.fac2cimid = strecimid;

			mapACLineSegment.insert(std::pair<std::string,ACLineSegment>(strid,acline));
		}

		std::string strtml1cimid = "";
		std::string strtml2cimid = "";

		std::string strnd1cimid = "";
		std::string strnd2cimid = "";

		if (factype==FACKEDONG || factype==FACDONGFANG)
		{
			//ȡ�����߶�һ�˶�Ӧ�˵�cimid
			strtml1cimid = pACLine->first_node(CIM_CDEQMENT_TERMINALS)->first_attribute(RDFRESOURCE)->value();
			strtml1cimid = strtml1cimid.substr(1,strtml1cimid.length()-1);

			//ȡ�����߶���һ�˶�Ӧ�˵�cimid
			strtml2cimid = pACLine->last_node(CIM_CDEQMENT_TERMINALS)->first_attribute(RDFRESOURCE)->value();
			strtml2cimid = strtml2cimid.substr(1,strtml2cimid.length()-1);

			std::map<std::string,Terminal>::iterator tmliter;
			std::map<std::string,ConnectivityNode>::iterator nodeiter;
			std::map<std::string,VoltageLevel>::iterator vliter;

			//ȡ����һ��˵�
			tmliter = mapTerminal.find(strtml1cimid);
			if (tmliter != mapTerminal.end())
			{
				strnd1cimid = tmliter->second.nodecimid;

				nodeiter = mapConnectivityNode.find(strnd1cimid);
				if (nodeiter != mapConnectivityNode.end())
				{
					strscimid = nodeiter->second.faccimid;
				}
			}
			
			//ȡ������һ��˵�
			tmliter = mapTerminal.find(strtml2cimid);
			if (tmliter != mapTerminal.end())
			{
				strnd2cimid = tmliter->second.nodecimid;

				nodeiter = mapConnectivityNode.find(strnd2cimid);
				if (nodeiter != mapConnectivityNode.end())
				{
					strecimid = nodeiter->second.faccimid;
				}
			}

			ACLineSegment acline;
			acline.aclcimid = strid;
			acline.aclname = strname;
			acline.bvcimid = strbvid;
			acline.fac1cimid = strscimid;
			acline.fac2cimid = strecimid;

			mapACLineSegment.insert(std::pair<std::string,ACLineSegment>(strid,acline));

		}

	}

	return RC_SUCCESS;
}


int ParseXML::dealTerminalRelation()
{
	mapTerminalRelation.clear();

	std::map<std::string,Terminal>::iterator tmliter;
	std::map<std::string,ConnectivityNode>::iterator nodeiter;
	std::map<std::string,TerminalRelation>::iterator trlaiter;

	std::map<std::string,std::string> mapTemp;
	std::map<std::string,std::string>::iterator tempiter;

	for (tmliter=mapTerminal.begin();tmliter!=mapTerminal.end();tmliter++)
	{
		//�ж϶˵��Ƿ����Ԫ��cimid��û��������ǰѭ��
		if (tmliter->second.cdcimid == "")
		{
			continue;
		}

		std::string nodecimid = "";
		std::string faccimid = "";
		
		nodecimid = tmliter->second.nodecimid;

		nodeiter = mapConnectivityNode.find(nodecimid);
		if (nodeiter != mapConnectivityNode.end())
		{
			faccimid = nodeiter->second.faccimid;

			TerminalRelation Tmlrelat;
			Tmlrelat.tmlcimid = tmliter->second.tmlcimid;
			Tmlrelat.tmlname = tmliter->second.tmlname;
			Tmlrelat.faccimid = faccimid;
			Tmlrelat.unitcimid = tmliter->second.cdcimid;
			Tmlrelat.nodecimid = nodecimid;
			Tmlrelat.tmlport = 0;

			//���˵��ж�Ӧ������Ԫ��cimid���ʱ��˵����ǰmap������ͬԪ���е�һ��˵㣬���Դ�ʱ�ٲ����ʾ��Ԫ������һ��
			/* //�˷���Ч�ʲ�
			trlaiter = mapTerminalRelation.begin();
			for (;trlaiter != mapTerminalRelation.end();trlaiter++)
			{
				if (tmliter->second.cdcimid == trlaiter->second.unitcimid)
				{
					Tmlrelat.tmlport = 1;
					continue;
				}
			}*/

			//������ʱmap���Ƿ��Ѿ������˶˵��ж�Ӧ������Ԫ��cimid����û��������ȥΪ�Ժ�˵���һ��Ĺ�����Ԫ��cimid���ʱ�õ�
			//���˵��ж�Ӧ������Ԫ��cimid���ʱ��˵����ǰmap������ͬԪ���е�һ��˵㣬���Դ�ʱ�ٲ����ʾ��Ԫ������һ��
			tempiter = mapTemp.find(tmliter->second.cdcimid);
			if (tempiter != mapTemp.end())
			{
				Tmlrelat.tmlport = 1;
			}
			else
			{
				mapTemp.insert(std::pair<std::string,std::string>(tmliter->second.cdcimid,tmliter->second.cdcimid));
			}

			mapTerminalRelation.insert(std::pair<std::string,TerminalRelation>(tmliter->second.tmlcimid,Tmlrelat));

		}

	}


	return RC_SUCCESS;
}


