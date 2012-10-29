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

Edge::Edge(QByteArray qba) : Component()
{
	QStringList qsl;
	QDataStream stream(&qba, QIODevice::ReadOnly);
	stream >> qsl;
	start = qsl[0].toStdString();
	end = qsl[1].toStdString();

	/*

	QDataStream stream(&qba, QIODevice::ReadOnly);
	QString qstart;
	QString qend;
	stream>>qstart;
	stream>>qend;
	start = qstart.toStdString();
	end = qend.toStdString();*/
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
	/*
	QByteArray bytes;
	bytes.append(QString::fromStdString(end));
	bytes.append(QString::fromStdString(start));
	return bytes;*/
}

Edge::Edge(std::string startpoint, std::string endpoint) : Component()
{
    start=startpoint;
    end=endpoint;
	SQLInsert();
	SQLSetValues();
}

Edge::Edge(const Edge& e) : Component(e)
{
    start=e.start;
    end=e.end;
	SQLInsert();
	SQLSetValues();
}

Edge::~Edge()
{
    SQLDelete();
}
DM::Components Edge::getType()
{
	return DM::EDGE;
}

std::string Edge::getStartpointName()
{
    return start;
}

std::string Edge::getEndpointName()
{
    return end;
}

void Edge::setStartpointName(std::string name) {
    this->start = name;
	SQLSetValues();
}

void Edge::setEndpointName(std::string name) {
    this->end = name;
	SQLSetValues();
}

Component* Edge::clone()
{
    return new Edge(*this);
}

void Edge::SQLInsert()
{
	SQLInsertAs("edge");
	SQLSetValues();
}
void Edge::SQLDelete()
{
	SQLDeleteAs("edge");
}

void Edge::SQLSetValues()
{
	QSqlQuery q;
	q.prepare("UPDATE edges SET start=?,end=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(start));
	q.addBindValue(QString::fromStdString(end));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}
