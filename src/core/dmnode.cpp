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
#include <dmnode.h>
#include <dmedge.h>
#include <cstdlib>
#include <math.h>

#include <dmdbconnector.h>
#include <QSqlQuery>
#include "dmlogger.h"

using namespace DM;


Node::Node( double x, double y, double z) : Component(true)
{
	this->x = x;
	this->y = y;
	this->z = z;
	connectedEdges = 0;
}

Node::Node() : Component(true)
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	connectedEdges = 0;
}

Node::Node(const Node& n) : Component(n, true)
{
	Vector3 refv;
	n.get(&refv.x);
	this->x = refv.x;
	this->y = refv.y;
	this->z = refv.z;
	isInserted = false;
	connectedEdges = 0;
	if(n.connectedEdges)
		foreach(Edge* e, *n.connectedEdges)
		this->addEdge(e);
}
Node::~Node()
{
	if(connectedEdges)	delete connectedEdges;
}

DM::Components Node::getType() const
{
	return DM::NODE;
}
QString Node::getTableName()
{
	return "nodes";	// also give in precache
}
double Node::getX() const
{
	return this->x;
}

double Node::getY() const
{
	return this->y;
}

double Node::getZ() const
{
	return this->z;
}

const void Node::get(double *vector) const
{
	vector[0] = this->x;
	vector[1] = this->y;
	vector[2] = this->z;
}

const double Node::get(unsigned int i) const {
	if(i==0)		return getX();
	else if(i==1)	return getY();
	else if(i==2)	return getZ();
	else			return 0;
}

std::vector<Edge*> Node::getEdges() const
{
	if(!connectedEdges)
		return std::vector<Edge*>();

	std::vector<Edge*> edges;
	foreach(Edge* e,*connectedEdges)
		edges.push_back(e);
	return edges;
}

void Node::set(double x, double y, double z)
{
	QMutexLocker ml(&mutex);

	this->x = x;
	this->y = y;
	this->z = z;
}

void Node::setX(double x)
{
	QMutexLocker ml(&mutex);
	this->x = x;
}

void Node::setY(double y)
{
	QMutexLocker ml(&mutex);
	this->y = y;
}

void Node::setZ(double z)
{
	QMutexLocker ml(&mutex);
	this->z = z;
}

Component* Node::clone()
{
	return new Node(*this);
}

Node& Node::operator=(const Node& other)
{
	QMutexLocker ml(&mutex);

	if(this != &other)
	{
		this->isInserted = false;
		double v[3];
		other.get(v);
		this->set(v[0],v[1],v[2]);

		if(!connectedEdges && other.connectedEdges)
			foreach(Edge* e, *other.connectedEdges)
			this->addEdge(e);
	}
	return *this;
}

bool Node::operator ==(const Node & other) const 
{
	double v0[3];
	double v1[3];
	this->get(v0);
	other.get(v1);
	return v0[0] == v1[0] &&
		v0[1] == v1[1] &&
		v0[2] == v1[2];
}
Node Node::operator -(const Node & other) const 
{
	double v0[3];
	double v1[3];
	this->get(v0);
	other.get(v1);
	return Node(v0[0]-v1[0],
		v0[1]-v1[1],
		v0[2]-v1[2]);
}

Node Node::operator +(const Node & other) const 
{
	double v0[3];
	double v1[3];
	other.get(v0);
	this->get(v1);
	return Node(v0[0]+v1[0],
		v0[1]+v1[1],
		v0[2]+v1[2]);
}
Node Node::operator *(const double &val) const
{
	double v0[3];
	this->get(v0);
	return Node(v0[0]*val,  v0[1]*val,  v0[2]*val);
}

Node Node::operator /(const double &val) const
{
	double v0[3];
	this->get(v0);
	return Node(v0[0]/val,  v0[1]/val,  v0[2]/val);
}

bool Node::compare2d(const Node &other, double round ) const 
{
	double v0[3];
	double v1[3];
	other.get(v0);
	this->get(v1);
	return fabs(v0[0]-v1[0]) <= round && fabs(v0[1]-v1[1]) <= round;
}
bool Node::compare2d(const Node * other , double round ) const 
{
	return compare2d(*other, round);
}

void Node::addEdge(Edge* e)
{
	if(!connectedEdges)
		connectedEdges = new std::list<Edge*>();
	connectedEdges->push_back(e);
}

void Node::removeEdge(Edge* e)
{
	if(connectedEdges)
		connectedEdges->remove(e);
}

void Node::_moveToDb()
{
	if(isInserted)
	{
		QSqlQuery *q = DBConnector::getInstance()->getQuery("UPDATE " + getTableName() + " SET owner=?,x=?,y=?,z=? WHERE uuid LIKE ?");
		q->addBindValue(((Component*)this->getCurrentSystem())->getQUUID().toByteArray());
		q->addBindValue(this->x);
		q->addBindValue(this->y);
		q->addBindValue(this->z);
		q->addBindValue(uuid.toByteArray());
		DBConnector::getInstance()->ExecuteQuery(q);

		isInserted = false;
	}
	else
	{
		QSqlQuery *q = DBConnector::getInstance()->getQuery("INSERT INTO " + getTableName() + " (uuid,owner,x,y,z) VALUES (?,?,?,?,?)");
		q->addBindValue(uuid.toByteArray());
		q->addBindValue(((Component*)this->getCurrentSystem())->getQUUID().toByteArray());
		q->addBindValue(this->x);
		q->addBindValue(this->y);
		q->addBindValue(this->z);
		DBConnector::getInstance()->ExecuteQuery(q);
	}
	_moveAttributesToDb();
	delete this;
}
