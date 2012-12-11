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


DBConnector* DBConnector::instance = 0;
//int DBConnector::_linkID = 1;
QMap<QString,QSqlQuery*> DBConnector::mapQuery;
bool DBConnector::_bTransaction = false;
QSqlDatabase* DBConnector::_db = new QSqlDatabase();
SingletonDestroyer DBConnector::_destroyer;

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
                                            y BIGINT, data BYTEA, \
                                            PRIMARY KEY (owner,y))")
    && query.exec("CREATE TABLE attributes(uuid BINARY(16) NOT NULL, \
                                            owner BINARY(16), \
                                            name VARCHAR(128), \
                                            type SMALLINT, \
                                            value BYTEA, \
                                            PRIMARY KEY (uuid))")
    /*&& query.exec("CREATE UNIQUE INDEX idx_systems ON systems(uuid, stateuuid)")
    && query.exec("CREATE UNIQUE INDEX idx_components ON components(uuid, stateuuid)")
    && query.exec("CREATE UNIQUE INDEX idx_nodes ON nodes(uuid, stateuuid)")
    && query.exec("CREATE UNIQUE INDEX idx_edges ON edges(uuid, stateuuid)")
    && query.exec("CREATE UNIQUE INDEX idx_faces ON faces(uuid, stateuuid)")
    && query.exec("CREATE UNIQUE INDEX idx_rasterdatas ON rasterdatas(uuid, stateuuid)")
    && query.exec("CREATE UNIQUE INDEX idx_rasterfields ON rasterfields(datalink, x, y)")
    && query.exec("CREATE UNIQUE INDEX idx_attributes ON attributes(uuid)")*/
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
    /*&&	query.exec("DROP INDEX IF EXISTS idx_systems")
    &&	query.exec("DROP INDEX IF EXISTS idx_components")
    &&	query.exec("DROP INDEX IF EXISTS idx_nodes")
    &&	query.exec("DROP INDEX IF EXISTS idx_edges")
    &&	query.exec("DROP INDEX IF EXISTS idx_faces")
    &&	query.exec("DROP INDEX IF EXISTS idx_rasterdatas")
    &&	query.exec("DROP INDEX IF EXISTS idx_rasterfields")
    &&	query.exec("DROP INDEX IF EXISTS idx_attributes")*/
            )
        return true;

    PrintSqlError(&query);
    return false;
}

DBConnector::DBConnector()
{
    if(QFile::exists("testdb"))
        QFile::remove("testdb");

    *_db = QSqlDatabase::addDatabase("QSQLITE");
    //_db.setDatabaseName(":memory:");
    _db->setDatabaseName("testdb");
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
    db.setPassword("this00");
*/
    /*
    //QString connectionString = "SERVER=localhost;DATABASE=dynamind;";
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("dynamind");
    db.setUserName("postgres");
    db.setPassword("this00");
*/
    if(!_db->open())
	{
		Logger(Error) << "Failed to open db connection";
		
        QSqlError e = _db->lastError();
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
        _db->removeDatabase("QSQLITE");
        _db->close();
		return;
	}

	Logger(Debug) << "DB created";
}
DBConnector::~DBConnector()
{
    // commit (destructor called sql deletes may exist!)
    CommitTransaction();
    //for(QMap<QString,QSqlQuery*>::const_iterator it = mapQuery.begin(); it != mapQuery.end(); ++it)
    //    delete it;
    delete _db;
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
/*int DBConnector::GetNewLinkID()
{
    return DBConnector::_linkID++;
}*/

QSqlQuery* DBConnector::getQuery(QString cmd)
{
    if(DBConnector::mapQuery.find(cmd)!=DBConnector::mapQuery.end())
        return DBConnector::mapQuery[cmd];
    else
    {
        QSqlQuery *q = new QSqlQuery();
        q->prepare(cmd);
        DBConnector::mapQuery[cmd] = q;
        return q;
    }
}
void DBConnector::ExecuteQuery(QSqlQuery *q)
{   
    this->BeginTransaction();

    if(!q->exec())	PrintSqlError(q);
}

bool DBConnector::ExecuteSelectQuery(QSqlQuery *q)
{

    if(_bTransaction)
        this->CommitTransaction();

    if(!q->exec() || !q->next())
    {
        PrintSqlError(q);
        return false;
    }
    return true;
}

void DBConnector::BeginTransaction()
{
    if(!_bTransaction)
    {
        _bTransaction = true;
        _db->transaction();
    }
    //QSqlQuery q;
    //if(!q.exec("BEGIN TRANSACTION"))
    //    PrintSqlError(&q);
}

void DBConnector::CommitTransaction()
{
    if(_bTransaction)
    {
        _bTransaction = false;
        if(!_db->commit())
        //QSqlQuery q;
        //if(!q.exec("END TRANSACTION"))
        {
            Logger(Error) << "rolling back commit";
            PrintSqlErrorE(_db->lastError());
            if(_db->rollback())
                Logger(Error) << "rollback failed";
        }
    }
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
