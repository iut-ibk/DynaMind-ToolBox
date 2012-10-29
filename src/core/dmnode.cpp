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
}

Node::Node( double x, double y, double z) : Component()
{
    this->x=x;
    this->y=y;
    this->z=z;
	SQLInsert();
}

Node::Node() : Component()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
	SQLInsert();
}

Node::Node(const Node& n) : Component(n)
{
    x=n.x;
    y=n.y;
    z=n.z;
	SQLInsert();
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
    return x;
}

double Node::getY() const
{
    return y;
}

double Node::getZ() const
{
    return z;
}

const double * const
Node::get() const {
    return v_;
}

void Node::setX(double x)
{
    this->x=x;
	SQLSetValues();
}

void Node::setY(double y)
{
    this->y=y;
	SQLSetValues();
}

void Node::setZ(double z)
{
    this->z=z;
	SQLSetValues();
}

Component* Node::clone()
{
    return new Node(*this);
}


bool Node::operator ==(const Node & other) const {

    return this->x == other.getX() && this->y == other.getY() && this->z == other.getZ();
}
Node Node::operator -(const Node & other) const {
    return(Node(this->x - other.getX(), this->y - other.getY(), this->z - other.getZ()));
}

Node Node::operator +(const Node & other) const {
    return(Node(this->x + other.getX(), this->y + other.getY(), this->z + other.getZ()));
}


bool Node::compare2d(const Node &other, double round ) const {

    return fabs( this->x - other.getX() ) <= round   &&  fabs( this->y - other.getY() ) <= round;
}

bool Node::compare2d(const Node * other , double round ) const {

    return fabs( this->x - other->getX() ) <= round   &&  fabs( this->y - other->getY() ) <= round;
}

void Node::SQLInsert()
{
	SQLInsertAs("node");
	SQLSetValues();
}
void Node::SQLDelete()
{
	SQLDeleteAs("node");
}

void Node::SQLSetValues()
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
