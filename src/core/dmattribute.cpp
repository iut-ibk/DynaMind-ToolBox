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

class RawAttribute
{
public:
    std::string name;
    Attribute::AttributeType type;
    QVariant value;

    RawAttribute(std::string name, Attribute::AttributeType type, QVariant value)
    {
        this->name = name;
        this->type = type;
        this->value = value;
    }
};

static Cache<QUuid,RawAttribute> attributeCache(2048);

QByteArray GetBinaryValue(std::vector<double> v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QVector<double>::fromStdVector(v);
	return bytes;
}

QByteArray GetBinaryValue(std::vector<std::string> v)
{
	std::vector<QString> qv;
	foreach(std::string item, v)
		qv.push_back(QString::fromStdString(item));

	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QVector<QString>::fromStdVector(qv);
	return bytes;
}

QByteArray GetBinaryValue(std::vector<LinkAttribute> v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << (int)v.size();
	foreach(LinkAttribute it, v)
	{
		s << QString::fromStdString(it.uuid);
		s << QString::fromStdString(it.viewname);
	}
	return bytes;
}

std::vector<LinkAttribute> ToLinkVector(QVariant q)
{
	std::vector<LinkAttribute> result;
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	int size;
	s >> size;
	QString uuid;
	QString view;
	LinkAttribute la;
	for(int i=0;i<size;i++)
	{
		s >> uuid;
		s >> view;
		la.uuid = uuid.toStdString();
		la.viewname = view.toStdString();
		result.push_back(la);
	}
	return result;
}

std::vector<double> ToDoubleVector(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	QVector<double> v;
	s >> v;
	return v.toStdVector();
}

std::vector<std::string> ToStringVector(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	QVector<QString> qv;
	s >> qv;
	std::vector<std::string> sv;
	foreach(QString qstr, qv)
		sv.push_back(qstr.toStdString());

	return sv;
}

Attribute::Attribute()
{
    this->name="";

	DBConnector::getInstance();
    SQLInsert(NOTYPE);
}
Attribute::Attribute(const Attribute &newattribute)
{
    this->name=newattribute.name;
    AttributeType type = newattribute.getType();
    QVariant value;
    newattribute.SQLGetValue(value);
    SQLInsert(type, value);
    //SQLInsertThis(type);
    //SQLSetValue(type, value);
}

Attribute::Attribute(std::string name)
{
    this->name=name;
    SQLInsert(NOTYPE);
}


Attribute::Attribute(std::string name, double val)
{
    this->name=name;

    SQLInsert(DOUBLE);
	this->setDouble(val);
}
Attribute::Attribute(std::string name, std::string val)
{
    this->name=name;

    SQLInsert(STRING);
	setString(val);
}

Attribute::~Attribute()
{
	SQLDeleteThis();
}

Attribute::AttributeType Attribute::getType() const
{
    return SQLGetType();
}

void Attribute::setName(std::string name)
{
    this->name=name;
	SQLSetName(name);
}

std::string Attribute::getName()
{
    return name;
}

void Attribute::setDouble(double v)
{
    SQLSetValue(DOUBLE, v);
}

double Attribute::getDouble()
{
	QVariant value;
    if(SQLGetValue(value))	return value.toDouble();
	
    return 0;
}

void Attribute::setString(std::string s)
{
    SQLSetValue(STRING, QString::fromStdString(s).toAscii());
}

std::string Attribute::getString()
{
    QVariant value;
    if(SQLGetValue(value))	return QString(value.toByteArray()).toStdString();
    return "";
}

void Attribute::setDoubleVector(std::vector<double> v)
{
	SQLSetValue(DOUBLEVECTOR, GetBinaryValue(v));
}

std::vector<double> Attribute::getDoubleVector()
{
	QVariant value;
	if(SQLGetValue(value))	return ToDoubleVector(value);
	return std::vector<double>();
}

void Attribute::setStringVector(std::vector<std::string> s)
{
	SQLSetValue(STRINGVECTOR, GetBinaryValue(s));
}

std::vector<std::string> Attribute::getStringVector()
{
	QVariant value;
	if(SQLGetValue(value))	return ToStringVector(value);
	return std::vector<string>();
}

bool Attribute::hasDouble()
{
    if (getType() == Attribute::DOUBLE)
        return true;
    return false;
}
bool Attribute::hasDoubleVector()
{
    if (getType() == Attribute::DOUBLEVECTOR)
        return true;
    return false;
}
bool Attribute::hasString()
{
    if (getType() == Attribute::STRING)
        return true;
    return false;
}
bool Attribute::hasStringVector()
{
    if (getType() == Attribute::STRINGVECTOR)
        return true;
    return false;
}

void Attribute::setLink(string viewname, string uuid)
{
	std::vector<LinkAttribute> links = getLinks();
	LinkAttribute att;
	att.uuid = uuid;
	att.viewname = viewname;
    links.push_back(att);
	setLinks(links);
}

