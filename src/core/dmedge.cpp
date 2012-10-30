/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair, Markus Sengthaler

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

#include <dmcomponent.h>
#include <dmedge.h>

#include <QSqlQuery>
#include <dmdbconnector.h>

using namespace DM;
/*
Edge::Edge(QByteArray qba) : Component()
{
	QStringList qsl;
	QDataStream stream(&qba, QIODevice::ReadOnly);
	stream >> qsl;
	start = qsl[0].toStdString();
	end = qsl[1].toStdString();
}

QByteArray Edge::GetValue()
{
	QStringList qsl;
	qsl.append(QString::fromStdString(start));
	qsl.append(QString::fromStdString(end));
	
	QByteArray bytes;
	QDataStream stream(&bytes, QIODevice::WriteOnly);
	stream << qsl;
	return bytes;
}
*/
Edge::Edge(std::string startpoint, std::string endpoint) : Component()
{
    //start=startpoint;
    //end=endpoint;
	SQLInsert(startpoint,endpoint);
}

Edge::Edge(const Edge& e) : Component(e)
{
    //start=e.start;
    //end=e.end;
	SQLInsert(e.getStartpointName(), e.getEndpointName());
}

Edge::~Edge()
{
    SQLDelete();
}
DM::Components Edge::getType()
{
	return DM::EDGE;
}

const std::string Edge::getStartpointName() const
{
    //return start;
	QSqlQuery q;
	q.prepare("SELECT start FROM edges WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);

	if(q.next())	
		return q.value(0).toString().toStdString();
	return "";
}

const std::string Edge::getEndpointName() const
{
    //return end;
	QSqlQuery q;
	q.prepare("SELECT end FROM edges WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);

	if(q.next())	return q.value(0).toString().toStdString();
	return "";
}

void Edge::setStartpointName(std::string start) 
{
    //this->start = name;
	//SQLSetValues();
	QSqlQuery q;
	q.prepare("UPDATE edges SET start=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(start));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}

void Edge::setEndpointName(std::string end) {
    //this->end = name;
	//SQLSetValues();
	QSqlQuery q;
	q.prepare("UPDATE edges SET end=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(end));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}

Component* Edge::clone()
{
    return new Edge(*this);
}

void Edge::SQLInsert(const std::string start, const std::string end)
{
	SQLInsertAs("edge");
	SQLSetValues(start,end);
}
void Edge::SQLDelete()
{
	SQLDeleteAs("edge");
}

void Edge::SQLSetValues(std::string start, std::string end)
{
	QSqlQuery q;
	q.prepare("UPDATE edges SET start=?,end=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(start));
	q.addBindValue(QString::fromStdString(end));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}
