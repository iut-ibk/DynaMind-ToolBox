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

DbCache<Node*,Vector3> Node::nodeCache(0); // defined in dmdbconnector.h

void Node::ResizeCache(unsigned long size)
{
	nodeCache.resize(size);
}
unsigned long Node::GetCacheSize()
{
	return nodeCache.getSize();
}

void Node::ClearCache()
{
	nodeCache.Clear();
}

void Node::PrintCacheStatistics()
{
#ifdef CACHE_PROFILING
    Logger(Standard) << "Node cache statistics:\t"
                     << "misses: " << (long)nodeCache.misses
                     << "\thits: " << (long)nodeCache.hits;
    nodeCache.ResetProfilingCounters();
#endif
}

Node::Node( double x, double y, double z) : Component(true)
{
    vector = new Vector3();
    vector->x = x;
    vector->y = y;
    vector->z = z;
    isInserted = false;
	connectedEdges = 0;
}

Node::Node() : Component(true)
{
    vector = new Vector3();
    vector->x = 0;
    vector->y = 0;
    vector->z = 0;
    isInserted = false;
	connectedEdges = 0;
}

Node::Node(const Node& n) : Component(n, true)
{
    vector = new Vector3();
    Vector3 refv;
    n.get(&refv.x);
    vector->x = refv.x;
    vector->y = refv.y;
    vector->z = refv.z;
    isInserted = false;
	connectedEdges = 0;
	if(n.connectedEdges)
		foreach(Edge* e, *n.connectedEdges)
			this->addEdge(e);
}
Node::~Node()
{
    nodeCache.remove(this);
    if(vector)			delete vector;
    //if(isInserted)		Component::SQLDelete();
	if(connectedEdges)	delete connectedEdges;
}
void Node::SetOwner(Component *owner)
{
	QMutexLocker ml(mutex);

    currentSys = owner->getCurrentSystem();
    if(currentSys)
    {
        nodeCache.add(this,new Vector3(*vector));
        delete vector;
        vector = NULL;
    }
	mforeach(Attribute* a, ownedattributes)
		a->setOwner(this);
    //for (std::map<std::string,Attribute*>::iterator it=ownedattributes.begin() ; it != ownedattributes.end(); ++it )
    //    it->second->SetOwner(this);
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
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    return v->x;
}

double Node::getY() const
{
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    return v->y;
}

double Node::getZ() const
{
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    return v->z;
}

const void Node::get(double *vector) const
{
    Vector3* v = this->vector ? this->vector:nodeCache.get((Node*)this);
    vector[0] = v->x;
    vector[1] = v->y;
    vector[2] = v->z;
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
	QMutexLocker ml(mutex);

    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
	v->x = x;
	v->y = y;
	v->z = z;
}

void Node::setX(double x)
{
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
	QMutexLocker ml(mutex);
    v->x = x;
}

void Node::setY(double y)
{
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
	QMutexLocker ml(mutex);
    v->y = y;
}

void Node::setZ(double z)
{
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
	QMutexLocker ml(mutex);
    v->z = z;
}

Component* Node::clone()
{
    return new Node(*this);
}

Node& Node::operator=(const Node& other)
{
	QMutexLocker ml(mutex);

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

Vector3* Node::LoadFromDb()
{
    QVariant v[3];
    DBConnector::getInstance()->Select(getTableName(), uuid,
                                       "x",     &v[0],
                                       "y",     &v[1],
                                       "z",     &v[2]);
    return new Vector3(v[0].toDouble(),v[1].toDouble(),v[2].toDouble());
}
void Node::SaveToDb(Vector3 *v)
{
    if(isInserted)
    {
        DBConnector::getInstance()->Update("nodes", uuid,
                                           "x",     QVariant::fromValue(v->x),
                                           "y",     QVariant::fromValue(v->y),
                                           "z",     QVariant::fromValue(v->z));
    }
    else
    {
        DBConnector::getInstance()->Insert("nodes", uuid,
                                           "x",v->x,"y",v->y,"z",v->z);
        isInserted = true;
    }
}

void Node::_PreCache(const QList<Node*>& keys, QList<Vector3*>& values)
{
	QList<QUuid*> uuids;
	long k = 0;
	while(k<keys.size())
	{
		foreach(Node* n, keys)
		{
			uuids.append(&n->uuid);
			if(++k > SQLBLOCKQUERYSIZE)
				break;
		}

		QList<QVariant> x,y,z;
		QList<QUuid>	resultUuids;
		if(!DBConnector::getInstance()->Select("nodes", uuids, &resultUuids,
										   "x",     &x,
										   "y",     &y,
										   "z",     &z))
			return;	// no elements retrieved

		for(int i=0;i<resultUuids.size();i++)
		{
			int j=0;
			for(;j<keys.size();j++)
				if(keys[j]->uuid == resultUuids[i])
					break;

			if( j != keys.size())	// should always be the case
				values[j] = new Vector3(x[i].toDouble(), y[i].toDouble(), z[i].toDouble());
		}
	}
}

void Node::PreCache(const QList<Node*>& keys)
{
	nodeCache.preCache(keys);
}