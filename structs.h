/************************************************************************/
/* 
	DESC:	����ṹ��.
	DATE:	2016-04-20
	AUTHOR:YUANLS
*/
/************************************************************************/

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

struct sClientMsg 
{
	unsigned int	connectId;		// ����ID
	unsigned int	length;				// ��Ϣ����
	unsigned int	type;				// ��Ϣ����
	char*				data;					// ��Ϣ����

	sClientMsg()
	{
		connectId = 0;
		length = 0;
		type = 0;
		data = 0;
	}
	~sClientMsg()
	{
		if (data != 0)
		{
			delete []data;
		}
	}
};

#endif