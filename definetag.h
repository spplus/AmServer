/************************************************************************/
/* 
		DESC:		�����ǩ�ȳ���.
		DATE:		2016-06-04
		AUTHOR:		ABELSKY
*/
/************************************************************************/


#ifndef __DEFINETAG_H__
#define __DEFINETAG_H__
#include <string>
//�����ļ�·��. 
//#define INIPATHNAME "cimin.ini"
////��־���ƺ�·������.
//#define LOGPATH		"LOGPATH" 
//#define LOGNAME		"logname"
//#define LOGPATHNAME "logpath"
////���ݿ���������.
//#define DBINFO		"DBINFO" 
//#define DBADDR		"DBAddr"
//#define DBPORT		"DBPort"
//#define DBUSER		"DBUser"
//#define USERPWD		"UserPwd"
//#define DBNAME		"DBName"

//���巵��ֵ
#define RC_FAILURE      -1
#define RC_SUCCESS		0

//����֧�ֽ���CIM�ļ�����:Ŀǰֻ֧�����𡢿ƶ����������ӡ����ɵ�����������
#define FACNARI			1
#define FACKEDONG		2
#define FACDONGFANG		3
#define FACJICHENGDZ	4

//�����ѹ��������е�
#define PRIMARY			1
#define	SECONDARY		2
#define	TERTIARY		3

#define STR_PRIMARY				"primary"
#define STR_SECONDARY			"secondary"
#define STR_TERTIARY			"tertiary"

// �����豸����
enum eUnitType{eBreaker,eSwitch,eBus,eCapacitor,eLoad,eGenerator,ePt,eCT,eTransformer,eLine,eGrdSwitch};

/************************************��ǩ����***********************************************/
//���幫����ǩ
#define RDFID						"rdf:ID"
#define RDFRESOURCE					"rdf:resource"

#define CIM_NAMING_NAME				"cim:Naming.name"
#define CIM_NAMING_ALSNAME			"cim:Naming.aliasName"
#define CIM_NAMING_DESCRIPTION		"cim:Naming.description"

//���������ѹ���ͱ�ǩ
#define CIM_BVOL					"cim:BaseVoltage"
#define CIM_NARI_BVOL_NAME			"cimNARI:BaseVoltage.name"
#define CIM_BVOL_NALVOL				"cim:BaseVoltage.nominalVoltage"
#define CIM_IES_BVOL_NAME			"cimIESLAB:BaseVoltage.name"

//���峧վ��ǩ
#define CIM_SUBSTATION				"cim:Substation"
#define CIM_NARI_SUBSTATION_BVOL	"cimNARI:Substation.MaxBaseVoltage"

//�����ѹ�ȼ���ǩ
#define CIM_VOLEVEL					"cim:VoltageLevel"
#define CIM_VOLEVEL_STATION			"cim:VoltageLevel.MemberOf_Substation"
#define CIM_VOLEVEL_BVOL			"cim:VoltageLevel.BaseVoltage"

//��������ǩ
#define CIM_BAY						"cim:Bay"
#define CIM_BAY_MOF_VOLEVEL			"cim:Bay.MemberOf_VoltageLevel"
#define CIM_BAY_MOF_STATION			"cim:Bay.MemberOf_Substation"

//���忪�ر�ǩ
#define CIM_BREAKER					"cim:Breaker"
#define CIM_CONDTEQPMENT_BVAL		"cim:ConductingEquipment.BaseVoltage"
#define CIM_EQPMENT_MOF_QPTCONT		"cim:Equipment.MemberOf_EquipmentContainer"

//���嵶բ��ǩ
#define CIM_DISCTOR					"cim:Disconnector"
//����ӵص�բ��ǩ
#define CIM_GRD_DISCTOR				"cim:GroundDisconnector"

//����ĸ�߱�ǩ
#define CIM_BUS						"cim:BusbarSection"

//�����������ǩ
#define CIM_COMPENSATOR				"cim:Compensator"

//���帺�ɱ�ǩ
#define CIM_ENERGY_CONSUMER			"cim:EnergyConsumer"

//���巢�����ǩ
#define CIM_SYNC_HRUS_MACHINE		"cim:SynchronousMachine"

//�����ѹ��������ǩ
#define CIM_PT						"cim:PotentialTransformer"

//�����ѹ����ǩ
#define CIM_POWER_TRANSFMER			"cim:PowerTransformer"

//�����ѹ�������ǩ
#define CIM_TRANSFMER_WINDING		"cim:TransformerWinding"
#define CIM_TRANSFMER_WINDING_TYPE	"cim:TransformerWinding.windingType"
#define CIM_TRANSFW_MOF_POWERTF		"cim:TransformerWinding.MemberOf_PowerTransformer"


//������·��ǩ
#define CIM_ACLINESGT				"cim:ACLineSegment"
#define CIM_NARI_ACLINSGT_ST		"cimNARI:ACLineSegment.StartST"
#define CIM_NARI_ACLINSGT_END		"cimNARI:ACLineSegment.EndST"

#define CIM_ACLINSGT_FROM			"cim:ACLineSegment.FromSubstation"
#define CIM_ACLINSGT_TO				"cim:ACLineSegment.FromSubstation"
#define CIM_CDEQMENT_TERMINALS		"cim:ConductingEquipment.Terminals"

