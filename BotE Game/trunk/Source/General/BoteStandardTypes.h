/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

/*
 */

#pragma once

#include <map>
#include <set>

template <class Key, class Value, class Serializable = Value>
class CBotEMap : public std::map<Key, Value>
{

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

public:
	void Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << size();
			for(std::map<Key, Value>::const_iterator it = begin(); it != end(); ++it)
				ar << it->first << static_cast<Serializable>(it->second);
		}
		else
		{
			clear();
			unsigned int size = 0;
			ar >> size;
			for (unsigned i = 0; i < size; ++i)
			{
				Key key;
				ar >> key;
				Serializable value;
				ar >> value;
				insert(std::pair<Key, Value>(key, static_cast<Value>(value)));
			}
		}
	}
};

template <class Value>
class CBotESet : public std::set<Value>
{
//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

public:
	void Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << size();
			for (std::set<Value>::const_iterator it = begin(); it != end(); ++it)
				ar << *it;
		}
		else
		{
			clear();
			unsigned int size = 0;
			ar >> size;
			for (unsigned i = 0; i < size; ++i)
			{
				Value value;
				ar >> value;
				insert(value);
			}
		}
	}
};
