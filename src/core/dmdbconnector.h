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

class QSqlQuery;
class QSqlError;
class QSqlDatabase;

namespace DM {
	
void DM_HELPER_DLL_EXPORT PrintSqlError(QSqlQuery *q);

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
	QAtomicInt maxOneLeft;
public:
	FIFOQueue()
	{
		root = NULL;
		last = NULL;
		isEmpty = true;
		maxOneLeft = true;
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
			maxOneLeft = true;
			last = NULL;
		}
		else if(root->next == last)
			maxOneLeft = true;

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
			maxOneLeft = false;
		}
		isEmpty = false;
		m.unlock();
	}
	inline bool IsEmpty()
	{
		return isEmpty;
	}
	inline bool IsMaxOneLeft()
	{
		return maxOneLeft;
	}
};

struct QueryList
{
	QString cmd;
	FIFOQueue<QSqlQuery> queryStack;
};

/**************************************************************//**
@class DM::DBWorker
@ingroup DynaMind-Core
@brief 
A threadclass, asuring asynchron query execution. 
As QT requires
the query to be created and prepared in the same thread as
executed, this also takes place in here.

COMMENTS

******************************************************************/
class DBWorker: public QThread
{
private:
	QMutex queryMutex;

	// flag for leaving run() loop
	bool	kill;
	
	// a stack offering created and prepared queries
	FIFOQueue<QSqlQuery> queryStack;

	// a mutex guaranties the receiver to wait until the worker
	// returns the value (synchronized read)
	QMutex	selectMutex;

	// single select query
	QSqlQuery *qSelect;

	// list of write queries
	std::list<QueryList*> queryLists;


	// to prevent the thread from blocking ressources, a waiter
	// ensures the thread to sleep while no operations are queued
	//QMutex waiterMutex;
	//QWaitCondition waiterCondition;

	// infinite loop to process queries
	// workflow: prepare-sleep if nothing to do-write-read
	// even though it would be fast to wait before read, it may lead to
	// an inconsistent database on small cache sizes or improper
	// data updates
	void run();

	// blocking idle method
	/*void WaitIfNothingToDo()
	{
		waiterMutex.lock();
		waiterCondition.wait(&waiterMutex);
		waiterMutex.unlock();
	}*/
public:
	DBWorker(): QThread()
	{
		qSelect = NULL;
		kill = false;
		selectStatus = 0;
	}
	~DBWorker();
	
	// exchange value between worker <-> receiver
	/*enum SelectStatus
	{
		SS_NOTDONE,
		SS_TRUE,
		SS_FALSE,
	}selectStatus;*/

#define SELECT_NOTDONE 0
#define SELECT_TRUE 1
#define SELECT_FALSE 2
	QAtomicInt selectStatus;
	
	//QMutex selectWaiterMutex;
	//QWaitCondition selectWaiterCondition;

	//QAtomicInt 

	//!< add a new query to the write list (asynchron)
	void addQuery(QSqlQuery *q);
	//!< execute a select query (synchron)
	bool ExecuteSelect(QSqlQuery* q);
	//!< forces the loop to break up
	void Kill()
	{
		kill = true;
	}
	//!< forces the worker to leave idle status
	/*void SignalWork()
	{
		todoCount = todoCount +1;
		//waiterMutex.unlock();
		waiterMutex.lock();
		waiterCondition.wakeOne();
		waiterMutex.unlock();
	}*/
	//!< receive a prepared query for execution
	QSqlQuery *getQuery(QString cmd);
};

/**************************************************************//**
** CACHE SETTINGS
******************************************************************/
// records cache hits and misses, it may decrease performance
#define CACHE_PROFILING 

// raster block size would change the address<->coordinate mapping
// changing will corrupt database!
#define RASTERBLOCKSIZE 64

// castercachesize is initialized when creating a rasterfield
// would require a raster registration class to change the value in runtime
#define RASTERBLOCKCACHESIZE 10000

// edge cache is infinite (Asynchron member)
// component cache is infinite (ComponentSyncMap: Asynchron member)
// face cache is infinite (Asynchron member)

