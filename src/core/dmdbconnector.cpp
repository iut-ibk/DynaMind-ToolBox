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
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QtCore>
#include <QSqlError>

	 //#include "qtdir/src/sql/drivers/psql/qsql_psql.cpp"

using namespace DM;

void DM::PrintSqlError(QSqlQuery *q)
{
	QSqlError e = q->lastError();
	Logger(Error) << "last cmd: " << q->lastQuery();
	Logger(Error) << "sql error: " << e.text();
}


DBConnector* DBConnector::instance = 0;
int DBConnector::_linkID = 1;
QMap<QString,QSqlQuery*> DBConnector::mapQuery;


DBConnector::DBConnector()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    //db.setDatabaseName("testdb");
	/*
	QString connectionString = "DRIVER={MySQL ODBC 5.2w Driver};SERVER=localhost;DATABASE=dynamind;";
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(connectionString);
    db.setUserName("root");
    db.setPassword("");
	*/
/*
    QString connectionString = "DRIVER={PostgreSQL Unicode};SERVER=localhost;DATABASE=dynamind;";
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(connectionString);
    db.setUserName("postgres");
    db.setPassword("this00");
*/
	if(!db.open())
	{
		Logger(Error) << "Failed to open db connection";
		
		QSqlError e = db.lastError();
		Logger(Error) << "driver error: " << e.driverText();
		Logger(Error) << "database error: " << e.databaseText();
	}
	else
		Logger(Debug) << "Db connection opened";

	// init table structure
	QSqlQuery query;
	if(	!query.exec("DROP TABLE IF EXISTS systems")
	||	!query.exec("DROP TABLE IF EXISTS components")
	||	!query.exec("DROP TABLE IF EXISTS nodes")
	||	!query.exec("DROP TABLE IF EXISTS edges")
	||	!query.exec("DROP TABLE IF EXISTS faces")
	||	!query.exec("DROP TABLE IF EXISTS rasterdatas")
	||	!query.exec("DROP TABLE IF EXISTS rasterfields")
	||	!query.exec("DROP TABLE IF EXISTS attributes")
    ||	!query.exec("CREATE TABLE systems(	uuid VARCHAR(128) NOT NULL, \
                                            stateuuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
											name text, \
											predecessors text, \
											sucessors text, \
											PRIMARY KEY (uuid,stateuuid))")
    || !query.exec("CREATE TABLE components(uuid VARCHAR(128) NOT NULL, \
                                            stateuuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
											name text, \
											PRIMARY KEY (uuid,stateuuid))")
    || !query.exec("CREATE TABLE nodes(uuid VARCHAR(128) NOT NULL, \
                                            stateuuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
											name text, \
											x DOUBLE PRECISION, y DOUBLE PRECISION, z DOUBLE PRECISION, \
											PRIMARY KEY (uuid,stateuuid))")
    || !query.exec("CREATE TABLE edges(uuid VARCHAR(128) NOT NULL, \
                                            stateuuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
											name text, \
                                            startnode VARCHAR(128), \
                                            endnode VARCHAR(128), \
											PRIMARY KEY (uuid,stateuuid))")
    || !query.exec("CREATE TABLE faces(uuid VARCHAR(128) NOT NULL, \
                                            stateuuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
											name text, \
											nodes text, \
											holes text, \
											PRIMARY KEY (uuid,stateuuid))")
    || !query.exec("CREATE TABLE rasterdatas(uuid VARCHAR(128) NOT NULL, \
                                            stateuuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
											name text, \
											datalink int,\
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE rasterfields(datalink int NOT NULL, \
											x bigint, y bigint, value DOUBLE PRECISION, \
											PRIMARY KEY (datalink,x,y))")
    || !query.exec("CREATE TABLE attributes(uuid VARCHAR(128) NOT NULL, \
                                            owner VARCHAR(128), \
                                            stateuuid VARCHAR(128), \
                                            name VARCHAR(128), \
                                            type smallint, \
                                            value bytea, \
                                            PRIMARY KEY (uuid))")
		)
	{
        Logger(Error) << "Cannot initialize db tables";
		PrintSqlError(&query);

		QSqlQuery q;
		q.prepare("DROP TABLE IF EXISTS systems, components, attributes");
		if(!q.exec())	PrintSqlError(&q);

        //db.removeDatabase("QODBC");
        db.removeDatabase("QSQLITE");
		db.close();
		return;
	}

	Logger(Debug) << "DB created";
}
DM::DBConnector::~DBConnector()
{

    for(QMap<QString,QSqlQuery*>::const_iterator it = mapQuery.begin(); it != mapQuery.end(); ++it)
        delete it;
}

