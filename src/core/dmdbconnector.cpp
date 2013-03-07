/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011 Markus Sengthaler

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

#include <dmlogger.h>

#include <dmsystem.h>
#include <dmcomponent.h>
#include <dmdbconnector.h>

#include <QSqlDatabase>
#include <qsqlquery.h>
#include <QSqlRecord>
#include <QVariant>
#include <QtCore>
#include <QSqlError>

using namespace DM;

void PrintSqlErrorE(QSqlError e)
{
    Logger(Error) << "sql error: " << e.text();
}

void DM::PrintSqlError(QSqlQuery *q)
{
    Logger(Error) << "last cmd: " << q->lastQuery();
    PrintSqlErrorE(q->lastError());
}


SingletonDestroyer::SingletonDestroyer (DBConnector *s) {
    _singleton = s;
}

SingletonDestroyer::~SingletonDestroyer () {
    delete _singleton;
}

void SingletonDestroyer::SetSingleton (DBConnector* s) {
    _singleton = s;
}

static std::list<Asynchron*>* syncList = NULL;
Asynchron::Asynchron()
{
	if(!syncList)	syncList = new std::list<Asynchron*>();

    syncList->push_back(this);
}
Asynchron::~Asynchron()
{
	if(syncList)
		syncList->remove(this);
}

DBConnector* DBConnector::instance = 0;
//int DBConnector::_linkID = 1;
//QMap<QString,QSqlQuery*> DBConnector::mapQuery;
bool DBConnector::_bTransaction = false;
SingletonDestroyer DBConnector::_destroyer;
//QSqlDatabase DBConnector::_db;
DBWorker* DBConnector::worker = NULL;
//QThread DBConnector::workerThread;

QSqlDatabase getDatabase() { return QSqlDatabase::database(); }

bool DBConnector::CreateTables()
{
    QSqlQuery query;

    if(        query.exec("PRAGMA synchronous=OFF")
               && query.exec("PRAGMA count_changes=OFF")
              // && query.exec("PRAGMA page_size=5120")
            && query.exec("PRAGMA journal_mode=OFF")
            && query.exec("PRAGMA temp_store=OFF")
           // && query.exec("PRAGMA cache_size=50000")
    && query.exec("CREATE TABLE systems(	uuid BINARY(16) NOT NULL, \
                                            owner BINARY16, \
                                            predecessors TEXT, \
                                            sucessors TEXT, \
                                            PRIMARY KEY (uuid))")
    && query.exec("CREATE TABLE components(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            PRIMARY KEY (uuid))")
    && query.exec("CREATE TABLE nodes(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            x DOUBLE PRECISION, y DOUBLE PRECISION, z DOUBLE PRECISION, \
                                            PRIMARY KEY (uuid))")
    && query.exec("CREATE TABLE edges(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            startnode BINARY(16), \
                                            endnode BINARY(16), \
                                            PRIMARY KEY (uuid))")
    && query.exec("CREATE TABLE faces(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            nodes TEXT, \
                                            holes TEXT, \
                                            PRIMARY KEY (uuid))")
    && query.exec("CREATE TABLE rasterdatas(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            datalink INT,\
                                            PRIMARY KEY (uuid))")
               && query.exec("CREATE TABLE rasterfields(owner BINARY(16) NOT NULL, \
                                            x BIGINT, \
                                            y BIGINT, \
											data BYTEA, \
                                            PRIMARY KEY (owner,x,y))")
    && query.exec("CREATE TABLE attributes(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            name VARCHAR(128), \
                                            type SMALLINT, \
                                            value BYTEA, \
                                            PRIMARY KEY (uuid))")
    )
        return true;

    PrintSqlError(&query);
    return false;
}

bool DBConnector::DropTables()
{
    QSqlQuery query;
    if(	query.exec("DROP TABLE IF EXISTS systems")
    &&	query.exec("DROP TABLE IF EXISTS components")
    &&	query.exec("DROP TABLE IF EXISTS nodes")
    &&	query.exec("DROP TABLE IF EXISTS edges")
    &&	query.exec("DROP TABLE IF EXISTS faces")
    &&	query.exec("DROP TABLE IF EXISTS rasterdatas")
    &&	query.exec("DROP TABLE IF EXISTS rasterfields")
    &&	query.exec("DROP TABLE IF EXISTS attributes")
            )
        return true;

    PrintSqlError(&query);
    return false;
}

