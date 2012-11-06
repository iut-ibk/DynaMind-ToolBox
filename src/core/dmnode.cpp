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
/*
Node::Node(QByteArray qba) : Component()
{
	QDataStream stream(&qba, QIODevice::ReadOnly);
	stream>>z;
	stream>>y;
	stream>>x;
}

QByteArray Node::GetValue()
{
	QByteArray bytes;
	QDataStream stream(&bytes, QIODevice::WriteOnly);
	stream<<z;
	stream<<y;
	stream<<x;
	return bytes;
}*/

Node::Node( double x, double y, double z) : Component()
{
	/*this->x=x;
    this->y=y;
    this->z=z;*/
	SQLInsert(x,y,z);
}

Node::Node() : Component()
{
    /*this->x = 0;
    this->y = 0;
    this->z = 0;*/
	SQLInsert(0,0,0);
}

Node::Node(const Node& n) : Component(n)
{
	/*this->x=x;
    this->y=y;
    this->z=z;*/
	SQLInsert(n.getX(),n.getY(),n.getZ());
}
Node::~Node()
{
	SQLDelete();
}
DM::Components Node::getType()
{
	return DM::NODE;
}

double Node::getX() const
{
    //return x;
	QSqlQuery q;
	q.prepare("SELECT x FROM nodes WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
		return q.value(0).toDouble();

	return 0;
}

double Node::getY() const
{
    //return y;
	QSqlQuery q;
	q.prepare("SELECT y FROM nodes WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
		return q.value(0).toDouble();

	return 0;
}

double Node::getZ() const
{
    //return z;
	QSqlQuery q;
	q.prepare("SELECT z FROM nodes WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
		return q.value(0).toDouble();

	return 0;
}
/*
const double * const
Node::get() const {
    return v_;
}*/

const QVector<double> Node::get() const
{
    QVector<double> v;
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
    //this->x=x;
	QSqlQuery q;
	q.prepare("UPDATE nodes SET x=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(x);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);}

void Node::setY(double y)
{
    //this->y=y;
	QSqlQuery q;
	q.prepare("UPDATE nodes SET y=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(y);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);}

void Node::setZ(double z)
{
    //this->z=z;
	QSqlQuery q;
	q.prepare("UPDATE nodes SET z=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(z);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);}

Component* Node::clone()
{
    return new Node(*this);
}


bool Node::operator ==(const Node & other) const 
{
    //return this->x == other.getX() && this->y == other.getY() && this->z == other.getZ();
	return this->getX() == other.getX() && this->getY() == other.getY() && this->getZ() == other.getZ();
}
Node Node::operator -(const Node & other) const 
{
    //return(Node(this->x - other.getX(), this->y - other.getY(), this->z - other.getZ()));
    return(Node(this->getX() - other.getX(), this->getY() - other.getY(), this->getZ() - other.getZ()));
}

Node Node::operator +(const Node & other) const 
{
    //return(Node(this->x + other.getX(), this->y + other.getY(), this->z + other.getZ()));
    return(Node(this->getX() + other.getX(), this->getY() + other.getY(), this->getZ() + other.getZ()));
}

Node Node::operator *(const double &val) const
{
    return(Node(this->getX() * val, this->getY()  * val, this->getZ() * val));
}

Node Node::operator /(const double &val) const
{
    return(Node(this->getX() / val, this->getY()  / val, this->getZ() / val));
}

bool Node::compare2d(const Node &other, double round ) const 
{
    //return fabs( this->x - other.getX() ) <= round   &&  fabs( this->y - other.getY() ) <= round;
    return fabs( this->getX() - other.getX() ) <= round   &&  fabs( this->getY() - other.getY() ) <= round;
}

bool Node::compare2d(const Node * other , double round ) const 
{
    //return fabs( this->x - other->getX() ) <= round   &&  fabs( this->y - other->getY() ) <= round;
    return fabs( this->getX() - other->getX() ) <= round   &&  fabs( this->getY() - other->getY() ) <= round;
}

void Node::SQLInsert(double x,double y,double z)
{
	SQLInsertAs("node");
	SQLSetValues(x,y,z);
}
void Node::SQLDelete()
{
	SQLDeleteAs("node");
}

void Node::SQLSetValues(double x,double y,double z)
{
	QSqlQuery q;
	q.prepare("UPDATE nodes SET x=?,y=?,z=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(x);
	q.addBindValue(y);
	q.addBindValue(z);
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}
