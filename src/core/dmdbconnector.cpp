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
#include <qvariant>
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

DBConnector::DBConnector()
{
	//QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	//db.setDatabaseName(":memory:");

	QString connectionString = "DRIVER={MySQL ODBC 5.2w Driver};SERVER=localhost;DATABASE=dynamind;";
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(connectionString);
    db.setUserName("root");
    db.setPassword("");

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
	if(!query.exec("DROP TABLE IF EXISTS systems, components, attributes")
	||	!query.exec("CREATE TABLE systems(	uuid varchar(128) NOT NULL, \
											stateuuid varchar(128) NOT NULL, \
											predecessors text, \
											sucessors text, \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE components(uuid varchar(128), \
											stateuuid varchar(128), \
											owner varchar(128), \
											name text, \
											type tinyint, \
											value blob, \
											PRIMARY KEY (uuid,stateuuid))")
	|| !query.exec("CREATE TABLE attributes(owner varchar(128), \
											stateuuid varchar(128), \
											name varchar(128), \
											type tinyint, \
											value blob, \
											PRIMARY KEY (owner,stateuuid,name))")
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
}

std::vector<std::string> DBConnector::GetStringVector(QByteArray qba)
{
	std::vector<std::string> stringvector;
	QStringList qsl;

	QDataStream stream(&qba, QIODevice::ReadOnly);

	stream>>qsl;

	foreach(QString item, qsl)
		stringvector.push_back(item.toStdString());
	
	return stringvector;
}	
	
std::vector<QString> DBConnector::ToStdString(std::vector<std::string> v)
{
	std::vector<QString> qv;

	foreach(std::string item, v)
	{
		qv.push_back(QString::fromStdString(item));
	}

	return qv;
}

std::vector<double> DBConnector::GetDoubleVector(QByteArray qba)
{
	std::vector<double> doublevector;
	QStringList qsl;

	QDataStream stream(&qba, QIODevice::ReadOnly);

	stream>>qsl;

	foreach(QString item, qsl)
		doublevector.push_back(item.toDouble());
	
	return doublevector;
}

QStringList DBConnector::GetStringList(std::vector<DM::Component*> v)
{
	QStringList list;
	foreach(Component* item, v)
	{
		list.append(QString::fromStdString(item->getUUID()));
	}
	return list;
}
QStringList DBConnector::GetStringList(std::vector<DM::System*> v)
{
	QStringList list;
	foreach(System* item, v)
	{
		list.append(QString::fromStdString(item->getUUID()));
	}
	return list;
}

