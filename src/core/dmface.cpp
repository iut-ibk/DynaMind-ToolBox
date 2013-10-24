/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich, Markus Sengthaler
 
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
#include "dmface.h"
#include "dmnode.h"
#include "dmsystem.h"
#include "dmlogger.h"

#include "dmdbconnector.h"
#include <QByteArray>

using namespace DM;

QByteArray GetBytes(std::vector<Node*> nodevector)
{
	QByteArray qba;
	QDataStream stream(&qba, QIODevice::WriteOnly);

	stream << (int)nodevector.size();
	for(unsigned int i=0;i<nodevector.size();i++)
		stream << nodevector[i]->getQUUID();

	return qba;
}

QByteArray GetBytes(std::vector<Face*> facevector)
{
	QByteArray qba;
	QDataStream stream(&qba, QIODevice::WriteOnly);

	stream << (int)facevector.size();
	for(unsigned int i=0;i<facevector.size();i++)
	{
		stream << GetBytes(facevector[i]->getNodePointers());
	}
	return qba;
}

Face::Face(const std::vector<Node*>& nodes) : Component(true)
{
	setNodes(nodes);
}

Face::Face(const Face& e) : Component(e, true)
{
	this->_nodes = e._nodes;
	this->_holes = e._holes;
}
Face::~Face()
{
	Component::SQLDelete();
}

std::vector<std::string> GetVector(QByteArray qba)
{
	QDataStream stream(&qba, QIODevice::ReadOnly);
	QString str;
	std::vector<std::string> result;

	unsigned int len=0;
	stream >> len;
	for(unsigned int i=0;i<len;i++)
	{
		stream>>str;
		result.push_back(str.toStdString());
	}
	return result;
}

std::vector<Node*> Face::getNodePointers() const
{
	return _nodes;
}

Component* Face::clone()
{
	return new Face(*this);
}

DM::Components Face::getType() const
{
	return DM::FACE;
}
QString Face::getTableName()
{
	return "faces";
}

const std::vector<Face*> Face::getHolePointers() const
{
	return _holes;
}

void Face::addHole(std::vector<Node*> hole)
{
	QMutexLocker ml(mutex);
	_holes.push_back(getCurrentSystem()->addFace(hole));
}

void Face::addHole(Face* hole)
{
	if(hole==this)
	{
		Logger(Error) << "addHole: self reference not possible";
		return;
	}
	QMutexLocker ml(mutex);
	_holes.push_back(hole);
}
void Face::_moveToDb()
{
	if(!getCurrentSystem())
		return;
	if(isInserted)
	{
		DBConnector::getInstance()->Update("faces", uuid,
			"owner", getCurrentSystem()->getQUUID().toByteArray(),
			"nodes", GetBytes(_nodes),
			"holes", GetBytes(_holes));

		isInserted = false;
	}
	else
	{
		DBConnector::getInstance()->Insert("faces", uuid,
			"owner", getCurrentSystem()->getQUUID().toByteArray(),
			"nodes", GetBytes(_nodes),
			"holes", GetBytes(_holes));
	}
	delete this;
}

void Face::setNodes(const std::vector<Node*>& nodes)
{
	QMutexLocker ml(mutex);
	// Check is start != end to garantee that endpoint is not the start point
	if (nodes.size() == 0) 
		Logger(Error) << "No Nodes given";
	else
	{
		this->_nodes = nodes;

		if (nodes.back() == nodes.front())
			this->_nodes.pop_back();
	}
}

void Face::clearHoles()
{
	QMutexLocker ml(mutex);

	this->_holes.clear();
}