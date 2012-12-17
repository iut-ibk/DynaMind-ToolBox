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
#include <cstdlib>
#include <math.h>

#include <dmdbconnector.h>
#include <QSqlQuery>
#include "dmlogger.h"

using namespace DM;

class Vector3
{
public:
    double x,y,z;

    Vector3(){}
    Vector3(double x,double y,double z){this->x=x;this->y=y;this->z=z;}
};

static Cache<QUuid,Vector3> nodeCache(512);

#ifdef CACHE_PROFILING
void Node::PrintStatistics()
{
    Logger(Standard) << "Node cache statistics:\t"
                     << "misses: " << (long)nodeCache.misses
                     << "\thits: " << (long)nodeCache.hits;
    nodeCache.ResetProfilingCounters();
}
#endif

Node::Node( double x, double y, double z) : Component(true)
{
    DBConnector::getInstance()->Insert("nodes", uuid,
                                       "x",x,"y",y,"z",z);
    nodeCache.add(getQUUID(), new Vector3(x,y,z));
}

Node::Node() : Component(true)
{
    DBConnector::getInstance()->Insert("nodes", uuid,
                                       "x",0,"y",0,"z",0);
    nodeCache.add(getQUUID(), new Vector3(0,0,0));
}

Node::Node(const Node& n) : Component(n, true)
{
    double v[3];
    n.get(v);
    DBConnector::getInstance()->Insert("nodes", uuid,
                                       "x",v[0],"y",v[1],"z",v[2]);
    nodeCache.add(getQUUID(), new Vector3(v[0],v[1],v[2]));
}
Node::~Node()
{
    nodeCache.remove(getQUUID());
    Component::SQLDelete();
}
DM::Components Node::getType()
{
	return DM::NODE;
}
QString Node::getTableName()
{
    return "nodes";
}
double Node::getX() const
{
    if(Vector3* v = nodeCache.get(getQUUID()))   return v->x;
    Vector3 v;
    get(&v.x);
    return v.x;
}

double Node::getY() const
{
    if(Vector3* v = nodeCache.get(getQUUID()))   return v->y;
    Vector3 v;
    get(&v.x);
    return v.y;
}

double Node::getZ() const
{
    if(Vector3* v = nodeCache.get(getQUUID()))   return v->z;
    Vector3 v;
    get(&v.x);
    return v.z;
}

const void Node::get(double *vector) const
{
    if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        vector[0] = v->x;
        vector[1] = v->y;
        vector[2] = v->z;
        return;
    }
    QVariant v[3];
    DBConnector::getInstance()->Select("nodes", uuid,
                                       "x",     &v[0],
                                       "y",     &v[1],
                                       "z",     &v[2]);
    vector[0] = v[0].toDouble();
    vector[1] = v[1].toDouble();
    vector[2] = v[2].toDouble();
    nodeCache.add(getQUUID(), new Vector3(vector[0],vector[1],vector[2]));
}

const double Node::get(unsigned int i) const {
	if(i==0)		return getX();
	else if(i==1)	return getY();
	else if(i==2)	return getZ();
	else			return 0;
}


std::vector<QUuid> Node::getEdges() const
{
    // TODO

    std::vector<QUuid> edges;

    QSqlQuery *q = DBConnector::getInstance()->getQuery("SELECT uuid FROM edges WHERE startnode LIKE ? OR endnode LIKE ?");
    q->addBindValue(uuid.toByteArray());
    q->addBindValue(uuid.toByteArray());
    if(DBConnector::getInstance()->ExecuteSelectQuery(q))
    {
        do
            edges.push_back(QUuid(q->value(0).toByteArray()));
        while(q->next());
    }
    return edges;
}

void Node::setX(double x)
{
    if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->x = x;
        nodeCache.get(getQUUID());  // push to front
    }
    DBConnector::getInstance()->Update("nodes", uuid,
                                       "x",     QVariant::fromValue(x));
}

void Node::setY(double y)
{
    if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->y = y;
        nodeCache.get(getQUUID());  // push to front
    }
    DBConnector::getInstance()->Update("nodes", uuid,
                                       "y",     QVariant::fromValue(y));
}

void Node::setZ(double z)
{
    if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->z = z;
        nodeCache.get(getQUUID());  // push to front
    }
    DBConnector::getInstance()->Update("nodes", uuid,
                                       "z",     QVariant::fromValue(z));
}

Component* Node::clone()
{
    return new Node(*this);
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
void Node::SQLSetValues(double x,double y,double z)
{
    if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->x = x;
        v->y = y;
        v->z = z;
        nodeCache.get(getQUUID());  // push to front
    }
    else
        nodeCache.add(getQUUID(), new Vector3(x,y,z));

    DBConnector::getInstance()->Update("nodes", uuid,
                                       "x",     QVariant::fromValue(x),
                                       "y",     QVariant::fromValue(y),
                                       "z",     QVariant::fromValue(z));
}
