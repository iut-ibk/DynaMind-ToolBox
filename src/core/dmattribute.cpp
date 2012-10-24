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

#include <dmmodule.h>
#include <dmcomponent.h>
#include <dmattribute.h>
#include <QVariant>
#include "dmdbconnector.h"

using namespace DM;

Attribute::Attribute(const Attribute &newattribute)
{
    this->name=newattribute.name;
    this->doublevalue=newattribute.doublevalue;
    this->stringvalue=newattribute.stringvalue;
    this->doublevector=newattribute.doublevector;
    this->stringvector=newattribute.stringvector;
    this->type = newattribute.type;
	SQLInsertThis();
}

Attribute::Attribute(std::string name)
{
    this->name=name;
    this->type = Attribute::NOTYPE;
	SQLInsertThis();
}
Attribute::Attribute()
{
    this->name="";
    this->type = Attribute::NOTYPE;
	SQLInsertThis();
}

Attribute::Attribute(std::string name, double val)
{
    this->doublevalue = val;
    this->name=name;

    this->type = Attribute::DOUBLE;
	SQLInsertThis();
}
Attribute::Attribute(std::string name, std::string val)
{
    this->stringvalue = val;
    this->name=name;

    this->type = Attribute::STRING;
	SQLInsertThis();
}

Attribute::~Attribute()
{
	SQLDeleteThis();
}

Attribute::AttributeType Attribute::getType()
{
    return type;
}

void Attribute::setName(std::string name)
{
	SQLSetName(name);
    this->name=name;
}


std::string Attribute::getName()
{
    return name;
}


void Attribute::setDouble(double v)
{
    doublevalue=v;
    this->type = Attribute::DOUBLE;
}

double Attribute::getDouble()
{
    return doublevalue;
}

void Attribute::setString(std::string s)
{
    stringvalue=s;
    this->type = Attribute::STRING;
	SQLUpdateValue();
}

std::string Attribute::getString()
{
    return stringvalue;
}

void Attribute::setDoubleVector(std::vector<double> v)
{
    doublevector=v;
    this->type = Attribute::DOUBLEVECTOR;
	SQLUpdateValue();
}

std::vector<double> Attribute::getDoubleVector()
{
    return doublevector;
}

void Attribute::setStringVector(std::vector<std::string> s)
{
    stringvector=s;
    this->type = Attribute::STRINGVECTOR;
	SQLUpdateValue();
}

std::vector<std::string> Attribute::getStringVector()
{
    return stringvector;
}

bool Attribute::hasDouble()
{
    if (type == Attribute::DOUBLE)
        return true;
    return false;
}
bool Attribute::hasDoubleVector()
{
    if (type == Attribute::DOUBLEVECTOR)
        return true;
    return false;
}
bool Attribute::hasString()
{
    if (type == Attribute::STRING)
        return true;
    return false;
}
bool Attribute::hasStringVector()
{
    if (type == Attribute::STRINGVECTOR)
        return true;
    return false;
}

void Attribute::setLink(string viewname, string uuid)
{
    this->type = Attribute::LINK;
    this->stringvector.push_back(viewname);
    this->stringvector.push_back(uuid);
	SQLUpdateValue();
}

void Attribute::setLinks(std::vector<LinkAttribute> links)
{
    this->type = Attribute::LINK;
    this->stringvector.clear();
    foreach (LinkAttribute attr, links) {
        this->stringvector.push_back(attr.viewname);
        this->stringvector.push_back(attr.uuid);
    }
	
	SQLUpdateValue();
}

LinkAttribute Attribute::getLink()
{
    LinkAttribute attr;
    attr.viewname = this->stringvector[0];
    attr.uuid = this->stringvector[1];
    return attr;

}

std::vector<LinkAttribute> Attribute::getLinks()
{
    std::vector<LinkAttribute> links;
    for (unsigned int i = 0; i < stringvector.size(); i+=2) {
        LinkAttribute attr;
        attr.viewname = this->stringvector[i];
        attr.uuid = this->stringvector[i+1];
        links.push_back(attr);
    }
    return links;
}

void Attribute::addTimeSeries(std::vector<std::string> timestamp, std::vector<double> value)
{
    if(timestamp.size()!=value.size())
    {
        DM::Logger(DM::Error) << "Length of time and value vector are not equal";
        return;
    }

    this->type = Attribute::TIMESERIES;
    this->doublevector = value;
    this->stringvector = timestamp;
	SQLUpdateValue();
}

