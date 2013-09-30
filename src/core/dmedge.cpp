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
#include <dmlogger.h>
#include <QSqlQuery>
#include <dmdbconnector.h>

using namespace DM;

Edge::Edge(Node *start, Node *end) : Component(true)
{
	this->start = start;
	this->end = end;
	start->addEdge(this);
	end->addEdge(this);

	isInserted = false;
}

Edge::Edge(const Edge& e) : Component(e, true)
{
	start = e.getStartNode();
	end = e.getEndNode();
	start->addEdge(this);
	end->addEdge(this);

	isInserted = false;
}

Edge::~Edge()
{
	if(isInserted)
		Component::SQLDelete();
}
DM::Components Edge::getType() const
{
	return DM::EDGE;
}
QString Edge::getTableName()
{
	return "edges";
}

Node* Edge::getStartNode() const
{
	return start;
}
const QUuid Edge::getStartpoint() const
{
	return start->getQUUID();
}

const std::string Edge::getStartpointName() const
{
	return start->getUUID();
}
Node* Edge::getEndNode() const
{
	return end;
}
const QUuid Edge::getEndpoint() const
{
	return end->getQUUID();
}

const std::string Edge::getEndpointName() const
{
	return end->getUUID();
}

void Edge::setStartpoint(Node *start)
{
	QMutexLocker ml(mutex);

	this->start->removeEdge(this);
	this->start = start;
	start->addEdge(this);
}
/*
void Edge::setStartpointName(std::string name)
{
	QMutexLocker ml(mutex);

	if(!currentSys)
	{
		Logger(Error) << "setStartpointName in unattached edge not possible";
		return;
	}
	setStartpoint(currentSys->getNode(name));
}*/

void Edge::setEndpoint(Node *end)
{
	QMutexLocker ml(mutex);

	this->end->removeEdge(this);
	this->end = end;
	end->addEdge(this);
}
/*
void Edge::setEndpointName(std::string name)
{
	QMutexLocker ml(mutex);

	if(!currentSys)
	{
		Logger(Error) << "setEndpointName in unattached edge not possible";
		return;
	}
	setEndpoint(currentSys->getNode(name));
}*/

Component* Edge::clone()
{
	return new Edge(*this);
}

void Edge::Synchronize()
{
	if(isInserted)
	{
		DBConnector::getInstance()->Update("edges", uuid,
			"startnode",  start->getQUUID().toByteArray(),
			"endnode",  end->getQUUID().toByteArray());
	}
	else
	{
		DBConnector::getInstance()->Insert("edges", uuid,
			"startnode",  start->getQUUID().toByteArray(),
			"endnode",  end->getQUUID().toByteArray());
		isInserted = true;
	}
}