//����˵���Ϣ
#define CIM_TERMINAL				"cim:Terminal"
#define CIM_TERMINAL_CONDEQMENT		"cim:Terminal.ConductingEquipment"
#define CIM_TERMINAL_CONDNODE		"cim:Terminal.ConnectivityNode"

//�������ӵ�
#define CIM_CONNECT_NODE			 "cim:ConnectivityNode"
#define CIM_CONNECT_NODE_MOFEQMNET	 "cim:ConnectivityNode.MemberOf_EquipmentContainer"

/************************************�ṹ�嶨��***********************************************/
//��ѹ������Ϣ
struct BaseVoltage
{
	std::string bvcimid;
	std::string bvname;
	std::string bvval;

};

//��ѹ�ȼ���Ϣ
struct VoltageLevel
{
	std::string vlcimid;
	std::string vlname;
	std::string faccimid;
	std::string bvcimid;
};

//վ����Ϣ
struct Substation
{
	std::string faccimid;
	std::string facname;
	std::string faccrname;
};

//�����Ϣ
struct Bay
{
	std::string baycimid;
	std::string bayname;
	std::string faccimid;
};

//�˵���Ϣ
struct Terminal
{
	std::string tmlcimid;
	std::string tmlname;
	std::string cdcimid;
	std::string nodecimid;
};

//���ӵ���Ϣ
struct ConnectivityNode
{
	std::string nodecimid;
	std::string nodename;
	std::string vlcimid;
	std::string bvcimid;
	std::string faccimid;
};

//��·����Ϣ
struct Breaker
{
	std::string brkcimid;
	std::string brkname;
	std::string faccimid;
	std::string bvcimid;
};


//��բ��Ϣ
struct Disconnector
{
	std::string distcimid;
	std::string distname;
	std::string faccimid;
	std::string bvcimid;
};


//�ӵص�բ����Ϣ
struct GroundDisconnector
{
	std::string gdistcimid;
	std::string gdistname;
	std::string faccimid;
	std::string bvcimid;
};

//ĸ����Ϣ
struct BusbarSection
{
	std::string buscimid;
	std::string busname;
	std::string faccimid;
	std::string bvcimid;
};

//��������Ϣ
struct Compensator
{
	std::string cptorcimid;
	std::string cptorname;
	std::string faccimid;
	std::string bvcimid;
};

//������Ϣ
struct EnergyConsumer
{
	std::string eyccimid;
	std::string eycname;
	std::string faccimid;
	std::string bvcimid;
};

//ˮƽ�������Ϣ
struct SynchronousMachine
{
	std::string symhcimid;
	std::string symhname;
	std::string faccimid;
	std::string bvcimid;
};

//��ѹ��������Ϣ
struct PT
{
	std::string ptcimid;
	std::string ptname;
	std::string faccimid;
	std::string bvcimid;
};


//��ѹ����Ϣ
struct PowerTransformer
{
	std::string ptfcimid;
	std::string ptfname;
	std::string faccimid;
};

//��ѹ����Ϣ
struct PowerTransformer_Units
{
	std::string ptfcimid;
	std::string ptfname;
	std::string faccimid;
	std::string bvcimid;
};

//��ѹ��������Ϣ
struct TransformerWinding
{
	std::string tfwcimid;
	std::string tfwname;
	std::string tfwtype;
	std::string vlcimid;
	std::string bvcimid;
	std::string ptfcimid;
};

//��ѹ��������Ϣ
struct TransformerWinding_Units
{
	std::string tfwcimid;
	std::string ptfcimid;
	std::string bvcimid;
	int wdgrade;
	int wdindex;	
};


//�����߶���Ϣ
struct ACLineSegment
{
	std::string aclcimid;
	std::string aclname;
	std::string fac1cimid;
	std::string fac2cimid;
	std::string bvcimid;
};


//�˵���Ϣ
struct TerminalRelation
{
	std::string tmlcimid;
	std::string tmlname;
	std::string faccimid;
	std::string unitcimid;
	int tmlport;
	std::string nodecimid;

};

/************************************map����***********************************************/
typedef std::map<std::string,BaseVoltage>		MAPBVOL;

typedef std::map<std::string,VoltageLevel>		MAPVOLEVEL;

typedef std::map<std::string,Substation>		MAPSUBSTATION;
typedef std::map<std::string,Bay>				MAPBAY;

typedef std::map<std::string,Terminal>			MAPTML;

typedef std::map<std::string,ConnectivityNode>	MAPCNODE;

typedef std::map<std::string,Breaker>			MAPBREAKER;

typedef	std::map<std::string,Disconnector>		MAPDISCTOR;

typedef std::map<std::string,GroundDisconnector> MAPGRDDISCTOR;

typedef std::map<std::string,BusbarSection>		MAPBUS;

typedef std::map<std::string,Compensator>		MAPCOMPTOR;

typedef std::map<std::string,EnergyConsumer>	MAPEYCONMER;

typedef std::map<std::string,SynchronousMachine> MAPSYNCMCHINE;

typedef std::map<std::string,PT>				MAPPT;

typedef std::map<std::string,PowerTransformer>	MAPPOWERTF;
typedef std::map<std::string,PowerTransformer_Units>	MAPPOWERTFUT;

typedef std::map<std::string,TransformerWinding>	MAPTRANSFW;
typedef std::map<std::string,TransformerWinding_Units>	MAPTRANSFWUT;

typedef std::map<std::string,ACLineSegment>		MAPACLINE;

typedef std::map<std::string,TerminalRelation>	MAPTMLRAL;
	




#endif

