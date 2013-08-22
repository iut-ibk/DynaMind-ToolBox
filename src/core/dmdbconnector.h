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
#include <dmstdutilities.h>
#include <qobject.h>
#include <queue>
#include <qatomic.h>
#include "dmcache.h"

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
	QMutex	clientSelectMutex;

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
public:
	DBWorker(): QThread()
	{
		qSelect = NULL;
		kill = false;
		selectStatus = 0;
	}
	~DBWorker();

#define SELECT_NOTDONE 0
//#define SELECT_TRUE 1
#define SELECT_FALSE -1
	QAtomicInt selectStatus;
    QList< QList<QVariant> >	selectRows;

	//!< add a new query to the write list (asynchron)
	void addQuery(QSqlQuery *q);
	//!< execute a select query (synchron)
	bool ExecuteSelect(QSqlQuery* q);
	//!< forces the loop to break up
	void Kill()
	{
		kill = true;
	}
	//!< receive a prepared query for execution
	QSqlQuery *getQuery(QString cmd);
};

/**************************************************************//**
** CACHE SETTINGS
******************************************************************/

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

// timeinterval in microseconds to wait for finished select states and new queries
// recommendation 1us
#define EXE_THREAD_SLEEP_TIME 1

// when e.g. precaching or sleecting large junks of data via uuids, we may exceed 
// the parameter count limit. 100 seems to work fine in all db products.
#define SQLBLOCKQUERYSIZE 100


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

	static void initWorker();
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

	QList<QList<QVariant> >* getResults();
	void killWorker();

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

	bool Select(QString table, const QList<QUuid*>& uuids, QList<QUuid>* resultUuids,
		QString valName0, QList<QVariant> *value0,
		QString valName1, QList<QVariant> *value1,
		QString valName2, QList<QVariant> *value2);
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

}   // namespace DM


#endif
