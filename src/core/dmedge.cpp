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
#include <dmnode.h>
#include <dmsystem.h>

#include <QSqlQuery>
#include <dmdbconnector.h>

using namespace DM;

Edge::Edge(Node *start, Node *end) : Component(true)
{
	this->start = start;
	this->end = end;
    DBConnector::getInstance()->Insert("edges", uuid,
                                       "startnode",  start->getQUUID().toByteArray(),
                                      "endnode",    end->getQUUID().toByteArray());
}

Edge::Edge(const Edge& e) : Component(e, true)
{
	start = e.getStart();
	end = e.getEnd();
	DBConnector::getInstance()->Insert("edges", uuid,
                                       "startnode",  start->getQUUID().toByteArray(),
                                      "endnode",    end->getQUUID().toByteArray());
    /*QUuid points[2];
    e.getPoints(points);
    DBConnector::getInstance()->Insert("edges", uuid,
                                       "startnode",points[0].toByteArray(),
                                       "endnode",points[1].toByteArray());*/
}

Edge::~Edge()
{
    Component::SQLDelete();
}
DM::Components Edge::getType()
{
	return DM::EDGE;
}
QString Edge::getTableName()
{
    return "edges";
}

/*const void Edge::getPoints(QUuid *points) const
{
    QVariant v1,v2;
    if(DBConnector::getInstance()->Select("edges", uuid,
                                       "startnode", &v1, "endnode", &v2))
    {
        points[0] = QUuid(v1.toByteArray());
        points[1] = QUuid(v2.toByteArray());
    }
}*/
Node* Edge::getStart() const
{
	return start;
}
const QUuid Edge::getStartpoint() const
{
	return start->getQUUID();
    /*QVariant value;
    if(DBConnector::getInstance()->Select("edges", uuid,
											"startnode", &value))
        return QUuid(value.toByteArray());

    return "";*/
}

const std::string Edge::getStartpointName() const
{
    return getStartpoint().toString().toStdString();
}
Node* Edge::getEnd() const
{
	return end;
}
const QUuid Edge::getEndpoint() const
{
	return end->getQUUID();
	/*
    QVariant value;
    if(DBConnector::getInstance()->Select("edges", uuid,
                                        "endnode", &value))
        return QUuid(value.toByteArray());

    return "";*/
}

const std::string Edge::getEndpointName() const
{
    return getEndpoint().toString().toStdString();
}

void Edge::setStartpoint(Node *start)
{
	this->start = start;
    DBConnector::getInstance()->Update("edges", uuid,
                                       "startnode", start->getQUUID().toByteArray());
}

void Edge::setStartpointName(std::string name)
{
	start = currentSys->getNode(name);
    //DBConnector::getInstance()->Update("edges", uuid,
    //                                   "startnode", QUuid(QString::fromStdString(name)).toByteArray());
}

void Edge::setEndpoint(Node *end)
{
	this->end = end;
    DBConnector::getInstance()->Update("edges", uuid,
                                       "endnode",  end->getQUUID().toByteArray());
}

void Edge::setEndpointName(std::string name)
{
	end = currentSys->getNode(name);
    //DBConnector::getInstance()->Update("edges", uuid,
    //                                   "endnode", QUuid(QString::fromStdString(name)).toByteArray());
}

Component* Edge::clone()
{
    return new Edge(*this);
}

/*void Edge::SQLSetValues(QUuid start, QUuid end)
{
    DBConnector::getInstance()->Update("edges", uuid,
                                       "startnode", start.toByteArray(),
                                       "endnode",   end.toByteArray());
}*/
