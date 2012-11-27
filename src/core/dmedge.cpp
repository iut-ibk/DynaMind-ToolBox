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

#include <QSqlQuery>
#include <dmdbconnector.h>

using namespace DM;

Edge::Edge(Node *start, Node *end) : Component(true)
{
    DBConnector::getInstance()->Insert("edges", uuid.toRfc4122(),
                                      "startnode",  start->getQUUID().toRfc4122(),
                                      "endnode",    end->getQUUID().toRfc4122());
}

Edge::Edge(const Edge& e) : Component(e, true)
{
    //std::vector<std::string> points = e.getPoints();
    QUuid points[2];
    e.getPoints(points);
    DBConnector::getInstance()->Insert("edges", uuid.toRfc4122(),
                                       "startnode",points[0].toRfc4122(),
                                       "endnode",points[1].toRfc4122());
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

const void Edge::getPoints(QUuid *points) const
{
    QVariant v1,v2;
    if(DBConnector::getInstance()->Select("edges", uuid.toRfc4122(),
                                       "startnode", &v1, "endnode", &v2))
    {
        points[0] = QUuid::fromRfc4122(v1.toByteArray());
        points[1] = QUuid::fromRfc4122(v2.toByteArray());
    }
}

const QUuid Edge::getStartpoint() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("edges", uuid.toRfc4122(),
											"startnode", &value))
        return QUuid::fromRfc4122(value.toByteArray());

    return "";
}

const std::string Edge::getStartpointName() const
{
    return getStartpoint().toString().toStdString();
}

const QUuid Edge::getEndpoint() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("edges", uuid.toRfc4122(),
                                        "endnode", &value))
        return QUuid::fromRfc4122(value.toByteArray());

    return "";
}

const std::string Edge::getEndpointName() const
{
    return getEndpoint().toString().toStdString();
}

void Edge::setStartpoint(Node *start)
{
    DBConnector::getInstance()->Update("edges", uuid.toRfc4122(),
                                       "startnode", start->getQUUID().toRfc4122());
}

void Edge::setStartpointName(std::string name)
{
    DBConnector::getInstance()->Update("edges", uuid.toRfc4122(),
                                       "startnode", QUuid(QString::fromStdString(name)).toRfc4122());
}

void Edge::setEndpoint(Node *end)
{
    DBConnector::getInstance()->Update("edges", uuid.toRfc4122(),
                                       "endnode",  end->getQUUID().toRfc4122());
}

void Edge::setEndpointName(std::string name)
{
    DBConnector::getInstance()->Update("edges", uuid.toRfc4122(),
                                       "endnode", QUuid(QString::fromStdString(name)).toRfc4122());
}

Component* Edge::clone()
{
    return new Edge(*this);
}

void Edge::SQLSetValues(QUuid start, QUuid end)
{
    DBConnector::getInstance()->Update("edges", uuid.toRfc4122(),
                                       "startnode", start.toRfc4122(),
                                       "endnode",   end.toRfc4122());
}
