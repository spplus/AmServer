/************************************************************************/
/* 
		DESC:	����豸ͣ��.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ44_H__
#define __RULEBIZ44_H__

#include "topobase.h"

class RuleBiz44		:public TopoBase
{
public:
	RuleBiz44();
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);
private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	// �����ĵ�բ����
	int				m_switchCount;

	// ��բ״̬
	bool				m_switchState;

	// next cim
	string			m_unitCim;

	// ��բ�б�
	vector<string>	m_switchList;
};

#endif