DM::DBConnector* DBConnector::getInstance()
{
	if(!DBConnector::instance)
		DBConnector::instance = new DBConnector();
	return DBConnector::instance;
}
int DBConnector::GetNewLinkID()
{
    return DBConnector::_linkID++;
}

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


/*
 *  INSERT with uuid
 */
void DBConnector::Insert(QString table, QString uuid)
{
    QSqlQuery q;
    q.prepare("INSERT INTO "+table+" (uuid) VALUES (?)");
    q.addBindValue(uuid);
    if(!q.exec())	PrintSqlError(&q);
}
void DBConnector::Insert(QString table,  QString uuid,
                         QString parName0, QVariant parValue0)
{
    QSqlQuery q;
    q.prepare("INSERT INTO "+table+" (uuid,"+
              parName0+") VALUES (?,?)");
    q.addBindValue(uuid);
    q.addBindValue(parValue0);
    if(!q.exec())	PrintSqlError(&q);
}
void DBConnector::Insert(QString table,  QString uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1)
 {
     QSqlQuery q;
     q.prepare("INSERT INTO "+table+" (uuid,"+
               parName0+","+
               parName1+") VALUES (?,?,?)");
     q.addBindValue(uuid);
     q.addBindValue(parValue0);
     q.addBindValue(parValue1);
     if(!q.exec())	PrintSqlError(&q);
 }
/*
 *  INSERT with uuid and stateuuid
 */
void DBConnector::Insert(QString table, QString uuid, QString stateUuid)
{
    QSqlQuery q;
    q.prepare("INSERT INTO "+table+" (uuid,stateuuid) VALUES (?,?)");
    q.addBindValue(uuid);
    q.addBindValue(stateUuid);
    if(!q.exec())	PrintSqlError(&q);
}
void DBConnector::Insert(QString table,  QString uuid, QString stateUuid,
                         QString parName0, QVariant parValue0)
{
    QSqlQuery q;
    q.prepare("INSERT INTO "+table+" (uuid,stateuuid,"+
              parName0+") VALUES (?,?,?)");
    q.addBindValue(uuid);
    q.addBindValue(stateUuid);
    q.addBindValue(parValue0);
    if(!q.exec())	PrintSqlError(&q);
}
void DBConnector::Insert(QString table,  QString uuid, QString stateUuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1)
 {
     QSqlQuery q;
     q.prepare("INSERT INTO "+table+" (uuid,stateuuid,"+
               parName0+","+
               parName1+") VALUES (?,?,?,?)");
     q.addBindValue(uuid);
     q.addBindValue(stateUuid);
     q.addBindValue(parValue0);
     q.addBindValue(parValue1);
     if(!q.exec())	PrintSqlError(&q);
 }
void DBConnector::Insert(QString table,  QString uuid, QString stateUuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1,
                         QString parName2, QVariant parValue2)
 {
     QSqlQuery q;
     q.prepare("INSERT INTO "+table+" (uuid,stateuuid,"+
               parName0+","+
               parName1+","+
               parName2+") VALUES (?,?,?,?,?)");
     q.addBindValue(uuid);
     q.addBindValue(stateUuid);
     q.addBindValue(parValue0);
     q.addBindValue(parValue1);
     q.addBindValue(parValue2);
     if(!q.exec())	PrintSqlError(&q);
 }
/*
 *  DELETE with uuid
 */