void Attribute::getTimeSeries(std::vector<std::string> *timestamp, std::vector<double> *value)
{
	if(!timestamp || !value)
	{
        DM::Logger(DM::Error) << "timestamp or value are equal null";
        return;
	}

	*value = doublevector;
	*timestamp = stringvector;
}

void Attribute::setType(AttributeType type)
{
	SQLSetType(type);
    this->type = type;
}

const char *Attribute::getTypeName() const
{
    return Attribute::getTypeName(this->type);
}

const char *Attribute::getTypeName(Attribute::AttributeType type)
{
    const char *arr[] = {
        "NOTYPE",
        "DOUBLE",
        "STRING",
        "TIMESERIES",
        "LINK",
        "DOUBLEVECTOR",
        "STRINGVECTOR"
    };
    return arr[type];
}
/*
Attribute::Attribute(std::string name, int type, QByteArray bytes)
{
	this->name = name;
	this->type = (AttributeType)type;
	switch(type)
	{
	case NOTYPE:
		break;
	case DOUBLE:
		doublevalue = bytes.toDouble();
		break;
	case STRING:
		stringvalue = QString(bytes).toStdString();
		break;
	case DOUBLEVECTOR:
		doublevector = DBConnector::GetDoubleVector(bytes);
		break;
	case STRINGVECTOR:
		stringvector = DBConnector::GetStringVector(bytes);
	case TIMESERIES:
		doublevector = DBConnector::GetDoubleVector(bytes);
		stringvector = DBConnector::GetStringVector(bytes);
		break;
	case LINK:	// TODO
		break;
	}
}*/

QByteArray Attribute::getValue()
{
	QByteArray bytes;

	QDataStream s(&bytes, QIODevice::WriteOnly);

	switch(type)
	{
	case NOTYPE:
		break;
	case DOUBLE:
		s << doublevalue;
		break;
	case STRING:
		s << QString::fromStdString(stringvalue);
		break;
	case DOUBLEVECTOR:
		s << QVector<double>::fromStdVector(doublevector);
		break;
	case STRINGVECTOR:
		s << QVector<QString>::fromStdVector(DBConnector::ToStdString(stringvector));
		break;
	case TIMESERIES:
		// TODO order?
		s << QVector<double>::fromStdVector(doublevector);
		s << QVector<QString>::fromStdVector(DBConnector::ToStdString(stringvector));
		break;
	case LINK:	// TODO
		break;
	}
	return bytes;
}
void Attribute::setValue(QByteArray bytes)
{
	switch(type)
	{
	case NOTYPE:
		break;
	case DOUBLE:
		doublevalue = bytes.toDouble();
		break;
	case STRING:
		stringvalue = QString(bytes).toStdString();
		break;
	case DOUBLEVECTOR:
		doublevector = DBConnector::GetDoubleVector(bytes);
		break;
	case STRINGVECTOR:
		stringvector = DBConnector::GetStringVector(bytes);
		break;
	case TIMESERIES:
		doublevector = DBConnector::GetDoubleVector(bytes);
		stringvector = DBConnector::GetStringVector(bytes);
		break;
	case LINK:	// TODO
		break;
	}
}


void Attribute::SQLInsertThis()
{
	_uuid = QUuid::createUuid().toString().toStdString();
	QSqlQuery q;
	q.prepare("INSERT INTO attributes (uuid,name,type,value) VALUES (?,?,?,?)");
	q.addBindValue(QString::fromStdString(_uuid));
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue((int)type);
	q.addBindValue(getValue());
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLDeleteThis()
{	
	QSqlQuery q;
	q.prepare("DELETE FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLUpdateValue()
{	
	QSqlQuery q;
	q.prepare("UPDATE attributes SET value=? WHERE uuid=?");
	q.addBindValue(getValue());
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SetOwner(Component* owner)
{
	// TODO: make shure its not bound to another component
	SQLSetOwner(owner);
}
void Attribute::SQLSetOwner(Component* owner)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET owner=?,stateuuid=? WHERE uuid=?");
	q.addBindValue(QString::fromStdString(owner->getUUID()));
	q.addBindValue(QString::fromStdString(owner->getStateUUID()));
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLSetName(std::string newname)
{	
	QSqlQuery q;
	q.prepare("UPDATE attributes SET name=? WHERE uuid=?");
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLSetType(AttributeType newtype)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET type=? WHERE uuid=?");
	q.addBindValue(newtype);
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