DBConnector::DBConnector()
{
	QString dbpath = QDir::tempPath() + "/dynaminddb";
	std::string str = dbpath.toStdString();

    if(QFile::exists(dbpath))
        QFile::remove(dbpath);
	//_db = new QSqlDatabase();
    QSqlDatabase _db = QSqlDatabase::addDatabase("QSQLITE");
    //_db.setDatabaseName(":memory:");
    _db.setDatabaseName(dbpath);
/*
	QString connectionString = "DRIVER={MySQL ODBC 5.2w Driver};SERVER=localhost;DATABASE=dynamind;";
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(connectionString);
    db.setUserName("root");
    db.setPassword("");
*/
/*
    QString connectionString = "DRIVER={PostgreSQL Unicode(x64)};SERVER=localhost;DATABASE=dynamind;";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(connectionString);
    db.setUserName("postgres");
    db.setPassword("");
*/
    /*
    //QString connectionString = "SERVER=localhost;DATABASE=dynamind;";
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("dynamind");
    db.setUserName("postgres");
    db.setPassword("");
*/
    if(!_db.open())
	{
		Logger(Error) << "Failed to open db connection";
		
        QSqlError e = _db.lastError();
		Logger(Error) << "driver error: " << e.driverText();
		Logger(Error) << "database error: " << e.databaseText();
        return;
	}
	else
		Logger(Debug) << "Db connection opened";

    // init table structure
    if(!DropTables() || !CreateTables())
	{
        Logger(Error) << "Cannot initialize db tables";
        DropTables();
        //db.removeDatabase("QODBC");
        _db.removeDatabase("QSQLITE");
        _db.close();
		return;
	}

	Logger(Debug) << "DB created";

	worker = new DBWorker();
	worker->start();
}
//#define DBWORKER_COUNTERS
#ifdef DBWORKER_COUNTERS
unsigned long loopCount = 0;
unsigned long writeCount = 0;
unsigned long readCount = 0;
unsigned long writesBeforeReadsCount = 0;
#endif

void DBWorker::run()
{
	std::list<QueryList*>::iterator it;
	QueryList* ql;
	while(!kill)
	{
		for(it = queryLists.begin(); it != queryLists.end(); ++it)
		//foreach(QueryList* ql, queryLists)
		{
			ql = *it;
			if(ql->queryStack.IsMaxOneLeft())
			{
				for(int i=0;i<SQLQUERY_STACKSIZE;i++)
				{
					QSqlQuery* q = new QSqlQuery();
					q->prepare(ql->cmd);
					ql->queryStack.push(q);
				}
			}
		}
#ifdef DBWORKER_COUNTERS
		loopCount++;
#endif
		unsigned long cnt = 0;
		while(QSqlQuery* q = queryStack.pop())
		{
#ifdef DBWORKER_COUNTERS
			writeCount++;
			cnt++;
#endif
			if(!q->exec())	PrintSqlError(q);
			delete q;
		}
		if(qSelect)
		{
#ifdef DBWORKER_COUNTERS
			writesBeforeReadsCount += cnt;
			readCount++;
#endif
			selectMutex.lock();
			selectStatus = (!qSelect->exec() || !qSelect->next())?SS_FALSE:SS_TRUE;
			qSelect = NULL;
			selectMutex.unlock();
		}
	}
	exec();
}

void DBWorker::addQuery(QSqlQuery *q)
{
	queryStack.push(q);
}

bool DBWorker::ExecuteSelect(QSqlQuery *q)
{
	selectMutex.lock();
	qSelect = q;
	selectStatus = SS_NOTDONE;
	selectMutex.unlock();

	while(selectStatus == SS_NOTDONE)
		;
	return selectStatus==SS_TRUE;
}

QSqlQuery* DBWorker::getQuery(QString cmd)
{
	QueryList* ql = NULL;
	// search for query list
	foreach(QueryList* it, queryLists)
	{
		if(it->cmd == cmd)
		{
			ql = it;
			break;
		}
	}
	if(!ql)
	{
		// no list found, create
		ql = new QueryList;
		ql->cmd = cmd;
		queryLists.push_back(ql);
	}

	QSqlQuery *q = NULL;
	while(!(q = ql->queryStack.pop()))
		;
	return q;
}


DBWorker::~DBWorker()
{
	selectMutex.unlock();
	getDatabase();
	foreach(QueryList* it, queryLists)
		delete it;
}

DBConnector::~DBConnector()
{
	worker->Kill();
	worker->terminate();
	delete worker;
}

DBConnector* DBConnector::getInstance()
{
	if(!DBConnector::instance)
    {
		DBConnector::instance = new DBConnector();
        _destroyer.SetSingleton(DBConnector::instance);
    }
	return DBConnector::instance;
}
QSqlQuery* DBConnector::getQuery(QString cmd)
{
	return worker->getQuery(cmd);
	/*
	QSqlQuery *q = NULL;
	while(!(q = worker->queryNewStack.pop()))
		;
	q->prepare(cmd);
	return q;*/
}
void DBConnector::ExecuteQuery(QSqlQuery *q)
{   
	// submit to worker
	worker->addQuery(q);
}

bool DBConnector::ExecuteSelectQuery(QSqlQuery *q)
{
	return worker->ExecuteSelect(q);
}