void DBConnector::Delete(QString table,  QString uuid)
{
    /*QSqlQuery *q = getQuery("DELETE FROM "+table+" WHERE uuid LIKE ?");
    q->addBindValue(uuid);
    if(!q->exec())	PrintSqlError(q);*/
    QSqlQuery q;
    q.prepare("DELETE FROM "+table+" WHERE uuid LIKE ?");
    q.addBindValue(uuid);
    if(!q.exec())	PrintSqlError(&q);
}
/*
 *  DELETE with uuid and stateuuid
 */
void DBConnector::Delete(QString table,  QString uuid, QString stateUuid)
{
    QSqlQuery q;
    q.prepare("DELETE FROM "+table+" WHERE uuid LIKE ? AND stateuuid LIKE ?");
    q.addBindValue(uuid);
    q.addBindValue(stateUuid);
    if(!q.exec())	PrintSqlError(&q);
    /*QSqlQuery *q = getQuery("DELETE FROM "+table+" WHERE uuid LIKE ? AND stateuuid LIKE ?");
    q->addBindValue(uuid);
    q->addBindValue(stateUuid);
    if(!q->exec())	PrintSqlError(q);*/
}
/*
 *  UPDATE with uuid
 */
void DBConnector::Update(QString table,  QString uuid,
                         QString parName0, QVariant parValue0)
{
    QSqlQuery q;
    q.prepare("UPDATE "+table+" SET "+parName0+"=? WHERE uuid LIKE ?");
    q.addBindValue(parValue0);
    q.addBindValue(uuid);
    if(!q.exec())	PrintSqlError(&q);
}

void DBConnector::Update(QString table,  QString uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1)
{
    QSqlQuery q;
    q.prepare("UPDATE "+table+" SET "
              +parName0+"=?,"
              +parName1+"=? WHERE uuid LIKE ?");
    q.addBindValue(parValue0);
    q.addBindValue(parValue1);
    q.addBindValue(uuid);
    if(!q.exec())	PrintSqlError(&q);
}
void DBConnector::Update(QString table,  QString uuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1,
                         QString parName2, QVariant parValue2)
{
    QSqlQuery q;
    q.prepare("UPDATE "+table+" SET "
              +parName0+"=?,"
              +parName1+"=?,"
              +parName2+"=? WHERE uuid LIKE ?");
    q.addBindValue(parValue0);
    q.addBindValue(parValue1);
    q.addBindValue(parValue2);
    q.addBindValue(uuid);
    if(!q.exec())	PrintSqlError(&q);
}
/*
 *  UPDATE with uuid and stateuuid
 */