// this flag will prevent the DBConnector to establish any db connection
// greatly improves performance
//#define NO_DB_SYNC

// time in milliseconds to wait, if worker is idle. starts with min, doubles
// at each idle loop tick.
// recommendation 1,128
#define WORKER_SLEEP_TIME_MIN 1
#define WORKER_SLEEP_TIME_MAX 64

// timeinterval in milliseconds to wait for finished select states and new queries
// recommendation 1ms
#define EXE_THREAD_SLEEP_TIME 1


/**************************************************************//**
@class DM::DBConnectorConfig
@ingroup DynaMind-Core
@brief Offers a structure to get and set settings from the  DBConnector.

COMMENTS
The constructor, although its neither performant nor advisable,
ensures a working db connector. The constructor default values
are also applied in the DBConnector constructor.
queryStackSize and cacheBlockwritingSize may not be smaller then 1.
cacheBlockwritingSize may not be smaller than any cache size-1.

******************************************************************/
class DBConnectorConfig
{
public:
	//!< the maximum size of prepared queries
	unsigned long queryStackSize;
	//!< values over 1 enable blocked writes on db
	unsigned long cacheBlockwritingSize;
	//!< size of the attribute cache, values over 1e7 recommended; 0 enables an infinite cache
	unsigned long attributeCacheSize;
	//!< size of the node cache, values over 1e7 recommended; 0 enables an infinite cache
	unsigned long nodeCacheSize;

	DBConnectorConfig()
	{
		queryStackSize = 100;
		cacheBlockwritingSize = 50;
		attributeCacheSize = 0;
		nodeCacheSize = 0;
	}
};

/**************************************************************//**
@class DM::DBConnector
@ingroup DynaMind-Core
@brief Main class to access the database. It offers all neccessary 
operations to create a db and read, write and delete entries in
the db.

COMMENTS
May only be used in the core, for unit tests and for syncronizing
purposes in the simulation class.

INTERNAL WORKFLOW
q = getQuery(string cmd)
q->addBindValue(param)
...
ExecuteQuery(q)/ExecuteSelectQuery(q);
******************************************************************/
class SingletonDestroyer;
class DM_HELPER_DLL_EXPORT DBConnector
{
private:
	// singleton stuff
	static DBConnector* instance;
    DBConnector();

	// destructor stuff
    friend class SingletonDestroyer;
    static SingletonDestroyer _destroyer;

	// worker thread
	static DBWorker* worker;

	// internal init functions
    bool CreateTables();
    bool DropTables();

	// settings
	bool noDBSync;
	unsigned long queryStackSize;
	unsigned long cacheBlockwritingSize;

protected:
    virtual ~DBConnector();
public:
	//!< returns the pointer to the singleton generated instance of DBConnector
    static DBConnector* getInstance();

	//!< get the current configuration, refer to DBConnectorConfig
	DBConnectorConfig getConfig();
	//!< sets a new configuration, it is applied instantly, refer to DBConnectorConfig
	void setConfig(DBConnectorConfig cfg);
	//!< accessor to query stack size, refer to DBConnectorConfig
	unsigned long  GetQueryStackSize()			{return queryStackSize;}
	//!< accessor to cache block writing size, refer to DBConnectorConfig
	unsigned long  GetCacheBlockwritingSize()	{return cacheBlockwritingSize;}
	//!< get a already prepared query. prepare parameters and send back via Execute(Select)Query
    QSqlQuery *getQuery(QString cmd);
	//!< enqueues a WRITE query (asynchron)
    void ExecuteQuery(QSqlQuery *q);
	//!< executes a select query, the value can be accessed via QSqlQuery::value(#)
    bool ExecuteSelectQuery(QSqlQuery *q);

	//!< synchronizes ALL classes with inherited asynchron class
    void Synchronize();
    //!< various insert methods to insert a NEW row in the database
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
    
