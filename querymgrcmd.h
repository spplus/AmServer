/************************************************************************/
/* 
	DESC:		��ѯ������߼�.
	DATE:		2016-06-28
	AUTHOR:	    ABELSKY	
*/
/************************************************************************/

#ifndef __QUERYMGRCMD_H__
#define __QUERYMGRCMD_H__
#include "comutils.h"
#include "cmdbase.h"
class QueryMgrCmd :public CmdBase
{
public:
	void	exec(sClientMsg* msg);

private:
	//��·��ѯ
	void	queryCricleList(sClientMsg* msg);

	//��ѯ��
	void	querySignList(sClientMsg* msg);
	void	queryGSwitchList(sClientMsg* msg);
	void	queryMsetList(sClientMsg* msg);
	void	queryEventList(sClientMsg* msg);

	//��·����
	void	cricleTopo(int saveid);

	//���ݿ��ء���բԪ�����л�·����
	void	cricleTopoByUnit(int saveid,string unitcim,STRMAP& passNodes);

	// ͨ��cimid��ȡunit��Ϣ
	PBNS::StateBean getUnitByCim(int saveid,string unitcim);

	// ����Ԫ��ID���Ҷ�Ӧ�����ӵ�
	LISTMAP		getConnIdByUnitsId(string unitid);

	// �������ӵ���ҹ������豸
	LISTMAP		getUnitsByConnId(string connid,string saveid);

	// �Ƿ��һ��
	bool		m_isFirst;

	// ��ʼ�����Ŀ��أ���բcim
	string		m_brkswhCim;

	//ĸ�߼�¼��
	int			m_busCount;

	vector<string>		m_buslist;
};

#endif