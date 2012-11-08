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
	SQLInsertThis(NOTYPE);
}
Attribute::Attribute(const Attribute &newattribute)
{
    this->name=newattribute.name;
	SQLInsertThis(newattribute.getType());
}

Attribute::Attribute(std::string name)
{
    this->name=name;
	SQLInsertThis(NOTYPE);
}


Attribute::Attribute(std::string name, double val)
{
    this->name=name;

	SQLInsertThis(DOUBLE);
	this->setDouble(val);
}
Attribute::Attribute(std::string name, std::string val)
{
    this->name=name;

	SQLInsertThis(STRING);
	setString(val);
}

Attribute::~Attribute()
{
	SQLDeleteThis();
}

Attribute::AttributeType Attribute::getType() const
{
    QVariant value;
    if(DBConnector::getInstance()->Select("attributes", QString::fromStdString(_uuid), "type", &value))
        return (AttributeType)value.toInt();
    return NOTYPE;
/*
	QSqlQuery q;
	q.prepare("SELECT type FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())	return (AttributeType)q.value(0).toInt();
    return NOTYPE;*/
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
	SQLSetValue(STRING, QString::fromStdString(s));
}

std::string Attribute::getString()
{
	QVariant value;
	if(SQLGetValue(value))	return value.toString().toStdString();
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
void Attribute::SQLInsertThis(AttributeType type)
{
	_uuid = QUuid::createUuid().toString().toStdString();


    DBConnector::getInstance()->Insert("attributes", QString::fromStdString(_uuid),
                                       "name", QString::fromStdString(name),
                                       "type", QVariant::fromValue((int)type));
/*
	QSqlQuery q;
	q.prepare("INSERT INTO attributes (uuid,name,type) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(_uuid));
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue((int)type);
    if(!q.exec())	PrintSqlError(&q);*/
}
void Attribute::SQLDeleteThis()
{
    DBConnector::getInstance()->Delete("attributes", QString::fromStdString(_uuid));
    /*QSqlQuery q;
	q.prepare("DELETE FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
    if(!q.exec())	PrintSqlError(&q);*/
}

void Attribute::SQLUpdateValue(QByteArray qba)
{	
    DBConnector::getInstance()->Update("attributes", QString::fromStdString(_uuid),
                                       "value",qba);
/*
	QSqlQuery q;
	q.prepare("UPDATE attributes SET value=? WHERE uuid=?");
	q.addBindValue(qba);
	q.addBindValue(QString::fromStdString(_uuid));
    if(!q.exec())	PrintSqlError(&q);*/
}
void Attribute::SetOwner(Component* owner)
{
	// TODO: make shure its not bound to another component
    DBConnector::getInstance()->Update("attributes", QString::fromStdString(_uuid),
                                       "owner",     QString::fromStdString(owner->getUUID()),
                                       "stateuuid", QString::fromStdString(owner->getStateUUID()));
    //SQLSetOwner(owner);
}
/*
void Attribute::SQLSetOwner(Component* owner)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET owner=?,stateuuid=? WHERE uuid=?");
	q.addBindValue(QString::fromStdString(owner->getUUID()));
	q.addBindValue(QString::fromStdString(owner->getStateUUID()));
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}*/
void Attribute::SQLSetName(std::string newname)
{	
    DBConnector::getInstance()->Update("attributes", QString::fromStdString(_uuid),
                                       "name",      QString::fromStdString(newname));
    /*
	QSqlQuery q;
	q.prepare("UPDATE attributes SET name=? WHERE uuid=?");
	q.addBindValue(QString::fromStdString(newname));
	q.addBindValue(QString::fromStdString(_uuid));
    if(!q.exec())	PrintSqlError(&q);*/
}
void Attribute::SQLSetType(AttributeType newtype)
{
    DBConnector::getInstance()->Update("attributes", QString::fromStdString(_uuid),
                                       "type",      QVariant::fromValue((int)newtype),
                                       "value",     QVariant::fromValue(0));
    /*
	QSqlQuery q;
	q.prepare("UPDATE attributes SET type=?,value=NULL WHERE uuid=?");
	q.addBindValue(newtype);
	q.addBindValue(QString::fromStdString(_uuid));
    if(!q.exec())	PrintSqlError(&q);*/
}

bool Attribute::SQLGetValue(QVariant &value)
{
    return DBConnector::getInstance()->Select("attributes",
                                              QString::fromStdString(_uuid), "value", &value);
    /*QSqlQuery q;
	q.prepare("SELECT value FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
	if(q.exec())	
	{
		if(q.next())	
		{
			value = q.value(0);
			return true;
		}
	}
	else
		PrintSqlError(&q);
    return false;*/
}
void Attribute::SQLSetValue(AttributeType type, QVariant value)
{
    DBConnector::getInstance()->Update("attributes", QString::fromStdString(_uuid),
                                        "type",      QVariant::fromValue((int)type),
                                        "value",     value);
    /*
	QSqlQuery q;
	q.prepare("UPDATE attributes SET type=?,value=? WHERE uuid=?");
	q.addBindValue((int)type);
	q.addBindValue(value);
	q.addBindValue(QString::fromStdString(_uuid));
    if(!q.exec())	PrintSqlError(&q);*/
}
