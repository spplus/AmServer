#include "comutils.h"

int	ComUtils::str2i(string val)
{
	return ACE_OS::atoi(val.c_str());
}

string	ComUtils::i2str(int val)
{
	char temp[16];
	ACE_OS::itoa(val,temp,10);
	string strval;
	strval.append(temp);
	return strval;
}

int ComUtils::getIval(STRMAP &smap,string key)
{
	return str2i(getVal(smap,key));
}

string ComUtils::getVal(STRMAP &smap,string key)
{
	MAP_ITERATOR iter = smap.find(key);
	if (iter != smap.end())
	{
		return iter->second;
	}
	else
	{
		return "";
	}
}

void ComUtils::triggerRule(RMAP &ruleMap,int rid)
{
	R_ITERATOR iter = ruleMap.find(rid);
	if (iter != ruleMap.end())
	{
		ruleMap.erase(iter);
	}
}