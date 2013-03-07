/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Markus Sengthaler

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#ifndef DMDBCONNECTOR_H
#define DMDBCONNECTOR_H

#include <dmcompilersettings.h>
#include <qobject.h>
#include <queue>
#include <qatomic.h>


#define SQLQUERY_STACKSIZE 100
#define CACHE_PROFILING
//#define CACHE_INFINITE

#define ATTRIBUTE_CACHE_SIZE 10000
#define NODE_CACHE_SIZE 10000
#define RASTERBLOCKSIZE 64
#define RASTERBLOCKCACHESIZE 20
// edge cache is infinite (Asynchron)
// component cache is infinite (ComponentSyncMap: Asynchron)
// face cache is infinite (Asynchron)
#define CACHE_WRITEBLOCK 50



class QSqlQuery;
class QSqlError;
class QSqlDatabase;

namespace DM {

void DM_HELPER_DLL_EXPORT PrintSqlError(QSqlQuery *q);

class Asynchron
{
public:
    Asynchron();
	~Asynchron();
    virtual void Synchronize() = 0;
};

class DMSqlParameter
{
public:
    QString name;
    QVariant value;
    DMSqlParameter(QString name, QVariant value)
    {
        this->name = name;
        this->value = value;
    }
};

template<typename T>
class FIFOQueue
{
	class Node
	{
	public:
		Node(T* data)
		{
			next = NULL;
			pData = data;
		}
		Node* next;
		T*	pData;
	};
	Node* root;
	Node* last;
	QMutex m;
	QAtomicInt isEmpty;
public:
	FIFOQueue()
	{
		root = NULL;
		last = NULL;
		isEmpty = true;
	}
	~FIFOQueue(){}
	T* pop()
	{
		if(isEmpty)
			return NULL;

		m.lock();
		Node* n = root;
		T* d = root->pData;

		if(root == last)
		{
			isEmpty = true;
			last = NULL;
		}

		root = n->next;
		delete n;
		m.unlock();
		return d;
	}
	void push(T* data)
	{
		m.lock();
		if(isEmpty)
			root = last = new Node(data);
		else
		{
			last->next = new Node(data);
			last = last->next;
		}
		isEmpty = false;
		m.unlock();
	}
	bool IsEmpty()
	{
		return isEmpty;
	}
};

struct QueryList
{
	QString cmd;
	FIFOQueue<QSqlQuery> queryStack;
};

class DBWorker: public QThread
{
private:
	bool	kill;
	QMutex	selectMutex;
	FIFOQueue<QSqlQuery> queryStack;

	QSqlQuery *qSelect;
protected:
	std::list<QueryList*> queryLists;
	void run();
public:
	enum SelectStatus
	{
		SS_NOTDONE,
		SS_TRUE,
		SS_FALSE,
	}selectStatus;

	void addQuery(QSqlQuery *q);

	bool ExecuteSelect(QSqlQuery* q);

	DBWorker(): QThread()
	{
		qSelect = NULL;
		kill = false;
	}
	void Kill()
	{
		kill = true;
	}
	~DBWorker();
	QSqlQuery *getQuery(QString cmd);
};

class SingletonDestroyer;

class DM_HELPER_DLL_EXPORT DBConnector
{
    friend class SingletonDestroyer;
private:
	static DBConnector* instance;
    DBConnector();
	
    static bool _bTransaction;

    static SingletonDestroyer _destroyer;

	static DBWorker* worker;

    bool CreateTables();
    bool DropTables();

protected:
    virtual ~DBConnector();
public:
    QSqlQuery *getQuery(QString cmd);
    void ExecuteQuery(QSqlQuery *q);
    bool ExecuteSelectQuery(QSqlQuery *q);

    static DBConnector* getInstance();

    void Synchronize();
    // inserts with uuid
    void Insert(QString table,  QUuid uuid);
    void Insert(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0);
    void Insert(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0,
                                QString parName1, QVariant parValue1);
    void Insert(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0,
                                QString parName1, QVariant parValue1,
                                QString parName2, QVariant parValue2);
    // updates with uuid
    void Update(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0);
    void Update(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0,
                                QString parName1, QVariant parValue1);
    void Update(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0,
                                QString parName1, QVariant parValue1,
                                QString parName2, QVariant parValue2);
    // delete with uuid
    void Delete(QString table,  QUuid uuid);
    // select single entry with uuid
    bool Select(QString table, QUuid uuid,
                QString valName, QVariant *value);
    bool Select(QString table, QUuid uuid,
                QString valName0, QVariant *value0,
                QString valName1, QVariant *value1);
    bool Select(QString table, QUuid uuid,
                QString valName0, QVariant *value0,
                QString valName1, QVariant *value1,
                QString valName2, QVariant *value2);

	

/*
    void Duplicate(QString table, QByteArray uuid, QString stateuuid,
                                               QString newuuid, QString newStateUuid);*/
};

class SingletonDestroyer
{
    public:
        SingletonDestroyer(DBConnector* = NULL);
        ~SingletonDestroyer();