void DBConnector::Update(QString table,  QString uuid, QString stateUuid,
                         QString parName0, QVariant parValue0)
{
    QSqlQuery q;
    q.prepare("UPDATE "+table+" SET "+parName0+"=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
    q.addBindValue(parValue0);
    q.addBindValue(uuid);
    q.addBindValue(stateUuid);
    if(!q.exec())	PrintSqlError(&q);
}

void DBConnector::Update(QString table,  QString uuid, QString stateUuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1)
{
    QSqlQuery q;
    q.prepare("UPDATE "+table+" SET "
              +parName0+"=?,"
              +parName1+"=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
    q.addBindValue(parValue0);
    q.addBindValue(parValue1);
    q.addBindValue(uuid);
    q.addBindValue(stateUuid);
    if(!q.exec())	PrintSqlError(&q);
}
void DBConnector::Update(QString table,  QString uuid, QString stateUuid,
                         QString parName0, QVariant parValue0,
                         QString parName1, QVariant parValue1,
                         QString parName2, QVariant parValue2)
{
    QSqlQuery q;
    q.prepare("UPDATE "+table+" SET "
              +parName0+"=?,"
              +parName1+"=?,"
              +parName2+"=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
    q.addBindValue(parValue0);
    q.addBindValue(parValue1);
    q.addBindValue(parValue2);
    q.addBindValue(uuid);
    q.addBindValue(stateUuid);
    if(!q.exec())	PrintSqlError(&q);
}
/*
 *  SELECT single entry with uuid
 */
bool DBConnector::Select(QString table, QString uuid,
                         QString valName, QVariant *value)
{
    QSqlQuery q;
    q.prepare("SELECT "+valName+" FROM "+table+" WHERE uuid LIKE ?");
    q.addBindValue(uuid);
    if(!q.exec() || !q.next())
    {
        PrintSqlError(&q);
        return false;
    }
    *value = q.value(0);
    return true;
}
bool DBConnector::Select(QString table, QString uuid,
                         QString valName0, QVariant *value0,
                         QString valName1, QVariant *value1)
 {
     QSqlQuery q;
     q.prepare("SELECT "+valName0+","+valName1+" FROM "+table+" WHERE uuid LIKE ?");
     q.addBindValue(uuid);
     if(!q.exec() || !q.next())
     {
         PrintSqlError(&q);
         return false;
     }
     *value0 = q.value(0);
     *value1 = q.value(1);
     return true;
 }
/*
 *  SELECT single entry with uuid and stateuuid
 */
bool DBConnector::Select(QString table, QString uuid, QString stateuuid,
                         QString valName, QVariant *value)
{
    QSqlQuery q;
    q.prepare("SELECT "+valName+" FROM "+table+" WHERE uuid LIKE ? AND stateuuid LIKE ?");
    q.addBindValue(uuid);
    q.addBindValue(stateuuid);
    if(!q.exec() || !q.next())
    {
        PrintSqlError(&q);
        return false;
    }
    *value = q.value(0);
    return true;
}
bool DBConnector::Select(QString table, QString uuid, QString stateuuid,
                         QString valName0, QVariant *value0,
                         QString valName1, QVariant *value1)
 {
     QSqlQuery q;
     q.prepare("SELECT "+valName0+","+valName1+" FROM "+table+" WHERE uuid LIKE ? AND stateuuid LIKE ?");
     q.addBindValue(uuid);
     q.addBindValue(stateuuid);
     if(!q.exec() || !q.next())
     {
         PrintSqlError(&q);
         return false;
     }
     *value0 = q.value(0);
     *value1 = q.value(1);
     return true;
 }
bool DBConnector::Select(QString table, QString uuid, QString stateuuid,
                         QString valName0, QVariant *value0,
                         QString valName1, QVariant *value1,
                         QString valName2, QVariant *value2)
   {
       QSqlQuery q;
       q.prepare("SELECT "+valName0+","+valName1+","+valName2+" FROM "+table+" WHERE uuid LIKE ? AND stateuuid LIKE ?");
       q.addBindValue(uuid);
       q.addBindValue(stateuuid);
       if(!q.exec() || !q.next())
       {
           PrintSqlError(&q);
           return false;
       }
       *value0 = q.value(0);
       *value1 = q.value(1);
       *value2 = q.value(2);
       return true;
   }

/*
void DBConnector::InsertX(std::string table, ...)
{
    QString strParams;
    QString strValues;
    QVariant qValue;
    QSqlQuery q;

    va_list params;
    va_start( params, table );

    QVariant name = va_arg(params, QVariant);
    QVariant value = va_arg(params, QVariant);



    while(name != NULL && value != NULL)
    {
        strParams += name.toString() + ",";
        strValues += "?,";

        name = va_arg(params, QVariant);
        value = va_arg(params, QVariant);
    }

    va_end( params );

    strParams.chop(1);
    strValues.chop(1);

    q.prepare("INSERT INTO "+QString::fromStdString(table)+
              " ("+strParams+") VALUES ("+strValues+")");

    p = par;
    while(p != NULL)
    {
        q.addBindValue(p->value);
        p = va_arg(params, DMSqlParameter*);
    }
    va_end( params );

    if(!q.exec())	PrintSqlError(&q);
}*/



void DBConnector::beginTransaction()
{
    QSqlQuery query;
    //if(!query.exec("BEGIN TRANSACTION"))
    if(!query.exec("START TRANSACTION"))
         PrintSqlError(&query);
}

void DBConnector::endTransaction()
{
    QSqlQuery query;
    //if(!query.exec("END TRANSACTION"))
    if(!query.exec("COMMIT"))
         PrintSqlError(&query);

	query.finish();
}
