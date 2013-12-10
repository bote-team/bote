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
#include <vector>

template <class Key, class Value, class Serializable = Value>
class CBotEMap : public std::map<Key, Value>
{

public:
	void Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << size();
			for(const_iterator it = map::begin(); it != map::end(); ++it)
			{
				ar << it->first;
				ar << static_cast<Serializable>(it->second);
			}
		}
		else
		{
			map::clear();
			unsigned int size = 0;
			ar >> size;
			for (unsigned i = 0; i < size; ++i)
			{
				Key key;
				ar >> key;
				Serializable value;
				ar >> value;
				map::insert(std::pair<Key, Value>(key, static_cast<Value>(value)));
			}
		}
	}

	CBotEMap& operator=(const std::map<Key, Value>& other){
		map::operator=(other);
		return *this;
	};
};

template <class Value>
class CBotESet : public std::set<Value>
{

public:
	void Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << set::size();
			for (const_iterator it = set::begin(); it != set::end(); ++it)
				ar << *it;
		}
		else
		{
			set::clear();
			unsigned int size = 0;
			ar >> size;
			for (unsigned i = 0; i < size; ++i)
			{
				Value value;
				ar >> value;
				set::insert(value);
			}
		}
	}
};

template <class Value>
class CBotEVector : public std::vector<Value>
{

public:
	void Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
			ar << vector::size();
		else
		{
			unsigned int size = 0;
			ar >> size;
			vector::clear();
			vector::resize(size);
		}
		for(iterator it = vector::begin(); it != vector::end(); ++it)
			it->Serialize(ar);
	}
};