void Attribute::setLinks(std::vector<LinkAttribute> links)
{
	SQLSetValue(LINK, GetBinaryValue(links));
}

LinkAttribute Attribute::getLink()
{
	QVariant value;
	if(SQLGetValue(value))
	{
		std::vector<LinkAttribute> linkvector = ToLinkVector(value);
		if(linkvector.size())
            return linkvector[0];
	}
	return LinkAttribute();
}

std::vector<LinkAttribute> Attribute::getLinks()
{
	std::vector<LinkAttribute> linkvector;

	QVariant value;
	if(SQLGetValue(value))
        linkvector = ToLinkVector(value);

    return linkvector;
}

void Attribute::addTimeSeries(std::vector<std::string> timestamp, std::vector<double> value)
{
    if(timestamp.size()!=value.size())
    {
        DM::Logger(DM::Error) << "Length of time and value vector are not equal";
        return;
    }
	QByteArray qba;
	QDataStream s(&qba, QIODevice::WriteOnly);
	s << (int)value.size();
	for(unsigned int i=0;i<value.size();i++)
	{
		s << value[i];
		s << QString::fromStdString(timestamp[i]);
	}
	SQLSetValue(LINK, qba);
}

void Attribute::getTimeSeries(std::vector<std::string> *timestamp, std::vector<double> *value)
{
	QVariant data;
	SQLGetValue(data);
	QByteArray qba = data.toByteArray();
	QDataStream s(&qba, QIODevice::ReadOnly);
	int size;
	s >> size;

	if(size==0)
	{
        DM::Logger(DM::Error) << "timestamp or value are equal null";
        return;
    }
	
	double dbl;
	QString str;
	for(int i=0;i<size;i++)
	{
		s >> dbl;
		s >> str;
		value->push_back(dbl);
		timestamp->push_back(str.toStdString());
    }
}

void Attribute::setType(AttributeType type)
{
    SQLSetType(type);
}
void Attribute::Change(Attribute &attribute)
{
    QVariant value;
    attribute.SQLGetValue(value);
    this->SQLSetValue(attribute.getType(), value);
}

const char *Attribute::getTypeName() const
{
	return Attribute::getTypeName(this->getType());
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
void Attribute::SQLInsert(AttributeType type)
{
    _uuid = QUuid::createUuid();

    DBConnector::getInstance()->Insert("attributes", _uuid,
                                       "name", QString::fromStdString(name),
                                       "type", QVariant::fromValue((int)type));
    attributeCache.add(_uuid, new RawAttribute(name,type,0));
}

void Attribute::SQLInsert(AttributeType type, QVariant value)
{
    _uuid = QUuid::createUuid();

    DBConnector::getInstance()->Insert("attributes", _uuid,
                                       "name", QString::fromStdString(name),
                                       "type", QVariant::fromValue((int)type),
                                       "value", value);
    attributeCache.add(_uuid, new RawAttribute(name,type,value));
}
void Attribute::SQLDeleteThis()
{
    DBConnector::getInstance()->Delete("attributes", _uuid);
    attributeCache.remove(_uuid);
}

void Attribute::SetOwner(Component* owner)
{
	// TODO: make shure its not bound to another component
    DBConnector::getInstance()->Update("attributes", _uuid,
                                       "owner",     QString::fromStdString(owner->getUUID()));
}

void Attribute::SQLSetName(std::string newname)
{	
    if(RawAttribute *att = attributeCache.get(_uuid))
    {
        att->name = name;
        attributeCache.get(_uuid);  // push to top
    }

    DBConnector::getInstance()->Update("attributes", _uuid,
                                       "name",      QString::fromStdString(newname));
}
void Attribute::SQLSetType(AttributeType newtype)
{
    SQLSetValue(newtype,0);
}

Attribute::AttributeType Attribute::SQLGetType() const
{
    if(RawAttribute *att = attributeCache.get(_uuid))
        return att->type;

    QVariant value;
    if(DBConnector::getInstance()->Select("attributes", _uuid, "type", &value))
        return (AttributeType)value.toInt();
    return NOTYPE;
}

bool Attribute::SQLGetValue(QVariant &value) const
{
    if(RawAttribute *att = attributeCache.get(_uuid))
    {
        value = att->value;
        return true;
    }
    return DBConnector::getInstance()->Select("attributes",
                                              _uuid, "value", &value);
}
void Attribute::SQLSetValue(AttributeType type, QVariant value)
{
    if(RawAttribute *att = attributeCache.get(_uuid))
    {
        att->type = type;
        att->value = value;
        attributeCache.get(_uuid);  // push to top
    }

    DBConnector::getInstance()->Update("attributes", _uuid,
                                        "type",      QVariant::fromValue((int)type),
                                        "value",     value);
}
