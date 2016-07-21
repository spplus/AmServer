/************************************************************************/
/* 
		DESC:	���˷������࣬�ṩ���˷����Ŀ���߼�������ҵ���߼���ͨ���麯����������ʵ��.
		DATE:	2016-06-25
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __TOPOBASE_H__
#define __TOPOBASE_H__

#include "comutils.h"
#include <string>

using namespace std;

class TopoBase
{
public:

	// ���˷�������㷨
	virtual	bool		topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

	// ִ�о����ҵ���߼� 
	/*
	����ֵ˵����1 �������� ��0 ���õݹ����ˣ�2 ֱ���˳������򲻻ᱻ����
	*/
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim="") = 0;

	// ���õ�ǰ�������豸
	void				setOpcim(string	cimid);

	// ���ÿͻ��˲����豸�б�
	void				setReq(PBNS::OprationMsg_Request req);

	// ���þ������Ľڵ�
	void				setPassedNodes(STRMAP &passedNodes);

protected:
	// ͨ��cimid��ȡunit��Ϣ
	PBNS::StateBean getUnitByCim(int saveid,string unitcim);

	// ����Ԫ��ID���Ҷ�Ӧ�����ӵ�
	LISTMAP			getConnIdByUnitsId(string unitid);

	// �������ӵ���ҹ������豸
	LISTMAP			getUnitsByConnId(string connid,string saveid);

protected:
	// ���β����豸��cim
	string			m_opcim;
	STRMAP			m_passedNodes;
	PBNS::OprationMsg_Request	m_req;
};

#endif

