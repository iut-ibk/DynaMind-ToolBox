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

static DbCache<Node*,Vector3> nodeCache(50000);

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
    vector = new Vector3();
    vector->x = x;
    vector->y = y;
    vector->z = z;
    isInserted = false;
    //DBConnector::getInstance()->Insert("nodes", uuid,
    //                                   "x",x,"y",y,"z",z);
    //nodeCache.add(getQUUID(), new Vector3(x,y,z));
}

Node::Node() : Component(true)
{
    vector = new Vector3();
    vector->x = 0;
    vector->y = 0;
    vector->z = 0;
    isInserted = false;
    //DBConnector::getInstance()->Insert("nodes", uuid,
    //                                   "x",0,"y",0,"z",0);
    //nodeCache.add(getQUUID(), new Vector3(0,0,0));
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
    //double v[3];
    //n.get(v);
    //DBConnector::getInstance()->Insert("nodes", uuid,
    //                                   "x",v[0],"y",v[1],"z",v[2]);
    //nodeCache.add(getQUUID(), new Vector3(v[0],v[1],v[2]));
}
Node::~Node()
{
    nodeCache.remove(this);
    if(vector)
        delete vector;
    if(isInserted)
        Component::SQLDelete();
}
void Node::SetOwner(Component *owner)
{
    currentSys = owner->getCurrentSystem();
    if(currentSys)
    {
        nodeCache.add(this,new Vector3(*vector));
        delete vector;
        vector = NULL;
        //DBConnector::getInstance()->Insert("nodes", uuid,
        //                                   "x",x,"y",y,"z",z);
    }
    for (std::map<std::string,Attribute*>::iterator it=ownedattributes.begin() ; it != ownedattributes.end(); ++it )
        it->second->SetOwner(this);
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
    /*if(Vector3* v = nodeCache.get(getQUUID()))   return v->x;
    Vector3 v;
    get(&v.x);
    return v.x;*/
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    return v->x;
}

double Node::getY() const
{
    /*if(Vector3* v = nodeCache.get(getQUUID()))   return v->y;
    Vector3 v;
    get(&v.x);
    return v.y;*/
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    return v->y;
}

double Node::getZ() const
{
    /*if(Vector3* v = nodeCache.get(getQUUID()))   return v->z;
    Vector3 v;
    get(&v.x);
    return v.z;*/
    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    return v->z;
}

const void Node::get(double *vector) const
{
    /*if(Vector3 *v = nodeCache.get(getQUUID()))
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
    nodeCache.add(getQUUID(), new Vector3(vector[0],vector[1],vector[2]));*/
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


std::vector<QUuid> Node::getEdges() const
{
    std::vector<QUuid> edges;
	/*
    QSqlQuery *q = DBConnector::getInstance()->getQuery("SELECT uuid FROM edges WHERE startnode LIKE ? OR endnode LIKE ?");
    q->addBindValue(uuid.toByteArray());
    q->addBindValue(uuid.toByteArray());
    if(DBConnector::getInstance()->ExecuteSelectQuery(q))
    {
        do
            edges.push_back(QUuid(q->value(0).toByteArray()));
        while(q->next());
    }*/

	foreach(Edge* e, connectedEdges)
		edges.push_back(e->getQUUID());

    return edges;
}

void Node::setX(double x)
{
    /*if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->x = x;
        nodeCache.get(getQUUID());  // push to front
    }*/
    /*this->x = x;
    if(currentSys)
        DBConnector::getInstance()->Update("nodes", uuid,
                                           "x",     QVariant::fromValue(x));*/

    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    v->x = x;
}

void Node::setY(double y)
{
    /*if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->y = y;
        nodeCache.get(getQUUID());  // push to front
    }*/
    /*this->x = x;
    if(currentSys)
        DBConnector::getInstance()->Update("nodes", uuid,
                                           "y",     QVariant::fromValue(y));*/

    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    v->y = y;
}

void Node::setZ(double z)
{
    /*if(Vector3 *v = nodeCache.get(getQUUID()))
    {
        v->z = z;
        nodeCache.get(getQUUID());  // push to front
    }*/
    /*this->x = x;
    if(currentSys)
        DBConnector::getInstance()->Update("nodes", uuid,
                                           "z",     QVariant::fromValue(z));*/

    Vector3* v = vector ? vector:nodeCache.get((Node*)this);
    v->z = z;
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

/*void Node::SQLSetValues(double x,double y,double z)
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
}*/