        void SetSingleton(DBConnector* s);
    private:
        DBConnector* _singleton;
};


template<class Tkey,class Tvalue>
class Cache
{
protected:
    class Node
    {
    public:
        Tkey key;
        Tvalue* value;
        Node* next;
        Node* last;
        Node(const Tkey &k, Tvalue* v)
        {
            key=k;
            value=v;
            next = NULL;
            last = NULL;
        }
        ~Node()
        {
            if(value)
                delete value;
        }
    };
private:
	std::map<Tkey,Node*> map;
protected:
    Node*   _root;
    Node*   _last;
    unsigned long    _size;
    unsigned long    _cnt;
	// internal
	inline Node* newNode(const Tkey &k, Tvalue* v)
	{
		Node* n = new Node(k,v);
		map[k] = n;
		return n;
	}
	inline void removeNode(Node* n)
	{
		map.erase(n->key);
		delete pop(n);
	}
	//
    void push_front(Node* n)
    {
        if(_last==NULL)
            _last = n;

        n->next = _root;
        n->last = NULL;
        if(_root != NULL)
            _root->last = n;
        _root = n;
        _cnt++;
    }
    Node* pop(Node* n)
    {
        Node* last = n->last;
        Node* next = n->next;
        if(last)
            last->next = next;
        if(next)
            next->last = last;
        if(n==_last)
            _last = last;
        if(n==_root)
            _root = next;
        _cnt--;
        return n;
    }
    Node* search(const Tkey &key)
    {
        typename std::map<Tkey,Node*>::iterator it = map.find(key);
		if(it==map.end())
			return NULL;
		return it->second;
    }

public:
#ifdef CACHE_PROFILING
    unsigned long hits;
    unsigned long misses;

    void ResetProfilingCounters()
    {
        misses = 0;
        hits = 0;
    }

#endif

    Cache(unsigned long size)
    {
        _size=size;
        _cnt=0;
        _root = NULL;
        _last = NULL;
#ifdef CACHE_PROFILING
            hits = 0;
            misses = 0;
#endif
    }
    ~Cache()
    {
        Node* cur;
        Node* next;
        next = _root;
        while(next!=NULL)
        {
            cur=next;
            next=cur->next;
            delete cur;
        }
    }
	unsigned long getSize(){return _size;};
    virtual Tvalue* get(const Tkey& key)
    {
        Node *n = search(key);
        // push front
        if(n!=NULL)
        {
            pop(n);
            push_front(n);
#ifdef CACHE_PROFILING
            hits++;
#endif
            return n->value;
        }
#ifdef CACHE_PROFILING
        misses++;
#endif
        return NULL;
    }
    virtual void add(const Tkey& key,Tvalue* value)
    {
        if(search(key)!=NULL)
            return;

        Node *n = newNode(key,value);
        push_front(n);
#ifndef CACHE_INFINITE
        if(_cnt>_size)
			removeNode(_last);
#endif
    }
    virtual bool replace(const Tkey& key,Tvalue* value)
    {
        Node *n = search(key);
        if(n==NULL)
            return false;

		removeNode(n);
        add(key, value);
        return true;
    }
    void remove(const Tkey& key)
    {
        Node *n = search(key);
        if(n)	removeNode(n);
    }
};

// like cache, but with db-functions
template<class Tkey,class Tvalue>
class DbCache: public Cache<Tkey,Tvalue>, Asynchron
{
public:
    typedef typename Cache<Tkey,Tvalue>::Node Node;

    DbCache(unsigned long size): Cache<Tkey,Tvalue>(size){}
    // add, save to db if something is dropped
    void add(Tkey key,Tvalue* value)
    {
        if(Cache<Tkey,Tvalue>::search(key)!=NULL)
            return;

        Node *n = Cache<Tkey,Tvalue>::newNode(key,value);
        Cache<Tkey,Tvalue>::push_front(n);
#ifndef CACHE_INFINITE
        if(Cache<Tkey,Tvalue>::_cnt > Cache<Tkey,Tvalue>::_size)
        {
			for(int i=0;i<CACHE_WRITEBLOCK && Cache<Tkey,Tvalue>::_cnt>1;i++)
			{
				Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
				Cache<Tkey,Tvalue>::removeNode(Cache<Tkey,Tvalue>::_last);
			}
        }
#endif
    }
    // get node, if not found, we may find it in the db
    Tvalue* get(const Tkey& key)
    {
        Tvalue* v = Cache<Tkey,Tvalue>::get(key);
        if(!v)
        {
            v = key->LoadFromDb();
            if(v)   add(key,v);
        }
        return v;
    }
	// note: currently removing from db is handled by the main class
    // void remove(const Tkey& key)

    // save everything to db
    void Synchronize()
    {
        Node* n=Cache<Tkey,Tvalue>::_root;
        while(n)
        {
            Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
            n = n->next;
        }
    }
	// resize cache
	void resize(unsigned long size)
	{
		Cache<Tkey,Tvalue>::_size = size;
#ifndef CACHE_INFINITE
		while(Cache<Tkey,Tvalue>::_cnt > size)
		{
			Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
			Cache<Tkey,Tvalue>::removeNode(Cache<Tkey,Tvalue>::_last);
		}
#endif
	}
};

}   // namespace DM


#endif
