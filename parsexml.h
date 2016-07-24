/************************************************************************/
/* 
		DESC:		����xml�ļ�����.
		DATE:		2016-06-04
		AUTHOR:		ABELSKY
*/
/************************************************************************/
#ifndef __PARSEXML_H__
#define __PARSEXML_H__

#include <map>

#include "definetag.h"

#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;

class ParseXML
{
public:
	ParseXML(void);
	~ParseXML(void);

	void	loadXML(std::string filename,int factype);							//����CIM�ļ�
	MAPBVOL			getBaseVoltage(){return mapBaseVol;}								//��ȡ������ѹmap
	MAPVOLEVEL		getVollevel(){return mapVollevel;}
	MAPSUBSTATION	getSubstation(){return mapSubstation;}
	MAPTML			getTerminal(){return mapTerminal;}
	MAPCNODE		getConnectivityNode(){return mapConnectivityNode;}

	MAPBREAKER		getBreaker(){return mapBreaker;}
	MAPDISCTOR		getDisconnector(){return mapDisconnector;}
	MAPGRDDISCTOR	getGroundDisconnector(){return mapGroundDisconnector;}
	MAPBUS			getBusbarSection(){return mapBusbarSection;}
	MAPCOMPTOR		getCompensator(){return mapCompensator;}
	MAPEYCONMER		getEnergyConsumer(){return mapEnergyConsumer;}
	MAPSYNCMCHINE	getSynchronousMachine(){return mapSynchronousMachine;}
	MAPPT			getPT(){return mapPT;}
	MAPPOWERTFUT	getPowerTransformerUnt(){return mapPowerTransformerUnt;}
	MAPTRANSFWUT	getTransformerWindingUnt(){return mapTransformerWindingUnt;}

	MAPACLINE		getACLineSegment(){return mapACLineSegment;}
	MAPTMLRAL		getTerminalRelation(){return mapTerminalRelation;}


private:
	int		parseBaseVoltages(xml_node<>* parnode,int factype);					//��������������ѹ
	int		parseSubStation(xml_node<>* parnode,int factype);					//����������վ
	int		parseVoltagLevel(xml_node<>* parnode,int factype);					//������ѹ�ȼ�
	int		parseBay(xml_node<>* parnode,int factype);							//���������Ϣ
	int		parseTerminal(xml_node<>* parnode,int factype);						//���������˵���Ϣ
	int		parseConnectivityNode(xml_node<>* parnode,int factype);				//�����������ӵ���Ϣ
	int		parseBreaker(xml_node<>* parnode,int factype);						//����������·��
	int		parseDisconnector(xml_node<>* parnode,int factype);					//����������բ��Ϣ
	int		parseGrdDisconnector(xml_node<>* parnode,int factype);				//���������ӵص�բ��Ϣ
	int		parseBusbarSection(xml_node<>* parnode,int factype);				//��������ĸ����Ϣ
	int		parseCompensator(xml_node<>* parnode,int factype);					//����������������Ϣ
	int		parseEnergyConsumer(xml_node<>* parnode,int factype);				//��������������Ϣ
	int		parseSynchronousMachine(xml_node<>* parnode,int factype);			//���������������Ϣ
	int		parsePotentialTransformer(xml_node<>* parnode,int factype);			//����������ѹ��������Ϣ
	int		parsePowerTransformer(xml_node<>* parnode,int factype);				//����������ѹ����Ϣ
	int		parseTransformerWinding(xml_node<>* parnode,int factype);			//����������ѹ��������Ϣ
	int		parseACLineSegment(xml_node<>* parnode,int factype);				//����������·��Ϣ

	int		dealPowerTransformer();												//�����ѹ����Ϣ
	int		dealTransformerWinding();											//�����ѹ��������Ϣ
	int		dealTerminalRelation();												//����˵������ϵ��


private:
	MAPBVOL				mapBaseVol;					//���������ѹ��Ϣ
	MAPVOLEVEL			mapVollevel;				//�����ѹ�ȼ���Ϣ
	MAPSUBSTATION		mapSubstation;				//���泧վ��Ϣ

	MAPBAY				mapBay;						//��������Ϣ
	MAPTML				mapTerminal;				//����˵���Ϣ
	MAPCNODE			mapConnectivityNode;		//�������ӵ���Ϣ

	MAPBREAKER			mapBreaker;					//�����·��������Ϣ
	MAPDISCTOR			mapDisconnector;			//���浶բ��Ϣ
	MAPGRDDISCTOR		mapGroundDisconnector;		//����ӵص�բ��Ϣ
	MAPBUS				mapBusbarSection;			//����ĸ����Ϣ
	MAPCOMPTOR			mapCompensator;				//�����������Ϣ
	MAPEYCONMER			mapEnergyConsumer;			//���渺����Ϣ
	MAPSYNCMCHINE		mapSynchronousMachine;		//���淢�����Ϣ
	MAPPT				mapPT;						//�����ѹ��������Ϣ

	MAPPOWERTF			mapPowerTransformer;		//�����ѹ����Ϣ
	MAPTRANSFW			mapTransformerWinding;		//�����ѹ��������Ϣ

	MAPPOWERTFUT		mapPowerTransformerUnt;		//�����ѹ���洢��Ԫ�����е���Ϣ
	MAPTRANSFWUT		mapTransformerWindingUnt;	//�����ѹ��������Ϣ��Ӧ��ѹ����Ȧ��Ϣ��

	MAPACLINE			mapACLineSegment;			//�����������Ϣ

	MAPTMLRAL			mapTerminalRelation;		//����˵����ӹ�ϵ��Ϣ

};
#endif // __PARSEXML_H__
