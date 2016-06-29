/************************************************************************/
/* 
		DESC:	��բ�����豸����ֹ����.
		DATE:	2016-06-29
		AUTHOR:	YUANLS
*/
/************************************************************************/

#ifndef __RULEBIZ25_H__
#define __RULEBIZ25_H__

#include "topobase.h"

class RuleBiz25		:public TopoBase
{
public:
	RuleBiz25();
	void				setOptype(int optype);
	bool				topoByUnit(int saveid,string unitcim,STRMAP& passNodes,RMAP& ruleMap);

private:
	virtual int		topoBiz(int saveid,string unitcim,RMAP& ruleMap,string stationcim/* ="" */);

private:
	// �ͻ��˲�������
	int				m_optype;

	// �Ƿ������բ
	bool				m_hasSwitch;

	// �Ƿ����ĸ��
	bool				m_hasBus;

	// �����ĵ�բ״̬
	int				m_hasSwitchState;

	// �����������ĵ�բcim
	string			m_switchCim;
};

#endif