void DBConnector::Synchronize()
{
    foreach(Asynchron *a, *syncList)
        a->Synchronize();
}

/*
 *  INSERT with uuid
 */
void DBConnector::Insert(QString table, QUuid uuid)
{
    QSqlQuery *q = getQuery("INSERT INTO "+table+" (uuid) VALUES (?)");
    q->addBindValue(uuid.toByteArray());
    this->ExecuteQuery(q);
}
void DBConnector::Insert(QString table,  QUuid uuid,
                         QString parName0, QVariant parValue0)
{
    QSqlQuery *q = getQuery("INSERT INTO "+table+" (uuid,"+
                            parName0+") VALUES (?,?)");
    q->addBindValue(uuid.toByteArray());
    q->addBindValue(parValue0);
    this->ExecuteQuery(q);
}
void DBConnector::Insert(QString table,  QUuid uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1)
 {
     QSqlQuery *q = getQuery("INSERT INTO "+table+" (uuid,"+
                             parName0+","+
                             parName1+") VALUES (?,?,?)");
     q->addBindValue(uuid.toByteArray());
     q->addBindValue(parValue0);
     q->addBindValue(parValue1);
     this->ExecuteQuery(q);
 }
void DBConnector::Insert(QString table,  QUuid uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1,
                         QString parName2, QVariant parValue2)
 {
     QSqlQuery *q = getQuery("INSERT INTO "+table+" (uuid,"+
                             parName0+","+
                             parName1+","+
                             parName2+") VALUES (?,?,?,?)");
     q->addBindValue(uuid.toByteArray());
     q->addBindValue(parValue0);
     q->addBindValue(parValue1);
     q->addBindValue(parValue2);
     this->ExecuteQuery(q);
 }
/*
 *  DELETE with uuid
 */
void DBConnector::Delete(QString table,  QUuid uuid)
{
    QSqlQuery *q = getQuery("DELETE FROM "+table+" WHERE uuid LIKE ?");
    q->addBindValue(uuid.toByteArray());
    this->ExecuteQuery(q);
}
/*
 *  UPDATE with uuid
 */
void DBConnector::Update(QString table,  QUuid uuid,
                         QString parName0, QVariant parValue0)
{
    QSqlQuery *q = getQuery("UPDATE "+table+" SET "+parName0+"=? WHERE uuid LIKE ?");
    q->addBindValue(parValue0);
    q->addBindValue(uuid.toByteArray());
    this->ExecuteQuery(q);
}

void DBConnector::Update(QString table,  QUuid uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1)
{
    QSqlQuery *q = getQuery("UPDATE "+table+" SET "
                            +parName0+"=?,"
                            +parName1+"=? WHERE uuid LIKE ?");
    q->addBindValue(parValue0);
    q->addBindValue(parValue1);
    q->addBindValue(uuid.toByteArray());
    this->ExecuteQuery(q);
}
void DBConnector::Update(QString table,  QUuid uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1,
                         QString parName2, QVariant parValue2)
{
    QSqlQuery *q = getQuery("UPDATE "+table+" SET "
                            +parName0+"=?,"
                            +parName1+"=?,"
                            +parName2+"=? WHERE uuid LIKE ?");
    q->addBindValue(parValue0);
    q->addBindValue(parValue1);
    q->addBindValue(parValue2);
    q->addBindValue(uuid.toByteArray());
    this->ExecuteQuery(q);
}
/*
 *  SELECT single entry with uuid
 */
bool DBConnector::Select(QString table, QUuid uuid,
                         QString valName, QVariant *value)
{
    QSqlQuery *q = getQuery("SELECT "+valName+" FROM "+table+" WHERE uuid LIKE ?");
    q->addBindValue(uuid.toByteArray());

    if(!ExecuteSelectQuery(q))
        return false;
    *value = q->value(0);
    return true;
}
bool DBConnector::Select(QString table, QUuid uuid,
                         QString valName0, QVariant *value0,
                         QString valName1, QVariant *value1)
 {
     QSqlQuery *q = getQuery("SELECT "+valName0+","+valName1+" FROM "+table+" WHERE uuid LIKE ?");
     q->addBindValue(uuid.toByteArray());
     if(!ExecuteSelectQuery(q))
         return false;

     *value0 = q->value(0);
     *value1 = q->value(1);
     return true;
 }
bool DBConnector::Select(QString table, QUuid uuid,
                         QString valName0, QVariant *value0,
                         QString valName1, QVariant *value1,
                         QString valName2, QVariant *value2)
 {
     QSqlQuery *q = getQuery("SELECT "+valName0+","+valName1+","+valName2+" FROM "+table+" WHERE uuid LIKE ?");
     q->addBindValue(uuid.toByteArray());
     if(!ExecuteSelectQuery(q))
         return false;

     *value0 = q->value(0);
     *value1 = q->value(1);
     *value2 = q->value(2);
     return true;
 }
