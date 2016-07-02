/************************************************************************/
/* 
		DESC:	�պ�ĸ�����أ������ڿ��������ĸ�����絶բ���պϺ�ſ��Բ���.
		DATE:	2016-07-02
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ39_H__
#define __RULEBIZ39_H__

#include "topobase.h"

class RuleBiz39		:public TopoBase
{
public:
	RuleBiz39();
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