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

Edge::Edge(std::string startpoint, std::string endpoint) : Component(true)
{
    DBConnector::getInstance()->Insert("edges", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                      "startnode",QString::fromStdString(startpoint),
                                      "endnode",QString::fromStdString(endpoint));
    //SQLInsert(startpoint,endpoint);
}

Edge::Edge(const Edge& e) : Component(e, true)
{
    //std::vector<std::string> points = e.getPoints();
    std::string points[2];
    getPoints(points);
    DBConnector::getInstance()->Insert("edges", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "startnode",QString::fromStdString(points[0]),
                                       "endnode",QString::fromStdString(points[1]));
    //SQLInsert(e.getStartpointName(), e.getEndpointName());
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

const void Edge::getPoints(std::string *points) const
{
    QVariant v1,v2;
    if(DBConnector::getInstance()->Select("edges", QString::fromStdString(uuid)
                                       , "startnode", &v1, "endnode", &v2))
    {
        points[0] = v1.toString().toStdString();
        points[1] = v2.toString().toStdString();
    }
}

const std::string Edge::getStartpointName() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("edges", QString::fromStdString(uuid),
												QString::fromStdString(stateUuid),
											"startnode", &value))
        return value.toString().toStdString();

    return "";
}

const std::string Edge::getEndpointName() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("edges", QString::fromStdString(uuid),
												QString::fromStdString(stateUuid),
										"endnode", &value))
        return value.toString().toStdString();

    return "";
}

void Edge::setStartpointName(std::string start) 
{
    DBConnector::getInstance()->Update("edges", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "startnode", QString::fromStdString(start));
}

void Edge::setEndpointName(std::string end)
{
    DBConnector::getInstance()->Update("edges", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "endnode",   QString::fromStdString(end));
}

Component* Edge::clone()
{
    return new Edge(*this);
}

void Edge::SQLSetValues(std::string start, std::string end)
{
    DBConnector::getInstance()->Update("edges", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "startnode", QString::fromStdString(start),
                                       "endnode",   QString::fromStdString(end));
}
