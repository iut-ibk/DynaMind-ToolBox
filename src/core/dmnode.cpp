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

//#include <QVariant>

using namespace DM;

Node::Node( double x, double y, double z) : Component(true)
{
    //DBConnector::getInstance()->InsertX("nodes", "uuid", QString::fromStdString(uuid),
    //                                    "value", QVariant::fromValue(438));

    DBConnector::getInstance()->Insert("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "x",x,"y",y,"z",z);
    //SQLInsert(x,y,z);
}

Node::Node() : Component(true)
{
    DBConnector::getInstance()->Insert("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "x",0,"y",0,"z",0);
    //SQLInsert(0,0,0);
}

Node::Node(const Node& n) : Component(n, true)
{
    std::vector<double> v = n.get();
    DBConnector::getInstance()->Insert("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "x",v[0],"y",v[1],"z",v[2]);
    //SQLInsert(n.getX(),n.getY(),n.getZ());
}
Node::~Node()
{
    Component::SQLDelete("nodes");
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
    QVariant value;
    if(DBConnector::getInstance()->Select("nodes",  QString::fromStdString(uuid),
                                                    QString::fromStdString(stateUuid),
                                          "x",      &value))
        return value.toDouble();
    /*
	QSqlQuery q;
	q.prepare("SELECT x FROM nodes WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
		return q.value(0).toDouble();
*/
	return 0;
}

double Node::getY() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("nodes",  QString::fromStdString(uuid),
                                                    QString::fromStdString(stateUuid),
                                          "y",      &value))
        return value.toDouble();
    /*
	QSqlQuery q;
	q.prepare("SELECT y FROM nodes WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
        return q.value(0).toDouble();*/
	return 0;
}

double Node::getZ() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("nodes",  QString::fromStdString(uuid),
                                                    QString::fromStdString(stateUuid),
                                          "z",      &value))
        return value.toDouble();
    /*
	QSqlQuery q;
	q.prepare("SELECT z FROM nodes WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
        return q.value(0).toDouble();*/
	return 0;
}

const std::vector<double> Node::get() const
{
    std::vector<double> v;
    v.push_back(getX());
    v.push_back(getY());
    v.push_back(getZ());
    return v;
}

const double Node::get(unsigned int i) const {
	if(i==0)		return getX();
	else if(i==1)	return getY();
	else if(i==2)	return getZ();
	else			return 0;
}

void Node::setX(double x)
{
    DBConnector::getInstance()->Update("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "x",     QVariant::fromValue(x));
    /*
	QSqlQuery q;
	q.prepare("UPDATE nodes SET x=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(x);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
    if(!q.exec())	PrintSqlError(&q);*/
}

void Node::setY(double y)
{
    DBConnector::getInstance()->Update("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "y",     QVariant::fromValue(y));
    /*QSqlQuery q;
	q.prepare("UPDATE nodes SET y=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(y);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
    if(!q.exec())	PrintSqlError(&q);*/
}

void Node::setZ(double z)
{
    DBConnector::getInstance()->Update("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "z",     QVariant::fromValue(z));
    /*QSqlQuery q;
	q.prepare("UPDATE nodes SET z=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(z);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
    if(!q.exec())	PrintSqlError(&q);*/
}

Component* Node::clone()
{
    return new Node(*this);
}

bool Node::operator ==(const Node & other) const 
{
    std::vector<double> vOther = other.get();
    std::vector<double> vThis = this->get();
    return vOther == vThis;
    //return  this->getX() == other.getX() &&
    //        this->getY() == other.getY() &&
    //        this->getZ() == other.getZ();
}
Node Node::operator -(const Node & other) const 
{
    std::vector<double> vOther = other.get();
    std::vector<double> vThis = this->get();
    return Node(vThis[0]-vOther[0],
                vThis[1]-vOther[1],
                vThis[2]-vOther[2]);
    //return(Node(this->getX() - other.getX(),
    //            this->getY() - other.getY(),
    //            this->getZ() - other.getZ()));
}

Node Node::operator +(const Node & other) const 
{
    std::vector<double> vOther = other.get();
    std::vector<double> vThis = this->get();
    return Node(vThis[0]+vOther[0],
                vThis[1]+vOther[1],
                vThis[2]+vOther[2]);
    //return(Node(this->getX() + other.getX(), this->getY() + other.getY(), this->getZ() + other.getZ()));
}

Node Node::operator *(const double &val) const
{
    std::vector<double> vThis = this->get();
    return Node(vThis[0]*val,
                vThis[1]*val,
                vThis[2]*val);
    //return(Node(this->getX() * val, this->getY()  * val, this->getZ() * val));
}

Node Node::operator /(const double &val) const
{
    std::vector<double> vThis = this->get();
    return Node(vThis[0]/val,
                vThis[1]/val,
                vThis[2]/val);
    //return(Node(this->getX() / val, this->getY()  / val, this->getZ() / val));
}

bool Node::compare2d(const Node &other, double round ) const 
{
    std::vector<double> vOther = other.get();
    std::vector<double> vThis = this->get();
    return fabs(vThis[0]-vOther[0]) <= round &&
           fabs(vThis[1]-vOther[1]) <= round;
    //return fabs( this->getX() - other.getX() ) <= round   &&  fabs( this->getY() - other.getY() ) <= round;
}

bool Node::compare2d(const Node * other , double round ) const 
{
    return compare2d(*other, round);
    //return fabs( this->getX() - other->getX() ) <= round   &&  fabs( this->getY() - other->getY() ) <= round;
}
/*
void Node::SQLInsert(double x,double y,double z)
{
	SQLInsertAs("node");
	SQLSetValues(x,y,z);
}*/
/*
void Node::SQLDelete()
{
	SQLDeleteAs("node");
}*/

void Node::SQLSetValues(double x,double y,double z)
{
    DBConnector::getInstance()->Update("nodes", QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid),
                                       "x",     QVariant::fromValue(x),
                                       "y",     QVariant::fromValue(y),
                                       "z",     QVariant::fromValue(z));
    /*
	QSqlQuery q;
	q.prepare("UPDATE nodes SET x=?,y=?,z=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(x);
	q.addBindValue(y);
	q.addBindValue(z);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
    if(!q.exec())	PrintSqlError(&q);*/
}