    //!< various update methods to update an EXISTING row in the database
    void Update(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0);
    void Update(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0,
                                QString parName1, QVariant parValue1);
    void Update(QString table,  QUuid uuid,
                                QString parName0, QVariant parValue0,
                                QString parName1, QVariant parValue1,
                                QString parName2, QVariant parValue2);
    
    //!< delete a database row, existence is not neccessary
    void Delete(QString table,  QUuid uuid);
    //!< various select methods to retrieve row data from database
    bool Select(QString table, QUuid uuid,
                QString valName, QVariant *value);
    bool Select(QString table, QUuid uuid,
                QString valName0, QVariant *value0,
                QString valName1, QVariant *value1);
    bool Select(QString table, QUuid uuid,
                QString valName0, QVariant *value0,
                QString valName1, QVariant *value1,
                QString valName2, QVariant *value2);
};

/**************************************************************//**
@class DM::SingletonDestroyer
@ingroup DynaMind-Core
@brief To ensure a specific order when destructing a singleton,
we implement a destructor class.

COMMENTS
http://libassa.sourceforge.net/libassa-manual/C/x2988.html
******************************************************************/
class SingletonDestroyer
{
    public:
        SingletonDestroyer(DBConnector* = NULL);
        ~SingletonDestroyer();

        void SetSingleton(DBConnector* s);
    private:
        DBConnector* _singleton;
};

/**************************************************************//**
@class DM::Asynchron
@ingroup DynaMind-Core
@brief inheriting this class enables the DbConnector to synchronize 
this element. The registration and deregistration is done in the
constructor and destructor respectively, while the synchronizing
can be implemented via the pure virtual Synchronize method.

COMMENTS
We may use this class for lightweight structures, which should be
synchronized, like Edge (only 2 pointers).
******************************************************************/
class Asynchron
{
public:
    Asynchron();
	~Asynchron();
private:
    virtual void Synchronize() = 0;

friend void DBConnector::Synchronize();
};

/**************************************************************//**
@class DM::Cache
@ingroup DynaMind-Core
@brief a dictionary, with limited size, sorted by access order.
Offers get, add, replace, remove for a key value pair.

COMMENTS
Implemented as highly performant double-ended list without iterator,
NULL pointer checks or similar safty structures. Searching is
accelerated by a std::map with pointers to the desired element.
Each method is optimized to maximum performance in several test
cases (e.g. unit tests). Modify with care.
******************************************************************/
template<class Tkey,class Tvalue>
class Cache
{
protected:
	// double ended node structure
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
	// map for fast searching
	std::map<Tkey,Node*> map;
protected:
    Node*   _root;
    Node*   _last;
    unsigned long    _size;
    unsigned long    _cnt;
	QMutex	mutex;

	// sets up a new node; be aware that no linking is done
	inline Node* newNode(const Tkey &k, Tvalue* v)
	{
		Node* n = new Node(k,v);
		map[k] = n;
		return n;
	}
	// removes a node; does not accoutn for linking
	inline void removeNode(Node* n)
	{
		map.erase(n->key);
		delete pop(n);
	}
	// pushes the given node to the front
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
	// releases the first list node
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
	// search for the node with the given key
    Node* search(const Tkey &key)
    {
        typename std::map<Tkey,Node*>::iterator it = map.find(key);
		if(it==map.end())
			return NULL;
		return it->second;
    }

public:
#ifdef CACHE_PROFILING
	// counters for profiling
    unsigned long hits;
    unsigned long misses;
    void ResetProfilingCounters()
    {
		mutex.lockInline();
        misses = 0;
        hits = 0;
		mutex.unlockInline();
    }
#endif
	//!< initializes a new cache structure with the given maximum size; a size of 0 results in an infinite cache
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
	//!< deletes all nodes, leaves the values untouched (non-deep delete)
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
		map.clear();
    }
	//!< returns the current element count
	unsigned long getSize(){return _size;};
	//!< returns the value associated with the given key 
    virtual Tvalue* get(const Tkey& key)
    {
		mutex.lockInline();
        Node *n = search(key);
        // push front
        if(n!=NULL)
        {
            pop(n);
            push_front(n);
#ifdef CACHE_PROFILING
            hits++;
#endif
			
			mutex.unlockInline();
            return n->value;
        }
#ifdef CACHE_PROFILING
        misses++;
#endif
		mutex.unlockInline();
        return NULL;
    }
	//!< adds a new key-value pair, does nothing if key exists. 
	// If the maximum size is reached, it will remove the last key
    virtual void add(const Tkey& key,Tvalue* value)
    {
		mutex.lockInline();
        if(search(key) != NULL)
		{
			mutex.unlockInline();
            return;
		}

        Node *n = newNode(key,value);
        push_front(n);

		if(_size)
			if(_cnt>_size)
				removeNode(_last);
		
		mutex.unlockInline();
    }
	//!< replaces the value associated with the given key, returns false if key was not existant
    virtual bool replace(const Tkey& key,Tvalue* value)
    {
		mutex.lockInline();
        Node *n = search(key);
        if(n==NULL)
		{
			mutex.unlockInline();
            return false;
		}

		removeNode(n);
        add(key, value);
		mutex.unlockInline();
        return true;
    }
	//!< removes the element from cache
    void remove(const Tkey& key)
    {
		mutex.lockInline();
        Node *n = search(key);
        if(n)	removeNode(n);
		mutex.unlockInline();
    }
};

