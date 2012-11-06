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
	||	!query.exec("CREATE TABLE systems(	uuid VARCHAR(128,128) NOT NULL, \
											stateuuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											name text, \
											predecessors text, \
											sucessors text, \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE components(uuid VARCHAR(128,128) NOT NULL, \
											stateuuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											name text, \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE nodes(uuid VARCHAR(128,128) NOT NULL, \
											stateuuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											name text, \
											x DOUBLE PRECISION, y DOUBLE PRECISION, z DOUBLE PRECISION, \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE edges(uuid VARCHAR(128,128) NOT NULL, \
											stateuuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											name text, \
											start VARCHAR(128,128), end VARCHAR(128,128), \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE faces(uuid VARCHAR(128,128) NOT NULL, \
											stateuuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											name text, \
											nodes text, \
											holes text, \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE rasterdatas(uuid VARCHAR(128,128) NOT NULL, \
											stateuuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											name text, \
											datalink int,\
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE rasterfields(datalink int NOT NULL, \
											x bigint, y bigint, value DOUBLE PRECISION, \
											PRIMARY KEY (datalink,x,y))")
	|| !query.exec("CREATE TABLE attributes(uuid VARCHAR(128,128) NOT NULL, \
											owner VARCHAR(128,128), \
											stateuuid VARCHAR(128,128), \
											name VARCHAR(128,128), \
											type tinyint, \
											value blob, \
											PRIMARY KEY (uuid))")
		)
	{
        Logger(Error) << "Cannot initialize db tables";
		PrintSqlError(&query);

		QSqlQuery q;
		q.prepare("DROP TABLE IF EXISTS systems, components, attributes");
		if(!q.exec())	PrintSqlError(&q);

		db.removeDatabase("QODBC");
		//db.removeDatabase("QSQLITE");
		db.close();
		return;
	}

	Logger(Debug) << "DB created";
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
