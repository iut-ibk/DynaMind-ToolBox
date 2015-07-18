#ifndef DMDBCACHE_H
#define DMDBCACHE_H

#include <dmcache.h>

/**************************************************************//**
@class DM::DbCache

@brief similar to DM::Cache, but saves data to database when size
limit is reached and tries to load data from database when element
is not found.
The key class has to implement:
Tkey::SaveToDb(Tvalue);
Tkey::LoadFromDb();

COMMENTS
Inherits DM::Asynchron

******************************************************************/
namespace DM {

template<class Tkey,class Tvalue>
class DbCache: public Cache<Tkey,Tvalue>, Asynchron
{
	//!< Save all elements to db, called by DBConnector
	void Synchronize()
	{
#ifdef NO_DB_SYNC
		return;
#endif

			Node* n=Cache<Tkey,Tvalue>::_root;
		while(n)
		{
			n->key->SaveToDb(n->value);
			n = n->next;
		}
	}
public:
	typedef typename Cache<Tkey,Tvalue>::Node Node;
	//!< initializes a new cache with the given size, 0 results in an infinite cache
	DbCache(unsigned long size): Cache<Tkey,Tvalue>(size){}
	//!< add a new key-value pair, calls SaveToDb if last element is dropped
	void add(Tkey key,Tvalue* value)
	{
		this->mutex->lockInline();
		if(Cache<Tkey,Tvalue>::search(key)!=NULL)
		{
			this->mutex->unlockInline();
			return;
		}

		Node *n = Cache<Tkey,Tvalue>::newNode(key,value);
		Cache<Tkey,Tvalue>::push_front(n);

		if(Cache<Tkey,Tvalue>::_size)
		{
			if(Cache<Tkey,Tvalue>::_cnt > Cache<Tkey,Tvalue>::_size)
			{
				for(unsigned long i=0;i<DBConnector::getInstance()->GetCacheBlockwritingSize()
					&& Cache<Tkey,Tvalue>::_cnt>1;i++)
				{
					Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
					Cache<Tkey,Tvalue>::removeNode(Cache<Tkey,Tvalue>::_last);
				}
			}
		}
		this->mutex->unlockInline();
	}
	//!< returns the value associated with the given key, if not found LoadFromDb is called. Neither found in db, returns NULL
	Tvalue* get(const Tkey& key)
	{
		this->mutex->lockInline();
		Tvalue* v = Cache<Tkey,Tvalue>::get(key);
		if(!v)
		{
			v = key->LoadFromDb();
			if(v)   add(key,v);
		}
		this->mutex->unlockInline();
		return v;
	}
	// NOTE: currently removing from db is handled by the main class
	// void remove(const Tkey& key)

	//<! resizes the cache, if the given value is 0, the cache is set to infinite
	void resize(unsigned long size)
	{
		this->mutex->lockInline();
		Cache<Tkey,Tvalue>::_size = size;

		if(Cache<Tkey,Tvalue>::_size)
		{
			while(Cache<Tkey,Tvalue>::_cnt > size)
			{
				Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
				Cache<Tkey,Tvalue>::removeNode(Cache<Tkey,Tvalue>::_last);
			}
		}
		this->mutex->unlockInline();
	}

	void preCache(const QList<Tkey>& keys)
	{
		this->mutex->lockInline();

		if(keys.size() > 0)
		{
			Tkey classHolder;	// just to get static _PreCache function
			QList<Tvalue*> values;
			// init with null
			for(int i=0;i<keys.size();i++)
				values.append(NULL);

			classHolder->_PreCache(keys, values);

			for(int i=0;i<keys.size();i++)
				if(values[i] != NULL)
					add(keys[i], values[i]);
		}
		this->mutex->unlockInline();
	}
	//!< deletes all nodes, leaves the values untouched (non-deep delete)
	void Clear()
	{
		Synchronize();
		Cache<Tkey,Tvalue>::Clear();
	}
};

}   // namespace DM

#endif // DMDBCACHE_H