/**************************************************************//**
@class DM::DbCache
@ingroup DynaMind-Core
@brief similar to DM::Cache, but saves data to database when size
limit is reached and tries to load data from database when element
is not found.
The key class has to implement:
Tkey::SaveToDb(Tvalue);
Tkey::LoadFromDb();

COMMENTS
Inherits DM::Asynchron

******************************************************************/
template<class Tkey,class Tvalue>
class DbCache: public Cache<Tkey,Tvalue>, Asynchron
{
    //!< Save all elements to db, called by DBConnector
    void Synchronize()
    {
#ifdef NO_DB_SYNC
		return
#endif

        Node* n=Cache<Tkey,Tvalue>::_root;
        while(n)
        {
            Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
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
		mutex.lockInline();
        if(Cache<Tkey,Tvalue>::search(key)!=NULL)
		{
			mutex.unlockInline();
            return;
		}

        Node *n = Cache<Tkey,Tvalue>::newNode(key,value);
        Cache<Tkey,Tvalue>::push_front(n);

        if(Cache<Tkey,Tvalue>::_size)
		{
			if(Cache<Tkey,Tvalue>::_cnt > Cache<Tkey,Tvalue>::_size)
			{
				for(int i=0;i<DBConnector::getInstance()->GetCacheBlockwritingSize() 
					&& Cache<Tkey,Tvalue>::_cnt>1;i++)
				{
					Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
					Cache<Tkey,Tvalue>::removeNode(Cache<Tkey,Tvalue>::_last);
				}
			}
		}
		mutex.unlockInline();
    }
    //!< returns the value associated with the given key, if not found LoadFromDb is called. Neither found in db, returns NULL
    Tvalue* get(const Tkey& key)
    {
		mutex.lockInline();
        Tvalue* v = Cache<Tkey,Tvalue>::get(key);
        if(!v)
        {
            v = key->LoadFromDb();
            if(v)   add(key,v);
        }
		mutex.unlockInline();
        return v;
    }
	// NOTE: currently removing from db is handled by the main class
    // void remove(const Tkey& key)

	//<! resizes the cache, if the given value is 0, the cache is set to infinite
	void resize(unsigned long size)
	{
		mutex.lockInline();
		Cache<Tkey,Tvalue>::_size = size;

        if(Cache<Tkey,Tvalue>::_size)
		{
			while(Cache<Tkey,Tvalue>::_cnt > size)
			{
				Cache<Tkey,Tvalue>::_last->key->SaveToDb(Cache<Tkey,Tvalue>::_last->value);
				Cache<Tkey,Tvalue>::removeNode(Cache<Tkey,Tvalue>::_last);
			}
		}
		mutex.unlockInline();
	}
};

}   // namespace DM


#endif
